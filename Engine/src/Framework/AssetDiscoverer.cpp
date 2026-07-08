// Copyright mzoesch. All rights reserved.

#include "Framework/AssetDiscoverer.h"

#define DETAIL_JAFG_PULL_HEADER(Path, Asset) \
    std::ifstream F{Path, std::ios::binary}; \
    serde::is_bin_archive Ar{F.rdbuf()}; \
    if (!Ar) \
    { \
        LOG_FATAL(LogAssetSubsystem, "[{}]: Failed to initialize in stream.", Path) \
    } \
    \
    Detail::LAssetHeader Header; \
    Ar(Header); \
    if (Header.Magic != Detail::AssetHeaderMagic) \
    { \
        LOG_FATAL(LogAssetSubsystem, "[{}]: Invalid magic. Expected [{}], got [{}].", Path, Detail::AssetHeaderMagic, Header.Magic) \
    } \
    if (Header.Type != Asset) \
    { \
        LOG_FATAL(LogAssetSubsystem, "[{}]: Invalid asset type. Expected [{}], got [{}].", Path, static_cast<u32>(Asset), static_cast<u32>(Header.Type)) \
    }

void Jafg::JAssetDiscoverer::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->Recache();
    return;
}

void Jafg::JAssetDiscoverer::PushAsset(LPath const& Path, LTextureView const& View)
{
    LOG_VERBOSE(LogAssetSubsystem, "[{}]: Pushing asset to disk.", Path)

    check(finder::descendant_of(Path, finder::content_dir()))
    check(finder::descendant_of(View.Texture, finder::content_dir()))

    auto Embedded{relative(View.Texture, finder::content_dir())};

    std::ofstream F{Path, std::ios::binary};
    serde::os_bin_archive Ar{F.rdbuf()};
    if (!Ar)
    {
        LOG_FATAL(LogAssetSubsystem, "[{}]: Failed to initialize out stream.", Path)
    }

    Detail::LAssetHeader Header{
        .Version = JAssetDiscoverer::LatestVersion,
        .Type = Detail::EAsset::Texture,
        };

    Ar(Header);
    Ar(Embedded);
    Ar(View.Format);
    Ar(View.MipLevels);
    Ar(View.MaxSampleCount);

    F.close();

    this->Recache(Path);
}

Jafg::LTextureView Jafg::JAssetDiscoverer::PullAssetTextureView(LPath const& Path) const
{
    auto Canonical{finder::canonical(Path)};
    check(finder::descendant_of(Path))
    DETAIL_JAFG_PULL_HEADER(Canonical, Detail::EAsset::Texture)

    LTextureView Result;
    Result.Path = Canonical;
    Result.Name = relative(Result.Path, finder::content_dir()).replace_extension().generic_string();
    Ar(Result.Texture); Result.Texture = finder::content_dir()/Result.Texture;
    Ar(Result.Format);
    Ar(Result.MipLevels);
    Ar(Result.MaxSampleCount);

    LOG_TRACE(LogTextureSubsystem,
        "[{}@{}]: Format: [{}], MipLevels [{}], MSAA [{}]."
        , Result.Name, Result.Path, vk::to_string(Result.Format)
        , Result.MipLevels.has_value() ? std::to_string(*Result.MipLevels) : "AUTO"
        , Result.MaxSampleCount.has_value() ? vk::to_string(*Result.MaxSampleCount) : "AUTO"
        )

    return Result;
}

void Jafg::JAssetDiscoverer::Recache()
{
    this->CachedHeaders.clear();

    for (auto& Entry: finder::recursive_directory_iterator{LITERAL_TEXT("Content")}
        | algo::views::filter([](auto const& Entry){ return Entry.is_regular_file() && Entry.path().extension() == Detail::AssetPostfix; })
        )
    {
        std::ifstream F{Entry.path(), std::ios::binary};
        serde::is_bin_archive Ar{F.rdbuf()};
        if (!Ar)
        {
            LOG_FATAL(LogAssetSubsystem, "[{}]: Failed to initialize in stream."
                , Entry.path())
        }

        Detail::LAssetHeader Header;
        Ar(Header);
        if (Header.Magic != Detail::AssetHeaderMagic)
        {
            LOG_FATAL(LogAssetSubsystem, "[{}]: Invalid magic. Expected [{}], got [{}]."
                , Entry.path(), Detail::AssetHeaderMagic, Header.Magic)
        }

        this->CachedHeaders.emplace(Entry.path(), std::move(Header));
    }

    LOG_VERBOSE(LogAssetSubsystem, "Found [{}] assets.", this->CachedHeaders.size())

    return;
}

void Jafg::JAssetDiscoverer::Recache(LPath const& Path)
{
    std::ifstream F{Path, std::ios::binary};
    serde::is_bin_archive Ar{F.rdbuf()};
    if (!Ar)
    {
        LOG_FATAL(LogAssetSubsystem, "[{}]: Failed to initialize in stream."
            , Path)
    }

    Detail::LAssetHeader Header;
    Ar(Header);
    if (Header.Magic != Detail::AssetHeaderMagic)
    {
        LOG_FATAL(LogAssetSubsystem, "[{}]: Invalid magic. Expected [{}], got [{}]."
            , Path, Detail::AssetHeaderMagic, Header.Magic)
    }

    this->CachedHeaders[Path] = std::move(Header);

    LOG_VERBOSE(LogAssetSubsystem, "Recached asset [{}].", Path)

    return;
}
