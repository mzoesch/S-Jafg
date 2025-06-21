// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/TextureMipMap2.h"
#include "Rhi/Texture2Handle.h"
#include "System/SystemForward.h"

namespace Jafg
{

class JUserPreferences;

namespace Texture2
{

ENGINE_API Smart::TUnique<u8> RotateCW (const u8* InData, const u32 InWidth, const u32 InHeight, const u32 InChannels);
ENGINE_API Smart::TUnique<u8> RotateCCW(const u8* InData, const u32 InWidth, const u32 InHeight, const u32 InChannels);

} /* ~Namespace ~Image */

//#
//# Represents a generic two-dimensional texture that can be uploaded to a graphics card.
//#
class LTexture2 final
{
public:

    inline LTexture2() = default;
    PROHIBIT_COPY(LTexture2)
    DEFAULT_MOVE(LTexture2)
    inline ~LTexture2() { this->Free(); }

    ENGINE_API bool CreateEmpty(const u32 InWidth, const u32 InHeight, const ERawImageFormat::Type InFormat);
    ENGINE_API bool LoadFromDisk(const LPath& Path, const ERawImageFormat::Type InFormat);
    ENGINE_API bool LoadFromDisk(const LEnginePath& Path, const ERawImageFormat::Type InFormat);
    ENGINE_API void Free();

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
    ENGINE_API void CopyTexture(const LTexture2& InTexture, const LPoint& InPoint = LPoint::Zero(), const bool bKeepCurrentTexture = false);

    FORCEINLINE bool IsValid() const noexcept { return this->MipMap.IsValid(); }

    FORCEINLINE u32   GetWidth() const noexcept{ return this->MipMap.GetWidth(); }
    FORCEINLINE u32   GetHeight() const noexcept { return this->MipMap.GetHeight(); }
    FORCEINLINE LSize GetSize() const noexcept { return this->MipMap.GetSize(); }
    FORCEINLINE auto  GetFormat() const noexcept -> ERawImageFormat::Type { return this->MipMap.GetFormat(); }
    FORCEINLINE i32   GetChannelsPerPixel() const noexcept { return this->MipMap.GetChannelsPerPixel(); }
    FORCEINLINE i32   GetBytesPerPixel() const noexcept { return this->MipMap.GetBytesPerPixel(); }
    FORCEINLINE auto  GetFirstMipMap() const noexcept -> const LTextureMipMap2& { return this->MipMap; }

    FORCEINLINE void Upload() const { this->Handle.Upload(*this); }
    FORCEINLINE bool IsUploaded() const noexcept { return this->Handle.IsValid(); }
    FORCEINLINE const LTexture2Handle& GetHandle() const noexcept;
    FORCEINLINE const LTexture2Handle& GetHandleStrong() const noexcept { return this->Handle; }

private:

    //#
    //# The first mip map of this texture.
    //#
    LTextureMipMap2 MipMap;

    //#
    //# A handle that is valid if the texture was uploaded to the graphics card.
    //#
    mutable LTexture2Handle Handle;
};

FORCEINLINE const LTexture2Handle& LTexture2::GetHandle() const noexcept
{
    check( this->IsValid() )

    if (this->IsUploaded() == false)
    {
        this->Upload();
    }

    return this->Handle;
}

} /* ~Namespace Jafg */
