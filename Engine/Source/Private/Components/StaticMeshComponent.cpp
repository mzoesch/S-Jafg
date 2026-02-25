// Copyright mzoesch. All rights reserved.

#include "Components/StaticMeshComponent.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/Frontend.h"
#include "Engine/Engine.h"

void Jafg::AStaticMeshComponent::Create(CreateInfo const& Info)
{
    auto& Frontend{this->GetLocalEgo().GetFrontend()};
    JMaterialSubsystem& MaterialSubsystem{*Frontend.GetSubsystemChecked<JMaterialSubsystem>()};
    JTextureSubsystem& TextureSubsystem{*Frontend.GetSubsystemChecked<JTextureSubsystem>()};

    this->SetShouldRender(Info.bRender);

    check(Info.MeshPath.empty() == false)
    this->Mesh = GetSingleton<JMeshSubsystem>().FromFile(Info.MeshPath, Info.MeshState);

    check(Info.TextureView.empty() == false)
    this->Texture = TextureSubsystem.FromTextureViewIdentifier(Info.TextureView);

    check(Info.Material.empty() == false)
    this->MaterialInstance = MaterialSubsystem.GetInstance(MaterialSubsystem.GetMaterial(Info.Material));
    MaterialSubsystem.SetCombinedImageSampler(this->MaterialInstance, "Texture", *this->Texture);

    return;
}

void Jafg::AStaticMeshComponent::Render(LRenderInfo const& Info) noexcept
{
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
