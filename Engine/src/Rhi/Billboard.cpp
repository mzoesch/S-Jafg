// Copyright mzoesch. All rights reserved.

#include "Rhi/Billboard.h"
#include "Engine/Engine.h"
#include "Framework/ActorComponentForward.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/Frontend.h"
#include "Framework/ShaderSubsystem.h"
#include "Rhi/VisualInstance.h"
#include "User/UserPreferences.h"

//# TODO: Very badly optimized. We should combine this into a global buffer. Maybe with the #ADebugDrawerComponent?
void Jafg::LBillboard::Render(LActorRenderInfo const& Info, RenderInfo const& RenderInfo) const
{
    if (Info.PreferredMaterial)
    {
        if (Info.PreferredMaterial->Material->Template.identifier == "Materials/Mesh.Outline"sv)
        {
            this->NextColor = Colors::Yellow;
        }
        /* We are no perspective shader. Therefore, we cannot use this material. */
        return;
    }

    LColor Tint{this->m_Config.Tint};
    if (this->NextColor)
    {
        Tint = *this->NextColor;
        this->NextColor.reset();
    }

    if (!this->Instance)
    {
        this->Instance = Info.MaterialSubsystem.GetInstanceFromName(JafgInfo, "Materials/VisualBatch"sv);
        check(this->Instance)
    }
    check(this->Size.x > 0.0f && this->Size.y > 0.0f)

    if (maths::magnitude(Info.PerspectiveEye.translation) > this->m_Config.MaxDistance)
    {
        return;
    }

    auto ViewProjection{Info.WorldData.proj * Info.WorldData.view};
    LVec4F Clip{ViewProjection * LVec4F{RenderInfo.Translation, 1.0f}};
    if (Clip.w <= 0.0f)
    {
        return;
    }

    LVec4F ViewPos{Info.WorldData.view * LVec4F(RenderInfo.Translation, 1.0f)};
    LVec2F Size{this->Size * (maths::abs(Info.WorldData.proj[1][1]) * Info.VkViewport.height / (2.0f * -ViewPos.z))};
    if (Size.x < this->m_Config.MinSize.x || Size.y < this->m_Config.MinSize.y)
    {
        return;
    }

    LVec3F Ndc{maths::xyz(Clip) / Clip.w};
    if (Ndc.z < 0.0f  || Ndc.z > 1.0f)
    {
        return;
    }

    rhi::object_range<SSBO::VisualInstance> Range {1,1};
    if (LRect2F Rect{LVec2F{
        (Info.VkViewport.x + (Ndc.x * 0.5f + 0.5f) * Info.VkViewport.width) - (Size.x * 0.5f),
        (Info.VkViewport.y + (1.0f - (-Ndc.y * 0.5f + 0.5f)) * Info.VkViewport.height) - (Size.y * 0.5f),
        }, Size,}; !maths::aabb(LRect2F{
        .offset = {Info.VkViewport.x, Info.VkViewport.y},
        .extent = {Info.VkViewport.width, Info.VkViewport.height},
        }, Rect))
    {
        return;
    }
    else
    {
        Range->push_back({
            .Rect = Rect,
            .Tint = Tint,
            .TextureIndex = this->Texture->GetBindlessIndex(),
            });
    }

    check(!Range->empty())

    constexpr auto VisualSharedBinding{0uz};
    constexpr auto InstanceBufferBinding{1uz};

    auto& DsInstance{this->Instance->Vk_GetUniqueDescriptorSet(0, Info.Frame)};
    check(DsInstance.Resources.size() == 2)

    LVec2F Dimensions{Info.VkViewport.width, Info.VkViewport.height};
    auto VisualSharedWriteInfo{UBO::VisualShared{
            .Proj = glm::orthoRH_ZO(0.0f, Dimensions.x, 0.0f, Dimensions.y, 0.0f, 1.0f),
            .Gamma = *GetSingleton<JUserPreferences>().InterfaceGamma,
            }
        .upload(DsInstance.Resources[VisualSharedBinding].AsBuffer())
        .write_info(*DsInstance.Resources[VisualSharedBinding].AsBuffer())
        };

    auto InstanceBufferWriteInfo{Range.upload(DsInstance.Resources[InstanceBufferBinding].AsBuffer())};

    std::array Writes{
        vk::WriteDescriptorSet{
            .dstSet = *DsInstance,
            .dstBinding = VisualSharedBinding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = UBO::VisualShared::descriptor_type(),
            .pBufferInfo = &VisualSharedWriteInfo,
            },
        vk::WriteDescriptorSet{
            .dstSet = *DsInstance,
            .dstBinding = InstanceBufferBinding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = SSBO::VisualInstance::descriptor_type(),
            .pBufferInfo = &InstanceBufferWriteInfo
            },
        };
    Info.Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

    std::array<vk::DescriptorSet, 2> DescriptorSetsToBind;
    DescriptorSetsToBind[0] = *DsInstance;
    DescriptorSetsToBind[1] = *Info.Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetBindlessTextureArrayDescriptorSet();
    Info.CommandBuffer.bindDescriptorSets2({
        .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
        .layout = *this->Instance->Material->Pipeline.pipeline_layout,
        .firstSet = 0,
        .descriptorSetCount = DescriptorSetsToBind.size(),
        .pDescriptorSets = DescriptorSetsToBind.data(),
        .dynamicOffsetCount = 0,
        .pDynamicOffsets = nullptr
        });

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *this->Instance->Material->Pipeline);
    Info.CommandBuffer.setPolygonModeEXT(vk::PolygonMode::eFill);
    Info.CommandBuffer.draw(4, static_cast<u32>(Range->size()), 0, 0);
}

void Jafg::LBillboard::SetTexture(LTexture2Ref Texture)
{
    if (Texture && !Texture->IsBindless())
    {
        Detail::GMutableEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JTextureSubsystem>()
            ->AddTextureToGlobalBindlessArray(&*Texture);
    }
    this->Texture = std::move(Texture);
}
