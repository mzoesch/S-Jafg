// Copyright mzoesch. All rights reserved.

#pragma once

/** Jafg implementation of an integral Vector2. */
template<typename T>
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

    FORCEINLINE          TIntVector2<T>()                                   = default;
    FORCEINLINE explicit TIntVector2<T>(const T InIntegral)                 : X(InIntegral), Y(InIntegral)  { }
    FORCEINLINE explicit TIntVector2<T>(const T InX, const T InY)           : X(InX), Y(InY)                { }
    FORCEINLINE explicit TIntVector2<T>(const T InXY[2])                    : X(InXY[0]), Y(InXY[1])        { }
    FORCEINLINE explicit TIntVector2<T>(const TIntVector2<T>& InVec)        : X(InVec.X), Y(InVec.Y)        { }
    FORCEINLINE          TIntVector2<T>(TIntVector2<T>&& InVec) noexcept    : X(InVec.X), Y(InVec.Y)        { }
};
