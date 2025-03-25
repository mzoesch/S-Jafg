// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "TextureResource2.h"
#include "System/SystemForward.h"

namespace Jafg
{

class JUserPreferences;

//#
//# Represents a generic two-dimensional texture.
//#
class LTexture2 final
{
public:

    LTexture2() = default;
    PROHIBIT_COPY(LTexture2)
    LTexture2(LTexture2&& InOther) noexcept { this->MipMap = std::move(InOther.MipMap); }
    LTexture2& operator=(LTexture2&& InOther) noexcept { this->MipMap = std::move(InOther.MipMap); return *this; }
    ~LTexture2() { this->Free(); }

    bool CreateEmpty(const u32 InWidth, const u32 InHeight, const ERawImageFormat::Type InFormat);
    bool LoadFromDisk(const LEnginePath& Path, const JUserPreferences& UserPreferences);
    void Free() { this->MipMap.FreeBulk(); }

    //#
    //# Copy another texture to this texture.
    //#
    //# @param InTexture           The texture to copy.
    //# @param InPoint             The point to copy the texture to.
    //# @param bKeepCurrentTexture If true, the current texture will be kept and the new texture will
    //#                            be copied on top of it. The other texture will override current pixels and not
    //#                            add them. It requires that the current texture is at least the same size as the
    //#                            new texture.
    //#
    void CopyTexture(const LTexture2& InTexture, const LPoint& InPoint = LPoint::Zero(), const bool bKeepCurrentTexture = false);

    const LTextureMipMap2& GetFirstMipMap() const { return this->MipMap; }

    FORCEINLINE auto GetWidth()  const noexcept -> u32 { return this->MipMap.Size.X; }
    FORCEINLINE auto GetHeight() const noexcept -> u32 { return this->MipMap.Size.Y; }
    FORCEINLINE auto GetFormat() const noexcept -> ERawImageFormat::Type { return this->MipMap.Format; }
    FORCEINLINE auto GetChannelsPerPixel() const noexcept -> i32 { return this->MipMap.GetChannelsPerPixel(); }
    FORCEINLINE auto GetBytesPerPixel() const noexcept -> i32 { return this->MipMap.GetBytesPerPixel(); }

private:

    LTextureMipMap2 MipMap = { };
};

} /* ~Namespace Jafg */
