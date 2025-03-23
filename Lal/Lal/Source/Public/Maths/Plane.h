// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/** Jafg implementation of a plane. */
template <typename T>
struct TPlane final : public TVector<T>
{
    T W;

    /** Global zero plane constant (0, 0, 0, 0). */
    static const TPlane<T> ZeroPlane;

    /** Global one plane constant (1, 1, 1, 1). */
    static const TPlane<T> OnePlane;

    /** Global unit plane constant along the x-axis (1, 0, 0, 0). */
    static const TPlane<T> UnitPlaneX;

    /** Global unit plane constant along the y-axis (0, 1, 0, 0). */
    static const TPlane<T> UnitPlaneY;

    /** Global unit plane constant along the z-axis (0, 0, 1, 0). */
    static const TPlane<T> UnitPlaneZ;

    /** Global unit plane constant along the w-axis (0, 0, 0, 1). */
    static const TPlane<T> UnitPlaneW;

    FORCEINLINE static TPlane<T> Zero()  { return TPlane<T>::ZeroPlane;  }
    FORCEINLINE static TPlane<T> One()   { return TPlane<T>::OnePlane;   }
    FORCEINLINE static TPlane<T> UnitX() { return TPlane<T>::UnitPlaneX; }
    FORCEINLINE static TPlane<T> UnitY() { return TPlane<T>::UnitPlaneY; }
    FORCEINLINE static TPlane<T> UnitZ() { return TPlane<T>::UnitPlaneZ; }
    FORCEINLINE static TPlane<T> UnitW() { return TPlane<T>::UnitPlaneW; }

    FORCEINLINE          TPlane() = default;
    FORCEINLINE explicit TPlane(const TVector4<T>& InVec);
    FORCEINLINE explicit TPlane(T InX, T InY, T InZ, T InW);
    FORCEINLINE explicit TPlane(TVector<T> InNormal, T InW);
    FORCEINLINE explicit TPlane(TVector<T> InBase, const TVector<T>& InNormal);
    FORCEINLINE explicit TPlane(TVector<T> A, TVector<T> B, TVector<T> C);
    FORCEINLINE          TPlane(const TPlane<T>& InPlane) : TVector<T>(InPlane), W(InPlane.W) { }
    FORCEINLINE          TPlane(TPlane<T>&& InPlane) noexcept : TVector<T>(InPlane), W(InPlane.W) { }

    FORCEINLINE TVector<T> GetPlaneNormal() const { return TVector<T>(*this); }
};

template <typename T>
TPlane<T>::TPlane(const TVector4<T>& InVec) : TVector<T>(InVec), W(InVec.W)
{
}

template <typename T>
TPlane<T>::TPlane(T InX, T InY, T InZ, T InW) : TVector<T>(InX, InY, InZ), W(InW)
{
}

template <typename T>
TPlane<T>::TPlane(TVector<T> InNormal, T InW) : TVector<T>(InNormal), W(InW)
{
}

template <typename T>
TPlane<T>::TPlane(TVector<T> InBase, const TVector<T>& InNormal) : TVector<T>(InNormal), W(InBase | InNormal)
{
}

template <typename T>
TPlane<T>::TPlane(TVector<T> A, TVector<T> B, TVector<T> C) : TVector<T>(((B - A).Corss((C - A))).GetSafeNormal())
{
    W = A | (TVector<T>)(*this);
}

} /* ~Namespace Jafg */
