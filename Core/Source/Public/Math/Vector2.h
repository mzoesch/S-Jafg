// Copyright mzoesch. All rights reserved.

#pragma once

/** Jafg implementation of a vector2. */
template<typename T>
struct TVector2 final
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TVector2 must be a floating point type.");

    union
    {
        struct
        {
            T X;
            T Y;
        };

        T XY[2];
    };

    /** Global 2d zero vector constant (0, 0). */
    static const TVector2<T> ZeroVector;

    /** Global 2d one vector constant (1, 1). */
    static const TVector2<T> OneVector;

    /**
     * Global 2d unit vector constant along the 45 degree angle or symmetrical
     * positive axes (sqrt(.5), sqrt(.5)) or (0.707, 0.707).
     * @see https://en.wikipedia.org/wiki/Unit_vector
     */
    static const TVector2<T> UnitVector;

    /** Global 2d unit vector constant along the x-axis (1, 0). */
    static const TVector2<T> UnitVectorX;

    /** Global 2d unit vector constant along the y-axis (0, 1). */
    static const TVector2<T> UnitVectorY;

    static inline TVector2<T> Zero()  { return TVector2<T>::ZeroVector;  }
    static inline TVector2<T> One()   { return TVector2<T>::OneVector;   }
    static inline TVector2<T> Unit()  { return TVector2<T>::UnitVector;  }
    static inline TVector2<T> UnitX() { return TVector2<T>::UnitVectorX; }
    static inline TVector2<T> UnitY() { return TVector2<T>::UnitVectorY; }

    FORCEINLINE          TVector2<T>()                              = default;
    FORCEINLINE explicit TVector2<T>(const T InFloatingPoint)       : X(InFloatingPoint), Y(InFloatingPoint)    { }
    FORCEINLINE explicit TVector2<T>(const T InX, const T InY)      : X(InX), Y(InY)                            { }
    FORCEINLINE explicit TVector2<T>(const T InXY[2])               : X(InXY[0]), Y(InXY[1])                    { }
    FORCEINLINE explicit TVector2<T>(const TVector2<T>& InVec)      : X(InVec.X), Y(InVec.Y)                    { }
    FORCEINLINE          TVector2<T>(TVector2<T>&& InVec) noexcept  : X(InVec.X), Y(InVec.Y)                    { }
};
