// Copyright mzoesch. All rights reserved.

#include "Framework/TextureViewSubsystem.h"
#include "Stats/Stats.h"
#include "Serialization/Json.h"

void Jafg::JTextureViewSubsystem::ReloadTextureViews()
{
    STAT_CYCLE_FUNCTION()
    LOG_VERBOSE(LogTextureSubsystem, "Reloading texture views.")

    for (auto TextureViewFiles{Finder::FindFilesRecursively("Content/TextureViews", true, ".*\\.tex")}; auto const& TextureViewFile : TextureViewFiles)
    {
        LTextureView TextureView{.Path = TextureViewFile};
        json TextureViewJson = json::parse(Finder::ReadFile(TextureView.Path), nullptr, false);
        if (TextureViewJson.is_discarded())
        {
            LOG_FATAL(LogTextureSubsystem, "Texture view [{}] is not valid json. Failed to load.", TextureView.Path)
        }

        if (LString MissingKey; Json::DoesObjectContainKeys(TextureViewJson, {"Texture", "Format"}, &MissingKey) == false)
        {
            if (MissingKey.empty())
            {
                LOG_FATAL(LogTextureSubsystem, "Texture view [{}] is not a json object. Failed to load.", TextureView.Path)
            }
            LOG_FATAL(LogTextureSubsystem, "Texture view [{}] does not contain key [{}]. Failed to load.", TextureView.Path, MissingKey)
        }

        if (TextureViewJson["Texture"].is_string() == false)
        {
            LOG_FATAL(LogTextureSubsystem, "Texture view [{}]: Texture key is not a string. Failed to load.", TextureView.Path)
        }
        TextureView.Texture = Finder::GetTexturesDir() / TextureViewJson["Texture"].get<LString>();
        if (Finder::DoesFileExist(TextureView.Texture) == false)
        {
            LOG_FATAL(LogTextureSubsystem, "[{}]: No such texture [{}]."
                , TextureView.Path, TextureView.Texture
                )
        }

        if (TextureViewJson["Format"].is_string() == false)
        {
            LOG_FATAL(LogTextureSubsystem, "Texture view [{}]: Format key is not a string. Failed to load.", TextureView.Path)
        }
        TextureView.Format = Vk_StringToFormat(TextureViewJson["Format"].get<LString>());
        if (TextureView.Format == vk::Format::eUndefined)
        {
            LOG_FATAL(LogTextureSubsystem, "Texture view [{}]: Unsupported format string [{}]. Failed to load."
                , TextureView.Path, TextureViewJson["Format"].get<LString>()
                )
        }

        this->TextureViews.emplace_back(std::move(TextureView));
    }

    LOG_VERBOSE(LogTextureSubsystem, "Finished loading [{}] texture views.", this->TextureViews.size())

    return;
}
