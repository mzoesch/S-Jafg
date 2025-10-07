// Copyright mzoesch. All rights reserved.

#include "System/TextureSubsystem.h"
#include "Engine/Engine.h"
#include "Rhi/Texture2.h"
#include "User/UserPreferences.h"

void Jafg::JTextureSubsystem::PurgeTextures()
{
    return;
}

std::shared_ptr<Jafg::LTexture2> Jafg::JTextureSubsystem::GetTexture(const LPath& InPath, const ERawImageFormat::Type InFormat) const
{
    check( InPath.empty() == false )

    std::string Path { InPath.generic_string() };
    if (const auto& It = this->Textures.find(Path); It != this->Textures.end())
    {
        return It->second;
    }

    LTexture2 Tex;
    if (Tex.LoadFromDisk(InPath, InFormat) == false)
    {
        LOG_ERROR(LogTextureSubsystem, "Cannot create shared pointer for texture [{}].", InPath)
        return { };
    }

    std::shared_ptr<LTexture2> TexPtr { std::make_shared<LTexture2>(std::move(Tex)) };

    this->Textures[std::move(Path)] = TexPtr;

    check( TexPtr->GetFormat() == InFormat )
    return TexPtr;
}

std::shared_ptr<Jafg::LTexture2> Jafg::JTextureSubsystem::GetTexture(const LEnginePath& InPath, const ERawImageFormat::Type InFormat) const
{
    return this->GetTexture(InPath.ResolvePath(), InFormat);
}
