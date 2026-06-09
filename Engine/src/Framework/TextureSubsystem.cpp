// Copyright mzoesch. All rights reserved.

#include "Framework/TextureSubsystem.h"
#include "Rhi/TextureView.h"
#include "Engine/Engine.h"
#include "Framework/AssetDiscoverer.h"
#include "Stats/Stats.h"
#include "Rhi/FromString.h"

void Jafg::JTextureSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->RefetchTextureViews();

    auto& Frontend{this->GetFrontend()};
    for (auto const& Gti: this->GuaranteedTextureIdentifiers)
    {
        this->GuaranteedTextures.emplace_back(this->FromAsset(Gti));
    }
    for (auto& Gt: this->GuaranteedTextures)
    {
        Frontend.Vk_AddTextureToGlobalBindlessArray(&*Gt);
    }

    return;
}

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

Jafg::LTextureView const& Jafg::JTextureSubsystem::GetTextureView(LStringView Asset) const noexcept
{
    auto It{algo::find(this->TextureViews, Asset, &LTextureView::Name)};
    if (It == this->TextureViews.end())
    {
        LOG_FATAL(LogTextureSubsystem, "No such texture view [{}].", Asset)
    }
    return *It;
}

void Jafg::JTextureSubsystem::RefetchTextureViews()
{
    STAT_CYCLE_FUNCTION()
    algo::orphan(&this->TextureViews);
    LOG_VERBOSE(LogTextureSubsystem, "Refetching texture views.")

    for (auto& Assets{*this->GetEngine().GetSubsystemChecked<JAssetDiscoverer>()};
        auto const& Path: Assets.GetHeaders()
            | algo::views::filter([](auto const& Pair){ return Pair.second.Type == Detail::EAsset::Texture; })
            | algo::views::keys)
    {
        if constexpr (IS_COMPILED_LOG(LogTextureSubsystem, Trace))
        {
            LTextureView TextureView = Assets.PullAsset<LTextureView>(Path);
            LOG_TRACE(LogTextureSubsystem,
                "[{}@{}]: Format: [{}], MipLevels [{}], MSAA [{}]."
                , TextureView.Name, TextureView.Path, vk::to_string(TextureView.Format)
                , TextureView.MipLevels.has_value() ? std::to_string(*TextureView.MipLevels) : "AUTO"
                , TextureView.MaxSampleCount.has_value() ? vk::to_string(*TextureView.MaxSampleCount) : "AUTO"
                )
            this->TextureViews.emplace_back(std::move(TextureView));
        }
        else
        {
            this->TextureViews.emplace_back(Assets.PullAsset<LTextureView>(Path));
        }
    }

    LOG_VERBOSE(LogTextureSubsystem, "Finished loading [{}] texture views.", this->TextureViews.size())
    return;
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromFile(LPath const& Path
    , LTexture2::HostInfo HostCreateInfo
    , LTexture2::DeviceInfo DeviceCreateInfo
    , ETexture2State State /* = ETexture2StateBits::Device */
    )
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

        return It->second;
    }

    this->Textures[Path] = std::make_shared<LTexture2>(Path, HostCreateInfo, DeviceCreateInfo, State);
    return this->Textures[Path];
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromAsset(LStringView Asset, ETexture2State State /* = ETexture2StateBits::Device */)
{
    return this->FromTextureView(this->GetTextureView(Asset), State);
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromTextureView(LTextureView const& View, ETexture2State State /* = ETexture2StateBits::Device */)
{
    return this->FromFile(View.Texture
        , LTexture2::HostInfo{.Format=View.Format}
        , LTexture2::DeviceInfo{.DesiredMipLevels=View.MipLevels,.Samples=View.MaxSampleCount}
        , State
        );
}
