// Copyright mzoesch. All rights reserved.

#pragma once

#include "Math/IntVector2.h"

/** Jafg implementation of an integral Vector. */
template <typename T>
struct TIntVector final
{
    static_assert(std::is_integral_v<T>, "Generic type T of TIntVector must be an integral type.");

    union
    {
        struct
        {
            T X;
            T Y;
            T Z;
        };

        T XYZ[3];
    };

    FORCEINLINE          TIntVector<T>()                                       = default;
    FORCEINLINE explicit TIntVector<T>(const T InIntegral) noexcept            : X(InIntegral), Y(InIntegral), Z(InIntegral)  { }
    FORCEINLINE explicit TIntVector<T>(const T InX, const T InY, const T InZ) noexcept      : X(InX), Y(InY), Z(InZ)                { }
    FORCEINLINE explicit TIntVector<T>(const TIntVector2<T> InVec, const T InZ) noexcept      : X(InVec.X), Y(InVec.Y), Z(InZ)                { }
    FORCEINLINE explicit TIntVector<T>(const T InXYZ[3]) noexcept               : X(InXYZ[0]), Y(InXYZ[1]), Z(InXYZ[2])        { }
    FORCEINLINE          TIntVector<T>(TIntVector<T>& InVec) noexcept         : X(InVec.X), Y(InVec.Y), Z(InVec.Z)        { }
    FORCEINLINE          TIntVector<T>(TIntVector<T>&& InVec) noexcept   : X(InVec.X), Y(InVec.Y), Z(InVec.Z)        { }
};
