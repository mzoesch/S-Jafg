// Copyright mzoesch. All rights reserved.

#include "Framework/SkeletalMeshComponent.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/EyeComponent.h"
#include "Framework/PersonaController.h"
#include "Framework/Frontend.h"
#include "Framework/Actor.h"
#include "Engine/Engine.h"

void Jafg::ASkeletalMeshComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    this->ReleaseFrameBuffers();
    Super::OnGarbage(Reason);
}

void Jafg::ASkeletalMeshComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);
    this->UpdatePose(Dt);
}

bool Jafg::ASkeletalMeshComponent::SetMesh(LPluginInfo Namespace, LStringView Path)
{
    LSkeletalMeshRef Loaded{this->GetEngine().GetSubsystemChecked<JMeshSubsystem>()->LoadSkeletalMesh(Namespace, Path)};
    if (!Loaded)
    {
        LOG_ERROR(LogAnimation, "[{}]: Failed to set the skeletal mesh [{}]. Keeping the previous one.", this->GetNameAsString(), Path)
        return false;
    }
    this->SetMesh(std::move(Loaded));
    return true;
}

void Jafg::ASkeletalMeshComponent::SetMesh(LSkeletalMeshRef InMesh)
{
    if (this->Mesh == InMesh)
    {
        return;
    }

    this->ReleaseFrameBuffers();
    this->Bindings.clear();
    this->Mesh = std::move(InMesh);
    this->bPoseValid = false;
    this->FirstPersonSubset = nullptr;
    /* They were made for the joints of the previous skeleton. */
    this->Proportions.clear();

    if (!this->Mesh)
    {
        this->LocalPose.clear();
        this->ModelPose.clear();
        this->SkinMatrices.clear();
        this->Aabb = maths::identity<LWorldAabb3>;
        return;
    }

    /* Draw something sensible even if this component never ticks (e.g., on an actor that does not tick). */
    this->UpdatePose(0.0f);
    this->RebuildFirstPersonSubset();
}

void Jafg::ASkeletalMeshComponent::SetMaterialInstance(LMaterialInstanceRef Instance) noexcept
{
    this->MaterialInstance = std::move(Instance);
    check(!this->MaterialInstance || !!this->MaterialInstance->Material.get())
}

void Jafg::ASkeletalMeshComponent::SetPbrMaterialInstance(LPbrMaterialInstanceRef Instance) noexcept
{
    this->PbrMaterialInstance = std::move(Instance);
    check(!this->PbrMaterialInstance || !!this->PbrMaterialInstance->Instance.get())
    if (this->PbrMaterialInstance)
    {
        this->SetMaterialInstance(this->PbrMaterialInstance->Instance);
    }
}

std::size_t Jafg::ASkeletalMeshComponent::AddAnimations(LPluginInfo Namespace, LStringView Path)
{
    LAnimationLibraryRef Library{this->GetEngine().GetSubsystemChecked<JMeshSubsystem>()->LoadAnimations(Namespace, Path)};
    if (!Library)
    {
        LOG_ERROR(LogAnimation, "[{}]: Failed to add the animations of [{}].", this->GetNameAsString(), Path)
        return 0;
    }
    std::size_t const Count{Library->Clips.size()};
    this->AddAnimations(std::move(Library));
    return Count;
}

void Jafg::ASkeletalMeshComponent::AddAnimations(LAnimationLibraryRef Library)
{
    check(Library)
    if (!algo::contains(this->Libraries, Library))
    {
        this->Libraries.push_back(std::move(Library));
    }
}

Jafg::LAnimationClipRef Jafg::ASkeletalMeshComponent::FindClip(LStringView Name) const noexcept
{
    /* Later files hide earlier ones. */
    for (auto It{this->Libraries.rbegin()}; It != this->Libraries.rend(); ++It)
    {
        if (LAnimationClipRef Clip{(*It)->FindClip(Name)})
        {
            return Clip;
        }
    }
    return nullptr;
}

bool Jafg::ASkeletalMeshComponent::Play(LStringView Clip, LAnimationPlayParams const& Params /* = {} */)
{
    LAnimationClipRef Found{this->FindClip(Clip)};
    if (!Found)
    {
        LOG_VERBOSE(LogAnimation, "[{}]: No clip [{}] to play.", this->GetNameAsString(), Clip)
        return false;
    }
    this->Player.Play(std::move(Found), Params);
    return true;
}

std::optional<u32> Jafg::ASkeletalMeshComponent::FindJoint(LStringView Name) const noexcept
{
    return this->Mesh ? this->Mesh->Skeleton->FindJoint(Name) : std::nullopt;
}

LWorldTrans Jafg::ASkeletalMeshComponent::GetJointWorldTransform(u32 Joint) const noexcept
{
    check(Joint < this->ModelPose.size())

    LMat4F const& M{this->ModelPose[Joint]};
    LVec3F const Scale{glm::length(LVec3F{M[0]}), glm::length(LVec3F{M[1]}), glm::length(LVec3F{M[2]})};
    LMat3F const Rotation{
        LVec3F{M[0]} / maths::max(Scale.x, 1.0e-8f),
        LVec3F{M[1]} / maths::max(Scale.y, 1.0e-8f),
        LVec3F{M[2]} / maths::max(Scale.z, 1.0e-8f),
        };

    LWorldTrans const Component{this->GetWorldTransformSlow()};
    return LWorldTrans{
        .t = Component.t + LWorldVec3{Component.r * (LVec3F{Component.s} * LVec3F{M[3]})},
        .r = maths::normalize(Component.r * LWorldQuat{glm::quat_cast(Rotation)}),
        .s = Component.s * LWorldVec3{Scale},
        };
}

void Jafg::ASkeletalMeshComponent::SetFirstPersonJoints(TArray<LString> NamePrefixes)
{
    this->FirstPersonJoints = std::move(NamePrefixes);
    this->RebuildFirstPersonSubset();
}

void Jafg::ASkeletalMeshComponent::SetProportions(TArray<LJointProportion> InProportions)
{
    if (!InProportions.empty() && (!this->Mesh || InProportions.size() != this->Mesh->Skeleton->GetJointCount()))
    {
        LOG_WARNING(LogAnimation, "[{}]: Ignoring [{}] proportions. The skeleton has [{}] joints."
            , this->GetNameAsString(), InProportions.size(), this->Mesh ? this->Mesh->Skeleton->GetJointCount() : 0uz)
        return;
    }
    /* All ones are the authored proportions. Skip them every frame. */
    if (algo::all_of(InProportions, [](LJointProportion const& Proportion){ return Proportion == LJointProportion{}; }))
    {
        InProportions.clear();
    }
    if (InProportions == this->Proportions)
    {
        return;
    }

    this->Proportions = std::move(InProportions);
    /* Also if this component does not tick (e.g., the world is not running). The clips do not advance. */
    this->bPoseValid = false;
    this->UpdatePose(0.0f);
}

void Jafg::ASkeletalMeshComponent::UpdatePose(f32 Dt)
{
    if (!this->Mesh)
    {
        return;
    }

    /* Nothing moves. Keep the pose (and the skinned vertices). */
    if (this->bPoseValid && !this->Player.IsPlaying() && !this->PoseModifier)
    {
        return;
    }

    LSkeleton const& Skeleton{*this->Mesh->Skeleton};
    this->Player.Advance(Dt * this->PlayRate);
    this->Player.Evaluate(Skeleton, [this](LAnimationClip const& Clip) -> LAnimationClipBinding const&
    {
        return this->GetBinding(Clip);
    }, this->LocalPose);

    if (this->PoseModifier)
    {
        this->PoseModifier(*this, this->LocalPose);
    }
    Skeleton.ApplyProportions(this->LocalPose, this->Proportions);

    this->ModelPose.resize(Skeleton.GetJointCount());
    this->SkinMatrices.resize(Skeleton.GetJointCount());
    Skeleton.LocalToModel(this->LocalPose, this->ModelPose);
    Skeleton.ModelToSkin(this->ModelPose, this->SkinMatrices, this->Proportions);

    this->Aabb = this->Mesh->ComputePosedAabb(this->ModelPose, this->SkinMatrices);
    ++this->PoseVersion;
    this->bPoseValid = true;
}

Jafg::LAnimationClipBinding const& Jafg::ASkeletalMeshComponent::GetBinding(LAnimationClip const& Clip) const
{
    check(this->Mesh)

    auto [It, bInserted]{this->Bindings.try_emplace(&Clip)};
    if (bInserted)
    {
        It->second = LAnimationClipBinding::Bind(Clip, *this->Mesh->Skeleton);
        if (It->second.Entries.empty())
        {
            LOG_WARNING(LogAnimation, "[{}]: Clip [{}] does not animate any joint of [{}]. Is it made for another skeleton?"
                , this->GetNameAsString(), Clip.Name, this->Mesh->Path)
        }
        else if (It->second.UnboundChannels > 0)
        {
            LOG_VERBOSE(LogAnimation, "[{}]: [{}] channels of clip [{}] animate nodes that [{}] does not have. They are ignored."
                , this->GetNameAsString(), It->second.UnboundChannels, Clip.Name, this->Mesh->Path)
        }
    }
    return It->second;
}

void Jafg::ASkeletalMeshComponent::RebuildFirstPersonSubset()
{
    this->FirstPersonSubset = nullptr;
    if (!this->Mesh || this->FirstPersonJoints.empty())
    {
        return;
    }

    LSkeleton const& Skeleton{*this->Mesh->Skeleton};
    std::vector<bool> Mask(Skeleton.GetJointCount());
    for (auto Idx{0uz}; Idx < Skeleton.GetJointCount(); ++Idx)
    {
        Mask[Idx] = algo::any_of(this->FirstPersonJoints, [&](LString const& Prefix)
        {
            return Skeleton.Joints[Idx].Name.starts_with(Prefix);
        });
    }
    if (std::ranges::none_of(Mask, std::identity{}))
    {
        LOG_WARNING(LogAnimation, "[{}]: No joint of [{}] matches the first-person joints. Nothing is drawn in first person."
            , this->GetNameAsString(), this->Mesh->Path)
        return;
    }
    this->FirstPersonSubset = &this->Mesh->GetSubset(Mask);
}

void Jafg::ASkeletalMeshComponent::ReleaseFrameBuffers() noexcept
{
    TArray<rhi::mapped_device_buffer> Buffers;
    for (LFrameBuffer& Frame: this->FrameBuffers)
    {
        if (*Frame.Vertices)
        {
            Buffers.push_back(std::move(Frame.Vertices));
        }
        Frame.Vertices = {};
        Frame.SkinnedVersion = ~0ull;
    }
    if (!Buffers.empty())
    {
        this->GetEngine().GetSubsystemChecked<JMeshSubsystem>()->DeferRelease(std::move(Buffers));
    }
}

bool Jafg::ASkeletalMeshComponent::IsSeenFromInside(LActorRenderInfo const& Info) const noexcept
{
    if (!Info.Viewer)
    {
        return false;
    }
    AEyeComponent const* const Eye{Info.Viewer->GetDominantViewEye()};
    return Eye && Eye->bFirstPerson && Eye->IsOwningActorValid() && &Eye->GetOwningActor() == &this->GetOwningActor();
}

void Jafg::ASkeletalMeshComponent::Render(LActorRenderInfo const& Info) const
{
    if (this->Mesh && this->Mesh->IsOnDevice() && this->bPoseValid)
    {
        LSkeletalMesh::LSubset const* Subset{};
        bool bDraw{true};
        if (this->IsSeenFromInside(Info))
        {
            switch (this->FirstPersonBody)
            {
            case EFirstPersonBody::Full:
            {
                break;
            }
            case EFirstPersonBody::Partial:
            {
                Subset = this->FirstPersonSubset;
                bDraw = Subset && Subset->TriangleCount > 0;
                break;
            }
            case EFirstPersonBody::Hidden:
            {
                bDraw = false;
                break;
            }
            }
        }

        if (bDraw)
        {
            this->DrawSkinned(Info, Subset);
        }
    }

    Super::Render(Info);
}

void Jafg::ASkeletalMeshComponent::DrawSkinned(LActorRenderInfo const& Info, LSkeletalMesh::LSubset const* Subset) const
{
    STAT_FUNCTION()
    check(this->Mesh && this->bPoseValid)
    check(Info.Frame < this->FrameBuffers.size())

    Info.MeshSubsystem.ReleaseDeferred();

    /*
     * One buffer per frame in flight. The frame in #Info waited for the device to finish with its buffer before
     * anything is drawn, so it is free to be written now. Draws of the same frame (shadows, depth, the scene) skin
     * only once.
     */
    LFrameBuffer& Frame{this->FrameBuffers[Info.Frame]};
    if (!*Frame.Vertices)
    {
        Frame.Vertices = Info.Frontend.Vk_CreateMappedBuffer(vk::BufferCreateInfo{
            .size = sizeof(LStaticMesh::Vertex) * this->Mesh->VertexCount,
            .usage = vk::BufferUsageFlagBits::eVertexBuffer,
            .sharingMode = vk::SharingMode::eExclusive,
            });
        Frame.SkinnedVersion = ~0ull;
        LOG_VERBOSE(LogAnimation, "[{}]: Allocated the skinned vertices of [{}] for frame [{}] ([{}] vertices)."
            , this->GetNameAsString(), this->Mesh->Path, Info.Frame, this->Mesh->VertexCount)
    }
    if (Frame.SkinnedVersion != this->PoseVersion)
    {
        STAT_ZONE("Jafg::ASkeletalMeshComponent::Skin")
        auto* const Vertices{static_cast<LStaticMesh::Vertex*>(Frame.Vertices.data())};
        for (auto const& Primitive: this->Mesh->Primitives)
        {
            SkinVertices(Primitive.Vertices, Primitive.Influences, this->SkinMatrices, Vertices + Primitive.FirstVertex);
        }
        Frame.SkinnedVersion = this->PoseVersion;
    }

    LWorldTrans const Transform{this->GetWorldTransformSlow()};
    rhi::resource_index_t const PbrIndex{this->PbrMaterialInstance ? this->PbrMaterialInstance->GetBindlessIndex() : rhi::invalid_resource_index};
    /* Called later in the same draw of the world if translucent. The skinned vertices of the frame stay until then. */
    auto Draw{[this, Subset, Transform, PbrIndex, Vertices=Frame.Vertices.get_buffer()](LActorRenderInfo const& Info, LMaterialInstance const& Instance)
    {
        BindActorMeshMaterial(Info, Transform, Instance, PbrIndex);

        Info.CommandBuffer.setPolygonModeEXT(Info.DefaultPerspectivePolygonMode);
        Info.CommandBuffer.bindVertexBuffers(0, Vertices, {0});
        for (auto Idx{0uz}; Idx < this->Mesh->Primitives.size(); ++Idx)
        {
            auto const& Primitive{this->Mesh->Primitives[Idx]};
            vk::Buffer const Indices{Subset ? *Subset->Parts[Idx].Indices : *Primitive.DeviceIndices};
            u32 const IndexCount{Subset ? Subset->Parts[Idx].IndexCount : static_cast<u32>(Primitive.Indices.size())};
            if (IndexCount == 0)
            {
                continue;
            }
            Info.CommandBuffer.bindIndexBuffer(Indices, 0, vk::IndexType::eUint32);
            Info.CommandBuffer.drawIndexed(IndexCount, 1, 0, static_cast<i32>(Primitive.FirstVertex), 0);
        }
    }};

    auto const& Preferred{GetPreferredMaterialInstance(Info, this->MaterialInstance.get())};
    if (this->PbrMaterialInstance.get() && this->MaterialInstance.get() == &Preferred)
    {
        DrawWithPbrMaterial(Info, this->PbrMaterialInstance, GetWorldCenter(this->Aabb, Transform), std::move(Draw));
    }
    else
    {
        /* The preferred material (e.g., of the shadows) might still want to know the pbr material. */
        Draw(Info, Preferred);
    }
}
