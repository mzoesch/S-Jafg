// Copyright mzoesch. All rights reserved.

#include "Rhi/RenderTarget.h"
#include "Rhi/NodeRenderInfo.h"
#include "Rhi/AmbientOcclusion.h"
#include "Framework/Frontend.h"

namespace
{

Jafg::LPushConstantRegistrator<Jafg::PC::Ssao> _0;
Jafg::LPushConstantRegistrator<Jafg::PC::SsaoBlur> _1;

} /* ~Namespace <Anonymous> */

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
    this->SceneDepthTarget = {};
    this->SceneDepthBufferTarget = {};
    this->AmbientOcclusionTarget = {};
    this->AmbientOcclusionBlurTarget = {};
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
        this->DepthTarget.ImageView = rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
            .image = *this->DepthTarget.Image,
            .viewType = vk::ImageViewType::e2D,
            .format = Frontend.Vk_GetPreferredDepthFormat(),
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eDepth,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
                },
            });
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
    this->MsaaTarget.ImageView = rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
        .image = *this->MsaaTarget.Image,
        .viewType = vk::ImageViewType::e2D,
        .format = Frontend.Vk_GetSurfaceFormat().format,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });

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
        ResolvedMsaa.ImageView = rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
            .image = *ResolvedMsaa.Image,
            .viewType = vk::ImageViewType::e2D,
            .format = Frontend.Vk_GetSurfaceFormat().format,
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
                },
            });
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
        this->SelectedTarget.ImageView = rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
            .image = *this->SelectedTarget.Image,
            .viewType = vk::ImageViewType::e2D,
            .format = vk::Format::eR8Unorm,
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
                },
            });
    }

    if (Info.bAmbientOcclusion)
    {
        auto MakeTarget{[&](vk::Format Format, vk::ImageUsageFlags Usage, vk::ImageAspectFlags Aspect)
        {
            Target Result;
            Result.Image = Frontend.Vk_CreateDeviceLocalImage({
                .imageType = vk::ImageType::e2D,
                .format = Format,
                .extent = vk::Extent3D{this->Extent.width, this->Extent.height, 1},
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = vk::SampleCountFlagBits::e1,
                .tiling = vk::ImageTiling::eOptimal,
                .usage = Usage,
                .sharingMode = vk::SharingMode::eExclusive,
                .initialLayout = vk::ImageLayout::eUndefined,
                });
            Result.ImageView = rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
                .image = *Result.Image,
                .viewType = vk::ImageViewType::e2D,
                .format = Format,
                .subresourceRange = {
                    .aspectMask = Aspect,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                    },
                });
            return Result;
        }};

        constexpr vk::ImageUsageFlags SampledColor{vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled};
        this->SceneDepthTarget = MakeTarget(SceneDepthFormat, SampledColor, vk::ImageAspectFlagBits::eColor);
        this->SceneDepthBufferTarget = MakeTarget(Frontend.Vk_GetPreferredDepthFormat()
            , vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);
        this->AmbientOcclusionTarget = MakeTarget(AmbientOcclusionFormat, SampledColor, vk::ImageAspectFlagBits::eColor);
        this->AmbientOcclusionBlurTarget = MakeTarget(AmbientOcclusionFormat, SampledColor, vk::ImageAspectFlagBits::eColor);
    }
}

void Jafg::LRenderTarget::RenderSceneDepth(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const
{
    check(this->HasAmbientOcclusion())
    this->RenderAuxiliary(Info, this->SceneDepthTarget, &this->SceneDepthBufferTarget
        , vk::ClearValue{vk::ClearColorValue{std::array<f32,4>{BackgroundSceneDepth, 0.0f, 0.0f, 0.0f}}}, std::move(What));
}

void Jafg::LRenderTarget::RenderAmbientOcclusion(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const
{
    check(this->HasAmbientOcclusion())
    this->RenderAuxiliary(Info, this->AmbientOcclusionTarget, nullptr
        , vk::ClearValue{vk::ClearColorValue{std::array<f32,4>{1.0f, 1.0f, 1.0f, 1.0f}}}, std::move(What));
}

void Jafg::LRenderTarget::RenderAmbientOcclusionBlur(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const
{
    check(this->HasAmbientOcclusion())
    this->RenderAuxiliary(Info, this->AmbientOcclusionBlurTarget, nullptr
        , vk::ClearValue{vk::ClearColorValue{std::array<f32,4>{1.0f, 1.0f, 1.0f, 1.0f}}}, std::move(What));
}

void Jafg::LRenderTarget::RenderAuxiliary(LRenderInfo const& Info, Target const& Color, Target const* Depth
    , vk::ClearValue const& Clear, TFunction2<void(LRenderInfo const& Info)> What) const
{
    check(!!What)
    check(!!*Color.Image)
    check(!Depth || !!*Depth->Image)
    check(this->Extent.width > 0 && this->Extent.height > 0)

    LRenderInfo RenderInfo{
        .UserPreferences = Info.UserPreferences,
        .Frontend = Info.Frontend,
        .Surface = Info.Surface,
        .TextureSubsystem = Info.TextureSubsystem,
        .MaterialSubsystem = Info.MaterialSubsystem,
        .ShaderSubsystem = Info.ShaderSubsystem,
        .MeshSubsystem = Info.MeshSubsystem,
        .FontSubsystem = Info.FontSubsystem,
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

    constexpr vk::ImageSubresourceRange ColorRange{
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
        };

    //# The previous content is never needed. So we can always come from an undefined layout.
    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = {},
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = *Color.Image,
        .subresourceRange = ColorRange,
        });
    if (Depth)
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
            .image = *Depth->Image,
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
        .imageView = Color.ImageView,
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .resolveMode = vk::ResolveModeFlagBits::eNone,
        .resolveImageView = nullptr,
        .resolveImageLayout = vk::ImageLayout::eUndefined,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = Clear,
        };
    constexpr vk::ClearValue ClearDepth{.depthStencil = vk::ClearDepthStencilValue{.depth = 1.0f, .stencil = 0}};
    vk::RenderingAttachmentInfo DepthAttachmentInfo{
        .imageView   = Depth ? *Depth->ImageView : nullptr,
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
        .pDepthAttachment = Depth ? &DepthAttachmentInfo : nullptr,
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
        .image = *Color.Image,
        .subresourceRange = ColorRange,
        });
}

void Jafg::LRenderTarget::RenderSelected(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const
{
    check(!!What)
    check(!!*this->SelectedTarget.Image)
    check(this->Extent.width > 0 && this->Extent.height > 0)

    LRenderInfo RenderInfo{
        .UserPreferences = Info.UserPreferences,
        .Frontend = Info.Frontend,
        .Surface = Info.Surface,
        .TextureSubsystem = Info.TextureSubsystem,
        .MaterialSubsystem = Info.MaterialSubsystem,
        .ShaderSubsystem = Info.ShaderSubsystem,
        .MeshSubsystem = Info.MeshSubsystem,
        .FontSubsystem = Info.FontSubsystem,
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
        .image = *this->SelectedTarget.Image,
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
    check(!!*this->MsaaTarget.Image)
    check(this->Extent.width > 0 && this->Extent.height > 0)

    LRenderInfo RenderInfo{
        .UserPreferences = Info.UserPreferences,
        .Frontend = Info.Frontend,
        .Surface = Info.Surface,
        .TextureSubsystem = Info.TextureSubsystem,
        .MaterialSubsystem = Info.MaterialSubsystem,
        .ShaderSubsystem = Info.ShaderSubsystem,
        .MeshSubsystem = Info.MeshSubsystem,
        .FontSubsystem = Info.FontSubsystem,
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
        .image = *this->MsaaTarget.Image,
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
            .image = *this->DepthTarget.Image,
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

    /*
     * TODO: Allow an option to discard the alpha channel, e.g. by rendering a full-sized quad setting the 4th channel
     *       to one - preserving the rgb-ones. But this is expensive as it requires an additional new render pass.
     *       There has to be better solutions...
     */

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
}
