// Copyright mzoesch. All rights reserved.

#include "Framework/TextureSubsystem.h"
#include "Rhi/TextureView.h"
#include "Engine/Engine.h"
#include "Stats/Stats.h"
#include "Serialization/Json.h"

void Jafg::JTextureSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->RefetchingTextureViews();

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

Jafg::LTextureView const& Jafg::JTextureSubsystem::GetTextureView(LStringView Name) const noexcept
{
    auto It{algo::find(this->TextureViews, Name, &LTextureView::Name)};
    if (It == this->TextureViews.end())
    {
        LOG_FATAL(LogTextureSubsystem, "No such texture view [{}].", Name)
    }
    return *It;
}

void Jafg::JTextureSubsystem::RefetchingTextureViews()
{
    STAT_CYCLE_FUNCTION()
    LOG_VERBOSE(LogTextureSubsystem, "Refetching texture views.")

    algo::orphan(&this->TextureViews);

    LString MissingKey; Json::EError Error;
    for (auto TextureViewFiles{Finder::FindFilesRecursively("Content/TextureViews", true, ".*\\.tv.json")}; auto const& TextureViewFile : TextureViewFiles)
    {
        LTextureView TextureView{.Path = TextureViewFile,.Name=TextureViewFile.stem().stem().string()};
        json TextureViewJson = json::parse(Finder::ReadFile(TextureView.Path), nullptr, false);
        if (TextureViewJson.is_discarded())
        {
            LOG_FATAL(LogTextureSubsystem, "[{}]: Texture view is not valid json. Failed to load.", TextureView.Path)
        }

        if (Json::DoesObjectContainTypeCheckedKeys(TextureViewJson, {{"Texture", Json::LKeyType::String}, {"Format", Json::LKeyType::String}}, &MissingKey, &Error) == false)
        {
            if (MissingKey.empty())
            {
                LOG_FATAL(LogTextureSubsystem, "[{}]: Texture view is not a json object. Failed to load.", TextureView.Path)
            }
            LOG_FATAL(LogTextureSubsystem, "[{}]: Texture view does not contain key [{}]. Failed to load.", TextureView.Path, MissingKey)
        }

        TextureView.Texture = Finder::GetTexturesDir() / TextureViewJson["Texture"].get<LString>();
        if (Finder::DoesFileExist(TextureView.Texture) == false)
        {
            LOG_FATAL(LogTextureSubsystem, "[{}]: No such texture [{}]."
                , TextureView.Path, TextureView.Texture
                )
        }

        TextureView.Format = Vk_StringToFormat(TextureViewJson["Format"].get<LString>());
        if (TextureView.Format == vk::Format::eUndefined)
        {
            LOG_FATAL(LogTextureSubsystem, "[{}]: Unsupported format string [{}]. Failed to load."
                , TextureView.Path, TextureViewJson["Format"].get<LString>()
                )
        }

        if (TextureViewJson.contains("MipLevels"))
        {
            if (TextureViewJson["MipLevels"].is_null())
            {
                check(TextureView.MipLevels.has_value() == false)
            }
            else
            {
                if (TextureViewJson["MipLevels"].is_number_unsigned() == false)
                {
                    LOG_FATAL(LogTextureSubsystem, "[{}]: MipLevels entry is not an unsinged number. Failed to load.", TextureView.Path)
                }
                u64 MipLevels{TextureViewJson["MipLevels"].get<u64>()};
                if (MipLevels == 0)
                {
                    LOG_FATAL(LogTextureSubsystem, "[{}]: MipLevels entry cannot be 0. Failed to load.", TextureView.Path)
                }
                TextureView.MipLevels = MipLevels;
            }
        }

        if (TextureViewJson.contains("MaxSampleCount"))
        {
            if (TextureViewJson["MaxSampleCount"].is_null())
            {
                check(TextureView.MaxSampleCount.has_value() == false)
            }
            else
            {
                if (TextureViewJson["MaxSampleCount"].is_string() == false)
                {
                    LOG_FATAL(LogTextureSubsystem, "[{}]: MaxSampleCount entry is not a string. Failed to load.", TextureView.Path)
                }
                TextureView.MaxSampleCount = Vk_StringToSampleCountFlagBits(TextureViewJson["MaxSampleCount"].get<LString>());
            }
        }

        LOG_TRACE(LogTextureSubsystem,
            "[{}@{}]: Format: [{}], MipLevels [{}], MSAA [{}]."
            , TextureView.Name, TextureView.Path, vk::to_string(TextureView.Format)
            , TextureView.MipLevels.has_value() ? std::to_string(*TextureView.MipLevels) : "AUTO"
            , TextureView.MaxSampleCount.has_value() ? vk::to_string(*TextureView.MaxSampleCount) : "AUTO"
            )
        this->TextureViews.emplace_back(std::move(TextureView));
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

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromTextureViewIdentifier(LStringView TextureView, ETexture2State State /* = ETexture2StateBits::Device */)
{
    return this->FromTextureView(this->GetTextureView(TextureView), State);
}

Jafg::LTexture2Ref Jafg::JTextureSubsystem::FromTextureView(LTextureView const& View, ETexture2State State /* = ETexture2StateBits::Device */)
{
    return this->FromFile(View.Texture
        , LTexture2::HostInfo{.Format=View.Format}
        , LTexture2::DeviceInfo{.DesiredMipLevels=View.MipLevels,.Samples=View.MaxSampleCount}
        , State
        );
}
