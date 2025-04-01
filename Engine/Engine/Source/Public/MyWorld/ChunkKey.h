// Copyright mzoesch. All rights reserved.

#pragma once

#include "MyWorld/MyWorldStatics.h"

namespace Jafg
{

typedef LIntVector  LChunkKeyTy;
typedef LIntVector2 LChunkKey2Ty;
typedef i32       LChunkKeyDomainTy;

struct LChunkKey;
struct LChunkKey2;

//#
//# Unique identifier for a chunk.
//#
struct LChunkKey final
{
    using LReal = float;

    static_assert(sizeof(LChunkKeyTy) == sizeof(LChunkKeyDomainTy) * 3, "LChunkKeyTy is not tightly packed.");

    enum EAxis : u8
    {
        None = 0x00 << 0,
        Xx   = 0x01 << 0,
        Yx   = 0x01 << 1,
        Zx   = 0x01 << 2,
        XYx  = Xx | Yx,
        XZx  = Xx | Zx,
        YZx  = Yx | Zx,
        XYZx = Xx | Yx | Zx,
    };

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
    FORCEINLINE LChunkKey(const LChunkKeyDomainTy InX, const LChunkKeyDomainTy InY, const LChunkKeyDomainTy InZ) : Key(InX, InY, InZ) { }
    FORCEINLINE LChunkKey(const LChunkKeyDomainTy InXyz) : Key(InXyz, InXyz, InXyz) { }
    FORCEINLINE LChunkKey(const LChunkKey& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE LChunkKey(LChunkKey&& InKey) noexcept : Key(std::move(InKey.Key)) { }
    FORCEINLINE LChunkKey(const LVector& InVec) noexcept;
    FORCEINLINE LChunkKey(const LChunkKey2& InKey2, const LChunkKeyDomainTy InZ);
    FORCEINLINE ~LChunkKey() = default;

    FORCEINLINE LChunkKey2 XY() const;

    FORCEINLINE LChunkKey& operator=(const LChunkKey&  InKey) noexcept;
    FORCEINLINE LChunkKey& operator=(      LChunkKey&& InKey) noexcept;

    FORCEINLINE bool Equals(const LChunkKey& InKey, const LChunkKey::EAxis InAxis = LChunkKey::EAxis::XYZx) const;
    FORCEINLINE bool operator==(const LChunkKey& InKey) const { return Key == InKey.Key; }
    FORCEINLINE bool operator!=(const LChunkKey& InKey) const { return Key != InKey.Key; }

    FORCEINLINE bool operator <(const LChunkKey& InKey) const;
    FORCEINLINE bool operator >(const LChunkKey& InKey) const;
    FORCEINLINE bool operator<=(const LChunkKey& InKey) const;
    FORCEINLINE bool operator>=(const LChunkKey& InKey) const;

    FORCEINLINE LVector ToWorldSpace() const
    {
        return LVector
        {
            static_cast<LReal>(Key.X * MwStatics::ChunkSize),
            static_cast<LReal>(Key.Y * MwStatics::ChunkSize),
            static_cast<LReal>(Key.Z * MwStatics::ChunkSize)
        };
    }
    FORCEINLINE auto ToVerticalKey() const -> LChunkKey2;

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

    FORCEINLINE LSimpleString ToString() const
    {
        return LSimpleString::SprintF("{{{}, {}, {}}}", Key.X, Key.Y, Key.Z);
    }
};
static_assert(sizeof(LChunkKey) == sizeof(LChunkKeyDomainTy) * 3, "LChunkKey is not tightly packed.");

ENUM_CLASS_FLAGS(LChunkKey::EAxis)

//# Unique identifier for a vertical chunk.
struct LChunkKey2 final
{
    using LReal = float;

    static_assert(sizeof(LChunkKey2Ty) == sizeof(LChunkKeyDomainTy) * 2, "LChunkKey2Ty is not tightly packed.");

    union
    {
        struct
        {
            LChunkKeyDomainTy X;
            LChunkKeyDomainTy Y;
        };

        LChunkKey2Ty Key;
    };

    FORCEINLINE LChunkKey2() = default;
    FORCEINLINE LChunkKey2(const LChunkKey2Ty& InKey) : Key(InKey) { }
    FORCEINLINE LChunkKey2(const LChunkKeyDomainTy InX, const LChunkKeyDomainTy InY) : Key(InX, InY) { }
    FORCEINLINE LChunkKey2(const LChunkKeyDomainTy InXy) : Key(InXy, InXy) { }
    FORCEINLINE LChunkKey2(const LChunkKey2& InKey) noexcept : Key(InKey.Key) { }
    FORCEINLINE LChunkKey2(LChunkKey2&& InKey) noexcept : Key(std::move(InKey.Key)) { }
    FORCEINLINE ~LChunkKey2() = default;

    FORCEINLINE LChunkKey2& operator=(const LChunkKey2&  InKey) noexcept;
    FORCEINLINE LChunkKey2& operator=(      LChunkKey2&& InKey) noexcept;
    FORCEINLINE bool operator==(const LChunkKey2& InKey) const { return Key == InKey.Key; }
    FORCEINLINE bool operator!=(const LChunkKey2& InKey) const { return Key != InKey.Key; }

    FORCEINLINE auto ToWorldSpaceVector() const -> LVector;

    FORCEINLINE auto GetNorthKey() const -> LChunkKey2 { return { Key.X + 1, Key.Y }; }
    FORCEINLINE auto GetSouthKey() const -> LChunkKey2 { return { Key.X - 1, Key.Y }; }
    FORCEINLINE auto GetEastKey()  const -> LChunkKey2 { return { Key.X, Key.Y + 1 }; }
    FORCEINLINE auto GetWestKey()  const -> LChunkKey2 { return { Key.X, Key.Y - 1 }; }
    FORCEINLINE auto GetNeighboringChunkKeys() const -> TArray<LChunkKey2>
    {
        TArray<LChunkKey2> Out; Out.Reserve(4);

        Out.Emplace(Key.X + 1, Key.Y    );
        Out.Emplace(Key.X - 1, Key.Y    );
        Out.Emplace(Key.X,     Key.Y + 1);
        Out.Emplace(Key.X,     Key.Y - 1);

        return Out;
    }

    FORCEINLINE LSimpleString ToString() const
    {
        return LSimpleString::SprintF("{{{}, {}}}", Key.X, Key.Y);
    }
};
static_assert(sizeof(LChunkKey2) == sizeof(LChunkKeyDomainTy) * 2, "LChunkKey2 is not tightly packed.");

LChunkKey::LChunkKey(const LVector& InVec) noexcept
{
    this->Key.X = InVec.X < 0.0f
        ? static_cast<LChunkKeyDomainTy>(Maths::Floor(InVec.X / MwStatics::ChunkSize))
        : static_cast<LChunkKeyDomainTy>(InVec.X / MwStatics::ChunkSize);
    this->Key.Y = InVec.Y < 0.0f
        ? static_cast<LChunkKeyDomainTy>(Maths::Floor(InVec.Y / MwStatics::ChunkSize))
        : static_cast<LChunkKeyDomainTy>(InVec.Y / MwStatics::ChunkSize);
    this->Key.Z = InVec.Z < 0.0f
        ? static_cast<LChunkKeyDomainTy>(Maths::Floor(InVec.Z / MwStatics::ChunkSize))
        : static_cast<LChunkKeyDomainTy>(InVec.Z / MwStatics::ChunkSize);

    return;
}

LChunkKey::LChunkKey(const LChunkKey2& InKey2, const LChunkKeyDomainTy InZ)
{
    this->Key.X = InKey2.X;
    this->Key.Y = InKey2.Y;
    this->Key.Z = InZ;
    return;
}

LChunkKey2 LChunkKey::XY() const
{
    return { Key.X, Key.Y };
}

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

bool LChunkKey::Equals(const LChunkKey& InKey, const LChunkKey::EAxis InAxis /* = LChunkKey::EAxis::XYZx */) const
{
    return ((InAxis & LChunkKey::EAxis::Xx) ? Key.X == InKey.X : true)
        && ((InAxis & LChunkKey::EAxis::Yx) ? Key.Y == InKey.Y : true)
        && ((InAxis & LChunkKey::EAxis::Zx) ? Key.Z == InKey.Z : true);
}

bool LChunkKey::operator<(const LChunkKey& InKey) const
{
    return  Key.X < InKey.X
        || (Key.X == InKey.X && Key.Y < InKey.Y)
        || (Key.X == InKey.X && Key.Y == InKey.Y && Key.Z < InKey.Z);
}

bool LChunkKey::operator>(const LChunkKey& InKey) const
{
    return  Key.X > InKey.X
        || (Key.X == InKey.X && Key.Y > InKey.Y)
        || (Key.X == InKey.X && Key.Y == InKey.Y && Key.Z > InKey.Z);
}

bool LChunkKey::operator<=(const LChunkKey& InKey) const
{
    return  Key.X <= InKey.X
        || (Key.X == InKey.X && Key.Y <= InKey.Y)
        || (Key.X == InKey.X && Key.Y == InKey.Y && Key.Z <= InKey.Z);
}

bool LChunkKey::operator>=(const LChunkKey& InKey) const
{
    return  Key.X >= InKey.X
        || (Key.X == InKey.X && Key.Y >= InKey.Y)
        || (Key.X == InKey.X && Key.Y == InKey.Y && Key.Z >= InKey.Z);
}

LChunkKey2 LChunkKey::ToVerticalKey() const
{
    return { this->Key.X, this->Key.Y };
}

LChunkKey2& LChunkKey2::operator=(const LChunkKey2& InKey) noexcept
{
    this->Key = InKey.Key;
    return *this;
}

LChunkKey2& LChunkKey2::operator=(LChunkKey2&& InKey) noexcept
{
    this->Key = InKey.Key;
    return *this;
}

LVector LChunkKey2::ToWorldSpaceVector() const
{
    return LVector
    {
        static_cast<LReal>(Key.X * MwStatics::ChunkSize),
        static_cast<LReal>(Key.Y * MwStatics::ChunkSize),
        0.0f
    };
}

} /* ~Namespace Jafg */

template <>
struct std::hash<::Jafg::LChunkKey>
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

template <>
struct std::hash<::Jafg::LChunkKey2>
{
    FORCEINLINE ::std::size_t operator()(const ::Jafg::LChunkKey2& InKey) const noexcept
    {
        const std::size_t HashX = std::hash<::Jafg::LChunkKeyDomainTy>()(InKey.X);
        const std::size_t HashY = std::hash<::Jafg::LChunkKeyDomainTy>()(InKey.Y);

        std::size_t FinalHash = HashX;
        FinalHash ^= HashY + 0x9e3779b9 + (FinalHash << 6) + (FinalHash >> 2);

        return FinalHash;
    }
};
