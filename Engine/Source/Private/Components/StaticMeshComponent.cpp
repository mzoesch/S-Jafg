// Copyright mzoesch. All rights reserved.

#include "Components/StaticMeshComponent.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/Frontend.h"

void Jafg::AStaticMeshComponent::Create(CreateInfo const& Info)
{
    this->SetShouldRender(Info.bRender);
    check(Info.MeshPath.empty() == false)
    this->Mesh = GetSingleton<JMeshSubsystem>().FromFile(Info.MeshPath, Info.MeshState);
    if (Info.TexturePath.empty() == false)
    {
        this->Texture = GetSingleton<JTextureSubsystem>().FromFile(
              Info.TexturePath
            , Info.TextureHostCreateInfo
            , Info.TextureDeviceCreateInfo
            , Info.TextureState
            );
    }
}

static u64 Frame{999999};

void Jafg::AStaticMeshComponent::Render(LRenderInfo const& Info) noexcept
{
    auto& Pipeline{Info.Frontend.Vk_GetPipelines().at({LStaticMesh::DefaultShader})};

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline);

    check(Pipeline.SharedDescriptorSetLayout.size() == 2)
    check(Pipeline.UniqueDescriptorSetLayout.size() == 0)

    vk::DescriptorImageInfo ImageInfo{
        .sampler = Info.Surface.GetFrontend().Vk_GetDefaultSampler(),
        .imageView = this->Texture->GetImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
        };

    if (Application::GetFrameCount() != Frame)
    {
        Frame = Application::GetFrameCount();
        std::array Writes{
            vk::WriteDescriptorSet{
                .dstSet = Info.DefaultMaterialDescriptorSet,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                .pImageInfo = &ImageInfo,
                },
            };
        Info.Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
    }

    Info.CommandBuffer.bindDescriptorSets2({
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .layout = *Pipeline.Layout,
        .firstSet = 0,
        .descriptorSetCount = 1,
        .pDescriptorSets = &Info.PerspectiveCameraDescriptorSet,
        .dynamicOffsetCount = 0,
        .pDynamicOffsets = nullptr
        });

    Info.CommandBuffer.bindDescriptorSets2({
        .stageFlags = vk::ShaderStageFlagBits::eFragment,
        .layout = *Pipeline.Layout,
        .firstSet = 1,
        .descriptorSetCount = 1,
        .pDescriptorSets = &Info.DefaultMaterialDescriptorSet,
        .dynamicOffsetCount = 0,
        .pDynamicOffsets = nullptr
        });

    LStaticMesh::VPC{.Model = maths::model(this->GetTransform())}.Push(Info, Pipeline);
    this->Mesh->DrawIndexed(Info);

    return;
}
