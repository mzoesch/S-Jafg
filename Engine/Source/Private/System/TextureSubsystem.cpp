// Copyright mzoesch. All rights reserved.

#include "System/TextureSubsystem.h"
#include "Engine/Engine.h"

void Jafg::JTextureSubsystem::PurgeUnused() noexcept
{
    std::erase_if(this->Textures, [](auto& E) -> bool
    {
        if (E.second.use_count() == 1)
        {
            LOG_VERBOSE(LogTextureSubsystem, "Purging unused texture [{}].", E.first)
            return true;
        }

        return false;
    });

    // TODO: Do we want to rehash??

    return;
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromFile(LPath const& Path
    , LTexture2::HostInfo HostCreateInfo
    , LTexture2::DeviceInfo DeviceCreateInfo
    , ETexture2State State /* = ETexture2StateBits::Device */
    ) const
{
    if (auto const& It{this->Textures.find(Path)}; It != this->Textures.end())
    {
        LTexture2& Texture{*It->second};

        check(Texture.GetFormat() == vk::Format::eUndefined || Texture.GetFormat() == HostCreateInfo.Format)
        if (State & ETexture2StateBits::Host || ((State & ETexture2StateBits::Device) && Texture.IsOnDevice() == false))
        {
            if (Texture.IsOnHost() == false)
            {
                auto Result{Texture.LoadToHost(HostCreateInfo)};
                jassert(Result == LTexture2::EResult::Success)
            }
        }
        if (State & ETexture2StateBits::Device)
        {
            if (Texture.IsOnDevice() == false)
            {
                Texture.LoadToDevice(DeviceCreateInfo);
                if ((State & ETexture2StateBits::Host) == ETexture2StateBits::None)
                {
                    Texture.FreeFromHost();
                }
            }
        }

        return LTexture2Ref{It->second};
    }

    this->Textures[Path] = std::make_shared<LTexture2>(Path, HostCreateInfo, DeviceCreateInfo, State);
    return LTexture2Ref{this->Textures[Path]};
}
