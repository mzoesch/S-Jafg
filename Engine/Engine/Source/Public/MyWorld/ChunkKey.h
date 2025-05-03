// Copyright mzoesch. All rights reserved.

#pragma once

#include "MyWorld/MyWorldStatics.h"

namespace Jafg
{

typedef Li32Vector  LChunkKeyTy;
typedef Li32Vector2 LChunkKey2Ty;
typedef i32         LChunkKeyDomain;

struct LChunkKey;

//#
//# Unique identifier for a chunk.
//#
struct LChunkKey final
{
    using LReal = f32;

    static_assert(sizeof(LChunkKeyTy) == sizeof(LChunkKeyDomain) * 3, "LChunkKeyTy is not tightly packed.");

    union
    {
        struct
        {
            LChunkKeyDomain X;
            LChunkKeyDomain Y;
            LChunkKeyDomain Z;
        };

        LChunkKeyTy Key;
    };

    FORCEINLINE LChunkKey() = default;
    FORCEINLINE LChunkKey(const LChunkKeyTy& InKey) noexcept : Key(InKey) { }
    FORCEINLINE LChunkKey(const LChunkKeyDomain InX, const LChunkKeyDomain InY, const LChunkKeyDomain InZ) noexcept : Key(InX, InY, InZ) { }
    FORCEINLINE LChunkKey(const LChunkKeyDomain InXyz) noexcept : Key(InXyz) { }
    FORCEINLINE LChunkKey(const LChunkKey& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE LChunkKey(LChunkKey&& InKey) noexcept : Key(std::move(InKey.Key)) { }
    FORCEINLINE LChunkKey(const LVector& InVec) noexcept;
    FORCEINLINE ~LChunkKey() = default;

    FORCEINLINE LChunkKey& operator=(const LChunkKey&  InKey) noexcept { this->Key = InKey.Key; return *this; }
    FORCEINLINE LChunkKey& operator=(      LChunkKey&& InKey) noexcept { this->Key = std::move(InKey.Key); return *this; }

    FORCEINLINE LChunkKey2Ty XY() const noexcept { return { this->X, this->Y }; }

    FORCEINLINE bool Equals(const LChunkKey& InKey) const { return this->Key == InKey.Key; }
    FORCEINLINE bool operator==(const LChunkKey& InKey) const { return this->Key == InKey.Key; }
    FORCEINLINE bool operator!=(const LChunkKey& InKey) const { return this->Key != InKey.Key; }

    FORCEINLINE bool operator >(const LChunkKey& InKey) const noexcept;
    FORCEINLINE bool operator <(const LChunkKey& InKey) const noexcept;
    FORCEINLINE bool operator>=(const LChunkKey& InKey) const noexcept;
    FORCEINLINE bool operator<=(const LChunkKey& InKey) const noexcept;

    FORCEINLINE LVector ToWorldSpace() const
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
    FORCEINLINE auto GetNeighboringChunkKeys() const -> TArray<LChunkKey>
    {
        TArray<LChunkKey> Out; Out.Reserve(6);

        Out.Emplace(Key.X + 1, Key.Y,     Key.Z    );
        Out.Emplace(Key.X - 1, Key.Y,     Key.Z    );
        Out.Emplace(Key.X,     Key.Y + 1, Key.Z    );
        Out.Emplace(Key.X,     Key.Y - 1, Key.Z    );
        Out.Emplace(Key.X,     Key.Y,     Key.Z + 1);
        Out.Emplace(Key.X,     Key.Y,     Key.Z - 1);

        return Out;
    }

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("{{{}, {}, {}}}", this->Key.X, this->Key.Y, this->Key.Z);
    }
};
static_assert(sizeof(LChunkKey) == sizeof(LChunkKeyDomain) * 3, "LChunkKey is not tightly packed.");

FORCEINLINE LChunkKey::LChunkKey(const LVector& InVec) noexcept
{
    this->Key.X = InVec.X < 0.0f
        ? static_cast<LChunkKeyDomain>(Maths::Floor(InVec.X / MwStatics::ChunkSize))
        : static_cast<LChunkKeyDomain>(InVec.X / MwStatics::ChunkSize);
    this->Key.Y = InVec.Y < 0.0f
        ? static_cast<LChunkKeyDomain>(Maths::Floor(InVec.Y / MwStatics::ChunkSize))
        : static_cast<LChunkKeyDomain>(InVec.Y / MwStatics::ChunkSize);
    this->Key.Z = InVec.Z < 0.0f
        ? static_cast<LChunkKeyDomain>(Maths::Floor(InVec.Z / MwStatics::ChunkSize))
        : static_cast<LChunkKeyDomain>(InVec.Z / MwStatics::ChunkSize);

    return;
}

FORCEINLINE bool LChunkKey::operator<(const LChunkKey& InKey) const noexcept
{
    return  this->Key.X < InKey.X
        || (this->Key.X == InKey.X && this->Key.Y < InKey.Y)
        || (this->Key.X == InKey.X && this->Key.Y == InKey.Y && this->Key.Z < InKey.Z);
}

FORCEINLINE bool LChunkKey::operator>(const LChunkKey& InKey) const noexcept
{
    return  this->Key.X > InKey.X
        || (this->Key.X == InKey.X && this->Key.Y > InKey.Y)
        || (this->Key.X == InKey.X && this->Key.Y == InKey.Y && this->Key.Z > InKey.Z);
}

FORCEINLINE bool LChunkKey::operator<=(const LChunkKey& InKey) const noexcept
{
    return  this->Key.X <= InKey.X
        || (this->Key.X == InKey.X && this->Key.Y <= InKey.Y)
        || (this->Key.X == InKey.X && this->Key.Y == InKey.Y && this->Key.Z <= InKey.Z);
}

FORCEINLINE bool LChunkKey::operator>=(const LChunkKey& InKey) const noexcept
{
    return  this->Key.X >= InKey.X
        || (this->Key.X == InKey.X && this->Key.Y >= InKey.Y)
        || (this->Key.X == InKey.X && this->Key.Y == InKey.Y && this->Key.Z >= InKey.Z);
}

} /* ~Namespace Jafg */

template <>
struct std::hash<::Jafg::LChunkKey>
{
    FORCEINLINE ::std::size_t operator()(const ::Jafg::LChunkKey& InKey) const noexcept
    {
        const std::size_t HashX = std::hash<::Jafg::LChunkKeyDomain>()(InKey.X);
        const std::size_t HashY = std::hash<::Jafg::LChunkKeyDomain>()(InKey.Y);
        const std::size_t HashZ = std::hash<::Jafg::LChunkKeyDomain>()(InKey.Z);

        std::size_t FinalHash = HashX;
        FinalHash ^= HashY + 0x9e3779b9 + (FinalHash << 6) + (FinalHash >> 2);
        FinalHash ^= HashZ + 0x9e3779b9 + (FinalHash << 6) + (FinalHash >> 2);

        return FinalHash;
    }
};
