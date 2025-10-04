// Copyright mzoesch. All rights reserved.

#pragma once

/** Jafg implementation of a three dimensional vector. */
template <typename T>
struct TVector3
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TVector3 must be a floating point type.");

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

    typedef TVector3<T> Self;

    /** Global zero vector constant (0, 0, 0). */
    LAL_API static const Self ZeroVector;

    /** Global one vector constant (1, 1, 1). */
    LAL_API static const Self OneVector;

    /** Global up vector constant (0, 0, 1). */
    LAL_API static const Self UpVector;

    /** Global down vector constant (0, 0, -1). */
    LAL_API static const Self DownVector;

    /** Global forward vector constant (1, 0, 0). */
    LAL_API static const Self ForwardVector;

    /** Global backward vector constant (-1, 0, 0). */
    LAL_API static const Self BackwardVector;

    /** Global right vector constant (0, 1, 0). */
    LAL_API static const Self RightVector;

    /** Global left vector constant (0, -1, 0). */
    LAL_API static const Self LeftVector;

    /** Global unit vector constant along the x-axis (1, 0, 0). */
    LAL_API static const Self UnitVectorX;

    /** Global unit vector constant along the y-axis (0, 1, 0). */
    LAL_API static const Self UnitVectorY;

    /** Global unit vector constant along the z-axis (0, 0, 1). */
    LAL_API static const Self UnitVectorZ;

    FORCEINLINE static Self Zero()     { return Self::ZeroVector;     }
    FORCEINLINE static Self One()      { return Self::OneVector;      }
    FORCEINLINE static Self Up()       { return Self::UpVector;       }
    FORCEINLINE static Self Down()     { return Self::DownVector;     }
    FORCEINLINE static Self Forward()  { return Self::ForwardVector;  }
    FORCEINLINE static Self Backward() { return Self::BackwardVector; }
    FORCEINLINE static Self Right()    { return Self::RightVector;    }
    FORCEINLINE static Self Left()     { return Self::LeftVector;     }
    FORCEINLINE static Self UnitX()    { return Self::UnitVectorX;    }
    FORCEINLINE static Self UnitY()    { return Self::UnitVectorY;    }
    FORCEINLINE static Self UnitZ()    { return Self::UnitVectorZ;    }

    FORCEINLINE constexpr          TVector3()                                      noexcept : X(0), Y(0), Z(0) { }
    FORCEINLINE constexpr explicit TVector3(const T InFloatingPoint)               noexcept : X(InFloatingPoint), Y(InFloatingPoint), Z(InFloatingPoint) { }
    FORCEINLINE constexpr          TVector3(const T InX, const T InY, const T InZ) noexcept : X(InX), Y(InY), Z(InZ) { }
    FORCEINLINE constexpr explicit TVector3(const TVector2<T> InVec, const T InZ)  noexcept : X(InVec.X), Y(InVec.Y), Z(InZ) { }
    FORCEINLINE constexpr explicit TVector3(const T InXYZ[3])                      noexcept : X(InXYZ[0]), Y(InXYZ[1]), Z(InXYZ[2]) { }
    FORCEINLINE constexpr          TVector3(const TVector3<T>& InVec)              noexcept : X(InVec.X), Y(InVec.Y), Z(InVec.Z) { }
    FORCEINLINE constexpr          TVector3(TVector3<T>&& InVec)                   noexcept : X(InVec.X), Y(InVec.Y), Z(InVec.Z) { }

    constexpr          TVector3(std::initializer_list<T> InList)       noexcept;

    FORCEINLINE constexpr auto  GetData()       noexcept ->       T* { return &this->X; }
    FORCEINLINE constexpr auto  GetData() const noexcept -> const T* { return &this->X; }
    FORCEINLINE constexpr LSize GetDataByteSize() const noexcept;

    FORCEINLINE auto operator[](const i32 InIndex)       ->       T&;
    FORCEINLINE auto operator[](const i32 InIndex) const -> const T&;

    FORCEINLINE TVector2<T> XY() const  { return TVector2<T>(this->X, this->Y); }
    FORCEINLINE TVector2<T> XZ() const  { return TVector2<T>(this->X, this->Z); }
    FORCEINLINE TVector2<T> YX() const  { return TVector2<T>(this->Y, this->X); }
    FORCEINLINE TVector2<T> YZ() const  { return TVector2<T>(this->Y, this->Z); }
    FORCEINLINE TVector3<T> XZY() const { return TVector3<T>(this->X, this->Z, this->Y); }
    FORCEINLINE TVector3<T> YXZ() const { return TVector3<T>(this->Y, this->X, this->Z); }
    FORCEINLINE TVector3<T> YZX() const { return TVector3<T>(this->Y, this->Z, this->X); }
    FORCEINLINE TVector3<T> ZXY() const { return TVector3<T>(this->Z, this->X, this->Y); }
    FORCEINLINE TVector3<T> ZYX() const { return TVector3<T>(this->Z, this->Y, this->X); }

    FORCEINLINE TVector3<T>& operator =(const TVector3<T>&  InVec) noexcept;
    FORCEINLINE TVector3<T>& operator =(      TVector3<T>&& InVec) noexcept;
    FORCEINLINE TVector3<T>& operator =(const TVector3<T>&& InVec) noexcept = delete;

    FORCEINLINE TVector3<T>  operator +(const T            InScalar) const;
    FORCEINLINE TVector3<T>  operator +(const TVector2<T>& InVec   ) const;
    FORCEINLINE TVector3<T>  operator +(const TVector3<T>&  InVec   ) const;
    FORCEINLINE TVector3<T>& operator+=(const T            InScalar);
    FORCEINLINE TVector3<T>& operator+=(const TVector2<T>& InVec   );
    FORCEINLINE TVector3<T>& operator+=(const TVector3<T>&  InVec   );
    FORCEINLINE TVector3<T>  operator -(const T            InScalar) const;
    FORCEINLINE TVector3<T>  operator -(const TVector2<T>& InVec   ) const;
    FORCEINLINE TVector3<T>  operator -(const TVector3<T>&  InVec   ) const;
    FORCEINLINE TVector3<T>& operator-=(const T            InScalar);
    FORCEINLINE TVector3<T>& operator-=(const TVector2<T>& InVec   );
    FORCEINLINE TVector3<T>& operator-=(const TVector3<T>&  InVec   );
    FORCEINLINE TVector3<T>  operator *(const T            InScalar) const;
    FORCEINLINE TVector3<T>  operator *(const TVector3<T>&  InVec   ) const;
    FORCEINLINE TVector3<T>& operator*=(const T            InScalar);
    FORCEINLINE TVector3<T>& operator*=(const TVector3<T>&  InVec   );
    FORCEINLINE TVector3<T>  operator /(const T            InScalar) const;
    FORCEINLINE TVector3<T>  operator /(const TVector3<T>&  InVec   ) const;
    FORCEINLINE TVector3<T>& operator/=(const T            InScalar);
    FORCEINLINE TVector3<T>& operator/=(const TVector3<T>&  InVec   );
    FORCEINLINE TVector3<T>  operator -() const { return TVector3<T>(-this->X, -this->Y, -this->Z); }

    FORCEINLINE T operator |(const TVector3<T>& InVec) const { return this->Dot(InVec); }

    FORCEINLINE bool Equals(const TVector3<T>& InVec, const T InTolerance = LAL_SMALL_NUMBER) const;
    FORCEINLINE bool operator==(const TVector3<T>& InVec) const;
    FORCEINLINE bool operator!=(const TVector3<T>& InVec) const;

    FORCEINLINE bool IsZero() const;
    FORCEINLINE bool IsNearlyZero(const T InTolerance = LAL_NOT_SO_SMALL_NUMBER) const;

    FORCEINLINE auto Magnitude() const -> T;
    FORCEINLINE auto SquaredMagnitude() const -> T;
    FORCEINLINE auto Magnitude2D() const -> T;
    FORCEINLINE auto SquaredMagnitude2D() const -> T;

    FORCEINLINE auto IsNormalized() const -> bool;
    FORCEINLINE auto Normalize(const T InTolerance = LAL_SMALL_NUMBER) -> void;
    FORCEINLINE auto NormalizeRet(const T InTolerance = LAL_SMALL_NUMBER) -> TVector3<T>&;
    FORCEINLINE auto GetNormalized(const T InTolerance = LAL_SMALL_NUMBER, const TVector3<T>& ResultIfZero = Zero()) const -> TVector3<T>;
    FORCEINLINE auto GetUnsafeNormalized() const -> TVector3<T>;

    FORCEINLINE auto Invert() -> void;
    FORCEINLINE auto InvertRet() -> TVector3<T>&;
    FORCEINLINE auto GetInvert() const -> TVector3<T>;
    FORCEINLINE auto Cross(const TVector3<T>& InVec) const -> TVector3<T>;
    FORCEINLINE auto Dot(const TVector3<T>& InVec) const -> T;

    FORCEINLINE auto Floor() -> void;
    FORCEINLINE auto GetFloor() const -> TVector3<T>;
    FORCEINLINE auto Ceil() -> void;
    FORCEINLINE auto GetCeil() const -> TVector3<T>;
    FORCEINLINE auto ModF() -> void;
    FORCEINLINE auto GetModF() const -> TVector3<T>;

    FORCEINLINE EVectorAxis::Type GetDominantAxis() const;
    FORCEINLINE EVectorAxis::Type GetMostInferiorAxis() const;

    LString ToString() const
    {
        return Lal::SprintF("{:.2f} {:.2f} {:.2f}", this->X, this->Y, this->Z);
    }
    LString ToHighPrecisionString() const
    {
        return Lal::SprintF("{:.9f} {:.9f} {:.9f}", this->X, this->Y, this->Z);
    }
};

template<typename T>
FORCEINLINE constexpr TVector3<T>::TVector3(std::initializer_list<T> InList) noexcept
{
    if (InList.size() == 3)
    {
        this->X = InList.begin()[0];
        this->Y = InList.begin()[1];
        this->Z = InList.begin()[2];
    }
    else if (InList.size() == 1)
    {
        this->X = InList.begin()[0];
        this->Y = InList.begin()[0];
        this->Z = InList.begin()[0];
    }
    else
    {
        panicMsgf("Invalid std::initializer_list<T> size for TVector3<T> constructor. Expected 1 or 3, got [{}].", InList.size())
    }

    return;
}

template <typename T>
FORCEINLINE constexpr LSize TVector3<T>::GetDataByteSize() const noexcept
{
    static_assert(sizeof(TVector3<T>) == sizeof(T) * 3, "TVector3<T> is not tightly packed.");
    return sizeof(T) * 3;
}

template <typename T>
T& TVector3<T>::operator[](const i32 InIndex)
{
    check( InIndex > INDEX_NONE && InIndex < 3 )
    return this->XYZ[InIndex];
}

template <typename T>
const T& TVector3<T>::operator[](const i32 InIndex) const
{
    check( InIndex > INDEX_NONE && InIndex < 3 )
    return this->XYZ[InIndex];
}

template <typename T>
TVector3<T>& TVector3<T>::operator=(const TVector3<T>& InVec) noexcept
{
    this->X = InVec.X;
    this->Y = InVec.Y;
    this->Z = InVec.Z;
    return *this;
}

template <typename T>
TVector3<T>& TVector3<T>::operator=(TVector3<T>&& InVec) noexcept
{
    this->X = InVec.X;
    this->Y = InVec.Y;
    this->Z = InVec.Z;
    return *this;
}

template <typename T>
TVector3<T> TVector3<T>::operator+(const T InScalar) const
{
    return TVector3<T>(this->X + InScalar, this->Y + InScalar, this->Z + InScalar);
}

template <typename T>
TVector3<T> TVector3<T>::operator+(const TVector2<T>& InVec) const
{
    return TVector3<T>(this->X + InVec.X, this->Y + InVec.Y, this->Z);
}

template <typename T>
TVector3<T> TVector3<T>::operator+(const TVector3<T>& InVec) const
{
    return TVector3<T>(this->X + InVec.X, this->Y + InVec.Y, this->Z + InVec.Z);
}

template <typename T>
TVector3<T>& TVector3<T>::operator+=(const T InScalar)
{
    this->X += InScalar;
    this->Y += InScalar;
    this->Z += InScalar;
    return *this;
}

template <typename T>
TVector3<T>& TVector3<T>::operator+=(const TVector2<T>& InVec)
{
    this->X += InVec.X;
    this->Y += InVec.Y;
    return *this;
}

template <typename T>
TVector3<T>& TVector3<T>::operator+=(const TVector3<T>& InVec)
{
    this->X += InVec.X;
    this->Y += InVec.Y;
    this->Z += InVec.Z;
    return *this;
}

template <typename T>
TVector3<T> TVector3<T>::operator-(const T InScalar) const
{
    return TVector3<T>(this->X - InScalar, this->Y - InScalar, this->Z - InScalar);
}

template <typename T>
TVector3<T> TVector3<T>::operator-(const TVector2<T>& InVec) const
{
    return TVector3<T>(this->X - InVec.X, this->Y - InVec.Y, this->Z);
}

template <typename T>
TVector3<T> TVector3<T>::operator-(const TVector3<T>& InVec) const
{
    return TVector3<T>(this->X - InVec.X, this->Y - InVec.Y, this->Z - InVec.Z);
}

template <typename T>
TVector3<T>& TVector3<T>::operator-=(const T InScalar)
{
    this->X -= InScalar;
    this->Y -= InScalar;
    this->Z -= InScalar;
    return *this;
}

template <typename T>
TVector3<T>& TVector3<T>::operator-=(const TVector2<T>& InVec)
{
    this->X -= InVec.X;
    this->Y -= InVec.Y;
    return *this;
}

template <typename T>
TVector3<T>& TVector3<T>::operator-=(const TVector3<T>& InVec)
{
    this->X -= InVec.X;
    this->Y -= InVec.Y;
    this->Z -= InVec.Z;
    return *this;
}

template <typename T>
TVector3<T> TVector3<T>::operator*(const T InScalar) const
{
    return TVector3<T>(this->X * InScalar, this->Y * InScalar, this->Z * InScalar);
}

template <typename T>
TVector3<T> TVector3<T>::operator*(const TVector3<T>& InVec) const
{
    return TVector3<T>(this->X * InVec.X, this->Y * InVec.Y, this->Z * InVec.Z);
}

template <typename T>
TVector3<T>& TVector3<T>::operator*=(const T InScalar)
{
    this->X *= InScalar;
    this->Y *= InScalar;
    this->Z *= InScalar;
    return *this;
}

template <typename T>
TVector3<T>& TVector3<T>::operator*=(const TVector3<T>& InVec)
{
    this->X *= InVec.X;
    this->Y *= InVec.Y;
    this->Z *= InVec.Z;
    return *this;
}

template <typename T>
TVector3<T> TVector3<T>::operator/(const T InScalar) const
{
    return TVector3<T>(this->X / InScalar, this->Y / InScalar, this->Z / InScalar);
}

template <typename T>
TVector3<T> TVector3<T>::operator/(const TVector3<T>& InVec) const
{
    return TVector3<T>(this->X / InVec.X, this->Y / InVec.Y, this->Z / InVec.Z);
}

template <typename T>
TVector3<T>& TVector3<T>::operator/=(const T InScalar)
{
    this->X /= InScalar;
    this->Y /= InScalar;
    this->Z /= InScalar;
    return *this;
}

template <typename T>
TVector3<T>& TVector3<T>::operator/=(const TVector3<T>& InVec)
{
    this->X /= InVec.X;
    this->Y /= InVec.Y;
    this->Z /= InVec.Z;
    return *this;
}

template <typename T>
bool TVector3<T>::Equals(const TVector3<T>& InVec, const T InTolerance) const
{
    return Maths::Absolute(this->X - InVec.X) < InTolerance
        && Maths::Absolute(this->Y - InVec.Y) < InTolerance
        && Maths::Absolute(this->Z - InVec.Z) < InTolerance;
}

template <typename T>
bool TVector3<T>::operator==(const TVector3<T>& InVec) const
{
    return this->X == InVec.X && this->Y == InVec.Y && this->Z == InVec.Z;
}

template <typename T>
bool TVector3<T>::operator!=(const TVector3<T>& InVec) const
{
    return !(*this == InVec);
}

template <typename T>
bool TVector3<T>::IsZero() const
{
    return this->X == 0.0f && this->Y == 0.0f && this->Z == 0.0f;
}

template <typename T>
bool TVector3<T>::IsNearlyZero(const T InTolerance) const
{
    return Maths::Absolute(this->X) < InTolerance
        && Maths::Absolute(this->Y) < InTolerance
        && Maths::Absolute(this->Z) < InTolerance;
}

template <typename T>
T TVector3<T>::Magnitude() const
{
    return Maths::Sqrt(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
}

template <typename T>
T TVector3<T>::SquaredMagnitude() const
{
    return this->X * this->X + this->Y * this->Y + this->Z * this->Z;
}

template <typename T>
T TVector3<T>::Magnitude2D() const
{
    return Maths::Sqrt(this->X * this->X + this->Y * this->Y);
}

template <typename T>
T TVector3<T>::SquaredMagnitude2D() const
{
    return this->X * this->X + this->Y * this->Y;
}

template <typename T>
bool TVector3<T>::IsNormalized() const
{
    return Maths::Absolute(1.0f - this->SquaredMagnitude()) < static_cast<T>(LAL_THRESHOLD_NORM_VEC_D);
}

template <typename T>
void TVector3<T>::Normalize(const T InTolerance)
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
TVector3<T>& TVector3<T>::NormalizeRet(const T InTolerance)
{
    this->Normalize(InTolerance);
    return *this;
}

template <typename T>
TVector3<T> TVector3<T>::GetNormalized(T InTolerance, const TVector3<T>& ResultIfZero) const
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
    return TVector3<T>(this->X * Scale, this->Y * Scale, this->Z * Scale);
}

template <typename T>
TVector3<T> TVector3<T>::GetUnsafeNormalized() const
{
    const T Scale = Maths::InverseSqrt(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
    return TVector3<T>(this->X * Scale, this->Y * Scale, this->Z * Scale);
}

template <typename T>
void TVector3<T>::Invert()
{
    this->X = -this->X;
    this->Y = -this->Y;
    this->Z = -this->Z;
    return;
}

template <typename T>
TVector3<T>& TVector3<T>::InvertRet()
{
    this->Invert();
    return *this;
}

template <typename T>
TVector3<T> TVector3<T>::GetInvert() const
{
    return TVector3<T>(-this->X, -this->Y, -this->Z);
}

template <typename T>
TVector3<T> TVector3<T>::Cross(const TVector3<T>& InVec) const
{
    return TVector3<T>(
        this->Y * InVec.Z - this->Z * InVec.Y,
        this->Z * InVec.X - this->X * InVec.Z,
        this->X * InVec.Y - this->Y * InVec.X
    );
}

template <typename T>
T TVector3<T>::Dot(const TVector3<T>& InVec) const
{
    return this->X * InVec.X + this->Y * InVec.Y + this->Z * InVec.Z;
}

template <typename T>
void TVector3<T>::Floor()
{
    this->X = Maths::Floor(this->X);
    this->Y = Maths::Floor(this->Y);
    this->Z = Maths::Floor(this->Z);
    return;
}

template <typename T>
TVector3<T> TVector3<T>::GetFloor() const
{
    return TVector3<T>(Maths::Floor(this->X), Maths::Floor(this->Y), Maths::Floor(this->Z));
}

template <typename T>
void TVector3<T>::Ceil()
{
    this->X = Maths::Ceil(this->X);
    this->Y = Maths::Ceil(this->Y);
    this->Z = Maths::Ceil(this->Z);
    return;
}

template <typename T>
TVector3<T> TVector3<T>::GetCeil() const
{
    return TVector3<T>(Maths::Ceil(this->X), Maths::Ceil(this->Y), Maths::Ceil(this->Z));
}

template <typename T>
void TVector3<T>::ModF()
{
    this->X = Maths::ModF(this->X);
    this->Y = Maths::ModF(this->Y);
    this->Z = Maths::ModF(this->Z);
    return;
}

template <typename T>
TVector3<T> TVector3<T>::GetModF() const
{
    return TVector3<T>(Maths::ModF(this->X), Maths::ModF(this->Y), Maths::ModF(this->Z));
}

template <typename T>
EVectorAxis::Type TVector3<T>::GetDominantAxis() const
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
EVectorAxis::Type TVector3<T>::GetMostInferiorAxis() const
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
