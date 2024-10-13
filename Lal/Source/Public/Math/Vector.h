// Copyright mzoesch. All rights reserved.

#pragma once

#include "Math/Vector2.h"

/** Jafg implementation of a vector. */
template <typename T>
struct TVector final
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TVector must be a floating point type.");

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

    /** Global zero vector constant (0, 0, 0). */
    static const TVector<T> ZeroVector;

    /** Global one vector constant (1, 1, 1). */
    static const TVector<T> OneVector;

    /** Global up vector constant (0, 0, 1). */
    static const TVector<T> UpVector;

    /** Global down vector constant (0, 0, -1). */
    static const TVector<T> DownVector;

    /** Global forward vector constant (1, 0, 0). */
    static const TVector<T> ForwardVector;

    /** Global backward vector constant (-1, 0, 0). */
    static const TVector<T> BackwardVector;

    /** Global right vector constant (0, 1, 0). */
    static const TVector<T> RightVector;

    /** Global left vector constant (0, -1, 0). */
    static const TVector<T> LeftVector;

    /** Global unit vector constant along the x-axis (1, 0, 0). */
    static const TVector<T> UnitVectorX;

    /** Global unit vector constant along the y-axis (0, 1, 0). */
    static const TVector<T> UnitVectorY;

    /** Global unit vector constant along the z-axis (0, 0, 1). */
    static const TVector<T> UnitVectorZ;

    static inline TVector<T> Zero()  { return TVector<T>::ZeroVector;  }
    static inline TVector<T> One()   { return TVector<T>::OneVector;   }
    static inline TVector<T> UnitX() { return TVector<T>::UnitVectorX; }
    static inline TVector<T> UnitY() { return TVector<T>::UnitVectorY; }
    static inline TVector<T> UnitZ() { return TVector<T>::UnitVectorZ; }

    FORCEINLINE          TVector<T>()                                       = default;
    FORCEINLINE explicit TVector<T>(const T InFloatingPoint)                : X(InFloatingPoint), Y(InFloatingPoint), Z(InFloatingPoint) { }
    FORCEINLINE explicit TVector<T>(const T InX, const T InY, const T InZ)  : X(InX), Y(InY), Z(InZ) { }
    FORCEINLINE explicit TVector<T>(const TVector2<T> InVec, const T InZ)   : X(InVec.X), Y(InVec.Y), Z(InZ) { }
    FORCEINLINE explicit TVector<T>(const T InXYZ[3])                       : X(InXYZ[0]), Y(InXYZ[1]), Z(InXYZ[2]) { }
    FORCEINLINE explicit TVector<T>(const TVector<T>& InVec)                : X(InVec.X), Y(InVec.Y), Z(InVec.Z) { }
    FORCEINLINE          TVector<T>(TVector<T>&& InVec) noexcept            : X(InVec.X), Y(InVec.Y), Z(InVec.Z) { }
};
