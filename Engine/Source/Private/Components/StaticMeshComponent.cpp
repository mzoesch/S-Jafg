// Copyright mzoesch. All rights reserved.

#include "Components/StaticMeshComponent.h"
#include "Framework/ShaderSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/Frontend.h"
#include "Engine/Engine.h"

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
    JMaterialSubsystem& MaterialSubsystem{*Frontend.GetSubsystemChecked<JMaterialSubsystem>()};
    JShaderSubsystem& ShaderSubsystem{*Frontend.GetSubsystemChecked<JShaderSubsystem>()};

    auto& FetchedMaterial{MaterialSubsystem.GetFetchedMaterial(InMaterialInstance->Material->FetchedMaterial)};
    auto& FetchedShader{ShaderSubsystem.GetFetchedShader(FetchedMaterial.Shader)};
    if (FetchedShader.Layouts.empty())
    {
        LOG_FATAL(LogRhi, "Expected shared [PerspectiveCamera] binding point at [[(0,0)]].")
    }
    else
    {
        auto& Layout{FetchedShader.Layouts[0]};
        if (Layout.Type != LFetchedShader::Layout::Type::Shared || Layout.Identifier != "PerspectiveCamera")
        {
            LOG_FATAL(LogRhi
                , "Expected shared [PerspectiveCamera] binding point at [[(0,0)]]. But got [{}] at [[({}, {})]]."
                , Layout.Identifier.has_value() ? Layout.Identifier.value() : "<UNIQUE>", 0, 0
                )
        }
    }

    this->MaterialInstance = std::move(InMaterialInstance);

    return;
}

void Jafg::AStaticMeshComponent::Render(LRenderInfo const& Info) noexcept
{
    if (this->Mesh.get() == nullptr)
    {
        LOG_FATAL(LogRhi, "No mesh set for this static mesh component. Failed to render.")
    }
    if (this->MaterialInstance.get() == nullptr)
    {
        LOG_FATAL(LogRhi, "No material instance set for this static mesh component. Failed to render.")
    }

    auto& Pipeline{this->MaterialInstance->Material->Pipeline};

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline);

    check(Pipeline.DescriptorSetLayouts.size() == 2)
    check(Pipeline._UniqueDescriptorSetLayout.size() == 1)
    check(this->MaterialInstance->_UniqueDescriptorSets.size() == 1)

    TArray<vk::DescriptorSet> DescriptorSets; DescriptorSets.reserve(Pipeline.DescriptorSetLayouts.size());
    DescriptorSets.emplace_back(Info.PerspectiveCameraDescriptorSet);
    for (auto const& Set : this->MaterialInstance->_UniqueDescriptorSets)
    {
        DescriptorSets.emplace_back(*Set);
    }

    Info.CommandBuffer.bindDescriptorSets2({
        /* TODO: Is this correct? The sets are vertex && fragment respectively -- not vertex | fragment. */
        .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        .layout = *Pipeline.Layout,
        .firstSet = 0,
        .descriptorSetCount = static_cast<u32>(DescriptorSets.size()),
        .pDescriptorSets = DescriptorSets.data(),
        .dynamicOffsetCount = 0,
        .pDynamicOffsets = nullptr
        });

    LStaticMesh::VPC{.Model = maths::model(this->GetTransform())}.Push(Info, Pipeline);

    this->Mesh->DrawIndexed(Info);

    return;
}
