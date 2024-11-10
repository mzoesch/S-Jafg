// Copyright mzoesch. All rights reserved.

#pragma once

#include "MyWorld/MyWorldStatics.h"

namespace Jafg
{

typedef LIntVector LChunkKeyTy;
typedef int32      LChunkKeyDomainTy;

/** Unique identifier for a chunk. */
struct LChunkKey final
{
    using LReal = float;

    static_assert(sizeof(LChunkKeyTy) == sizeof(LChunkKeyDomainTy) * 3, "LChunkKeyTy is not tightly packed.");

    union
    {
        struct
        {
            LChunkKeyDomainTy X;
            LChunkKeyDomainTy Y;
            LChunkKeyDomainTy Z;
        };

        LChunkKeyTy Key;
    };

    FORCEINLINE LChunkKey() = default;
    FORCEINLINE LChunkKey(const LChunkKeyTy& InKey) : Key(InKey) { }
    FORCEINLINE LChunkKey(const LChunkKeyDomainTy& InX, const LChunkKeyDomainTy& InY, const LChunkKeyDomainTy& InZ)
        : Key(InX, InY, InZ) { }
    FORCEINLINE LChunkKey(const LChunkKey& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE LChunkKey(LChunkKey&& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE ~LChunkKey() = default;

    FORCEINLINE LChunkKey& operator =(const LChunkKey&  InKey) noexcept;
    FORCEINLINE LChunkKey& operator =(      LChunkKey&& InKey) noexcept;

    FORCEINLINE bool operator==(const LChunkKey& InKey) const { return Key == InKey.Key; }
    FORCEINLINE bool operator!=(const LChunkKey& InKey) const { return Key != InKey.Key; }

    FORCEINLINE LVector ToWorldSpaceVector() const
    {
        return LVector
        {
            static_cast<LReal>(Key.X * MwStatics::ChunkSize),
            static_cast<LReal>(Key.Y * MwStatics::ChunkSize),
            static_cast<LReal>(Key.Z * MwStatics::ChunkSize)
        };
    }

    FORCEINLINE auto GetNorthKey() const -> LChunkKey { return { Key.X + 1, Key.Y, Key.Z }; }
    FORCEINLINE auto GetSouthKey() const -> LChunkKey { return { Key.X - 1, Key.Y, Key.Z }; }
    FORCEINLINE auto GetEastKey()  const -> LChunkKey { return { Key.X, Key.Y + 1, Key.Z }; }
    FORCEINLINE auto GetWestKey()  const -> LChunkKey { return { Key.X, Key.Y - 1, Key.Z }; }
    FORCEINLINE auto GetUpKey()    const -> LChunkKey { return { Key.X, Key.Y, Key.Z + 1 }; }
    FORCEINLINE auto GetDownKey()  const -> LChunkKey { return { Key.X, Key.Y, Key.Z - 1 }; }
    FORCEINLINE auto GetNeighboringChunkKeys() const -> TdhArray<LChunkKey>
    {
        TdhArray<LChunkKey> Out; Out.Reserve(6);

        Out.Emplace(Key.X + 1, Key.Y,     Key.Z    );
        Out.Emplace(Key.X - 1, Key.Y,     Key.Z    );
        Out.Emplace(Key.X,     Key.Y + 1, Key.Z    );
        Out.Emplace(Key.X,     Key.Y - 1, Key.Z    );
        Out.Emplace(Key.X,     Key.Y,     Key.Z + 1);
        Out.Emplace(Key.X,     Key.Y,     Key.Z - 1);

        return Out;
    }

    FORCEINLINE LSimpleString ToString() const
    {
        return LSimpleString::SprintF("{{{}, {}, {}}}", Key.X, Key.Y, Key.Z);
    }
};

LChunkKey& LChunkKey::operator=(const LChunkKey& InKey) noexcept
{
    this->Key = InKey.Key;
    return *this;
}

LChunkKey& LChunkKey::operator=(LChunkKey&& InKey) noexcept
{
    this->Key = InKey.Key;
    return *this;
}
} /* ~Namespace Jafg */

template <>
struct ::std::hash<::Jafg::LChunkKey>
{
    FORCEINLINE ::std::size_t operator()(const ::Jafg::LChunkKey& InKey) const noexcept
    {
        const std::size_t HashX = std::hash<::Jafg::LChunkKeyDomainTy>()(InKey.X);
        const std::size_t HashY = std::hash<::Jafg::LChunkKeyDomainTy>()(InKey.Y);
        const std::size_t HashZ = std::hash<::Jafg::LChunkKeyDomainTy>()(InKey.Z);

        std::size_t FinalHash = HashX;
        FinalHash ^= HashY + 0x9e3779b9 + (FinalHash << 6) + (FinalHash >> 2);
        FinalHash ^= HashZ + 0x9e3779b9 + (FinalHash << 6) + (FinalHash >> 2);

        return FinalHash;
    }
};
