// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi.h"
#include "System/EnginePath.h"
#include "Engine/CxxClass.h"
#include "TextureSubsystem.generated.h"

namespace Jafg
{

class LTexture2;

//#
//# A texture that was loaded by the program and may be used across many different widgets.
//# @remark This is not intended for textures that are very specific.
//#
DECLARE_JAFG_CLASS(ECxxClassFlags::Singleton)
class JTextureSubsystem : public JCxxClass
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JTextureSubsystem)

public:

    //# Removes all loaded textures that are not referenced anymore.
    void PurgeUnused() noexcept;

    LImage GetImage(LString const& Ident) const
    {
        return LImage{this->GetTexture(nullptr, Ident, {}, ETextureLoadFlagBits::Default)};
    }
    LImage GetImage(LPath const& Path, LTexture2::LMetadata Meta, ETextureLoadFlags Flags = ETextureLoadFlagBits::Load) const
    {
        return LImage{this->GetTexture(&Path, Path.generic_string(), Meta, Flags)};
    }
    LImage GetImage(LEnginePath const& Path, LTexture2::LMetadata Meta, ETextureLoadFlags Flags = ETextureLoadFlagBits::Load) const
    {
        return this->GetImage(Path.ResolvePath(), Meta, Flags);
    }

    void AddImage(LImage const& Image, LString const& Ident) const noexcept
    {
        check( Image.HasTexture() )
        check( this->Textures.contains(Ident) == false )
        this->Textures.emplace(Ident, Image.GetNewTextureHandle());
        return;
    }

    FORCEINLINE auto GetLoadedTextureCount() const noexcept { return this->Textures.size(); }
    FORCEINLINE auto const& GetTextures() const noexcept { return this->Textures; }

private:

    std::shared_ptr<LTexture2> GetTexture(LPath const* Path, LString const& Ident, LTexture2::LMetadata Meta, ETextureLoadFlags Flags) const;

    mutable std::unordered_map<LString, std::shared_ptr<LTexture2>> Textures;
};

} /* ~Namespace Jafg */
