// Copyright mzoesch. All rights reserved.

#pragma once

#include "Serialization/BulkData.h"
#include "Rhi/TextureCore.h"

namespace Jafg
{

class LTexture2;

struct LTextureMipMap2 final
{
    friend LTexture2;

    inline LTextureMipMap2() = default;

    PROHIBIT_COPY(LTextureMipMap2)

    ENGINE_API LTextureMipMap2(LTextureMipMap2&& InOther) noexcept;
    ENGINE_API LTextureMipMap2& operator=(LTextureMipMap2&& InOther) noexcept;

    inline ~LTextureMipMap2() { this->FreeBulk(); }

    FORCEINLINE bool IsValid() const noexcept { return this->Bulk.IsAllocated() && this->Size.X > 0 && this->Size.Y > 0; }

    ENGINE_API void FreeBulk();
    ENGINE_API void AllocateBulk();
    ENGINE_API void LoadFromBuffer(const u8* Buffer, const i32 Offset);

    FORCEINLINE i32   GetWidth() const noexcept { return this->Size.X; }
    FORCEINLINE i32   GetHeight() const noexcept { return this->Size.Y; }
    FORCEINLINE LSize GetSize() const noexcept { return this->Size; }

    FORCEINLINE const LByteBulkData& GetBulk() const noexcept { return this->Bulk; }

    FORCEINLINE i32 GetChannelsPerPixel() const noexcept { return ERawImageFormat::GetChannelsPerPixel(this->Format); }
    FORCEINLINE i32 GetBytesPerPixel() const noexcept { return ERawImageFormat::GetBytesPerPixel(this->Format); }
    FORCEINLINE ERawImageFormat::Type GetFormat() const noexcept { return this->Format; }

private:

    LSize                 Size;
    LByteBulkData         Bulk;
    ERawImageFormat::Type Format;
};

} /* ~Namespace Jafg */
