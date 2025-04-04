// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

typedef Li8Vector LVoxelKeyTy;
typedef i8        LVoxelKeyDomainTy;

namespace EVoxelKeyLocation
{

enum Type : i8
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

//# Unique identifier for a voxel inside a context.
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
    FORCEINLINE LVoxelKey(const i32& InX, const i32& InY, const i32& InZ)
        : Key(static_cast<LVoxelKeyDomainTy>(InX), static_cast<LVoxelKeyDomainTy>(InY), static_cast<LVoxelKeyDomainTy>(InZ)) { }
    FORCEINLINE LVoxelKey(const LVoxelKey& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE LVoxelKey(LVoxelKey&& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE ~LVoxelKey() = default;

    //#
    //# From world location (or relative world location to another origin).
    //# Note that this function will normalize the key to the local space.
    //#
    static LVoxelKey FromWorldSpace(const LVector& InVector);

    //#
    //# From world location (or relative world location to another origin). This function will not normalize the key
    //# to the local space. Note that this only works if the given world vector does not exceed the domain space.
    //#
    static LVoxelKey FromWorldLocationPreserveLocalSpace(const LVector& InVector);

    FORCEINLINE LVoxelKey& operator =(const LVoxelKey&  InKey) noexcept;
    FORCEINLINE LVoxelKey& operator =(      LVoxelKey&& InKey) noexcept;

    FORCEINLINE bool operator==(const LVoxelKey& InKey) const { return this->Key == InKey.Key; }
    FORCEINLINE bool operator!=(const LVoxelKey& InKey) const { return this->Key != InKey.Key; }

    FORCEINLINE auto ToWorldSpace() -> LVector;

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
    FORCEINLINE auto GetNeighboringVoxelKeys() const -> TArray<LVoxelKey>;

    NODISCARD FORCEINLINE auto NormalizeKeyForNeighbor() -> EVoxelKeyLocation::Type;
    NODISCARD FORCEINLINE auto ToString() const -> LString;
};

inline LVoxelKey LVoxelKey::FromWorldSpace(const LVector& InVector)
{
    LVoxelKey Out;

    if (InVector.X < 0)
    {
        Out.X = static_cast<LVoxelKeyDomainTy>(MwStatics::ChunkSize + (static_cast<i64>(Maths::Ceil(InVector.X)) % MwStatics::ChunkSize) - 1);
    }
    else
    {
        Out.X = static_cast<LVoxelKeyDomainTy>(static_cast<i64>(Maths::Floor(InVector.X)) % MwStatics::ChunkSize);
    }

    if (InVector.Y < 0)
    {
        Out.Y = static_cast<LVoxelKeyDomainTy>(MwStatics::ChunkSize + (static_cast<i64>(Maths::Ceil(InVector.Y)) % MwStatics::ChunkSize) - 1);
    }
    else
    {
        Out.Y = static_cast<LVoxelKeyDomainTy>(static_cast<i64>(Maths::Floor(InVector.Y)) % MwStatics::ChunkSize);
    }

    if (InVector.Z < 0)
    {
        Out.Z = static_cast<LVoxelKeyDomainTy>(MwStatics::ChunkSize + (static_cast<i64>(Maths::Ceil(InVector.Z)) % MwStatics::ChunkSize) - 1);
    }
    else
    {
        Out.Z = static_cast<LVoxelKeyDomainTy>(static_cast<i64>(Maths::Floor(InVector.Z)) % MwStatics::ChunkSize);
    }

    return Out;
}

inline LVoxelKey LVoxelKey::FromWorldLocationPreserveLocalSpace(const LVector& InVector)
{
    check( InVector.X >= std::numeric_limits<LVoxelKeyDomainTy>::min() && InVector.X <= std::numeric_limits<LVoxelKeyDomainTy>::max() )
    check( InVector.Y >= std::numeric_limits<LVoxelKeyDomainTy>::min() && InVector.Y <= std::numeric_limits<LVoxelKeyDomainTy>::max() )
    check( InVector.Z >= std::numeric_limits<LVoxelKeyDomainTy>::min() && InVector.Z <= std::numeric_limits<LVoxelKeyDomainTy>::max() )

    LVoxelKey Out;

    if (InVector.X < 0)
    {
        Out.X = static_cast<LVoxelKeyDomainTy>(Maths::Ceil(InVector.X) - 1);
    }
    else
    {
        Out.X = static_cast<LVoxelKeyDomainTy>(Maths::Floor(InVector.X));
    }

    if (InVector.Y < 0)
    {
        Out.Y = static_cast<LVoxelKeyDomainTy>(Maths::Ceil(InVector.Y) - 1);
    }
    else
    {
        Out.Y = static_cast<LVoxelKeyDomainTy>(Maths::Floor(InVector.Y));
    }

    if (InVector.Z < 0)
    {
        Out.Z = static_cast<LVoxelKeyDomainTy>(Maths::Ceil(InVector.Z) - 1);
    }
    else
    {
        Out.Z = static_cast<LVoxelKeyDomainTy>(Maths::Floor(InVector.Z));
    }

    return Out;
}

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

LVector LVoxelKey::ToWorldSpace()
{
    return LVector
    {
        static_cast<float>(this->Key.X),
        static_cast<float>(this->Key.Y),
        static_cast<float>(this->Key.Z)
    };
}

FORCEINLINE TArray<LVoxelKey> LVoxelKey::GetNeighboringVoxelKeys() const
{
    TArray<LVoxelKey> Out; Out.Reserve(6);

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

FORCEINLINE LString LVoxelKey::ToString() const
{
    return LString::SprintF("{{{},{},{}}}", this->Key.X, this->Key.Y, this->Key.Z);
}

} /* ~Namespace Jafg */

template <>
struct std::hash<::Jafg::LVoxelKey>
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
