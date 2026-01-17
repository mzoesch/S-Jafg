// Copyright mzoesch. All rights reserved.

#include "Components/StaticMeshComponent.h"
#include "System/MeshSubsystem.h"
#include "System/TextureSubsystem.h"
#include "Framework/Frontend.h"

static LMatrix4F Model(LTransformF Transform)
{
    LMatrix4F M;

    M.Matrix[3][0] = Transform.Translation.X;
    M.Matrix[3][1] = -Transform.Translation.Y;
    M.Matrix[3][2] = Transform.Translation.Z;

    return M;
}

void Jafg::JStaticMeshComponent::CreateImpl(LCreateInfo const& Info)
{
    this->Mesh = GetDefault<JMeshSubsystem>()->GetMesh(Info.MeshPath, Info.MeshLoadBehavior, Info.MeshHostMemoryBehavior);
    this->SetShouldRender(Info.bRender);
    if (Info.TexturePath.empty() == false)
    {
        this->Image = GetDefault<JTextureSubsystem>()->GetImage(
              Info.TexturePath
            , Info.TextureMetadata
            , Info.TextureLoadFlags
            );
    }

    return;
}

void Jafg::JStaticMeshComponent::Render(LRenderInfo const& Info) noexcept
{
    auto& Pipeline{Info.Frontend.Vk_GetPipelines().at({LStaticMesh::DefaultShader})};

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline);

    auto Sets{(*Info.Frontend.Vk_GetDevice()).allocateDescriptorSets({
        .descriptorPool = Info.DescriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &*Pipeline.DescriptorSetLayout,
        })};
    check( Sets.size() == 1 )
    auto Set{ Sets[0] };

    vk::DescriptorImageInfo ImageInfo{
        .sampler = Info.Surface.GetFrontend().Vk_GetDefaultSampler(),
        .imageView = this->Image.GetTexture().GetImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
        };

    std::array Writes{
        vk::WriteDescriptorSet{
            .dstSet = Set,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pBufferInfo = &Info.PerspectiveCameraWriteInfo,
            },
        vk::WriteDescriptorSet{
            .dstSet = Set,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = &ImageInfo,
            },
        };
    Info.Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

    Info.CommandBuffer.bindDescriptorSets2({
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .layout = *Pipeline.Layout,
        .firstSet = 0,
        .descriptorSetCount = 1,
        .pDescriptorSets = &Set,
        .dynamicOffsetCount = 0,
        .pDynamicOffsets = nullptr
        });

    LStaticMesh::LRootLocation
    {
        .Model = ::Model(this->GetTransform())}
        .Push(Info, Pipeline);
    this->Mesh.GetMesh().DrawIndex(Info);

    return;
}
