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
    this->SceneColorTarget = {};
    this->SceneColorMipLevels = 0;
    this->ThicknessTarget = {};
    this->ThicknessDepthBufferTarget = {};
    this->ResolvedTarget.reset();

    auto Samples{rhi::vk_clamp_msaa_samples(Info.SampleCount, Frontend.Vk_GetMaxMsaaSampleCount())};
    this->Samples = Samples;

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
                | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst
                | vk::ImageUsageFlagBits::eTransferSrc,
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

    if (Info.bTranslucency)
    {
        /* The full chain down to a single texel. The mips are the blur of rough transmission. */
        this->SceneColorMipLevels = 1u + static_cast<u32>(std::floor(std::log2(static_cast<f64>(std::max(this->Extent.width, this->Extent.height)))));
        this->SceneColorTarget.Image = Frontend.Vk_CreateDeviceLocalImage({
            .imageType = vk::ImageType::e2D,
            .format = Frontend.Vk_GetSurfaceFormat().format,
            .extent = vk::Extent3D{this->Extent.width, this->Extent.height, 1},
            .mipLevels = this->SceneColorMipLevels,
            .arrayLayers = 1,
            .samples = vk::SampleCountFlagBits::e1,
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst,
            .sharingMode = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined,
            });
        this->SceneColorTarget.ImageView = rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
            .image = *this->SceneColorTarget.Image,
            .viewType = vk::ImageViewType::e2D,
            .format = Frontend.Vk_GetSurfaceFormat().format,
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = this->SceneColorMipLevels,
                .baseArrayLayer = 0,
                .layerCount = 1,
                },
            });

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
        this->ThicknessTarget = MakeTarget(SceneDepthFormat
            , vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlagBits::eColor);
        this->ThicknessDepthBufferTarget = MakeTarget(Frontend.Vk_GetPreferredDepthFormat()
            , vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);
    }
}

void Jafg::LRenderTarget::RenderThickness(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const
{
    check(this->HasTranslucency())
    check(!this->Running.bRendering)
    this->RenderAuxiliary(Info, this->ThicknessTarget, &this->ThicknessDepthBufferTarget
        , vk::ClearValue{vk::ClearColorValue{std::array<f32,4>{0.0f, 0.0f, 0.0f, 0.0f}}}, std::move(What));
}

void Jafg::LRenderTarget::CopySceneColor(LRenderInfo const& Info) const
{
    check(this->HasTranslucency())
    check(!this->Running.bRendering)

    auto& Surface{Info.Surface};
    auto const& Command{Info.CommandBuffer};

    auto Range{[](u32 BaseMip, u32 Mips) -> vk::ImageSubresourceRange
    {
        return {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = BaseMip,
            .levelCount = Mips,
            .baseArrayLayer = 0,
            .layerCount = 1,
            };
    }};
    auto Layers{[](u32 Mip) -> vk::ImageSubresourceLayers
    {
        return {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .mipLevel = Mip,
            .baseArrayLayer = 0,
            .layerCount = 1,
            };
    }};

    /* Translucent draws of the last frame might still sample the copy. Its previous content is never needed. */
    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eFragmentShader,
        .srcAccessMask = {},
        .dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
        .dstAccessMask = vk::AccessFlagBits2::eTransferWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eTransferDstOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = *this->SceneColorTarget.Image,
        .subresourceRange = Range(0, this->SceneColorMipLevels),
        });

    /* What has been rendered so far, single sampled. The resolve happened when the rendering ended. */
    bool const bFromResolved{this->IsResolvingMsaa()};
    vk::Image const Source{bFromResolved ? *this->ResolvedTarget->Image : *this->MsaaTarget.Image};
    /* Both are attachments of the rendering, the resolved one as its resolve attachment. */
    constexpr vk::ImageLayout SourceLayout{vk::ImageLayout::eColorAttachmentOptimal};
    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
        .dstAccessMask = vk::AccessFlagBits2::eTransferRead,
        .oldLayout = SourceLayout,
        .newLayout = vk::ImageLayout::eTransferSrcOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = Source,
        .subresourceRange = Range(0, 1),
        });

    vk::Extent3D const Extent{this->Extent.width, this->Extent.height, 1};
    if (bFromResolved || this->Samples == vk::SampleCountFlagBits::e1)
    {
        Command.copyImage(Source, vk::ImageLayout::eTransferSrcOptimal
            , *this->SceneColorTarget.Image, vk::ImageLayout::eTransferDstOptimal
            , vk::ImageCopy{.srcSubresource = Layers(0), .srcOffset = {}, .dstSubresource = Layers(0), .dstOffset = {}, .extent = Extent});
    }
    else
    {
        Command.resolveImage(Source, vk::ImageLayout::eTransferSrcOptimal
            , *this->SceneColorTarget.Image, vk::ImageLayout::eTransferDstOptimal
            , vk::ImageResolve{.srcSubresource = Layers(0), .srcOffset = {}, .dstSubresource = Layers(0), .dstOffset = {}, .extent = Extent});
    }

    /* The rendering continues into the source afterward. */
    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eTransfer,
        .srcAccessMask = {},
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eTransferSrcOptimal,
        .newLayout = SourceLayout,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = Source,
        .subresourceRange = Range(0, 1),
        });

    /* Every mip is the blurred (box filtered) previous one. */
    i32 Width{static_cast<i32>(this->Extent.width)};
    i32 Height{static_cast<i32>(this->Extent.height)};
    for (u32 Mip{1}; Mip < this->SceneColorMipLevels; ++Mip)
    {
        Surface.Vk_TransitionImageLayout({
            .srcStageMask = vk::PipelineStageFlagBits2::eTransfer,
            .srcAccessMask = vk::AccessFlagBits2::eTransferWrite,
            .dstStageMask = vk::PipelineStageFlagBits2::eTransfer,
            .dstAccessMask = vk::AccessFlagBits2::eTransferRead,
            .oldLayout = vk::ImageLayout::eTransferDstOptimal,
            .newLayout = vk::ImageLayout::eTransferSrcOptimal,
            .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
            .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
            .image = *this->SceneColorTarget.Image,
            .subresourceRange = Range(Mip - 1, 1),
            });

        i32 const NextWidth{std::max(Width / 2, 1)};
        i32 const NextHeight{std::max(Height / 2, 1)};
        Command.blitImage(*this->SceneColorTarget.Image, vk::ImageLayout::eTransferSrcOptimal
            , *this->SceneColorTarget.Image, vk::ImageLayout::eTransferDstOptimal
            , vk::ImageBlit{
                .srcSubresource = Layers(Mip - 1),
                .srcOffsets = std::array{vk::Offset3D{0, 0, 0}, vk::Offset3D{Width, Height, 1}},
                .dstSubresource = Layers(Mip),
                .dstOffsets = std::array{vk::Offset3D{0, 0, 0}, vk::Offset3D{NextWidth, NextHeight, 1}},
                }
            , vk::Filter::eLinear);
        Width = NextWidth;
        Height = NextHeight;
    }

    /* All but the last mip are sources by now. */
    if (this->SceneColorMipLevels > 1)
    {
        Surface.Vk_TransitionImageLayout({
            .srcStageMask = vk::PipelineStageFlagBits2::eTransfer,
            .srcAccessMask = {},
            .dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader,
            .dstAccessMask = vk::AccessFlagBits2::eShaderRead,
            .oldLayout = vk::ImageLayout::eTransferSrcOptimal,
            .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
            .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
            .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
            .image = *this->SceneColorTarget.Image,
            .subresourceRange = Range(0, this->SceneColorMipLevels - 1),
            });
    }
    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eTransfer,
        .srcAccessMask = vk::AccessFlagBits2::eTransferWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader,
        .dstAccessMask = vk::AccessFlagBits2::eShaderRead,
        .oldLayout = vk::ImageLayout::eTransferDstOptimal,
        .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = *this->SceneColorTarget.Image,
        .subresourceRange = Range(this->SceneColorMipLevels - 1, 1),
        });
}

void Jafg::LRenderTarget::InterruptRendering(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> Between) const
{
    check(this->Running.bRendering)
    check(!!Between)

    auto& Surface{Info.Surface};

    Info.CommandBuffer.endRendering();
    this->Running.bRendering = false;

    Between(Info);

    /* The resolve at the end of the continued rendering writes the resolved image again. */
    if (this->IsResolvingMsaa())
    {
        Surface.Vk_TransitionImageLayout({
            .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
            .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
            .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
            .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
            .image = *this->ResolvedTarget->Image,
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
                },
            });
    }

    /* What has been rendered so far is loaded again. Its writes have to be visible to the loads. */
    Surface.Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
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
            .dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
            .oldLayout = vk::ImageLayout::eDepthAttachmentOptimal,
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

    vk::RenderingAttachmentInfo Color{this->Running.Color};
    Color.loadOp = vk::AttachmentLoadOp::eLoad;
    vk::RenderingAttachmentInfo Depth{this->Running.Depth};
    Depth.loadOp = vk::AttachmentLoadOp::eLoad;

    Info.CommandBuffer.beginRendering({
        .renderArea = {.offset={0, 0}, .extent=*this->Extent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &Color,
        .pDepthAttachment = this->IsDepthTested() ? &Depth : nullptr,
        .pStencilAttachment = nullptr,
        });
    this->Running.bRendering = true;
    Info.CommandBuffer.setViewport(0, vk::Viewport{
        .x = 0.0f, .y = 0.0f,
        .width=static_cast<f32>(this->Extent.width), .height=static_cast<f32>(this->Extent.height),
        .minDepth = 0.0f, .maxDepth = 1.0f
        });
    Info.CommandBuffer.setScissor(0, vk::Rect2D{.offset = {0, 0}, .extent = *this->Extent});
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

    /* The resolve overwrites all of the resolved image. Its previous content (it is sampled afterward) is never needed. */
    if (this->IsResolvingMsaa())
    {
        Surface.Vk_TransitionImageLayout({
            .srcStageMask = vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .srcAccessMask = {},
            .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
            .oldLayout = vk::ImageLayout::eUndefined,
            .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
            .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
            .image = *this->ResolvedTarget->Image,
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
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
        /* Kept for when the rendering is interrupted (@see #InterruptRendering). */
        .storeOp     = this->HasTranslucency() ? vk::AttachmentStoreOp::eStore : vk::AttachmentStoreOp::eDontCare,
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
    this->Running = {.bRendering = true, .Color = ColorAttachmentInfo, .Depth = DepthAttachmentInfo};
    What(RenderInfo);
    check(this->Running.bRendering)
    this->Running.bRendering = false;

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
