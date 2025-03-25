// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/** Jafg implementation of an integral Vector. */
template <typename T>
struct TIntVector final
{
    static_assert(std::is_integral_v<T>, "Generic type T of TIntVector must be an integral type.");

    using LReal = float;

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
    static const TIntVector<T> ZeroVector;

    /** Global one vector constant (1, 1, 1). */
    static const TIntVector<T> OneVector;

    /** Global up vector constant (0, 0, 1). */
    static const TIntVector<T> UpVector;

    /** Global down vector constant (0, 0, -1). */
    static const TIntVector<T> DownVector;

    /** Global forward vector constant (1, 0, 0). */
    static const TIntVector<T> ForwardVector;

    /** Global backward vector constant (-1, 0, 0). */
    static const TIntVector<T> BackwardVector;

    /** Global right vector constant (0, 1, 0). */
    static const TIntVector<T> RightVector;

    /** Global left vector constant (0, -1, 0). */
    static const TIntVector<T> LeftVector;

    /** Global unit vector constant along the x-axis (1, 0, 0). */
    static const TIntVector<T> UnitVectorX;

    /** Global unit vector constant along the y-axis (0, 1, 0). */
    static const TIntVector<T> UnitVectorY;

    /** Global unit vector constant along the z-axis (0, 0, 1). */
    static const TIntVector<T> UnitVectorZ;

    FORCEINLINE static TIntVector<T> Zero()     { return TIntVector<T>::ZeroVector;     }
    FORCEINLINE static TIntVector<T> One()      { return TIntVector<T>::OneVector;      }
    FORCEINLINE static TIntVector<T> Up()       { return TIntVector<T>::UpVector;       }
    FORCEINLINE static TIntVector<T> Down()     { return TIntVector<T>::DownVector;     }
    FORCEINLINE static TIntVector<T> Forward()  { return TIntVector<T>::ForwardVector;  }
    FORCEINLINE static TIntVector<T> Backward() { return TIntVector<T>::BackwardVector; }
    FORCEINLINE static TIntVector<T> Right()    { return TIntVector<T>::RightVector;    }
    FORCEINLINE static TIntVector<T> Left()     { return TIntVector<T>::LeftVector;     }
    FORCEINLINE static TIntVector<T> UnitX()    { return TIntVector<T>::UnitVectorX;    }
    FORCEINLINE static TIntVector<T> UnitY()    { return TIntVector<T>::UnitVectorY;    }
    FORCEINLINE static TIntVector<T> UnitZ()    { return TIntVector<T>::UnitVectorZ;    }

    FORCEINLINE          TIntVector()                                        = default;
    FORCEINLINE explicit TIntVector(const T InIntegral)                      noexcept : X(InIntegral), Y(InIntegral), Z(InIntegral) { }
    FORCEINLINE explicit TIntVector(const T InX, const T InY, const T InZ)   noexcept : X(InX), Y(InY), Z(InZ) { }
    FORCEINLINE explicit TIntVector(const TIntVector2<T> InVec, const T InZ) noexcept : X(InVec.X), Y(InVec.Y), Z(InZ) { }
    FORCEINLINE explicit TIntVector(const T InXYZ[3])                        noexcept : X(InXYZ[0]), Y(InXYZ[1]), Z(InXYZ[2]) { }
    FORCEINLINE          TIntVector(const TIntVector<T>& InVec)              noexcept : X(InVec.X), Y(InVec.Y), Z(InVec.Z) { }
    FORCEINLINE          TIntVector(TIntVector<T>&& InVec)                   noexcept : X(InVec.X), Y(InVec.Y), Z(InVec.Z) { }

    FORCEINLINE auto GetData()       noexcept ->       T* { return &this->X; }
    FORCEINLINE auto GetData() const noexcept -> const T* { return &this->X; }

    FORCEINLINE auto operator[](const i32 InIndex)       ->       T&;
    FORCEINLINE auto operator[](const i32 InIndex) const -> const T&;

    FORCEINLINE TIntVector<T>& operator =(const TIntVector<T>&  InVec) noexcept;
    FORCEINLINE TIntVector<T>& operator =(      TIntVector<T>&& InVec) noexcept;
    FORCEINLINE TIntVector<T>& operator =(const TIntVector<T>&& InVec) noexcept = delete;

    FORCEINLINE TIntVector<T>  operator +(const T              InScalar) const;
    FORCEINLINE TIntVector<T>  operator +(const TIntVector<T>& InVec   ) const;
    FORCEINLINE TIntVector<T>& operator+=(const T              InScalar);
    FORCEINLINE TIntVector<T>& operator+=(const TIntVector<T>& InVec   );
    FORCEINLINE TIntVector<T>  operator -(const T              InScalar) const;
    FORCEINLINE TIntVector<T>  operator -(const TIntVector<T>& InVec   ) const;
    FORCEINLINE TIntVector<T>& operator-=(const T              InScalar);
    FORCEINLINE TIntVector<T>& operator-=(const TIntVector<T>& InVec   );
    FORCEINLINE TIntVector<T>  operator *(const T              InScalar) const;
    FORCEINLINE TIntVector<T>  operator *(const TIntVector<T>& InVec   ) const;
    FORCEINLINE TIntVector<T>& operator*=(const T              InScalar);
    FORCEINLINE TIntVector<T>& operator*=(const TIntVector<T>& InVec   );
    FORCEINLINE TIntVector<T>  operator -() const { return TIntVector<T>(-this->X, -this->Y, -this->Z); }

    FORCEINLINE bool Equals(const TIntVector<T>& InVec) const;
    FORCEINLINE bool operator==(const TIntVector<T>& InVec) const;
    FORCEINLINE bool operator!=(const TIntVector<T>& InVec) const;

    FORCEINLINE bool IsZero() const;

    FORCEINLINE auto Magnitude() const -> LReal;
    FORCEINLINE auto SquaredMagnitude() const -> LReal;
    FORCEINLINE auto Magnitude2D() const -> LReal;
    FORCEINLINE auto SquaredMagnitude2D() const -> LReal;

    FORCEINLINE auto Invert() -> void;
    FORCEINLINE auto InvertRet() -> TIntVector<T>&;
    FORCEINLINE auto GetInvert() const -> TIntVector<T>;
};

template <typename T>
T& TIntVector<T>::operator[](const i32 InIndex)
{
    check( InIndex > INDEX_NONE && InIndex < 3 )
    return this->XYZ[InIndex];
}

template <typename T>
const T& TIntVector<T>::operator[](const i32 InIndex) const
{
    check( InIndex > INDEX_NONE && InIndex < 3 )
    return this->XYZ[InIndex];
}

template <typename T>
TIntVector<T>& TIntVector<T>::operator=(const TIntVector<T>& InVec) noexcept
{
    this->X = InVec.X;
    this->Y = InVec.Y;
    this->Z = InVec.Z;
    return *this;
}

template <typename T>
TIntVector<T>& TIntVector<T>::operator=(TIntVector<T>&& InVec) noexcept
{
    this->X = InVec.X;
    this->Y = InVec.Y;
    this->Z = InVec.Z;
    return *this;
}

template <typename T>
TIntVector<T> TIntVector<T>::operator+(const T InScalar) const
{
    return TIntVector<T>(this->X + InScalar, this->Y + InScalar, this->Z + InScalar);
}

template <typename T>
TIntVector<T> TIntVector<T>::operator+(const TIntVector<T>& InVec) const
{
    return TIntVector<T>(this->X + InVec.X, this->Y + InVec.Y, this->Z + InVec.Z);
}

template <typename T>
TIntVector<T>& TIntVector<T>::operator+=(const T InScalar)
{
    this->X += InScalar;
    this->Y += InScalar;
    this->Z += InScalar;
    return *this;
}

template <typename T>
TIntVector<T>& TIntVector<T>::operator+=(const TIntVector<T>& InVec)
{
    this->X += InVec.X;
    this->Y += InVec.Y;
    this->Z += InVec.Z;
    return *this;
}

template <typename T>
TIntVector<T> TIntVector<T>::operator-(const T InScalar) const
{
    return TIntVector<T>(this->X - InScalar, this->Y - InScalar, this->Z - InScalar);
}

template <typename T>
TIntVector<T> TIntVector<T>::operator-(const TIntVector<T>& InVec) const
{
    return TIntVector<T>(this->X - InVec.X, this->Y - InVec.Y, this->Z - InVec.Z);
}

template <typename T>
TIntVector<T>& TIntVector<T>::operator-=(const T InScalar)
{
    this->X -= InScalar;
    this->Y -= InScalar;
    this->Z -= InScalar;
    return *this;
}

template <typename T>
TIntVector<T>& TIntVector<T>::operator-=(const TIntVector<T>& InVec)
{
    this->X -= InVec.X;
    this->Y -= InVec.Y;
    this->Z -= InVec.Z;
    return *this;
}

template <typename T>
TIntVector<T> TIntVector<T>::operator*(const T InScalar) const
{
    return TIntVector<T>(this->X * InScalar, this->Y * InScalar, this->Z * InScalar);
}

template <typename T>
TIntVector<T> TIntVector<T>::operator*(const TIntVector<T>& InVec) const
{
    return TIntVector<T>(this->X * InVec.X, this->Y * InVec.Y, this->Z * InVec.Z);
}

template <typename T>
TIntVector<T>& TIntVector<T>::operator*=(const T InScalar)
{
    this->X *= InScalar;
    this->Y *= InScalar;
    this->Z *= InScalar;
    return *this;
}

template <typename T>
TIntVector<T>& TIntVector<T>::operator*=(const TIntVector<T>& InVec)
{
    this->X *= InVec.X;
    this->Y *= InVec.Y;
    this->Z *= InVec.Z;
    return *this;
}

template <typename T>
bool TIntVector<T>::Equals(const TIntVector<T>& InVec) const
{
    return *this == InVec;
}

template <typename T>
bool TIntVector<T>::operator==(const TIntVector<T>& InVec) const
{
    return this->X == InVec.X && this->Y == InVec.Y && this->Z == InVec.Z;
}

template <typename T>
bool TIntVector<T>::operator!=(const TIntVector<T>& InVec) const
{
    return !(*this == InVec);
}

template <typename T>
bool TIntVector<T>::IsZero() const
{
    return this->X == 0 && this->Y == 0 && this->Z == 0;
}

template <typename T>
typename TIntVector<T>::LReal TIntVector<T>::Magnitude() const
{
    return Maths::Sqrt(static_cast<LReal>(this->X * this->X + this->Y * this->Y + this->Z * this->Z));
}

template <typename T>
typename TIntVector<T>::LReal TIntVector<T>::SquaredMagnitude() const
{
    return static_cast<LReal>(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
}

template <typename T>
typename TIntVector<T>::LReal TIntVector<T>::Magnitude2D() const
{
    return Maths::Sqrt(static_cast<LReal>(this->X * this->X + this->Y * this->Y));
}

template <typename T>
typename TIntVector<T>::LReal TIntVector<T>::SquaredMagnitude2D() const
{
    return static_cast<LReal>(this->X * this->X + this->Y * this->Y);
}

template <typename T>
void TIntVector<T>::Invert()
{
    this->X = -this->X;
    this->Y = -this->Y;
    this->Z = -this->Z;
}

template <typename T>
TIntVector<T>& TIntVector<T>::InvertRet()
{
    this->Invert();
    return *this;
}

template <typename T>
TIntVector<T> TIntVector<T>::GetInvert() const
{
    return TIntVector<T>(-this->X, -this->Y, -this->Z);
}

} /* ~Namespace Jafg */
