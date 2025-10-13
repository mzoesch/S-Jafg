// Copyright mzoesch. All rights reserved.

#pragma once

#include "Texture2.h"
#include "MyWorld/CommonTypes.h"

namespace Jafg
{

//#
//# Represents a generic two-dimensional atlas.
//#
class LAtlas2 final
{
public:

    LAtlas2() = default;
    LAtlas2( LAtlas2& Atlas2) noexcept = delete;
    LAtlas2(LAtlas2 const& Atlas2) noexcept
    {
        check( Atlas2.Data.IsValid() == false )
    }
    LAtlas2& operator=(LAtlas2 const& Atlas2) noexcept = delete;
    DEFAULT_MOVE(LAtlas2)
    ~LAtlas2() { this->Free(); }

    //# Create the atlas from the given textures.
    void Make(const TArray<LTexture2>& InTextures, const bool bFreeOld = true);
    void Free() { this->Data.Free(); this->TexWidth = 0; }

    FORCEINLINE auto GetData() const -> const LTexture2& { return this->Data; }
    FORCEINLINE u32 GetWidth()  const noexcept { return this->Data.GetWidth(); }
    FORCEINLINE u32 GetHeight() const noexcept { return this->Data.GetHeight(); }
    FORCEINLINE u32 GetUnderlyingTextureWidth() const noexcept { return this->TexWidth; }
    FORCEINLINE u32 GetDomainWidth() const noexcept { return this->Data.GetFirstMipMap().GetWidth() / this->TexWidth; }

    FORCEINLINE auto GetMostSignificantMipMap() const -> const LTextureMipMap2& { return this->Data.GetFirstMipMap(); }
    FORCEINLINE auto CalculateSpecificTexturePointOnAtlas(const LTextureIndex InTextureIndex) const -> LPoint;

private:

    //#
    //# The texture width to use for all textures. Is guaranteed to be a power of two.
    //# Lower resolutions will be scaled up.
    //#
    i32 TexWidth { 0 };
    LTexture2 Data;
};

LPoint LAtlas2::CalculateSpecificTexturePointOnAtlas(const LTextureIndex InTextureIndex) const
{
    const u32 Width = this->Data.GetFirstMipMap().GetWidth();

    const i32 X = static_cast<i32>((InTextureIndex * this->TexWidth) % Width);
    const i32 Y = static_cast<i32>(((InTextureIndex * this->TexWidth) / Width) * this->TexWidth);

    return LPoint(X, Y);
}

} /* ~Namespace Jafg */
