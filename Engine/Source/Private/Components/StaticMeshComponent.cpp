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

void Jafg::AStaticMeshComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);
    this->ShaderSubsystem = this->GetLocalEgo().GetFrontend().GetSubsystemChecked<JShaderSubsystem>();
    return;
}

void Jafg::AStaticMeshComponent::SetMesh(LPath const& Mesh, EStaticMeshState MeshState)
{
    this->Mesh = GetSingleton<JMeshSubsystem>().FromFile(Mesh, MeshState);
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
    if (this->Mesh.get() == nullptr)
    {
        LOG_FATAL(LogRhi, "No mesh set for this static mesh component. Failed to render.")
    }
    if (this->MaterialInstance.get() == nullptr)
    {
        LOG_FATAL(LogRhi, "No material instance set for this static mesh component. Failed to render.")
    }
    check(this->MaterialInstance->Material.get())

    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    auto& Instance{Info.UserPreferences.MeshMaterialPreference.has_value()
        ? **Info.UserPreferences.MeshMaterialPreference
        : *this->MaterialInstance
        };

    auto& Material{*Instance.Material};
    auto& Pipeline{Material.Pipeline};
    auto& FetchedMaterial{Material.FetchedMaterial};
    auto& FetchedShader{FetchedMaterial.FetchedShader};

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline);

    if (FetchedShader.Layouts.empty() == false)
    {
        // Vulkan specs states at least 4.
        std::array<vk::DescriptorSet, 4> DescriptorSetsToBind;
        u32 NumDescriptorSets{0};
        algo::for_each(Instance.InfrequentDescriptorSets, [&DescriptorSetsToBind, &NumDescriptorSets](auto const& Set)
        {
            check(Set.first < DescriptorSetsToBind.size())
            check(DescriptorSetsToBind[Set.first] == nullptr)
            DescriptorSetsToBind[Set.first] = *Set.second;
            NumDescriptorSets = maths::max(NumDescriptorSets, Set.first + 1);
        });
        algo::for_each(Instance.FrequentDescriptorSets[Info.Frame], [&DescriptorSetsToBind, &NumDescriptorSets](auto const& Set)
        {
            check(Set.first < DescriptorSetsToBind.size())
            check(DescriptorSetsToBind[Set.first] == nullptr)
            DescriptorSetsToBind[Set.first] = *Set.second;
            NumDescriptorSets = maths::max(NumDescriptorSets, Set.first + 1);
        });

        for (auto Idx{0uz}; Idx < FetchedShader.Layouts.size(); ++Idx)
        {
            if (auto const& Layout{FetchedShader.Layouts[Idx]}; Layout.Type == LFetchedShader::Layout::eShared)
            {
                check(Layout.Identifier.has_value())
                if (Layout.Identifier.value() == "WorldData")
                {
                    check(DescriptorSetsToBind[Idx] == nullptr)
                    DescriptorSetsToBind[Idx] = Info.WorldDataDescriptorSet;

                    // TODO: ?
                    NumDescriptorSets = maths::max(NumDescriptorSets, static_cast<u32>(Idx + 1));
                }
            }
        }

        checkCode
        (
            for (vk::DescriptorSet const& SetToBind : DescriptorSetsToBind | std::views::take(NumDescriptorSets))
            {
                check(SetToBind != nullptr)
            }
        )

        Info.CommandBuffer.bindDescriptorSets2({
            /* TODO: Is this correct? The sets are vertex && fragment respectively -- not vertex | fragment. */
            .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            .layout = *Pipeline.Layout,
            .firstSet = 0,
            .descriptorSetCount = NumDescriptorSets,
            .pDescriptorSets = DescriptorSetsToBind.data(),
            .dynamicOffsetCount = 0,
            .pDynamicOffsets = nullptr
            });
    }

    check(this->ShaderSubsystem)
    for (auto const& PushConstantName : FetchedMaterial.FetchedShader.PushConstants)
    {
        auto const& PushConstant{this->ShaderSubsystem->GetPushConstant(PushConstantName)};
        check(PushConstant.AutoActorPush)
        PushConstant.AutoActorPush(Info, Pipeline, LActorDrawInfo{.Transform=this->GetTransform()});
    }

    this->Mesh->DrawIndexed(Info);

    return;
}

