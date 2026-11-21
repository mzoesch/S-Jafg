// Copyright mzoesch. All rights reserved.

#include "Rhi/StaticMesh.h"
#include "Engine/Engine.h"
#include "Rhi/RenderInfo.h"
#include "Framework/Editor.h"
#include "Widgets/EditorFactory.h"
#include "User/UserPreferences.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/ShaderSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "../Framework/PhysicsForeignCore.h"
#include "Gltf.h"

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-W#warnings"
    #pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
    #pragma clang diagnostic ignored "-Wcast-qual"
    #pragma clang diagnostic ignored "-Wdeprecated-literal-operator"
    #pragma clang diagnostic ignored "-Wextra-semi-stmt"
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
    #pragma clang diagnostic ignored "-Wmissing-format-attribute"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wtautological-type-limit-compare"
    #pragma clang diagnostic ignored "-Wunused-function"
#endif /* JAFG_WITH_CLANG */
#if JAFG_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif /* JAFG_WITH_GCC */
    #define TINYOBJLOADER_IMPLEMENTATION
    #include <tiny_obj_loader.h>
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */
#if JAFG_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* JAFG_WITH_GCC */

namespace
{

Jafg::LVertexInputRegistrator<Jafg::LStaticMesh::Vertex> _{};

} /* ~Namespace <Anonymous> */

template<>
Jafg::Detail::LNodeFactoryBase Jafg::GetEditorNode<Jafg::LStaticMeshRef>(TEditorNodeCreateInfo<LStaticMeshRef> const& Info) noexcept
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto SharedState(std::make_shared<LEditorRowState>());

    return NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
    [
        EditorComponentLabel(Info.Flags, Info.Viewport, "Mesh", SharedState)
        + EditorComponentContentWrapper(Info.Flags, Info.Viewport, SharedState)
        [
            NewNode(Info.Viewport).Class<WHParent>()
                .Anchor(EAnchor::Fill)
            [
                NewNode(Info.Viewport).Class<WEditableTextButton>()
                    .Anchor(EAnchor::HFill)
                    .Palette(*Prefs.InputPaletteSolid)
                    .Content(Info.Field ? algo::sprintf("{}", Info.Field->GetSource()) : "<not-set>")
                + NewNode(Info.Viewport).Class<WTextButtonIconizedDouble>()
                    .Palette(*Prefs.SecondaryPaletteSolid)
                    .Content("Browse")
                    .LeftIcon({JafgInfo, "Icons/DirectorySearch"})
                    .OnKeyEventFocused([FieldPtr=&Info.Field](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton) && Self.AabbTest(Info))
                        {
                            check(FieldPtr)
                            auto& Field{*FieldPtr};
                            check(Field.get())

                            LPath DefaultPath;
                            if (!Field->GetPath().empty() && Field->GetPath().has_parent_path())
                            {
                                DefaultPath = Field->GetPath().parent_path();
                            }
                            else
                            {
                                DefaultPath = **JafgInfo;
                            }

                            if (auto AbsolutePath{Info.Viewport.GetSurface().OpenBlockingDialogForFile({
                                .Filters = {FileDialogFilterPresets::Models}
                                }, DefaultPath)}; !AbsolutePath)
                            {
                                LOG_INFO(LogEditor, "Directory selection cancelled. No path provided. Discarding request.")
                            }
                            else
                            {
                                LOG_VERBOSE(LogEditor, "Selected path: [{}].", *AbsolutePath)
                                //# Fatal if the path is not inside the content dir of any namespace.
                                auto const [Namespace, Path]{SplitIntoNamespace(*AbsolutePath)};
                                EStaticMeshState NewState{EStaticMeshStateBits::None};
                                if (Field->IsOnHost())
                                {
                                    NewState |= EStaticMeshStateBits::Host;
                                }
                                if (Field->IsOnDevice())
                                {
                                    NewState |= EStaticMeshStateBits::Device;
                                }
                                if (Field->HasConvex())
                                {
                                    NewState |= EStaticMeshStateBits::Convex;
                                }

                                Field = GEngine->GetSubsystemChecked<JMeshSubsystem>()->LoadMesh(Namespace, Path, NewState);
                            }

                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
            ]
        ]
        + EditorResetButton(Info.Flags, Info.Viewport, true, [Field=Info.Field](WButton& Self)
        {
            check(!!Field)
            check(GEngine)
            Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_EditorWaitIdle();
            Field->FreeFromHost();
            Field->FreeFromDevice();
            return;
        }, LEditorRowState::Lambda<WButton>(SharedState))
    ];
}

Jafg::LStaticMesh::EResult Jafg::LStaticMesh::BringToState(EStaticMeshState State)
{
    return this->BringToState(State, [this](TArray<HostMesh>& Out) -> EResult
    {
        return this->PullHostFromSource(Out);
    });
}

Jafg::LStaticMesh::EResult Jafg::LStaticMesh::BringToState(EStaticMeshState State, LPullHost const& PullHost)
{
    bool const bWasOnHost{this->IsOnHost()};
    bool const bNeedsHost{
           (State & EStaticMeshStateBits::Host)
        || ((State & EStaticMeshStateBits::Device) && !this->IsOnDevice())
        || ((State & EStaticMeshStateBits::Convex) && !this->HasConvex())
        };

    if (bNeedsHost && !bWasOnHost)
    {
        if (auto Result{this->LoadToHost(false, PullHost)}; Result != EResult::Success)
        {
            return Result;
        }
    }
    if ((State & EStaticMeshStateBits::Convex) && !this->HasConvex())
    {
        this->ReloadConvex();
    }
    if ((State & EStaticMeshStateBits::Device) && !this->IsOnDevice())
    {
        this->LoadToDevice();
    }
    /* Host memory is orphaned if it was neither requested now nor by another client before. */
    if (!(State & EStaticMeshStateBits::Host) && !bWasOnHost)
    {
        this->FreeFromHost();
    }

    return EResult::Success;
}

#if JAFG_WITH_EDITOR
void Jafg::LStaticMesh::Reload(EStaticMeshState State)
{
    if (State & EStaticMeshStateBits::Host || State & EStaticMeshStateBits::Device)
    {
        if (auto Result{this->LoadToHost(!!(State & EStaticMeshStateBits::Convex))}; Result != EResult::Success)
        {
            LOG_FATAL(LogRhi, "[{}]: Failed to load mesh to host: {}.", this->Source, std::to_underlying(Result))
        }
    }
    else if (!!(State & EStaticMeshStateBits::Convex))
    {
        this->ReloadConvex();
    }

    if (State & EStaticMeshStateBits::Device)
    {
        this->LoadToDevice();
        if (!(State & EStaticMeshStateBits::Host))
        {
            this->FreeFromHost();
        }
    }
}
#endif /* JAFG_WITH_EDITOR */

Jafg::LStaticMesh::EResult Jafg::LStaticMesh::PullHostFromSource(TArray<HostMesh>& Out) const
{
    if (!is_regular_file(this->AbsolutePath))
    {
        return EResult::FileNotFound;
    }

    Gltf::LModel Model;
    if (auto Result{Gltf::Parse(this->AbsolutePath, Model)}; Result != EResult::Success)
    {
        return Result;
    }

    return Gltf::PullMesh(Model, this->AbsolutePath, this->Source.MeshIdx, Out);
}

Jafg::LStaticMesh::EResult Jafg::LStaticMesh::LoadToHost(bool bLoadConvex)
{
    return this->LoadToHost(bLoadConvex, [this](TArray<HostMesh>& Out) -> EResult
    {
        return this->PullHostFromSource(Out);
    });
}

Jafg::LStaticMesh::EResult Jafg::LStaticMesh::LoadToHost(bool bLoadConvex, LPullHost const& PullHost)
{
    STAT_FUNCTION()

    check(!this->Source.Path.empty())
    LOG_VERBOSE(LogRhi, "[{}]: Loading static mesh.", this->Source)

    /* Pulled aside first. A failed reload keeps the current host data. */
    TArray<HostMesh> Meshes;
    if (auto Result{PullHost(Meshes)}; Result != EResult::Success)
    {
        return Result;
    }
    if (Meshes.empty())
    {
        return EResult::Empty;
    }

    if (this->IsOnHost())
    {
        LOG_VERBOSE(LogRhi, "[{}]: Already loaded to host. Reloading.", this->Source)
    }
    if (this->Source.MeshIdx && this->IsOnDevice() && this->DeviceMeshes.front().Name != Meshes.front().Name)
    {
        LOG_WARNING(LogRhi, "[{}]: The mesh index now refers to [{}] instead of [{}]. Did a re-export reorder the meshes?"
            , this->Source, Meshes.front().Name, this->DeviceMeshes.front().Name)
    }

    this->HostMeshes = std::move(Meshes);
    check(this->IsOnHost())
    if constexpr (LogRhi.CompilesFor<ELogVerbosity::Trace>)
    {
        std::stringstream SS;
        SS << "Meshes: " << this->HostMeshes.size() << "\n";
        for (auto const& Mesh: this->HostMeshes)
        {
            SS << "  Mesh: " << Mesh.Name << ", Primitives: " << Mesh.Primitives.size() << "\n";
            for (auto const& Primitive: Mesh.Primitives)
            {
                SS << "    Primitive: Vertices: " << Primitive.Vertices.size() << ", Indices: " << Primitive.Indices.size() << "\n";
            }
        }
        LOG_TRACE(LogRhi, "[{}]: {}", this->Source, SS.str())
    }

    {
        STAT_NAMED_ZONE(__CalculatingAabb, "LoadToHost::CalculatingAabb")
        this->Aabb.min = this->HostMeshes[0].Primitives[0].Vertices[0].Position;
        this->Aabb.max = this->HostMeshes[0].Primitives[0].Vertices[0].Position;
        for (auto& Mesh: this->HostMeshes)
        {
            for (auto const& Primitive: Mesh.Primitives)
            {
                for (auto const& Vertex: Primitive.Vertices)
                {
                    this->Aabb.min = maths::min(this->Aabb.min, Vertex.Position);
                    this->Aabb.max = maths::max(this->Aabb.max, Vertex.Position);
                }
            }
        }
    }

#if JAFG_WITH_EDITOR
    this->BuildTraceGeometry();
#endif /* JAFG_WITH_EDITOR */

    if (bLoadConvex)
    {
        this->ReloadConvex();
    }

    return EResult::Success;
}

void Jafg::LStaticMesh::BuildTraceGeometry()
{
    STAT_FUNCTION()
    check(this->IsOnHost())

    this->TracePositions.clear();
    this->TraceIndices.clear();

    {
        size_t NumPositions{0};
        size_t NumIndices{0};
        for (auto const& Mesh: this->HostMeshes)
        {
            for (auto const& Primitive: Mesh.Primitives)
            {
                NumPositions += Primitive.Vertices.size();
                NumIndices += Primitive.Indices.size();
            }
        }
        this->TracePositions.reserve(NumPositions);
        this->TraceIndices.reserve(NumIndices);
    }

    for (auto const& Mesh: this->HostMeshes)
    {
        for (auto const& Primitive: Mesh.Primitives)
        {
            auto const Base{static_cast<u32>(this->TracePositions.size())};
            for (auto const& Vertex: Primitive.Vertices)
            {
                this->TracePositions.push_back(Vertex.Position);
            }
            for (auto Idx{0uz}; Idx + 2 < Primitive.Indices.size(); Idx += 3)
            {
                u32 const A{Primitive.Indices[Idx + 0]};
                u32 const B{Primitive.Indices[Idx + 1]};
                u32 const C{Primitive.Indices[Idx + 2]};
                if (A >= Primitive.Vertices.size() || B >= Primitive.Vertices.size() || C >= Primitive.Vertices.size())
                {
                    /* Malformed. Index values are not validated when parsing. */
                    continue;
                }
                this->TraceIndices.push_back(Base + A);
                this->TraceIndices.push_back(Base + B);
                this->TraceIndices.push_back(Base + C);
            }
        }
    }
}

std::optional<f32> Jafg::LStaticMesh::TraceTriangles(LVec3F const& Origin, LVec3F const& Direction, f32 MaxParameter) const noexcept
{
    STAT_FUNCTION()

    /* Moeller-Trumbore. Brute force; fine for single traces like clicks. */
    std::optional<f32> Closest;
    f32 Best{MaxParameter};
    for (auto Idx{0uz}; Idx + 2 < this->TraceIndices.size(); Idx += 3)
    {
        LVec3F const& A{this->TracePositions[this->TraceIndices[Idx + 0]]};
        LVec3F const& B{this->TracePositions[this->TraceIndices[Idx + 1]]};
        LVec3F const& C{this->TracePositions[this->TraceIndices[Idx + 2]]};

        LVec3F const E1{B - A};
        LVec3F const E2{C - A};
        LVec3F const P{maths::cross(Direction, E2)};
        f32 const Det{maths::dot(E1, P)};
        if (maths::abs(Det) < std::numeric_limits<f32>::min())
        {
            /* Parallel to the triangle (or degenerate). */
            continue;
        }
        f32 const InvDet{1.0f / Det};

        LVec3F const S{Origin - A};
        f32 const U{maths::dot(S, P) * InvDet};
        if (U < 0.0f || U > 1.0f)
        {
            continue;
        }
        LVec3F const Q{maths::cross(S, E1)};
        f32 const V{maths::dot(Direction, Q) * InvDet};
        if (V < 0.0f || U + V > 1.0f)
        {
            continue;
        }

        if (f32 const T{maths::dot(E2, Q) * InvDet}; T >= 0.0f && T <= Best)
        {
            Best = T;
            Closest = T;
        }
    }

    return Closest;
}

void Jafg::LStaticMesh::ReloadConvex()
{
    STAT_FUNCTION()

    this->FreeConvex();

    bool const bWasOnHost{this->IsOnHost()};
    if (!bWasOnHost)
    {
        LOG_VERBOSE(LogRhi, "[{}]: Not on host. Temporarily loading from secondary storage to build convex hull.", this->Source)
        if (auto Result{this->LoadToHost(false)}; Result != EResult::Success)
        {
            LOG_FATAL(LogRhi, "[{}]: Failed to load to host for convex hull generation: [{}]."
                , this->Source, std::to_underlying(Result))
        }
    }

    JPH::Array<JPH::Vec3> Positions;
    {
        size_t Count{0};
        for (auto const& Mesh: this->HostMeshes)
        {
            for (auto const& Primitive: Mesh.Primitives)
            {
                Count += Primitive.Vertices.size();
            }
        }
        Positions.reserve(Count);

        for (auto const& Mesh: this->HostMeshes)
        {
            for (auto const& Primitive: Mesh.Primitives)
            {
                for (auto const& Vertex: Primitive.Vertices)
                {
                    Positions.emplace_back(Vertex.Position.x, Vertex.Position.y, Vertex.Position.z);
                }
            }
        }
    }

    if (!bWasOnHost)
    {
        this->FreeFromHost();
    }

    char const* Error{nullptr};
    JPH::ConvexHullBuilder Builder{Positions};
    auto const Result{Builder.Initialize(this->MaxConvexVertices, this->ConvexTolerance, Error)};
    if (Result == JPH::ConvexHullBuilder::EResult::MaxVerticesReached)
    {
        LOG_TRACE(LogRhi, "[{}]: Convex hull reached the max vertex count of [{}]. The hull is an approximation."
            , this->Source, this->MaxConvexVertices
            )
    }
    else if (Result != JPH::ConvexHullBuilder::EResult::Success)
    {
        if (Result == JPH::ConvexHullBuilder::EResult::TooFewPoints)
        {
            LOG_FATAL(LogRhi, "[{}]: Failed to build convex hull. The mesh has too few points: [{}]."
                , this->Source, Error ? Error : "Unknown error."
                )
        }
        if (Result == JPH::ConvexHullBuilder::EResult::TooFewFaces)
        {
            LOG_FATAL(LogRhi, "[{}]: Failed to build convex hull. The mesh has too few faces: [{}]."
                , this->Source, Error ? Error : "Unknown error."
                )
        }
        if (Result == JPH::ConvexHullBuilder::EResult::Degenerate)
        {
            LOG_FATAL(LogRhi, "[{}]: Failed to build convex hull. The mesh is degenerate: [{}]."
                , this->Source, Error ? Error : "Unknown error."
                )
        }
        LOG_FATAL(LogRhi, "[{}]: Failed to build convex hull. Unknown error: [{}]."
            , this->Source, Error ? Error : "Unknown error."
            )
    }

    TArray<u8> bUsed(Positions.size(), 0);
    for (JPH::ConvexHullBuilder::Face const* Face: Builder.GetFaces())
    {
        if (Face->mRemoved)
        {
            continue;
        }

        JPH::ConvexHullBuilder::Edge const* Edge{Face->mFirstEdge};
        do
        {
            if (bUsed[Edge->mStartIdx] == 0)
            {
                bUsed[Edge->mStartIdx] = 1;
                JPH::Vec3 const& P{Positions[Edge->mStartIdx]};
                this->ConvexHull.emplace_back(P.GetX(), P.GetY(), P.GetZ());
            }
            Edge = Edge->mNextEdge;
        }
        while (Edge != Face->mFirstEdge);
    }

    LOG_VERBOSE(LogRhi, "[{}]: Built convex hull with [{}] vertices from [{}] vertices."
        , this->Source, this->ConvexHull.size(), Positions.size()
        )

    if (!this->HasConvex())
    {
        LOG_FATAL(LogRhi, "[{}]: Failed to build convex hull from [{}] vertices."
            , this->Source, Positions.size()
            )
    }
}

void Jafg::LStaticMesh::LoadToDevice()
{
    LOG_TRACE(LogRhi, "[{}]: Loading to device.", this->Source)
    check(this->IsOnHost())

    if (this->IsOnDevice())
    {
        LOG_TRACE(LogRhi, "[{}]: Already loaded to device. Reloading.", this->Source)
        this->FreeFromDevice();
    }
    check(this->DeviceMeshes.empty())

    for (auto const& HostMesh: this->HostMeshes)
    {
        auto& DeviceMesh{this->DeviceMeshes.emplace_back(HostMesh.Name)};
        for (auto const& HostPrimitive: HostMesh.Primitives)
        {
            auto& DevicePrimitive{DeviceMesh.Primitives.emplace_back()};
            DevicePrimitive.Vertices = Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Vertex({
                .BufferCopy = vk::BufferCopy{0, 0, sizeof(decltype(HostPrimitive.Vertices)::value_type) * HostPrimitive.Vertices.size()},
                .Data = HostPrimitive.Vertices.data(),
                }));
            DevicePrimitive.Indices = Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Index({
                .BufferCopy = vk::BufferCopy{0, 0, sizeof(decltype(HostPrimitive.Indices)::value_type) * HostPrimitive.Indices.size()},
                .Data = HostPrimitive.Indices.data(),
                }));
            DevicePrimitive.IndexCount = static_cast<u32>(HostPrimitive.Indices.size());
        }
    }
}

void Jafg::LStaticMesh::Render(LActorRenderInfo const& Info, LWorldTrans const& Transform, std::variant<LMaterialInstance const*,LPbrMaterialInstance const*> InstanceVariant) const
{
#if JAFG_WITH_EDITOR
    /* Ok, but only in the editor. Else we should cull this comp from rendering beforehand; because performance. */
    if (!this->IsOnDevice())
    {
        return;
    }
#endif /* !JAFG_WITH_EDITOR */
    check(this->IsOnDevice())

    LMaterialInstance const* Instance{};
    if (std::holds_alternative<LMaterialInstance const*>(InstanceVariant))
    {
        Instance = std::get<LMaterialInstance const*>(InstanceVariant);
    }
    else if (std::holds_alternative<LPbrMaterialInstance const*>(InstanceVariant))
    {
        Instance = std::get<LPbrMaterialInstance const*>(InstanceVariant)->Instance.get();
    }
    check(Instance)
    check(Instance->Material.get())

    auto& Material{*Instance->Material};
    auto& Pipeline{Material.Pipeline};
    auto& MaterialTemplate{Material.Template};
    auto& Shader{MaterialTemplate.my_shader};

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline);

    {
        std::array<vk::DescriptorSet, rhi::bound_descriptor_set_limit> DescriptorSetsToBind;
        u32 NumDescriptorSets{0};
        for (auto It{Shader.begin_space()}; It != Shader.end_space(); ++It)
        {
            checkCode
            (
                if (*It >= rhi::bound_descriptor_set_limit)
                {
                    LOG_FATAL(LogRhi, "Expected at most [{}] descriptor sets to bind. But got [{}].", rhi::bound_descriptor_set_limit, *It)
                }
            )

            if (auto* Set{Instance->Vk_FindUniqueDescriptorSet(*It, Info.Frame)}; Set)
            {
                DescriptorSetsToBind[*It] = **Set;
            }
            else
            {
                if (auto* Attribute{It->find_user_attribute("Shared", 1uz)}; Attribute != nullptr)
                {
                    if (auto Set{Info.Sets.find(Attribute->front())}; Set != Info.Sets.end())
                    {
                        check(DescriptorSetsToBind[*It] == nullptr)
                        DescriptorSetsToBind[*It] = Set->second;
                    }
                    else
                    {
                        LOG_FATAL(LogRhi, "[{}]: No descriptor set for space [{}@{}] found. Unexpected shared attribute [{}]."
                            , Shader.Identifier, *It, Info.Frame, Attribute->front())
                    }
                }
                else
                {
                    LOG_FATAL(LogRhi, "[{}]: No descriptor set for space [{}@{}] found."
                        , Shader.Identifier, *It, Info.Frame)
                }
            }

            NumDescriptorSets = maths::max(NumDescriptorSets, *It + 1);
        }

        checkCode
        (
            for (vk::DescriptorSet const& SetToBind: DescriptorSetsToBind | algo::views::take(NumDescriptorSets))
            {
                check(!!SetToBind)
            }
        )

        if (NumDescriptorSets > 0)
        {
            Info.CommandBuffer.bindDescriptorSets2({
                /* TODO: Is this correct? The sets are vertex && fragment respectively -- not vertex | fragment. */
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .layout = *Pipeline.pipeline_layout,
                .firstSet = 0,
                .descriptorSetCount = NumDescriptorSets,
                .pDescriptorSets = DescriptorSetsToBind.data(),
                .dynamicOffsetCount = 0,
                .pDynamicOffsets = nullptr
                });
        }
    }

    for (auto const& PushConstant: Shader.push_constant_iter())
    {
        checkCode
        (
            if (!PushConstant.CxxName.has_value())
            {
                LOG_FATAL(LogRhi, "[{}]: Push constant at [{}@{}] has no CXX name."
                    , Shader.Identifier, PushConstant.Space, PushConstant.Index)
            }
        )
        auto& Pc{Info.ShaderSubsystem.GetPushConstant(*PushConstant.CxxName)};
        check(Pc.PushForActor)
        Pc.PushForActor(Info, Material, {
            .Transform=Transform,
            .PbrMaterial=std::holds_alternative<LPbrMaterialInstance const*>(InstanceVariant)
                ? std::get<LPbrMaterialInstance const*>(InstanceVariant)->GetBindlessIndex()
                : rhi::invalid_resource_index
            });
    }

    Info.CommandBuffer.setPolygonModeEXT(Info.DefaultPerspectivePolygonMode);

    this->DrawIndexed(Info);
}

void Jafg::LStaticMesh::DrawIndexed(LRenderInfo const& Info) const
{
    check(this->IsOnDevice())

    for (auto& Mesh: this->DeviceMeshes)
    {
        for (auto& Primitive: Mesh.Primitives)
        {
            check(Primitive.IndexCount > 0)
            check(*Primitive.Vertices && *Primitive.Indices)

            Info.CommandBuffer.bindVertexBuffers(0, *Primitive.Vertices, {0});
            Info.CommandBuffer.bindIndexBuffer(*Primitive.Indices, 0, vk::IndexTypeValue<decltype(this->HostMeshes[0].Primitives[0].Indices)::value_type>::value);

            Info.CommandBuffer.drawIndexed(Primitive.IndexCount, 1, 0, 0, 0);
        }
    }
}
