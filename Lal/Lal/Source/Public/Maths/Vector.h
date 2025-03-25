// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/** Jafg implementation of a vector. */
template <typename T>
struct TVector
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
    LAL_API static const TVector<T> ZeroVector;

    /** Global one vector constant (1, 1, 1). */
    LAL_API static const TVector<T> OneVector;

    /** Global up vector constant (0, 0, 1). */
    LAL_API static const TVector<T> UpVector;

    /** Global down vector constant (0, 0, -1). */
    LAL_API static const TVector<T> DownVector;

    /** Global forward vector constant (1, 0, 0). */
    LAL_API static const TVector<T> ForwardVector;

    /** Global backward vector constant (-1, 0, 0). */
    LAL_API static const TVector<T> BackwardVector;

    /** Global right vector constant (0, 1, 0). */
    LAL_API static const TVector<T> RightVector;

    /** Global left vector constant (0, -1, 0). */
    LAL_API static const TVector<T> LeftVector;

    /** Global unit vector constant along the x-axis (1, 0, 0). */
    LAL_API static const TVector<T> UnitVectorX;

    /** Global unit vector constant along the y-axis (0, 1, 0). */
    LAL_API static const TVector<T> UnitVectorY;

    /** Global unit vector constant along the z-axis (0, 0, 1). */
    LAL_API static const TVector<T> UnitVectorZ;

    FORCEINLINE static TVector<T> Zero()     { return TVector<T>::ZeroVector;     }
    FORCEINLINE static TVector<T> One()      { return TVector<T>::OneVector;      }
    FORCEINLINE static TVector<T> Up()       { return TVector<T>::UpVector;       }
    FORCEINLINE static TVector<T> Down()     { return TVector<T>::DownVector;     }
    FORCEINLINE static TVector<T> Forward()  { return TVector<T>::ForwardVector;  }
    FORCEINLINE static TVector<T> Backward() { return TVector<T>::BackwardVector; }
    FORCEINLINE static TVector<T> Right()    { return TVector<T>::RightVector;    }
    FORCEINLINE static TVector<T> Left()     { return TVector<T>::LeftVector;     }
    FORCEINLINE static TVector<T> UnitX()    { return TVector<T>::UnitVectorX;    }
    FORCEINLINE static TVector<T> UnitY()    { return TVector<T>::UnitVectorY;    }
    FORCEINLINE static TVector<T> UnitZ()    { return TVector<T>::UnitVectorZ;    }

    FORCEINLINE          TVector()                                      noexcept = default;
    FORCEINLINE explicit TVector(const T InFloatingPoint)               noexcept : X(InFloatingPoint), Y(InFloatingPoint), Z(InFloatingPoint) { }
    FORCEINLINE explicit TVector(const T InX, const T InY, const T InZ) noexcept : X(InX), Y(InY), Z(InZ) { }
    FORCEINLINE explicit TVector(const TVector2<T> InVec, const T InZ)  noexcept : X(InVec.X), Y(InVec.Y), Z(InZ) { }
    FORCEINLINE explicit TVector(const T InXYZ[3])                      noexcept : X(InXYZ[0]), Y(InXYZ[1]), Z(InXYZ[2]) { }
    FORCEINLINE          TVector(const TVector<T>& InVec)               noexcept : X(InVec.X), Y(InVec.Y), Z(InVec.Z) { }
    FORCEINLINE          TVector(TVector<T>&& InVec)                    noexcept : X(InVec.X), Y(InVec.Y), Z(InVec.Z) { }

    FORCEINLINE auto GetData()       noexcept ->       T* { return &this->X; }
    FORCEINLINE auto GetData() const noexcept -> const T* { return &this->X; }
    FORCEINLINE auto constexpr GetDataByteSize() const noexcept -> LuBigSizeTy;

    FORCEINLINE auto operator[](const i32 InIndex)       ->       T&;
    FORCEINLINE auto operator[](const i32 InIndex) const -> const T&;

    FORCEINLINE TVector2<T> XY() const { return TVector2<T>(this->X, this->Y); }
    FORCEINLINE TVector2<T> XZ() const { return TVector2<T>(this->X, this->Z); }
    FORCEINLINE TVector2<T> YX() const { return TVector2<T>(this->Y, this->X); }
    FORCEINLINE TVector2<T> YZ() const { return TVector2<T>(this->Y, this->Z); }
    FORCEINLINE TVector<T> XZY() const { return TVector<T>(this->X, this->Z, this->Y); }
    FORCEINLINE TVector<T> YXZ() const { return TVector<T>(this->Y, this->X, this->Z); }
    FORCEINLINE TVector<T> YZX() const { return TVector<T>(this->Y, this->Z, this->X); }
    FORCEINLINE TVector<T> ZXY() const { return TVector<T>(this->Z, this->X, this->Y); }
    FORCEINLINE TVector<T> ZYX() const { return TVector<T>(this->Z, this->Y, this->X); }

    FORCEINLINE TVector<T>& operator =(const TVector<T>&  InVec) noexcept;
    FORCEINLINE TVector<T>& operator =(      TVector<T>&& InVec) noexcept;
    FORCEINLINE TVector<T>& operator =(const TVector<T>&& InVec) noexcept = delete;

    FORCEINLINE TVector<T>  operator +(const T            InScalar) const;
    FORCEINLINE TVector<T>  operator +(const TVector2<T>& InVec   ) const;
    FORCEINLINE TVector<T>  operator +(const TVector<T>&  InVec   ) const;
    FORCEINLINE TVector<T>& operator+=(const T            InScalar);
    FORCEINLINE TVector<T>& operator+=(const TVector2<T>& InVec   );
    FORCEINLINE TVector<T>& operator+=(const TVector<T>&  InVec   );
    FORCEINLINE TVector<T>  operator -(const T            InScalar) const;
    FORCEINLINE TVector<T>  operator -(const TVector2<T>& InVec   ) const;
    FORCEINLINE TVector<T>  operator -(const TVector<T>&  InVec   ) const;
    FORCEINLINE TVector<T>& operator-=(const T            InScalar);
    FORCEINLINE TVector<T>& operator-=(const TVector2<T>& InVec   );
    FORCEINLINE TVector<T>& operator-=(const TVector<T>&  InVec   );
    FORCEINLINE TVector<T>  operator *(const T            InScalar) const;
    FORCEINLINE TVector<T>  operator *(const TVector<T>&  InVec   ) const;
    FORCEINLINE TVector<T>& operator*=(const T            InScalar);
    FORCEINLINE TVector<T>& operator*=(const TVector<T>&  InVec   );
    FORCEINLINE TVector<T>  operator /(const T            InScalar) const;
    FORCEINLINE TVector<T>  operator /(const TVector<T>&  InVec   ) const;
    FORCEINLINE TVector<T>& operator/=(const T            InScalar);
    FORCEINLINE TVector<T>& operator/=(const TVector<T>&  InVec   );
    FORCEINLINE TVector<T>  operator -() const { return TVector<T>(-this->X, -this->Y, -this->Z); }

    FORCEINLINE T operator |(const TVector<T>& InVec) const { return this->Dot(InVec); }

    FORCEINLINE bool Equals(const TVector<T>& InVec, const T InTolerance = JAFG_SMALL_NUMBER) const;
    FORCEINLINE bool operator==(const TVector<T>& InVec) const;
    FORCEINLINE bool operator!=(const TVector<T>& InVec) const;

    FORCEINLINE bool IsZero() const;
    FORCEINLINE bool IsNearlyZero(const T InTolerance = JAFG_NOT_SO_SMALL_NUMBER) const;

    FORCEINLINE auto Magnitude() const -> T;
    FORCEINLINE auto SquaredMagnitude() const -> T;
    FORCEINLINE auto Magnitude2D() const -> T;
    FORCEINLINE auto SquaredMagnitude2D() const -> T;

    FORCEINLINE auto IsNormalized() const -> bool;
    FORCEINLINE auto Normalize(const T InTolerance = JAFG_SMALL_NUMBER) -> void;
    FORCEINLINE auto NormalizeRet(const T InTolerance = JAFG_SMALL_NUMBER) -> TVector<T>&;
    FORCEINLINE auto GetNormalized(const T InTolerance = JAFG_SMALL_NUMBER, const TVector<T>& ResultIfZero = Zero()) const -> TVector<T>;
    FORCEINLINE auto GetUnsafeNormalized() const -> TVector<T>;

    FORCEINLINE auto Invert() -> void;
    FORCEINLINE auto InvertRet() -> TVector<T>&;
    FORCEINLINE auto GetInvert() const -> TVector<T>;
    FORCEINLINE auto Cross(const TVector<T>& InVec) const -> TVector<T>;
    FORCEINLINE auto Dot(const TVector<T>& InVec) const -> T;

    FORCEINLINE auto Floor() -> void;
    FORCEINLINE auto GetFloor() const -> TVector<T>;
    FORCEINLINE auto Ceil() -> void;
    FORCEINLINE auto GetCeil() const -> TVector<T>;
    FORCEINLINE auto ModF() -> void;
    FORCEINLINE auto GetModF() const -> TVector<T>;

    FORCEINLINE EVectorAxis::Type GetDominantAxis() const;
    FORCEINLINE EVectorAxis::Type GetMostInferiorAxis() const;

    LSimpleString ToString() const
    {
        return LSimpleString::SprintF("{:.2f} {:.2f} {:.2f}", this->X, this->Y, this->Z);
    }
    LSimpleString ToHighPrecisionString() const
    {
        return LSimpleString::SprintF("{:.9f} {:.9f} {:.9f}", this->X, this->Y, this->Z);
    }
};

template <typename T>
constexpr LuBigSizeTy TVector<T>::GetDataByteSize() const noexcept
{
    static_assert(sizeof(TVector<T>) == sizeof(T) * 3, "TVector<T> is not tightly packed.");
    return sizeof(T) * 3;
}

template <typename T>
T& TVector<T>::operator[](const i32 InIndex)
{
    check( InIndex > INDEX_NONE && InIndex < 3 )
    return this->XYZ[InIndex];
}

template <typename T>
const T& TVector<T>::operator[](const i32 InIndex) const
{
    check( InIndex > INDEX_NONE && InIndex < 3 )
    return this->XYZ[InIndex];
}

template <typename T>
TVector<T>& TVector<T>::operator=(const TVector<T>& InVec) noexcept
{
    this->X = InVec.X;
    this->Y = InVec.Y;
    this->Z = InVec.Z;
    return *this;
}

template <typename T>
TVector<T>& TVector<T>::operator=(TVector<T>&& InVec) noexcept
{
    this->X = InVec.X;
    this->Y = InVec.Y;
    this->Z = InVec.Z;
    return *this;
}

template <typename T>
TVector<T> TVector<T>::operator+(const T InScalar) const
{
    return TVector<T>(this->X + InScalar, this->Y + InScalar, this->Z + InScalar);
}

template <typename T>
TVector<T> TVector<T>::operator+(const TVector2<T>& InVec) const
{
    return TVector<T>(this->X + InVec.X, this->Y + InVec.Y, this->Z);
}

template <typename T>
TVector<T> TVector<T>::operator+(const TVector<T>& InVec) const
{
    return TVector<T>(this->X + InVec.X, this->Y + InVec.Y, this->Z + InVec.Z);
}

template <typename T>
TVector<T>& TVector<T>::operator+=(const T InScalar)
{
    this->X += InScalar;
    this->Y += InScalar;
    this->Z += InScalar;
    return *this;
}

template <typename T>
TVector<T>& TVector<T>::operator+=(const TVector2<T>& InVec)
{
    this->X += InVec.X;
    this->Y += InVec.Y;
    return *this;
}

template <typename T>
TVector<T>& TVector<T>::operator+=(const TVector<T>& InVec)
{
    this->X += InVec.X;
    this->Y += InVec.Y;
    this->Z += InVec.Z;
    return *this;
}

template <typename T>
TVector<T> TVector<T>::operator-(const T InScalar) const
{
    return TVector<T>(this->X - InScalar, this->Y - InScalar, this->Z - InScalar);
}

template <typename T>
TVector<T> TVector<T>::operator-(const TVector2<T>& InVec) const
{
    return TVector<T>(this->X - InVec.X, this->Y - InVec.Y, this->Z);
}

template <typename T>
TVector<T> TVector<T>::operator-(const TVector<T>& InVec) const
{
    return TVector<T>(this->X - InVec.X, this->Y - InVec.Y, this->Z - InVec.Z);
}

template <typename T>
TVector<T>& TVector<T>::operator-=(const T InScalar)
{
    this->X -= InScalar;
    this->Y -= InScalar;
    this->Z -= InScalar;
    return *this;
}

template <typename T>
TVector<T>& TVector<T>::operator-=(const TVector2<T>& InVec)
{
    this->X -= InVec.X;
    this->Y -= InVec.Y;
    return *this;
}

template <typename T>
TVector<T>& TVector<T>::operator-=(const TVector<T>& InVec)
{
    this->X -= InVec.X;
    this->Y -= InVec.Y;
    this->Z -= InVec.Z;
    return *this;
}

template <typename T>
TVector<T> TVector<T>::operator*(const T InScalar) const
{
    return TVector<T>(this->X * InScalar, this->Y * InScalar, this->Z * InScalar);
}

template <typename T>
TVector<T> TVector<T>::operator*(const TVector<T>& InVec) const
{
    return TVector<T>(this->X * InVec.X, this->Y * InVec.Y, this->Z * InVec.Z);
}

template <typename T>
TVector<T>& TVector<T>::operator*=(const T InScalar)
{
    this->X *= InScalar;
    this->Y *= InScalar;
    this->Z *= InScalar;
    return *this;
}

template <typename T>
TVector<T>& TVector<T>::operator*=(const TVector<T>& InVec)
{
    this->X *= InVec.X;
    this->Y *= InVec.Y;
    this->Z *= InVec.Z;
    return *this;
}

template <typename T>
TVector<T> TVector<T>::operator/(const T InScalar) const
{
    return TVector<T>(this->X / InScalar, this->Y / InScalar, this->Z / InScalar);
}

template <typename T>
TVector<T> TVector<T>::operator/(const TVector<T>& InVec) const
{
    return TVector<T>(this->X / InVec.X, this->Y / InVec.Y, this->Z / InVec.Z);
}

template <typename T>
TVector<T>& TVector<T>::operator/=(const T InScalar)
{
    this->X /= InScalar;
    this->Y /= InScalar;
    this->Z /= InScalar;
    return *this;
}

template <typename T>
TVector<T>& TVector<T>::operator/=(const TVector<T>& InVec)
{
    this->X /= InVec.X;
    this->Y /= InVec.Y;
    this->Z /= InVec.Z;
    return *this;
}

template <typename T>
bool TVector<T>::Equals(const TVector<T>& InVec, const T InTolerance) const
{
    return Maths::Absolute(this->X - InVec.X) < InTolerance
        && Maths::Absolute(this->Y - InVec.Y) < InTolerance
        && Maths::Absolute(this->Z - InVec.Z) < InTolerance;
}

template <typename T>
bool TVector<T>::operator==(const TVector<T>& InVec) const
{
    return this->X == InVec.X && this->Y == InVec.Y && this->Z == InVec.Z;
}

template <typename T>
bool TVector<T>::operator!=(const TVector<T>& InVec) const
{
    return !(*this == InVec);
}

template <typename T>
bool TVector<T>::IsZero() const
{
    return this->X == 0.0f && this->Y == 0.0f && this->Z == 0.0f;
}

template <typename T>
bool TVector<T>::IsNearlyZero(const T InTolerance) const
{
    return Maths::Absolute(this->X) < InTolerance
        && Maths::Absolute(this->Y) < InTolerance
        && Maths::Absolute(this->Z) < InTolerance;
}

template <typename T>
T TVector<T>::Magnitude() const
{
    return Maths::Sqrt(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
}

template <typename T>
T TVector<T>::SquaredMagnitude() const
{
    return this->X * this->X + this->Y * this->Y + this->Z * this->Z;
}

template <typename T>
T TVector<T>::Magnitude2D() const
{
    return Maths::Sqrt(this->X * this->X + this->Y * this->Y);
}

template <typename T>
T TVector<T>::SquaredMagnitude2D() const
{
    return this->X * this->X + this->Y * this->Y;
}

template <typename T>
bool TVector<T>::IsNormalized() const
{
    return Maths::Absolute(1.0f - this->SquaredMagnitude()) < static_cast<T>(JAFG_THRESHOLD_NORM_VEC_D);
}

template <typename T>
void TVector<T>::Normalize(const T InTolerance)
{
    const T SquareSum = this->X * this->X + this->Y * this->Y + this->Z * this->Z;
    if (SquareSum > InTolerance)
    {
        const T Scale = Maths::InverseSqrt(SquareSum);
        this->X *= Scale;
        this->Y *= Scale;
        this->Z *= Scale;
    }

    return;
}

template <typename T>
TVector<T>& TVector<T>::NormalizeRet(const T InTolerance)
{
    this->Normalize(InTolerance);
    return *this;
}

template <typename T>
TVector<T> TVector<T>::GetNormalized(T InTolerance, const TVector<T>& ResultIfZero) const
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
    return TVector<T>(this->X * Scale, this->Y * Scale, this->Z * Scale);
}

template <typename T>
TVector<T> TVector<T>::GetUnsafeNormalized() const
{
    const T Scale = Maths::InverseSqrt(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
    return TVector<T>(this->X * Scale, this->Y * Scale, this->Z * Scale);
}

template <typename T>
void TVector<T>::Invert()
{
    this->X = -this->X;
    this->Y = -this->Y;
    this->Z = -this->Z;
    return;
}

template <typename T>
TVector<T>& TVector<T>::InvertRet()
{
    this->Invert();
    return *this;
}

template <typename T>
TVector<T> TVector<T>::GetInvert() const
{
    return TVector<T>(-this->X, -this->Y, -this->Z);
}

template <typename T>
TVector<T> TVector<T>::Cross(const TVector<T>& InVec) const
{
    return TVector<T>(
        this->Y * InVec.Z - this->Z * InVec.Y,
        this->Z * InVec.X - this->X * InVec.Z,
        this->X * InVec.Y - this->Y * InVec.X
    );
}

template <typename T>
T TVector<T>::Dot(const TVector<T>& InVec) const
{
    return this->X * InVec.X + this->Y * InVec.Y + this->Z * InVec.Z;
}

template <typename T>
void TVector<T>::Floor()
{
    this->X = Maths::Floor(this->X);
    this->Y = Maths::Floor(this->Y);
    this->Z = Maths::Floor(this->Z);
    return;
}

template <typename T>
TVector<T> TVector<T>::GetFloor() const
{
    return TVector<T>(Maths::Floor(this->X), Maths::Floor(this->Y), Maths::Floor(this->Z));
}

template <typename T>
void TVector<T>::Ceil()
{
    this->X = Maths::Ceil(this->X);
    this->Y = Maths::Ceil(this->Y);
    this->Z = Maths::Ceil(this->Z);
    return;
}

template <typename T>
TVector<T> TVector<T>::GetCeil() const
{
    return TVector<T>(Maths::Ceil(this->X), Maths::Ceil(this->Y), Maths::Ceil(this->Z));
}

template <typename T>
void TVector<T>::ModF()
{
    this->X = Maths::ModF(this->X);
    this->Y = Maths::ModF(this->Y);
    this->Z = Maths::ModF(this->Z);
    return;
}

template <typename T>
TVector<T> TVector<T>::GetModF() const
{
    return TVector<T>(Maths::ModF(this->X), Maths::ModF(this->Y), Maths::ModF(this->Z));
}

template <typename T>
EVectorAxis::Type TVector<T>::GetDominantAxis() const
{
    const T AbsX = Maths::Absolute(this->X);
    const T AbsY = Maths::Absolute(this->Y);
    const T AbsZ = Maths::Absolute(this->Z);

    if (AbsX > AbsY)
    {
        if (AbsX > AbsZ)
        {
            return EVectorAxis::Type::X;
        }

        return EVectorAxis::Type::Z;
    }

    if (AbsY > AbsZ)
    {
        return EVectorAxis::Type::Y;
    }

    return EVectorAxis::Type::Z;
}

template <typename T>
EVectorAxis::Type TVector<T>::GetMostInferiorAxis() const
{
    const T AbsX = Maths::Absolute(this->X);
    const T AbsY = Maths::Absolute(this->Y);
    const T AbsZ = Maths::Absolute(this->Z);

    if (AbsX < AbsY)
    {
        if (AbsX < AbsZ)
        {
            return EVectorAxis::Type::X;
        }

        return EVectorAxis::Type::Z;
    }

    if (AbsY < AbsZ)
    {
        return EVectorAxis::Type::Y;
    }

    return EVectorAxis::Type::Z;
}

} /* ~Namespace Jafg */
