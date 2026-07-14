// Copyright mzoesch. All rights reserved.

#include "Components/StaticMeshComponent.h"
#include "User/UserPreferences.h"
#include "Framework/ShaderSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/Frontend.h"
#include "Engine/Engine.h"
#include "Rhi/PhysicalRendering.h"
#include "Framework/Actor.h"
#include "Rhi/OutlineRendering.h"

void Jafg::AStaticMeshComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);
    this->ShaderSubsystem = this->GetMutableLocalEgo().GetFrontend().GetSubsystemChecked<JShaderSubsystem>();
}

void Jafg::AStaticMeshComponent::SetMesh(LPath const& Mesh, EStaticMeshState MeshState)
{
    this->Mesh = this->GetEngine().GetSubsystemChecked<JMeshSubsystem>()->FromFile(Mesh, MeshState);
    this->SetAabb(this->Mesh->GetAabb());
}

void Jafg::AStaticMeshComponent::SetMaterialInstance(LMaterialInstanceRef InMaterialInstance) noexcept
{
    if (InMaterialInstance.get() == nullptr)
    {
        this->MaterialInstance = nullptr;
        return;
    }
    check(InMaterialInstance->Material.get() != nullptr)

    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    // JMaterialSubsystem& MaterialSubsystem{*Frontend.GetSubsystemChecked<JMaterialSubsystem>()};
    // JShaderSubsystem& ShaderSubsystem{*Frontend.GetSubsystemChecked<JShaderSubsystem>()};

    // auto& FetchedMaterial{MaterialSubsystem.GetFetchedMaterial(InMaterialInstance->Material->FetchedMaterial)};
    // auto& FetchedShader{ShaderSubsystem.GetFetchedShader(FetchedMaterial.Shader)};
    // if (FetchedShader.Layouts.empty())
    // {
    //     LOG_FATAL(LogRhi, "Expected shared [PerspectiveCamera] binding point at [[(0,0)]].")
    // }
    // else
    // {
    //     auto& Layout{FetchedShader.Layouts[0]};
    //     if (Layout.Type != LFetchedShader::Layout::Type::Shared || Layout.Identifier != "PerspectiveCamera")
    //     {
    //         LOG_FATAL(LogRhi
    //             , "Expected shared [PerspectiveCamera] binding point at [[(0,0)]]. But got [{}] at [[({}, {})]]."
    //             , Layout.Identifier.has_value() ? Layout.Identifier.value() : "<UNIQUE>", 0, 0
    //             )
    //     }
    // }
    this->MaterialInstance = std::move(InMaterialInstance);

    for (auto Idx{0uz}; Idx < Frontend.Vk_GetNumberOfFramesInFlight(); ++Idx)
    {
        this->uniformBuffers[Idx] = Frontend.Vk_CreateMappedBuffer({
            .size = sizeof(UBO::WorldData),
            .usage = vk::BufferUsageFlagBits::eUniformBuffer,
            });
    }

    return;
}

void Jafg::AStaticMeshComponent::Render(LActorRenderInfo const& Info) noexcept
{
    checkCode
    (
        if (!this->Mesh.get())
        {
            LOG_FATAL(LogRhi, "No mesh set for this static mesh component. Failed to render.")
        }
        if (!this->MaterialInstance.get())
        {
            LOG_FATAL(LogRhi, "No material instance set for this static mesh component. Failed to render.")
        }
        check(this->MaterialInstance->Material.get())
    )

#if JAFG_WITH_EDITOR
    /* Ok, but only in the editor. Else we should cull this comp from rendering beforehand; because performance. */
    if (!this->Mesh->IsOnDevice())
    {
        return;
    }
#endif /* !JAFG_WITH_EDITOR */
    check(this->Mesh->IsOnDevice())

    LMaterialInstance* InstancePtr{};
    if (Info.PreferredMaterial)
    {
        InstancePtr = &*Info.PreferredMaterial;
    }
    else if (Info.UserPreferences.EditorMeshMaterialPreference)
    {
        InstancePtr = &**Info.UserPreferences.EditorMeshMaterialPreference;
    }
    else
    {
        InstancePtr = this->MaterialInstance.get();
    }
    check(InstancePtr)
    LMaterialInstance& Instance{*InstancePtr};

    auto& Material{*Instance.Material};
    auto& Pipeline{Material.Pipeline};
    auto& MaterialTemplate{Material.Template};
    auto& Shader{MaterialTemplate.my_shader};

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline);

    {
        // Vulkan specs states at least 4.
        std::array<vk::DescriptorSet, 4> DescriptorSetsToBind;
        u32 NumDescriptorSets{0};
        for (auto It{Shader.begin_space()}; It != Shader.end_space(); ++It)
        {
            checkCode
            (
                if (*It >= 4)
                {
                    LOG_FATAL(LogRhi, "Expected at most 4 descriptor sets to bind. But got [{}].", *It)
                }
            )

            if (auto* Set{Instance.Vk_FindUniqueDescriptorSet(*It, Info.Frame)}; Set)
            {
                DescriptorSetsToBind[*It] = *Set;
            }
            else
            {
                if (auto* Attribute{It->find_user_attribute("Shared", 1uz)}; Attribute != nullptr)
                {
                    if (Attribute->front() == "Jafg::UBO::WorldData")
                    {
                        check(DescriptorSetsToBind[*It] == nullptr)
                        DescriptorSetsToBind[*It] = Info.WorldDataDescriptorSet;
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

    check(this->ShaderSubsystem)
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
        auto& Pc{this->ShaderSubsystem->GetPushConstant(*PushConstant.CxxName)};
        check(Pc.PushForActor)
        Pc.PushForActor(Info, Material, {this->GetTransform()});
    }

    this->Mesh->DrawIndexed(Info);
}
