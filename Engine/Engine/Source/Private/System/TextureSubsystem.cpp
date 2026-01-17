// Copyright mzoesch. All rights reserved.

#include "System/TextureSubsystem.h"
#include "Engine/Engine.h"

void Jafg::JTextureSubsystem::PurgeUnused() noexcept
{
    std::erase_if(this->Textures, [](auto& E) -> bool
    {
        if (E.second.unique())
        {
            LOG_VERBOSE(LogTextureSubsystem, "Purging unused texture [{}].", E.first)
            return true;
        }

        return false;
    });

    // TODO: Do we want to rehash??

    return;
}

std::shared_ptr<Jafg::LTexture2> Jafg::JTextureSubsystem::GetTexture(
      LPath const* Path
    , LString const& Ident
    , LTexture2::LMetadata Meta
    , ETextureLoadFlags Flags
    ) const
{
    check( Ident.empty() == false )
    check( static_cast<bool>(Flags & ETextureLoadFlagBits::Stage) ? static_cast<bool>(Flags & ETextureLoadFlagBits::Load) : true )

    if (auto const& It{this->Textures.find(Ident)}; It != this->Textures.end())
    {
        check( It->second->GetFormat() == Meta.Format )

        if (Flags & ETextureLoadFlagBits::Load && It->second->IsOnMainMemory() == false)
        {
            It->second->AllocateFromDisk(Flags);
        }
        else if (Flags & ETextureLoadFlagBits::Stage && It->second->IsOnDevice() == false)
        {
            It->second->StageToDevice();
        }

        return It->second;
    }

    if (Path == nullptr)
    {
        return {};
    }

    this->Textures[Ident] = std::make_unique<LTexture2>(*Path, Meta, Flags);
    auto& Reference{this->Textures[Ident]};
    return Reference;

}

void Jafg::LTexture2::AllocateFromDisk(ETextureLoadFlags Flags /* = ETextureLoadFlagBits::Default */)
{
    check( this->Meta.Format != vk::Format::eUndefined )

    auto Bin{ Finder::ReadFileAsBinary(this->Path) };

    Li32Vector2 Extent;
    i32 NrChannels;

    ::stbi_set_flip_vertically_on_load(static_cast<bool>(Flags & ETextureLoadFlagBits::FlipY));
    u8* Data{::stbi_load_from_memory(
          Bin.data()
        , static_cast<int>(Bin.size())
        , &Extent.X
        , &Extent.Y
        , &NrChannels
        , static_cast<i32>(Vk_GetChannelsPerPixel(this->Meta.Format))
        )};

    if (stbi_failure_reason())
    {
        LOG_FATAL(LogRhi, "Failed to load texture [{}] from disk. Reason: [{}].", this->Path, stbi_failure_reason())
    }
    check( Data && Extent.X > 0 && Extent.Y > 0 )

    if constexpr (IS_COMPILED_LOG(LogRhi, Verbose))
    {
        if (NrChannels != static_cast<i32>(Vk_GetChannelsPerPixel(Meta.Format)))
        {
            LOG_VERBOSE(LogRhi,
                "Potential misuse: Texture2 [{}] was loaded from disk with [{}] channels to memory with [{}] channels.",
                this->Path, NrChannels, Vk_GetChannelsPerPixel(Meta.Format)
                )
        }
    }

    this->Extent = LTextureExtent{static_cast<LTextureExtent::value_type>(Extent.X), static_cast<LTextureExtent::value_type>(Extent.Y)};
    this->MipMap0.Serialize(Data, static_cast<LSize>(Extent.X * Extent.Y) * this->GetBytesPerPixel());

    if (Flags & ETextureLoadFlagBits::Stage)
    {
        this->StageToDevice();
    }

    ::stbi_image_free(Data);

    return;
}

void Jafg::LTexture2::AllocateEmpty(LTextureExtent const& Extent, bool bZeroed)
{
    this->Extent = Extent;

    if (bZeroed)
    {
        this->MipMap0.AllocateZeroed(this->Extent.Width * this->Extent.Height * this->GetBytesPerPixel());
    }
    else
    {
        this->MipMap0.Allocate(this->Extent.Width * this->Extent.Height * this->GetBytesPerPixel());
    }

    return;
}

void Jafg::LTexture2::StageToDevice()
{
    check( this->MipMap0.IsAllocated() )

    auto& Frontend{GEngine->GetLocalEgo().GetFrontend()};

    if (this->Meta.DesiredMipLevels.has_value() == false)
    {
        this->Meta.DesiredMipLevels = static_cast<u32>(std::floor(std::log2(std::max(this->GetWidth(), this->GetHeight())))) + 1;
    }

    this->Handle = Frontend.Vk_StageLinearImage({
        .Data = this->MipMap0.data(),
        .Info = {
            .flags ={},
            .imageType = vk::ImageType::e2D,
            .format = this->GetFormat(),
            .extent = vk::Extent3D{this->GetWidth(), this->GetHeight(), 1},
            .mipLevels = this->GetDesiredMipLevels(),
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
            .levelCount = this->GetDesiredMipLevels(),
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        }};

    return;
}
