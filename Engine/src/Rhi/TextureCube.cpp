// Copyright mzoesch. All rights reserved.

#include "Rhi/TextureCube.h"
#include "Engine/Engine.h"
#include "Framework/TextureSubsystem.h"

#include "Definitions/PushNoWarnings.h"
    #define STBI_ONLY_PNG
    #include <stb_image.h>
#include "Definitions/PopDiagnostics.h"

void Jafg::LTextureCube2::LoadToDevice()
{
#if !JAFG_IN_SHIPPING
    LOG_TRACE(LogRhi, "[{}]: Loading TextureCube to device.", this->GetDebugName())
#endif /* !JAFG_IN_SHIPPING */
    auto& Frontend{GEngine->GetLocalEgo().GetFrontend()};

    if (this->IsOnDevice())
    {
#if !JAFG_IN_SHIPPING
        LOG_TRACE(LogRhi, "[{}]: TextureCube already loaded to device memory. Reloading.", this->GetDebugName())
#endif /* !JAFG_IN_SHIPPING */
        this->FreeFromDevice();
    }

    /*
     * TODO: This is jut not right. We flip the x-axis in the shader...
     *       But how cares rn. Is this even relevant??
     */
    if (std::holds_alternative<CubeMap>(this->Source))
    {
        auto Path{std::move(std::get<CubeMap>(this->Source).Path)};
        this->Source = Custom{{
            Path/"PX.png", // Actually NX
            Path/"NX.png", // Actually PX...
            Path/"PY.png",
            Path/"NY.png",
            Path/"PZ.png",
            Path/"NZ.png"
            }};
    }
    auto& Custom{std::get<LTextureCube2::Custom>(this->Source).Paths};

    u32 MipLevels{1};
    vk::Format Format{vk::Format::eR8G8B8A8Srgb};
    vk::SampleCountFlagBits Samples{vk::SampleCountFlagBits::e1};

    std::array<LTexture2,6> Textures{
        LTexture2{Custom[0], {.Format=Format}, {.DesiredMipLevels=MipLevels,.Samples=Samples}, ETexture2StateBits::Host},
        LTexture2{Custom[1], {.Format=Format}, {.DesiredMipLevels=MipLevels,.Samples=Samples}, ETexture2StateBits::Host},
        LTexture2{Custom[2], {.Format=Format}, {.DesiredMipLevels=MipLevels,.Samples=Samples}, ETexture2StateBits::Host},
        LTexture2{Custom[3], {.Format=Format}, {.DesiredMipLevels=MipLevels,.Samples=Samples}, ETexture2StateBits::Host},
        LTexture2{Custom[4], {.Format=Format}, {.DesiredMipLevels=MipLevels,.Samples=Samples}, ETexture2StateBits::Host},
        LTexture2{Custom[5], {.Format=Format}, {.DesiredMipLevels=MipLevels,.Samples=Samples}, ETexture2StateBits::Host},
        };
    auto Extent{Textures[0].GetExtent()};
    checkCode
    (
        for (auto const& Tex: Textures)
        {
            check(Tex.GetExtent() == Extent)
            check(Tex.GetFormat() == Format)
            check(Tex.GetSamplesPerTexel() == Samples)
        }
    )
    auto N{rhi::vk_bytes_per_pixel(Format) * Extent.width * Extent.height};
    std::array<rhi::mapped_device_buffer,6> StagingBuffers{
        Frontend.Vk_CreateMappedBuffer({.size=N,.usage=vk::BufferUsageFlagBits::eTransferSrc}),
        Frontend.Vk_CreateMappedBuffer({.size=N,.usage=vk::BufferUsageFlagBits::eTransferSrc}),
        Frontend.Vk_CreateMappedBuffer({.size=N,.usage=vk::BufferUsageFlagBits::eTransferSrc}),
        Frontend.Vk_CreateMappedBuffer({.size=N,.usage=vk::BufferUsageFlagBits::eTransferSrc}),
        Frontend.Vk_CreateMappedBuffer({.size=N,.usage=vk::BufferUsageFlagBits::eTransferSrc}),
        Frontend.Vk_CreateMappedBuffer({.size=N,.usage=vk::BufferUsageFlagBits::eTransferSrc}),
        };
    for (auto Layer{0uz}; Layer < StagingBuffers.size(); ++Layer)
    {
        std::memcpy(StagingBuffers[Layer].data(), Textures[Layer].GetFirstMipMap().data(), N);
    }

    this->Handle = Frontend.Vk_CreateDeviceLocalImage({
        .flags = vk::ImageCreateFlagBits::eCubeCompatible,
        .imageType = vk::ImageType::e2D,
        .format = Format,
        .extent = vk::Extent3D{Extent.width, Extent.height, 1},
        .mipLevels = MipLevels,
        .arrayLayers = 6,
        .samples = Samples,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
        });
    Frontend.Vk_TransitionImageLayout({
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eTransferDstOptimal,
        .image = *Handle,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = MipLevels,
            .baseArrayLayer = 0,
            .layerCount = 6,
            },
        });

    {
        auto CommandBuffer{Frontend.Vk_BeginSingleTimeCommands()};
        for (auto Layer{0uz}; Layer < 6; ++Layer)
        {
            vk::BufferImageCopy Region{
                .bufferOffset = 0,
                .bufferRowLength = 0,
                .bufferImageHeight = 0,
                .imageSubresource = {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = 0,
                    .baseArrayLayer = static_cast<u32>(Layer),
                    .layerCount = 1
                    },
                .imageOffset = {0, 0, 0},
                .imageExtent = {Extent.width, Extent.height, 1},
                };
            CommandBuffer.copyBufferToImage(*StagingBuffers[Layer], *this->Handle, vk::ImageLayout::eTransferDstOptimal, Region);
        }
        Frontend.Vk_EndSingleTimeCommands(std::move(CommandBuffer));
    }

    Frontend.Vk_TransitionImageLayout({
        .oldLayout = vk::ImageLayout::eTransferDstOptimal,
        .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        .image = *this->Handle,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = MipLevels,
            .baseArrayLayer = 0,
            .layerCount = 6
            },
        });

    this->View = rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
        .image = *this->Handle,
        .viewType = vk::ImageViewType::eCube,
        .format = Format,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = MipLevels,
            .baseArrayLayer = 0,
            .layerCount = 6
            },
        });
}
