// Copyright mzoesch. All rights reserved.

#include "Rhi/RenderTarget.h"
#include "Rhi/NodeRenderInfo.h"
#include "Framework/Frontend.h"

void Jafg::LRenderTarget::Initialize(CreateInfo const& Info)
{
    auto& Frontend{Info.Frontend};

    check(Info.Extent.width > 0 && Info.Extent.height > 0)

    if (Frontend.Vk_GetSurfaceFormat().format == vk::Format::eB8G8R8A8Srgb)
    {
        //# TODO: Why does this work? Why is this f32?
        this->ClearColor = {vk::ClearColorValue(std::array<f32,4>{
            Info.ClearColor.r, Info.ClearColor.g, Info.ClearColor.b, Info.ClearColor.a})};
    }
    else
    {
        LOG_FATAL(LogVulkan, "Render target does not support format [{}].", vk::to_string(Frontend.Vk_GetSurfaceFormat().format))
    }

    this->Extent = Info.Extent;
    this->ResolveFlags = Info.ResolveMode;

    this->DepthTarget = {};
    this->MsaaTarget = {};
    this->SelectedTarget = {};
    this->ResolvedTarget.reset();

    auto Samples{rhi::vk_clamp_msaa_samples(Info.SampleCount, Frontend.Vk_GetMaxMsaaSampleCount())};

    if (Info.bDepthTest)
    {
        this->DepthTarget.Image = Frontend.Vk_CreateDeviceLocalImage({
            .imageType = vk::ImageType::e2D,
            .format = Frontend.Vk_GetPreferredDepthFormat(),
            .extent = vk::Extent3D{this->Extent.width, this->Extent.height, 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = Samples,
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
            .sharingMode = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined,
            });
        this->DepthTarget.ImageView = vk::raii::ImageView{Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
            .image = this->DepthTarget.Image.GetBuffer(),
            .viewType = vk::ImageViewType::e2D,
            .format = Frontend.Vk_GetPreferredDepthFormat(),
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eDepth,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
                },
            }};
    }

    this->MsaaTarget.Image = Frontend.Vk_CreateDeviceLocalImage({
        .imageType = vk::ImageType::e2D,
        .format = Frontend.Vk_GetSurfaceFormat().format,
        .extent = vk::Extent3D{this->Extent.width, this->Extent.height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = Samples,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eColorAttachment
            | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
        .initialLayout = vk::ImageLayout::eUndefined,
        });
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
        this->ResolvedTarget.emplace(BindlessTarget{});
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

    if (Info.bAllowSelection)
    {
        this->SelectedTarget.Image = Frontend.Vk_CreateDeviceLocalImage({
            .imageType = vk::ImageType::e2D,
            .format = vk::Format::eR8Unorm,
            .extent = vk::Extent3D{this->Extent.width, this->Extent.height, 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = vk::SampleCountFlagBits::e1,
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
            .initialLayout = vk::ImageLayout::eUndefined,
            });
        this->SelectedTarget.ImageView = vk::raii::ImageView{Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
            .image = this->SelectedTarget.Image.GetBuffer(),
            .viewType = vk::ImageViewType::e2D,
            .format = vk::Format::eR8Unorm,
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

void Jafg::LRenderTarget::RenderSelected(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const
{
    check(!!What)
    check(!!this->SelectedTarget.Image.GetBuffer())
    check(this->Extent.width > 0 && this->Extent.height > 0)

    LRenderInfo RenderInfo{
        .UserPreferences = Info.UserPreferences,
        .Frontend = Info.Frontend,
        .Surface = Info.Surface,
        .CommandBuffer = Info.CommandBuffer,
        .DescriptorPool = Info.DescriptorPool,
        .Frame = Info.Frame,
        .Image = Info.Image,
        .VkViewport = vk::Viewport{
            .x = 0.0f, .y = 0.0f,
            .width=static_cast<f32>(this->Extent.width), .height=static_cast<f32>(this->Extent.height),
            .minDepth = 0.0f, .maxDepth = 1.0f
            },
        .VkScissor = vk::Rect2D{
            .offset = {0, 0},
            .extent = *this->Extent,
            },
        };

    auto& Surface{RenderInfo.Surface};

    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = {},
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = this->SelectedTarget.Image.GetBuffer(),
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });

    vk::RenderingAttachmentInfo ColorAttachmentInfo{
        .imageView = this->SelectedTarget.ImageView,
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .resolveMode = vk::ResolveModeFlagBits::eNone,
        .resolveImageView = nullptr,
        .resolveImageLayout = vk::ImageLayout::eUndefined,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = this->ClearColor,
        };

    RenderInfo.CommandBuffer.beginRendering({
        .renderArea = {.offset={0, 0}, .extent=*this->Extent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &ColorAttachmentInfo,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
        });
    RenderInfo.CommandBuffer.setViewport(0, RenderInfo.VkViewport);
    RenderInfo.CommandBuffer.setScissor(0, RenderInfo.VkScissor);
    What(RenderInfo);
    RenderInfo.CommandBuffer.endRendering();

    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader,
        .dstAccessMask = vk::AccessFlagBits2::eShaderRead,
        .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = *this->SelectedTarget.Image,
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

void Jafg::LRenderTarget::Render(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const
{
    check(!!What)
    check(!!this->MsaaTarget.Image.GetBuffer())
    check(this->Extent.width > 0 && this->Extent.height > 0)

    LRenderInfo RenderInfo{
        .UserPreferences = Info.UserPreferences,
        .Frontend = Info.Frontend,
        .Surface = Info.Surface,
        .CommandBuffer = Info.CommandBuffer,
        .DescriptorPool = Info.DescriptorPool,
        .Frame = Info.Frame,
        .Image = Info.Image,
        .VkViewport = vk::Viewport{
            .x = 0.0f, .y = 0.0f,
            .width=static_cast<f32>(this->Extent.width), .height=static_cast<f32>(this->Extent.height),
            .minDepth = 0.0f, .maxDepth = 1.0f
            },
        .VkScissor = vk::Rect2D{
            .offset = {0, 0},
            .extent = *this->Extent,
            },
        };

    auto& Surface{RenderInfo.Surface};

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

    if (this->IsDepthTested())
    {
        Surface.Vk_TransitionImageLayout({
            .srcStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
            .srcAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
            .dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
            .dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
            .oldLayout = vk::ImageLayout::eUndefined,
            .newLayout = vk::ImageLayout::eDepthAttachmentOptimal,
            .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
            .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
            .image = this->DepthTarget.Image.GetBuffer(),
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eDepth,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
                },
            });
    }

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
    constexpr vk::ClearValue ClearDepth{.depthStencil = vk::ClearDepthStencilValue{.depth = 1.0f, .stencil = 0}};
    vk::RenderingAttachmentInfo DepthAttachmentInfo{
        .imageView   = this->DepthTarget.ImageView,
        .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eDontCare,
        .clearValue  = ClearDepth
        };

    RenderInfo.CommandBuffer.beginRendering({
        .renderArea = {.offset={0, 0}, .extent=*this->Extent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &ColorAttachmentInfo,
        .pDepthAttachment = this->IsDepthTested() ? &DepthAttachmentInfo : nullptr,
        .pStencilAttachment = nullptr,
        });
    RenderInfo.CommandBuffer.setViewport(0, RenderInfo.VkViewport);
    RenderInfo.CommandBuffer.setScissor(0, RenderInfo.VkScissor);
    What(RenderInfo);
    RenderInfo.CommandBuffer.endRendering();

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
