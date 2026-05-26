// Copyright mzoesch. All rights reserved.

#include "Rhi/RenderTarget.h"
#include "Rhi/NodeRenderInfo.h"
#include "Framework/Frontend.h"

void Jafg::LRenderTarget::Initialize(CreateInfo Info)
{
    auto& Frontend{Info.Frontend};

    check(Info.Extent.width > 0 && Info.Extent.height > 0)

    if (Frontend.Vk_GetSurfaceFormat().format == vk::Format::eB8G8R8A8Srgb)
    {
        //# TODO: Why does this work?
        auto C{Info.ClearColor.ToLinearColor()};
        this->ClearColor = {vk::ClearColorValue(std::array<f32,4>{C.R, C.G, C.B, C.A})};
    }
    else
    {
        LOG_FATAL(LogVulkan, "Render target does not support format [{}].", vk::to_string(Frontend.Vk_GetSurfaceFormat().format))
    }

    this->Extent = Info.Extent;
    this->ResolveFlags = Info.ResolveMode;

    this->MsaaTarget = {};
    this->ResolvedTarget.reset();

    vk::ImageCreateInfo ImageCreateInfo{
        .imageType = vk::ImageType::e2D,
        .format = Frontend.Vk_GetSurfaceFormat().format,
        .extent = vk::Extent3D{this->Extent.width, this->Extent.height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples =  rhi::vk_clamp_msaa_samples(Info.SampleCount, Frontend.Vk_GetMsaaSampleLimits()),
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eColorAttachment
            | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
        .initialLayout = vk::ImageLayout::eUndefined,
        };
    this->MsaaTarget.Image = Frontend.Vk_CreateDeviceLocalImage(ImageCreateInfo);
    this->MsaaTarget.ImageView = vk::raii::ImageView{Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
        .image = this->MsaaTarget.Image.GetBuffer(),
        .viewType = vk::ImageViewType::e2D,
        .format = Frontend.Vk_GetSurfaceFormat().format,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
            },};

    if (this->ResolveFlags != vk::ResolveModeFlagBits::eNone)
    {
        vk::ImageCreateInfo ResolveImageCreateInfo{
            .imageType = vk::ImageType::e2D,
            .format = Frontend.Vk_GetSurfaceFormat().format,
            .extent = vk::Extent3D{this->Extent.width, this->Extent.height, 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = vk::SampleCountFlagBits::e1,
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eColorAttachment
                | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
            .initialLayout = vk::ImageLayout::eUndefined,
            };
        this->ResolvedTarget.emplace(Target{});
        auto& ResolvedMsaa{*this->ResolvedTarget};
        ResolvedMsaa.Image = Frontend.Vk_CreateDeviceLocalImage(ResolveImageCreateInfo);
        ResolvedMsaa.ImageView = vk::raii::ImageView{Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
            .image = ResolvedMsaa.Image.GetBuffer(),
            .viewType = vk::ImageViewType::e2D,
            .format = Frontend.Vk_GetSurfaceFormat().format,
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
                },
                },};
    }

    return;
}

void Jafg::LRenderTarget::Render(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const
{
    check(!!What)
    check(!!this->MsaaTarget.Image.GetBuffer())
    check(this->Extent.width > 0 && this->Extent.height > 0)

    auto& Surface{Info.Surface};

    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = {},
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = this->MsaaTarget.Image.GetBuffer(),
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });

    vk::RenderingAttachmentInfo ColorAttachmentInfo{
        .imageView = this->MsaaTarget.ImageView,
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .resolveMode = this->ResolveFlags,
        .resolveImageView = this->IsResolvingMsaa() ? *this->ResolvedTarget->ImageView : nullptr,
        .resolveImageLayout = this->IsResolvingMsaa() ? vk::ImageLayout::eColorAttachmentOptimal : vk::ImageLayout::eUndefined,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = ClearColor,
        };

    vk::RenderingInfo RenderingInfo{
        .renderArea = {.offset={0, 0}, .extent=*this->Extent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &ColorAttachmentInfo,
        .pDepthAttachment = nullptr,
        };

    Info.CommandBuffer.beginRendering(RenderingInfo);
    Info.CommandBuffer.setViewport(0, vk::Viewport{
        .x = 0.0f, .y = 0.0f,
        .width=static_cast<f32>(this->Extent.width), .height=static_cast<f32>(this->Extent.height),
        .minDepth = 0.0f, .maxDepth = 1.0f
        });
    Info.CommandBuffer.setScissor(0, vk::Rect2D{.offset={0, 0}, .extent=*this->Extent,});
    What(Info);
    Info.CommandBuffer.endRendering();

    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader,
        .dstAccessMask = vk::AccessFlagBits2::eShaderRead,
        .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = *this->MsaaTarget.Image,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });

    return;
}
