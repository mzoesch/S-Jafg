// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

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

    FORCEINLINE static TVector2<T> Zero()  { return TVector2<T>(TVector2<T>::ZeroVector);  }
    FORCEINLINE static TVector2<T> One()   { return TVector2<T>(TVector2<T>::OneVector);   }
    FORCEINLINE static TVector2<T> Unit()  { return TVector2<T>(TVector2<T>::UnitVector);  }
    FORCEINLINE static TVector2<T> UnitX() { return TVector2<T>(TVector2<T>::UnitVectorX); }
    FORCEINLINE static TVector2<T> UnitY() { return TVector2<T>(TVector2<T>::UnitVectorY); }

    FORCEINLINE          TVector2<T>()                         noexcept = default;
    FORCEINLINE explicit TVector2<T>(const T InFloatingPoint)  noexcept : Y(InFloatingPoint), X(InFloatingPoint) { }
    FORCEINLINE explicit TVector2<T>(const T InX, const T InY) noexcept : X(InX), Y(InY) { }
    FORCEINLINE explicit TVector2<T>(const T InXY[2])          noexcept : X(InXY[0]), Y(InXY[1]) { }
    FORCEINLINE          TVector2<T>(const TVector2<T>& InVec) noexcept : X(InVec.X), Y(InVec.Y) { }
    FORCEINLINE          TVector2<T>(TVector2<T>&& InVec)      noexcept : X(InVec.X), Y(InVec.Y) { }

    FORCEINLINE auto GetData()       noexcept ->       T* { return &this->X; }
    FORCEINLINE auto GetData() const noexcept -> const T* { return &this->X; }

    FORCEINLINE auto operator[](const int32 InIndex)       noexcept ->       T&;
    FORCEINLINE auto operator[](const int32 InIndex) const noexcept -> const T&;

    FORCEINLINE TVector2<T>& operator =(const TVector2<T>&  InVec) noexcept;
    FORCEINLINE TVector2<T>& operator =(      TVector2<T>&& InVec) noexcept;
    FORCEINLINE TVector2<T>& operator =(const TVector2<T>&& InVec) noexcept = delete;

    FORCEINLINE TVector2<T>  operator +(const T            InScalar) const;
    FORCEINLINE TVector2<T>  operator +(const TVector2<T>& InVec   ) const;
    FORCEINLINE TVector2<T>& operator+=(const T            InScalar);
    FORCEINLINE TVector2<T>& operator+=(const TVector2<T>& InVec   );
    FORCEINLINE TVector2<T>  operator -(const T            InScalar) const;
    FORCEINLINE TVector2<T>  operator -(const TVector2<T>& InVec   ) const;
    FORCEINLINE TVector2<T>& operator-=(const T            InScalar);
    FORCEINLINE TVector2<T>& operator-=(const TVector2<T>& InVec   );
    FORCEINLINE TVector2<T>  operator *(const T            InScalar) const;
    FORCEINLINE TVector2<T>  operator *(const TVector2<T>& InVec   ) const;
    FORCEINLINE TVector2<T>& operator*=(const T            InScalar);
    FORCEINLINE TVector2<T>& operator*=(const TVector2<T>& InVec   );
    FORCEINLINE TVector2<T>  operator /(const T            InScalar) const;
    FORCEINLINE TVector2<T>  operator /(const TVector2<T>& InVec   ) const;
    FORCEINLINE TVector2<T>& operator/=(const T            InScalar);
    FORCEINLINE TVector2<T>& operator/=(const TVector2<T>& InVec   );
    FORCEINLINE TVector2<T>  operator -() const { return TVector2<T>(-this->X, -this->Y); }

    FORCEINLINE bool Equals(const TVector2<T>& InVec, const T InTolerance = JAFG_SMALL_NUMBER) const;
    FORCEINLINE bool operator==(const TVector2<T>& InVec) const;
    FORCEINLINE bool operator!=(const TVector2<T>& InVec) const;

    FORCEINLINE bool IsZero() const;
    FORCEINLINE bool IsNearlyZero(const T InTolerance = JAFG_NOT_SO_SMALL_NUMBER) const;

    FORCEINLINE auto Magnitude() const -> T;
    FORCEINLINE auto SquaredMagnitude() const -> T;

    FORCEINLINE auto Invert() -> void;
    FORCEINLINE auto InvertRet() const -> TVector2<T>&;
    FORCEINLINE auto GetInvert() const -> TVector2<T>;
};

template <typename T>
T& TVector2<T>::operator[](const int32 InIndex) noexcept
{
    check( InIndex > INDEX_NONE && InIndex < 2 )
    return this->XY[InIndex];
}

template <typename T>
const T& TVector2<T>::operator[](const int32 InIndex) const noexcept
{
    check( InIndex > INDEX_NONE && InIndex < 2 )
    return this->XY[InIndex];
}

template <typename T>
TVector2<T>& TVector2<T>::operator=(const TVector2<T>& InVec) noexcept
{
    this->X = InVec.X;
    this->Y = InVec.Y;
    return *this;
}

template <typename T>
TVector2<T>& TVector2<T>::operator=(TVector2<T>&& InVec) noexcept
{
    this->X = InVec.X;
    this->Y = InVec.Y;
    return *this;
}

template <typename T>
TVector2<T> TVector2<T>::operator+(const T InScalar) const
{
    return TVector2<T>(this->X + InScalar, this->Y + InScalar);
}

template <typename T>
TVector2<T> TVector2<T>::operator+(const TVector2<T>& InVec) const
{
    return TVector2<T>(this->X + InVec.X, this->Y + InVec.Y);
}

template <typename T>
TVector2<T>& TVector2<T>::operator+=(const T InScalar)
{
    this->X += InScalar;
    this->Y += InScalar;
    return *this;
}

template <typename T>
TVector2<T>& TVector2<T>::operator+=(const TVector2<T>& InVec)
{
    this->X += InVec.X;
    this->Y += InVec.Y;
    return *this;
}

template <typename T>
TVector2<T> TVector2<T>::operator-(const T InScalar) const
{
    return TVector2<T>(this->X - InScalar, this->Y - InScalar);
}

template <typename T>
TVector2<T> TVector2<T>::operator-(const TVector2<T>& InVec) const
{
    return TVector2<T>(this->X - InVec.X, this->Y - InVec.Y);
}

template <typename T>
TVector2<T>& TVector2<T>::operator-=(const T InScalar)
{
    this->X -= InScalar;
    this->Y -= InScalar;
    return *this;
}

template <typename T>
TVector2<T>& TVector2<T>::operator-=(const TVector2<T>& InVec)
{
    this->X -= InVec.X;
    this->Y -= InVec.Y;
    return *this;
}

template <typename T>
TVector2<T> TVector2<T>::operator*(const T InScalar) const
{
    return TVector2<T>(this->X * InScalar, this->Y * InScalar);
}

template <typename T>
TVector2<T> TVector2<T>::operator*(const TVector2<T>& InVec) const
{
    return TVector2<T>(this->X * InVec.X, this->Y * InVec.Y);
}

template <typename T>
TVector2<T>& TVector2<T>::operator*=(const T InScalar)
{
    this->X *= InScalar;
    this->Y *= InScalar;
    return *this;
}

template <typename T>
TVector2<T>& TVector2<T>::operator*=(const TVector2<T>& InVec)
{
    this->X *= InVec.X;
    this->Y *= InVec.Y;
    return *this;
}

template <typename T>
TVector2<T> TVector2<T>::operator/(const T InScalar) const
{
    return TVector2<T>(this->X / InScalar, this->Y / InScalar);
}

template <typename T>
TVector2<T> TVector2<T>::operator/(const TVector2<T>& InVec) const
{
    return TVector2<T>(this->X / InVec.X, this->Y / InVec.Y);
}

template <typename T>
TVector2<T>& TVector2<T>::operator/=(const T InScalar)
{
    this->X /= InScalar;
    this->Y /= InScalar;
    return *this;
}

template <typename T>
TVector2<T>& TVector2<T>::operator/=(const TVector2<T>& InVec)
{
    this->X /= InVec.X;
    this->Y /= InVec.Y;
    return *this;
}

template <typename T>
bool TVector2<T>::Equals(const TVector2<T>& InVec, const T InTolerance) const
{
    return Maths::Absolute(this->X - InVec.X) < InTolerance
        && Maths::Absolute(this->Y - InVec.Y) < InTolerance;
}

template <typename T>
bool TVector2<T>::operator==(const TVector2<T>& InVec) const
{
    return this->X == InVec.X && this->Y == InVec.Y;
}

template <typename T>
bool TVector2<T>::operator!=(const TVector2<T>& InVec) const
{
    return !(*this == InVec);
}

template <typename T>
bool TVector2<T>::IsZero() const
{
    return this->X == 0.0f && this->Y == 0.0f;
}

template <typename T>
bool TVector2<T>::IsNearlyZero(const T InTolerance) const
{
    return Maths::Absolute(this->X) < InTolerance
        && Maths::Absolute(this->Y) < InTolerance;
}

template <typename T>
T TVector2<T>::Magnitude() const
{
    return Maths::Sqrt(this->X * this->X + this->Y * this->Y);
}

template <typename T>
T TVector2<T>::SquaredMagnitude() const
{
    return this->X * this->X + this->Y * this->Y;
}

template <typename T>
void TVector2<T>::Invert()
{
    this->X = -this->X;
    this->Y = -this->Y;
    return;
}

template <typename T>
TVector2<T>& TVector2<T>::InvertRet() const
{
    this->Invert();
    return *this;
}

template <typename T>
TVector2<T> TVector2<T>::GetInvert() const
{
    return TVector2<T>(-this->X, -this->Y);
}

} /* ~Namespace Jafg */
