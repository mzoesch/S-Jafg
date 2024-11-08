// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "MyWorld/MyWorldStatics.h"

namespace Jafg
{

typedef LIntVector LChunkKeyTy;
typedef int32      LChunkKeyDomainTy;

/** Unique identifier for a chunk. */
struct LChunkKey final
{
    using LReal = float;

    FORCEINLINE LChunkKey() = default;
    FORCEINLINE LChunkKey(const LChunkKeyTy& InKey) : Key(InKey) { }
    FORCEINLINE LChunkKey(const LChunkKeyDomainTy& InX, const LChunkKeyDomainTy& InY, const LChunkKeyDomainTy& InZ)
        : Key(InX, InY, InZ) { }
    FORCEINLINE LChunkKey(const LChunkKey& InKey) = default;
    FORCEINLINE LChunkKey(LChunkKey&& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE ~LChunkKey() = default;

    FORCEINLINE LChunkKey& operator =(const LChunkKey&  InKey) noexcept = default;
    FORCEINLINE LChunkKey& operator =(      LChunkKey&& InKey) noexcept = default;

    LChunkKeyTy Key;

    FORCEINLINE LVector ToWorldSpaceVector() const
    {
        return LVector
        {
            static_cast<LReal>(Key.X * MwStatics::ChunkSize),
            static_cast<LReal>(Key.Y * MwStatics::ChunkSize),
            static_cast<LReal>(Key.Z * MwStatics::ChunkSize)
        };
    }
};

} /* ~Namespace Jafg */
