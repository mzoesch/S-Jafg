// Copyright mzoesch. All rights reserved.

#include "Components/StaticMeshComponent.h"
#include "System/MeshSubsystem.h"
#include "System/TextureSubsystem.h"
#include "Framework/Frontend.h"

// static glm::mat4 ScaleIdentity(LVector3F const& V)
// {
//     return {};
// }
//
// static glm::mat4 RotateIdentity(LRotatorF Rotator)
// {
//     LMatrix4F M;
//     glm::mat4 m = *reinterpret_cast<glm::mat4*>(&M);
//     check( m == glm::identity<glm::mat4>() )
//
//
//
//
//     // Rotator = LRotatorF{
//     //     Rotator.Yaw,
//     //     Rotator.Pitch,
//     //     Rotator.Roll,
//     //     };
//     //
//     // f32 tmp_ch = Maths::Cos(Rotator.Yaw);
//     // f32 tmp_sh = Maths::Sin(Rotator.Yaw);
//     // f32 tmp_cp = Maths::Cos(Rotator.Pitch);
//     // f32 tmp_sp = Maths::Sin(Rotator.Pitch);
//     // f32 tmp_cb = Maths::Cos(Rotator.Roll);
//     // f32 tmp_sb = Maths::Sin(Rotator.Roll);
//
//     glm::mat4 Result = glm::identity<glm::mat4>();
//     // Result.Matrix[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
//     // Result.Matrix[0][1] = tmp_sb * tmp_cp;
//     // Result.Matrix[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
//     // Result.Matrix[0][3] = static_cast<f32>(0);
//     // Result.Matrix[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
//     // Result.Matrix[1][1] = tmp_cb * tmp_cp;
//     // Result.Matrix[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
//     // Result.Matrix[1][3] = static_cast<f32>(0);
//     // Result.Matrix[2][0] = tmp_sh * tmp_cp;
//     // Result.Matrix[2][1] = -tmp_sp;
//     // Result.Matrix[2][2] = tmp_ch * tmp_cp;
//     // Result.Matrix[2][3] = static_cast<f32>(0);
//     // Result.Matrix[3][0] = static_cast<f32>(0);
//     // Result.Matrix[3][1] = static_cast<f32>(0);
//     // Result.Matrix[3][2] = static_cast<f32>(0);
//     // Result.Matrix[3][3] = static_cast<f32>(1);
//
//     return Result;
// }
//
// static glm::mat4 TranslateIdentity(LVector3F const& V)
// {
//     // LMatrix4F M;
//     // M.Matrix[3][0] = V.X;
//     // M.Matrix[3][1] = V.Y;
//     // M.Matrix[3][2] = V.Z;
//
//     return glm::translate(glm::identity<glm::mat4>(), *reinterpret_cast<glm::vec3 const*>(&V));
//     // return M;
// }

static LMat4F Model(LWorldTrans Transform)
{
    // LMatrix4F T = TranslateIdentity(Transform.Translation);
    // LMatrix4F R = RotateIdentity(Transform.Rotator);
    // LMatrix4F S = ScaleIdentity(Transform.Scale);
    //
    // auto temp = R * S;
    // temp = T * temp;

    // auto T = TranslateIdentity(Transform.Translation);
    // auto R = RotateIdentity(Transform.Rotator);
    //
    // auto t = T * R;

    // return *reinterpret_cast<LMatrix4F*>(&T);
    return LMat4F (1);
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

    LStaticMesh::LRootLocation{
        // .Model = ::Model(this->GetTransform())
        .Model = ::Model(maths::zero_trans<LWorldTrans>)
        }.Push(Info, Pipeline);
    this->Mesh.GetMesh().DrawIndex(Info);

    return;
}
