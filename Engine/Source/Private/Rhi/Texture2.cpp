// Copyright mzoesch. All rights reserved.

#include "Rhi/Texture2.h"
#include "Engine/Engine.h"
#include "Framework/TextureSubsystem.h"

#define STB_IMAGE_IMPLEMENTATION
// #define STBI_NO_JPEG
// #define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
//        STBI_ONLY_JPEG
//        STBI_ONLY_PNG
//        STBI_ONLY_BMP
//        STBI_ONLY_PSD
//        STBI_ONLY_TGA
//        STBI_ONLY_GIF
//        STBI_ONLY_HDR
//        STBI_ONLY_PIC
//        STBI_ONLY_PNM   (.ppm and .pgm)
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Jafg::TSharedRef<Jafg::LTexture2> Jafg::LTexture2::FromMemory(LStringView HumanReadableName, LByteBulkData&& Data, vk::Format SrcFormat, rhi::extent2 Extent, HostInfo Info)
{
    check(HumanReadableName.empty() == false)

    auto Result{std::make_shared<LTexture2>()};
    auto& Tex{*Result};
    check(Tex.IsOnHost() == false && Tex.IsOnDevice() == false)
    Tex.Path = LPath{"@"}; Tex.Path.concat(HumanReadableName);

    Tex.Meta.Format = Info.Format;
    Tex.Meta.Extent = Extent;

    if (SrcFormat == Info.Format)
    {
        Tex.MipMap0 = std::move(Data);
    }
    else if (SrcFormat == vk::Format::eR8G8B8Unorm && Info.Format == vk::Format::eR8G8B8A8Srgb)
    {
        Tex.MipMap0.Allocate(Vk_GetBytesPerPixel(Info.Format) * Tex.GetWidth() * Tex.GetHeight());
        for (u32 i{0}; i < Tex.GetWidth() * Tex.GetHeight(); ++i)
        {
            Tex.MipMap0[i * 4 + 0] = Data[i * 3 + 0];
            Tex.MipMap0[i * 4 + 1] = Data[i * 3 + 1];
            Tex.MipMap0[i * 4 + 2] = Data[i * 3 + 2];
            Tex.MipMap0[i * 4 + 3] = std::numeric_limits<u8>::max();
        }
    }
    else
    {
        LOG_FATAL(LogTextureSubsystem, "[{}]: Unsupported format conversion from [{}] to [{}] when loading texture from memory."
            , HumanReadableName, vk::to_string(SrcFormat), vk::to_string(Info.Format)
            )
    }
    check(Tex.MipMap0.IsAllocated())

    return Result;
}

Jafg::TSharedRef<Jafg::LTexture2> Jafg::LTexture2::FromTextureView(LStringView View)
{
    if (GMutableEngine)
    {
        return GMutableEngine->GetLocalEgo().GetFrontend().GetSubsystemChecked<JTextureSubsystem>()
            ->FromTextureViewIdentifier(View);
    }
    LOG_FATAL(LogRhi, "GMutableEngine is invalid.")
}

Jafg::LTexture2::EResult Jafg::LTexture2::LoadToHost(HostInfo const& Info)
{
    check(this->Path.empty() == false)
    LOG_VERBOSE(LogRhi, "Loading texture2 from path [{}].", this->Path)

    if (Finder::DoesFileExist(this->Path) == false)
    {
        return EResult::FileNotFound;
    }

    if (this->IsOnHost())
    {
        LOG_VERBOSE(LogRhi, "Texture2 [{}] already loaded to host memory. Freeing previous host memory and reloading.", this->Path)
        this->FreeFromHost();
    }
    check(this->Handle.GetAllocation() == nullptr)

    this->Meta.Format = Info.Format;

    auto Bin{ Finder::ReadFileAsBinary(this->Path) };
    LVec2i32 StbiExtent;
    i32 NrChannels;
    ::stbi_set_flip_vertically_on_load(false);
    u8* Data{::stbi_load_from_memory(
          Bin.data(), static_cast<int>(Bin.size())
        , &StbiExtent.x, &StbiExtent.y, &NrChannels
        , static_cast<i32>(Vk_GetChannelsPerPixel(this->Meta.Format))
        )};
    if (stbi_failure_reason())
    {
        LOG_FATAL(LogRhi, "Failed to load texture2 [{}] from disk. Reason: [{}].", this->Path, stbi_failure_reason())
    }
    check(Data && StbiExtent.x > 0 && StbiExtent.y > 0)

    if constexpr (IS_COMPILED_LOG(LogRhi, Verbose))
    if (NrChannels != static_cast<i32>(Vk_GetChannelsPerPixel(Meta.Format)))
    {
        LOG_TRACE(LogRhi,
            "Potential misuse: Texture2 [{}] was loaded from disk with [{}] channels to memory with [{}] channels.",
            this->Path, NrChannels, Vk_GetChannelsPerPixel(Meta.Format)
            )
    }

    this->Meta.Extent = rhi::extent2{static_cast<rhi::extent2::domain_type>(StbiExtent.x), static_cast<rhi::extent2::domain_type>(StbiExtent.y)};
    this->MipMap0.Serialize(Data, static_cast<std::size_t>(StbiExtent.x * StbiExtent.y) * this->GetBytesPerPixel());

    ::stbi_image_free(Data);

    return EResult::Success;
}

// void Jafg::LTexture2::AllocateEmpty(LTexture2Extent const& Extent, bool bZeroed)
// {
//     this->Extent = Extent;
//
//     if (bZeroed)
//     {
//         this->MipMap0.AllocateZeroed(this->Extent.Width * this->Extent.Height * this->GetBytesPerPixel());
//     }
//     else
//     {
//         this->MipMap0.Allocate(this->Extent.Width * this->Extent.Height * this->GetBytesPerPixel());
//     }
//
//     return;
// }

void Jafg::LTexture2::LoadToDevice(DeviceInfo const& Info)
{
    check(this->IsOnHost())
    LOG_TRACE(LogRhi, "Loading texture2 [{}] to device.", this->Path)
    auto& Frontend{GEngine->GetLocalEgo().GetFrontend()};

    if (this->IsOnDevice())
    {
        LOG_TRACE(LogRhi, "Texture2 [{}] already loaded to device memory. Freeing previous device memory and reloading.", this->Path)
        this->FreeFromDevice();
    }

    if (Info.DesiredMipLevels.has_value())
    {
        this->Meta.MipLevels = Info.DesiredMipLevels.value();
    }
    else if (this->Meta.MipLevels == std::numeric_limits<decltype(this->Meta.MipLevels)>::max())
    {
        this->Meta.MipLevels = static_cast<u32>(std::floor(std::log2(std::max(this->GetWidth(), this->GetHeight())))) + 1;
    }

    // TODO: This should be more like. GetPreferredMaxMsaaSamples (based of user preferences).
    this->Meta.Samples = Frontend.Vk_GetMaxMsaaSampleCount();
    if (Info.Samples.has_value())
    {
        if (auto PreferredSampleCount{rhi::vk_get_max_msaa_sample(Info.Samples.value())}; PreferredSampleCount > Frontend.Vk_GetMaxMsaaSampleCount())
        {
            LOG_WARNING(LogRhi, "Requested MSAA samples [{}] for texture2 [{}] exceeds device capabilities. Clamping to maximum supported samples [{}].",
                        vk::to_string(*Info.Samples), this->Path, vk::to_string(Frontend.Vk_GetMaxMsaaSampleCount()))
            this->Meta.Samples = Frontend.Vk_GetMaxMsaaSampleCount();
        }
        else
        {
            this->Meta.Samples = PreferredSampleCount;
        }
    }

    checkCode
    (
        if (this->Meta.MipLevels > 1)
        {
            if (this->Meta.Samples != vk::SampleCountFlagBits::e1)
            {
                LOG_FATAL(LogRhi,
                    "[{}]: Texture2 has [{}] mipmaps and uses MSAA samples [{}] which is not VK_SAMPLE_COUNT_1_BIT.",
                    this->Path, this->Meta.MipLevels, vk::to_string(this->Meta.Samples)
                    )
            }
        }
    )

    this->Handle = Frontend.Vk_StageLinearImage({
        .Data = this->MipMap0.data(),
        .Info = {
            .flags ={},
            .imageType = vk::ImageType::e2D,
            .format = this->GetFormat(),
            .extent = vk::Extent3D{this->GetWidth(), this->GetHeight(), 1},
            .mipLevels = this->GetMipLevels(),
            .arrayLayers = 1,
            .samples = this->GetSamplesPerTexel(),
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            .sharingMode = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined,
            }
        });

    this->View = vk::raii::ImageView{Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
        .image = this->Handle.GetBuffer(),
        .viewType = vk::ImageViewType::e2D,
        .format = this->GetFormat(),
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0, // TODO: This should probably be an user option.
            .levelCount = this->GetMipLevels(),
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        }};

    return;
}
