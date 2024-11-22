// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

typedef LInt8Vector LVoxelKeyTy;
typedef int8        LVoxelKeyDomainTy;

namespace EVoxelKeyLocation
{

enum Type : int8
{
    Local,
    North,
    South,
    East,
    West,
    Up,
    Down,
};

} /* ~Namespace EVoxelKeyLocation */

/** Unique identifier for a voxel inside a context. */
struct LVoxelKey final
{
    using LReal = float;

    union
    {
        struct
        {
            LVoxelKeyDomainTy X;
            LVoxelKeyDomainTy Y;
            LVoxelKeyDomainTy Z;
        };

        LVoxelKeyTy Key;
    };

    FORCEINLINE LVoxelKey() = default;
    FORCEINLINE LVoxelKey(const LVoxelKeyTy& InKey) : Key(InKey) { }
    FORCEINLINE LVoxelKey(const LVoxelKeyDomainTy& InX, const LVoxelKeyDomainTy& InY, const LVoxelKeyDomainTy& InZ)
        : Key(InX, InY, InZ) { }
    FORCEINLINE LVoxelKey(const int32& InX, const int32& InY, const int32& InZ)
        : Key(static_cast<LVoxelKeyDomainTy>(InX), static_cast<LVoxelKeyDomainTy>(InY), static_cast<LVoxelKeyDomainTy>(InZ)) { }
    FORCEINLINE LVoxelKey(const LVoxelKey& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE LVoxelKey(LVoxelKey&& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE ~LVoxelKey() = default;

    FORCEINLINE LVoxelKey& operator =(const LVoxelKey&  InKey) noexcept;
    FORCEINLINE LVoxelKey& operator =(      LVoxelKey&& InKey) noexcept;

    FORCEINLINE bool operator==(const LVoxelKey& InKey) const { return this->Key == InKey.Key; }
    FORCEINLINE bool operator!=(const LVoxelKey& InKey) const { return this->Key != InKey.Key; }

    FORCEINLINE bool IsLocal() const
    {
        return this->X > INDEX_NONE && this->X < MwStatics::ChunkSize
            && this->Y > INDEX_NONE && this->Y < MwStatics::ChunkSize
            && this->Z > INDEX_NONE && this->Z < MwStatics::ChunkSize;
    }

    FORCEINLINE auto GetNorthKey() const -> LVoxelKey { return { static_cast<LVoxelKeyDomainTy>(this->Key.X + 1), this->Key.Y, this->Key.Z }; }
    FORCEINLINE auto GetSouthKey() const -> LVoxelKey { return { static_cast<LVoxelKeyDomainTy>(this->Key.X - 1), this->Key.Y, this->Key.Z }; }
    FORCEINLINE auto GetEastKey()  const -> LVoxelKey { return { this->Key.X, static_cast<LVoxelKeyDomainTy>(this->Key.Y + 1), this->Key.Z }; }
    FORCEINLINE auto GetWestKey()  const -> LVoxelKey { return { this->Key.X, static_cast<LVoxelKeyDomainTy>(this->Key.Y - 1), this->Key.Z }; }
    FORCEINLINE auto GetUpKey()    const -> LVoxelKey { return { this->Key.X, this->Key.Y, static_cast<LVoxelKeyDomainTy>(this->Key.Z + 1) }; }
    FORCEINLINE auto GetDownKey()  const -> LVoxelKey { return { this->Key.X, this->Key.Y, static_cast<LVoxelKeyDomainTy>(this->Key.Z - 1) }; }
    FORCEINLINE auto GetNeighboringVoxelKeys() const -> TdhArray<LVoxelKey>;

    NODISCARD FORCEINLINE auto NormalizeKeyForNeighbor() -> EVoxelKeyLocation::Type;
    NODISCARD FORCEINLINE auto ToString() const -> LSimpleString;
};

LVoxelKey& LVoxelKey::operator=(const LVoxelKey& InKey) noexcept
{
    this->Key = InKey.Key;
    return *this;
}

LVoxelKey& LVoxelKey::operator=(LVoxelKey&& InKey) noexcept
{
    this->Key = InKey.Key;
    return *this;
}

FORCEINLINE TdhArray<LVoxelKey> LVoxelKey::GetNeighboringVoxelKeys() const
{
    TdhArray<LVoxelKey> Out; Out.Reserve(6);

    Out.Emplace(static_cast<LVoxelKeyDomainTy>(this->Key.X + 1), this->Key.Y, this->Key.Z);
    Out.Emplace(static_cast<LVoxelKeyDomainTy>(this->Key.X - 1), this->Key.Y, this->Key.Z);
    Out.Emplace(this->Key.X, static_cast<LVoxelKeyDomainTy>(this->Key.Y + 1), this->Key.Z);
    Out.Emplace(this->Key.X, static_cast<LVoxelKeyDomainTy>(this->Key.Y - 1), this->Key.Z);
    Out.Emplace(this->Key.X, this->Key.Y, static_cast<LVoxelKeyDomainTy>(this->Key.Z + 1));
    Out.Emplace(this->Key.X, this->Key.Y, static_cast<LVoxelKeyDomainTy>(this->Key.Z - 1));

    return Out;
}

FORCEINLINE EVoxelKeyLocation::Type LVoxelKey::NormalizeKeyForNeighbor()
{
    if (this->X < 0)
    {
        this->X += MwStatics::ChunkSize;
        return EVoxelKeyLocation::South;
    }
    if (this->X >= MwStatics::ChunkSize)
    {
        this->X -= MwStatics::ChunkSize;
        return EVoxelKeyLocation::North;
    }
    if (this->Y < 0)
    {
        this->Y += MwStatics::ChunkSize;
        return EVoxelKeyLocation::West;
    }
    if (this->Y >= MwStatics::ChunkSize)
    {
        this->Y -= MwStatics::ChunkSize;
        return EVoxelKeyLocation::East;
    }
    if (this->Z < 0)
    {
        this->Z += MwStatics::ChunkSize;
        return EVoxelKeyLocation::Down;
    }
    if (this->Z >= MwStatics::ChunkSize)
    {
        this->Z -= MwStatics::ChunkSize;
        return EVoxelKeyLocation::Up;
    }

    return EVoxelKeyLocation::Local;
}

FORCEINLINE LSimpleString LVoxelKey::ToString() const
{
    return LSimpleString::SprintF("{{{},{},{}}}", this->Key.X, this->Key.Y, this->Key.Z);
}

} /* ~Namespace Jafg */

template <>
struct ::std::hash<::Jafg::LVoxelKey>
{
    FORCEINLINE ::std::size_t operator()(const ::Jafg::LVoxelKey& InKey) const noexcept
    {
        const std::size_t HashX = std::hash<::Jafg::LVoxelKeyDomainTy>()(InKey.X);
        const std::size_t HashY = std::hash<::Jafg::LVoxelKeyDomainTy>()(InKey.Y);
        const std::size_t HashZ = std::hash<::Jafg::LVoxelKeyDomainTy>()(InKey.Z);

        std::size_t FinalHash = HashX;
        FinalHash ^= HashY + 0x9e3779b9 + (FinalHash << 6) + (FinalHash >> 2);
        FinalHash ^= HashZ + 0x9e3779b9 + (FinalHash << 6) + (FinalHash >> 2);

        return FinalHash;
    }
};
