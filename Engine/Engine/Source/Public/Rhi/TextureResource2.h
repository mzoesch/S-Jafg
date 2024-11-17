// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Serialization/BulkData.h"
#include "Rhi/TextureCore.h"

namespace Jafg
{

struct LTextureMipMap2 final
{
    LTextureMipMap2() = default;
    LTextureMipMap2(LTextureMipMap2&& InOther) noexcept;
    LTextureMipMap2& operator=(LTextureMipMap2&& InOther) noexcept;
    ~LTextureMipMap2() { this->FreeBulk(); }

    LSize                 Size;
    LByteBulkData         Bulk;
    ERawImageFormat::Type Format;

    void FreeBulk();
    void AllocateBulk();
    void LoadFromBuffer(const uint8* Buffer, const int32 Offset);

    FORCEINLINE int32 GetChannelsPerPixel() const { return ERawImageFormat::GetChannelsPerPixel(this->Format); }
    FORCEINLINE int32 GetBytesPerPixel() const { return ERawImageFormat::GetBytesPerPixel(this->Format); }
};

} /* ~Namespace Jafg */
