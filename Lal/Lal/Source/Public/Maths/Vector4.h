// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/** Jafg implementation of a vector4. */
template <typename T>
struct TVector4 final
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TVector4 must be a floating point type.");

    union
    {
        struct
        {
            T X;
            T Y;
            T Z;
            T W;
        };

        T XYZW[4];
    };

    /** Global zero vector constant (0, 0, 0, 0). */
    static const TVector4<T> ZeroVector;

    /** Global identity vector constant (0, 0, 0, 1). */
    static const TVector4<T> IdentityVector;

    /** Global one vector constant (1, 1, 1, 1). */
    static const TVector4<T> OneVector;

    /** Global unit vector constant along the x-axis (1, 0, 0, 0). */
    static const TVector4<T> UnitVectorX;

    /** Global unit vector constant along the y-axis (0, 1, 0, 0). */
    static const TVector4<T> UnitVectorY;

    /** Global unit vector constant along the z-axis (0, 0, 1, 0). */
    static const TVector4<T> UnitVectorZ;

    /** Global unit vector constant along the w-axis (0, 0, 0, 1). */
    static const TVector4<T> UnitVectorW;

    FORCEINLINE static TVector4<T> Zero()     { return TVector4<T>::ZeroVector;     }
    FORCEINLINE static TVector4<T> Identity() { return TVector4<T>::IdentityVector; }
    FORCEINLINE static TVector4<T> One()      { return TVector4<T>::OneVector;      }
    FORCEINLINE static TVector4<T> UnitX()    { return TVector4<T>::UnitVectorX;    }
    FORCEINLINE static TVector4<T> UnitY()    { return TVector4<T>::UnitVectorY;    }
    FORCEINLINE static TVector4<T> UnitZ()    { return TVector4<T>::UnitVectorZ;    }
    FORCEINLINE static TVector4<T> UnitW()    { return TVector4<T>::UnitVectorW;    }

    FORCEINLINE          TVector4() : X(0.0f), Y(0.0f), Z(0.0f), W(1.0f) { }
    FORCEINLINE explicit TVector4(const T InX, const T InY, const T InZ, const T InW = 1.0f);
    FORCEINLINE explicit TVector4(const TVector<T> InVec, const T InW) : X(InVec.X), Y(InVec.Y), Z(InVec.Z), W(InW) { }
    FORCEINLINE          TVector4(const TVector4<T>& InVector) : X(InVector.X), Y(InVector.Y), Z(InVector.Z), W(InVector.W) { }
    FORCEINLINE          TVector4(TVector4<T>&& InVector) noexcept : X(InVector.X), Y(InVector.Y), Z(InVector.Z), W(InVector.W) { }

    FORCEINLINE auto GetData()       ->       T* { return &this->X; }
    FORCEINLINE auto GetData() const -> const T* { return &this->X; }

    FORCEINLINE auto operator[](const int32 InIndex)       ->       T&;
    FORCEINLINE auto operator[](const int32 InIndex) const -> const T&;

    FORCEINLINE TVector4<T>& operator =(const TVector4<T>& InVector)  noexcept;
    FORCEINLINE TVector4<T>& operator =(      TVector4<T>&& InVector) noexcept;
    FORCEINLINE TVector4<T>& operator =(const TVector4<T>&& InVector) noexcept = delete;

    FORCEINLINE TVector4<T>  operator +(const T            InScalar) const;
    FORCEINLINE TVector4<T>  operator +(const TVector4<T>& InVector) const;
    FORCEINLINE TVector4<T>& operator+=(const T            InScalar);
    FORCEINLINE TVector4<T>& operator+=(const TVector4<T>& InVector);
    FORCEINLINE TVector4<T>  operator -(const T            InScalar) const;
    FORCEINLINE TVector4<T>  operator -(const TVector4<T>& InVector) const;
    FORCEINLINE TVector4<T>& operator-=(const T            InScalar);
    FORCEINLINE TVector4<T>& operator-=(const TVector4<T>& InVector);
    FORCEINLINE TVector4<T>  operator *(const T            InScalar) const;
    FORCEINLINE TVector4<T>  operator *(const TVector4<T>& InVector) const;
    FORCEINLINE TVector4<T>& operator*=(const T            InScalar);
    FORCEINLINE TVector4<T>& operator*=(const TVector4<T>& InVector);
    FORCEINLINE TVector4<T>  operator /(const T            InScalar) const;
    FORCEINLINE TVector4<T>  operator /(const TVector4<T>& InVector) const;
    FORCEINLINE TVector4<T>& operator/=(const T            InScalar);
    FORCEINLINE TVector4<T>& operator/=(const TVector4<T>& InVector);
    FORCEINLINE TVector4<T>  operator -() const;

    FORCEINLINE bool Equals(const TVector4<T>& InVector, const T InTolerance = JAFG_SMALL_NUMBER) const;
    FORCEINLINE bool operator==(const TVector4<T>& InVector) const;
    FORCEINLINE bool operator!=(const TVector4<T>& InVector) const;

    FORCEINLINE auto IsZero3() const -> bool;
    FORCEINLINE auto IsNearlyZero3(const T InTolerance = JAFG_SMALL_NUMBER) const -> bool;

    FORCEINLINE auto Magnitude() const -> T;
    FORCEINLINE auto SquaredMagnitude() const -> T;
    FORCEINLINE auto Magnitude3() const -> T;
    FORCEINLINE auto SquaredMagnitude3() const -> T;

    FORCEINLINE auto IsNormalized() const -> bool;
    FORCEINLINE auto Normalize(const T InTolerance = JAFG_SMALL_NUMBER) -> void;
    FORCEINLINE auto NormalizeRet(const T InTolerance = JAFG_SMALL_NUMBER) -> TVector4<T>&;
    FORCEINLINE auto GetNormalized(const T InTolerance = JAFG_SMALL_NUMBER, const TVector4<T>& ResultIfZero = Identity()) const -> TVector4<T>;
    FORCEINLINE auto GetUnsafeNormalized() const -> TVector4<T>;

    FORCEINLINE auto Invert() -> void;
    FORCEINLINE auto Invert4() -> TVector4<T>&;
    FORCEINLINE auto InvertRet() -> TVector<T>&;
    FORCEINLINE auto InvertRet4() -> TVector4<T>&;
    FORCEINLINE auto GetInvert() const -> TVector4<T>;
    FORCEINLINE auto GetInvert4() const -> TVector4<T>;
};

template <typename T>
TVector4<T>::TVector4(const T InX, const T InY, const T InZ, const T InW): X(InX), Y(InY), Z(InZ), W(InW)
{
    return;
}

template <typename T>
T& TVector4<T>::operator[](const int32 InIndex)
{
    check( InIndex > INDEX_NONE && InIndex < 4 )
    return this->XYZW[InIndex];
}

template <typename T>
const T& TVector4<T>::operator[](const int32 InIndex) const
{
    check( InIndex > INDEX_NONE && InIndex < 4 )
    return this->XYZW[InIndex];
}

template <typename T>
TVector4<T>& TVector4<T>::operator=(const TVector4<T>& InVector) noexcept
{
    this->X = InVector.X; this->Y = InVector.Y; this->Z = InVector.Z; this->W = InVector.W;
    return *this;
}

template <typename T>
TVector4<T>& TVector4<T>::operator=(TVector4<T>&& InVector) noexcept
{
    this->X = InVector.X; this->Y = InVector.Y; this->Z = InVector.Z; this->W = InVector.W;
    return *this;
}

template <typename T>
TVector4<T> TVector4<T>::operator+(const T InScalar) const
{
    return TVector4<T>(this->X + InScalar, this->Y + InScalar, this->Z + InScalar, this->W + InScalar);
}

template <typename T>
TVector4<T> TVector4<T>::operator+(const TVector4<T>& InVector) const
{
    return TVector4<T>(this->X + InVector.X, this->Y + InVector.Y, this->Z + InVector.Z, this->W + InVector.W);
}

template <typename T>
TVector4<T>& TVector4<T>::operator+=(const T InScalar)
{
    this->X += InScalar;
    this->Y += InScalar;
    this->Z += InScalar;
    this->W += InScalar;
    return *this;
}

template <typename T>
TVector4<T>& TVector4<T>::operator+=(const TVector4<T>& InVector)
{
    this->X += InVector.X;
    this->Y += InVector.Y;
    this->Z += InVector.Z;
    this->W += InVector.W;
    return *this;
}

template <typename T>
TVector4<T> TVector4<T>::operator-(const T InScalar) const
{
    return TVector4<T>(this->X - InScalar, this->Y - InScalar, this->Z - InScalar, this->W - InScalar);
}

template <typename T>
TVector4<T> TVector4<T>::operator-(const TVector4<T>& InVector) const
{
    return TVector4<T>(this->X - InVector.X, this->Y - InVector.Y, this->Z - InVector.Z, this->W - InVector.W);
}

template <typename T>
TVector4<T>& TVector4<T>::operator-=(const T InScalar)
{
    this->X -= InScalar;
    this->Y -= InScalar;
    this->Z -= InScalar;
    this->W -= InScalar;
    return *this;
}

template <typename T>
TVector4<T>& TVector4<T>::operator-=(const TVector4<T>& InVector)
{
    this->X -= InVector.X;
    this->Y -= InVector.Y;
    this->Z -= InVector.Z;
    this->W -= InVector.W;
    return *this;
}

template <typename T>
TVector4<T> TVector4<T>::operator*(const T InScalar) const
{
    return TVector4<T>(this->X * InScalar, this->Y * InScalar, this->Z * InScalar, this->W * InScalar);
}

template <typename T>
TVector4<T> TVector4<T>::operator*(const TVector4<T>& InVector) const
{
    return TVector4<T>(this->X * InVector.X, this->Y * InVector.Y, this->Z * InVector.Z, this->W * InVector.W);
}

template <typename T>
TVector4<T>& TVector4<T>::operator*=(const T InScalar)
{
    this->X *= InScalar;
    this->Y *= InScalar;
    this->Z *= InScalar;
    this->W *= InScalar;
    return *this;
}

template <typename T>
TVector4<T>& TVector4<T>::operator*=(const TVector4<T>& InVector)
{
    this->X *= InVector.X;
    this->Y *= InVector.Y;
    this->Z *= InVector.Z;
    this->W *= InVector.W;
    return *this;
}

template <typename T>
TVector4<T> TVector4<T>::operator/(const T InScalar) const
{
    return TVector4<T>(this->X / InScalar, this->Y / InScalar, this->Z / InScalar, this->W / InScalar);
}

template <typename T>
TVector4<T> TVector4<T>::operator/(const TVector4<T>& InVector) const
{
    return TVector4<T>(this->X / InVector.X, this->Y / InVector.Y, this->Z / InVector.Z, this->W / InVector.W);
}

template <typename T>
TVector4<T>& TVector4<T>::operator/=(const T InScalar)
{
    this->X /= InScalar;
    this->Y /= InScalar;
    this->Z /= InScalar;
    this->W /= InScalar;
    return *this;
}

template <typename T>
TVector4<T>& TVector4<T>::operator/=(const TVector4<T>& InVector)
{
    this->X /= InVector.X;
    this->Y /= InVector.Y;
    this->Z /= InVector.Z;
    this->W /= InVector.W;
    return *this;
}

template <typename T>
TVector4<T> TVector4<T>::operator-() const
{
    return TVector4<T>(-this->X, -this->Y, -this->Z, -this->W);
}

template <typename T>
bool TVector4<T>::Equals(const TVector4<T>& InVector, const T InTolerance) const
{
    return Maths::Absolute(this->X - InVector.X) < InTolerance
        && Maths::Absolute(this->Y - InVector.Y) < InTolerance
        && Maths::Absolute(this->Z - InVector.Z) < InTolerance
        && Maths::Absolute(this->W - InVector.W) < InTolerance;
}

template <typename T>
bool TVector4<T>::operator==(const TVector4<T>& InVector) const
{
    return this->X == InVector.X && this->Y == InVector.Y && this->Z == InVector.Z && this->W == InVector.W;
}

template <typename T>
bool TVector4<T>::operator!=(const TVector4<T>& InVector) const
{
    return !(*this == InVector);
}

template <typename T>
bool TVector4<T>::IsZero3() const
{
    return this->X == 0.0f && this->Y == 0.0f && this->Z == 0.0f;
}

template <typename T>
bool TVector4<T>::IsNearlyZero3(const T InTolerance) const
{
    return Maths::Absolute(this->X) < InTolerance
        && Maths::Absolute(this->Y) < InTolerance
        && Maths::Absolute(this->Z) < InTolerance;
}

template <typename T>
T TVector4<T>::Magnitude() const
{
    return Maths::Sqrt(this->X * this->X + this->Y * this->Y + this->Z * this->Z + this->W * this->W);
}

template <typename T>
T TVector4<T>::SquaredMagnitude() const
{
    return this->X * this->X + this->Y * this->Y + this->Z * this->Z + this->W * this->W;
}

template <typename T>
T TVector4<T>::Magnitude3() const
{
    return Maths::Sqrt(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
}

template <typename T>
T TVector4<T>::SquaredMagnitude3() const
{
    return this->X * this->X + this->Y * this->Y + this->Z * this->Z;
}

template <typename T>
bool TVector4<T>::IsNormalized() const
{
    return
           (Maths::Absolute(1.0f - this->SquaredMagnitude()) < static_cast<T>(JAFG_THRESHOLD_NORM_VEC_D))
        && (this->W != 1.0f);
}

template <typename T>
void TVector4<T>::Normalize(const T InTolerance)
{
    const T SquareSum = this->X * this->X + this->Y * this->Y;
    if (SquareSum > InTolerance)
    {
        const T Scale = Maths::InverseSqrt(SquareSum);
        this->X *= Scale;
        this->Y *= Scale;
        this->Z *= Scale;
        this->W  =  0.0f;
    }

    return;
}

template <typename T>
TVector4<T>& TVector4<T>::NormalizeRet(const T InTolerance)
{
    this->Normalize(InTolerance);
    return *this;
}

template <typename T>
TVector4<T> TVector4<T>::GetNormalized(const T InTolerance, const TVector4<T>& ResultIfZero) const
{
    const T SquareSum = this->X * this->X + this->Y * this->Y + this->Z * this->Z;
    if (SquareSum == 1.0f)
    {
        return *this;
    }

    if (SquareSum < InTolerance)
    {
        return ResultIfZero;
    }

    const T Scale = Maths::InverseSqrt(SquareSum);
    return TVector4<T>(this->X * Scale, this->Y * Scale, this->Z * Scale, 0.0f);
}

template <typename T>
TVector4<T> TVector4<T>::GetUnsafeNormalized() const
{
    const T Scale = Maths::InverseSqrt(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
    return TVector4<T>(this->X * Scale, this->Y * Scale, this->Z * Scale, 0.0f);
}

template <typename T>
void TVector4<T>::Invert()
{
    this->X = -this->X;
    this->Y = -this->Y;
    this->Z = -this->Z;

    return;
}

template <typename T>
TVector4<T>& TVector4<T>::Invert4()
{
    this->X = -this->X;
    this->Y = -this->Y;
    this->Z = -this->Z;
    this->W = -this->W;

    return *this;
}

template <typename T>
TVector<T>& TVector4<T>::InvertRet()
{
    this->Invert();
    return *this;
}

template <typename T>
TVector4<T>& TVector4<T>::InvertRet4()
{
    this->Invert4();
    return *this;
}

template <typename T>
TVector4<T> TVector4<T>::GetInvert() const
{
    return TVector4<T>(-this->X, -this->Y, -this->Z, this->W);
}

template <typename T>
TVector4<T> TVector4<T>::GetInvert4() const
{
    return TVector4<T>(-this->X, -this->Y, -this->Z, -this->W);
}

} /* ~Namespace Jafg */
