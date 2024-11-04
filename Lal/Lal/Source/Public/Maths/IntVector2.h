// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/** Jafg implementation of an integral Vector2. */
template <typename T>
struct TIntVector2 final
{
    static_assert(std::is_integral_v<T>, "Generic type T of TIntVector2 must be an integral type.");

    union
    {
        struct
        {
            T X;
            T Y;
        };

        T XY[2];
    };

    /** Global zero vector constant (0, 0). */
    static const TIntVector2<T> ZeroVector;

    /** Global one vector constant (1, 1). */
    static const TIntVector2<T> OneVector;

    static FORCEINLINE TIntVector2<T> Zero() { return TIntVector2<T>::ZeroVector; }
    static FORCEINLINE TIntVector2<T> One()  { return TIntVector2<T>::OneVector;  }

    FORCEINLINE          TIntVector2<T>()                                       = default;
    FORCEINLINE explicit TIntVector2<T>(const T InIntegral) noexcept            : X(InIntegral), Y(InIntegral)  { }
    FORCEINLINE explicit TIntVector2<T>(const T InX, const T InY) noexcept      : X(InX), Y(InY)                { }
    FORCEINLINE explicit TIntVector2<T>(const T InXY[2]) noexcept               : X(InXY[0]), Y(InXY[1])        { }
    FORCEINLINE          TIntVector2<T>(TIntVector2<T>& InVec) noexcept         : X(InVec.X), Y(InVec.Y)        { }
    FORCEINLINE          TIntVector2<T>(const TIntVector2<T>& InVec) noexcept   : X(InVec.X), Y(InVec.Y)        { }
    FORCEINLINE          TIntVector2<T>(TIntVector2<T>&& InVec) noexcept        : X(InVec.X), Y(InVec.Y)        { }
    FORCEINLINE          TIntVector2<T>(const TIntVector2<T>&& InVec) noexcept  : X(InVec.X), Y(InVec.Y)        { }

    FORCEINLINE TIntVector2<T>& operator=(const TIntVector2<T>& InVec) noexcept { X = InVec.X; Y = InVec.Y; return *this; }
    FORCEINLINE TIntVector2<T>& operator=(TIntVector2<T>&& InVec) noexcept      { X = InVec.X; Y = InVec.Y; return *this; }

    FORCEINLINE       T& operator[](const uint8 Index)       { return XY[Index]; }
    FORCEINLINE const T& operator[](const uint8 Index) const { return XY[Index]; }
    FORCEINLINE bool     operator==(const TIntVector2<T>& Other) const { return X == Other.X && Y == Other.Y; }
    FORCEINLINE bool     operator!=(const TIntVector2<T>& Other) const { return X != Other.X || Y != Other.Y; }
};

} /* ~Namespace Jafg */
