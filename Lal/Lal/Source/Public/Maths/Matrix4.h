// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/** Jafg implementation of a four dimensional matrix with TMatrix[C][R] notation. */
template <typename T>
struct TMatrix4 final
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TMatrix must be a floating point type.");

    enum EMatrixAxis : u8
    {
        X = 0,
        Y = 1,
        Z = 2,
        W = 3,
    };

    union
    {
        T Matrix[4][4];
        T Bits[16];
    };

    typedef TVector4<T> ColumnTy;
    typedef TVector4<T> RowTy;
    typedef TMatrix4<T> Self;

    /** Global identity matrix constant. */
    LAL_API static const Self Identity;

    /** Global zero matrix constant. */
    LAL_API static const Self Zero;

    FORCEINLINE           TMatrix4() noexcept { Self::FastCopy(Self::Identity, this); }
    FORCEINLINE           TMatrix4(EForceInit) noexcept { Self::FastCopy(Self::Identity, this); }
    FORCEINLINE           TMatrix4(EZeroInit) noexcept  { Self::FastCopy(Self::Zero, this); }
    FORCEINLINE constexpr TMatrix4(ESkipInit) noexcept { }
    FORCEINLINE           TMatrix4(const Matrix::Type InType) noexcept;
    FORCEINLINE constexpr TMatrix4(const ColumnTy& InX, const ColumnTy& InY, const ColumnTy& InZ, const ColumnTy& InW) noexcept
        : Matrix
        {
            { InX.X, InX.Y, InX.Z, InX.W },
            { InY.X, InY.Y, InY.Z, InY.W },
            { InZ.X, InZ.Y, InZ.Z, InZ.W },
            { InW.X, InW.Y, InW.Z, InW.W }
        }
    {
    }
    FORCEINLINE TMatrix4(const TVector<T>& InX, const TVector<T>& InY, const TVector<T>& InZ, const TVector<T>& InW)
        : Matrix
        {
            { InX.X, InX.Y, InX.Z, 0.0f },
            { InY.X, InY.Y, InY.Z, 0.0f },
            { InZ.X, InZ.Y, InZ.Z, 0.0f },
            { InW.X, InW.Y, InW.Z, 1.0f }
        }
    {
    }
    FORCEINLINE TMatrix4(const Self&  InMatrix) noexcept { Self::FastCopy( InMatrix, this); return; }
    FORCEINLINE TMatrix4(      Self&& InMatrix) noexcept { Self::FastMove(&InMatrix, this); return; }

    FORCEINLINE constexpr       T* GetData()       noexcept { return static_cast<T*>(this); }
    FORCEINLINE constexpr const T* GetData() const noexcept { return reinterpret_cast<const T*>(this); }

    FORCEINLINE constexpr ColumnTy GetRow(const EMatrixAxis Axis) const noexcept { return { this->Matrix[Axis][0], this->Matrix[Axis][1], this->Matrix[Axis][2], this->Matrix[Axis][3] }; }
    FORCEINLINE constexpr RowTy    GetColumn(const EMatrixAxis Axis) const noexcept { return { this->Matrix[0][Axis], this->Matrix[1][Axis], this->Matrix[2][Axis], this->Matrix[3][Axis] }; }

    FORCEINLINE void SetIdentity() noexcept { Self::FastCopy(Self::Identity, this); }
    FORCEINLINE void SetZero() noexcept { Self::FastCopy(Self::Zero, this); }

    FORCEINLINE Self& operator =(const Self&  InMatrix) noexcept { Self::FastCopy( InMatrix, this); return *this; }
    FORCEINLINE Self& operator =(      Self&& InMatrix) noexcept { Self::FastMove(&InMatrix, this); return *this; }

    FORCEINLINE constexpr Self  operator +(const Self& InMatrix) const noexcept;
    FORCEINLINE constexpr Self  operator -(const Self& InMatrix) const noexcept;
    FORCEINLINE constexpr Self  operator *(const Self& InMatrix) const noexcept;
    FORCEINLINE constexpr Self  operator /(const Self& InMatrix) const noexcept;
    FORCEINLINE           Self  operator +(const T Scalar) const noexcept;
    FORCEINLINE           Self  operator -(const T Scalar) const noexcept;
    FORCEINLINE           Self  operator *(const T Scalar) const noexcept;
    FORCEINLINE           Self  operator /(const T Scalar) const noexcept;
    FORCEINLINE           Self  operator -() const noexcept;
    FORCEINLINE constexpr Self& operator+=(const Self& InMatrix) noexcept;
    FORCEINLINE constexpr Self& operator-=(const Self& InMatrix) noexcept;
    FORCEINLINE constexpr Self& operator*=(const Self& InMatrix) noexcept;
    FORCEINLINE constexpr Self& operator/=(const Self& InMatrix) noexcept;
    FORCEINLINE constexpr Self& operator+=(const T Scalar) noexcept;
    FORCEINLINE constexpr Self& operator-=(const T Scalar) noexcept;
    FORCEINLINE constexpr Self& operator*=(const T Scalar) noexcept;
    FORCEINLINE constexpr Self& operator/=(const T Scalar) noexcept;

    FORCEINLINE constexpr bool Equals(const Self& InMatrix, const T InTolerance = static_cast<T>(JAFG_DOUBLE_SMALL_NUMBER)) const noexcept;
    FORCEINLINE           bool operator==(const Self& InMatrix) const noexcept;
    FORCEINLINE           bool operator!=(const Self& InMatrix) const noexcept;

    /** Translate this matrix inline by the given translation vector. */
    FORCEINLINE void InlineTranslate(const TVector3<T>& InTranslation);

    LString ToString() const;

private:

    FORCEINLINE static void FastCopy(const Self& InMatrix, Self* OutMatrix);
    FORCEINLINE static void FastMove(Self* InMatrix, Self* OutMatrix);
};

template<typename T>
FORCEINLINE TMatrix4<T>::TMatrix4(const Matrix::Type InType) noexcept
{
    if (InType == Matrix::Identity)
    {
        Self::FastCopy(Self::Identity, this);
    }
    else if (InType == Matrix::Zero)
    {
        Self::FastCopy(Self::Zero, this);
    }
    else
    {
        panic( "Unknown matrix initialization type." )
    }
}

template <typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self TMatrix4<T>::operator+(const Self& InMatrix) const noexcept
{
    Self Result = SkipInit;
    for (i32 Col = 0; Col < 4; ++Col)
    {
        Result.Matrix[Col][0] = this->Matrix[Col][0] + InMatrix.Matrix[Col][0];
        Result.Matrix[Col][1] = this->Matrix[Col][1] + InMatrix.Matrix[Col][1];
        Result.Matrix[Col][2] = this->Matrix[Col][2] + InMatrix.Matrix[Col][2];
        Result.Matrix[Col][3] = this->Matrix[Col][3] + InMatrix.Matrix[Col][3];

        continue;
    }

    return Result;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self TMatrix4<T>::operator-(const Self& InMatrix) const noexcept
{
    Self Result = SkipInit;
    for (i32 Col = 0; Col < 4; ++Col)
    {
        Result.Matrix[Col][0] = this->Matrix[Col][0] - InMatrix.Matrix[Col][0];
        Result.Matrix[Col][1] = this->Matrix[Col][1] - InMatrix.Matrix[Col][1];
        Result.Matrix[Col][2] = this->Matrix[Col][2] - InMatrix.Matrix[Col][2];
        Result.Matrix[Col][3] = this->Matrix[Col][3] - InMatrix.Matrix[Col][3];

        continue;
    }

    return Result;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self TMatrix4<T>::operator*(const Self& InMatrix) const noexcept
{
    ColumnTy const Me0 = this->Matrix[0];
    ColumnTy const Me1 = this->Matrix[1];
    ColumnTy const Me2 = this->Matrix[2];
    ColumnTy const Me3 = this->Matrix[3];

    ColumnTy const In0 = InMatrix.Matrix[0];
    ColumnTy const In1 = InMatrix.Matrix[1];
    ColumnTy const In2 = InMatrix.Matrix[2];
    ColumnTy const In3 = InMatrix.Matrix[3];

    Self Result = SkipInit;
    Result.Matrix[0] = Me0 * In0[0] + Me1 * In0[1] + Me2 * In0[2] + Me3 * In0[3];
    Result.Matrix[1] = Me0 * In1[0] + Me1 * In1[1] + Me2 * In1[2] + Me3 * In1[3];
    Result.Matrix[2] = Me0 * In2[0] + Me1 * In2[1] + Me2 * In2[2] + Me3 * In2[3];
    Result.Matrix[3] = Me0 * In3[0] + Me1 * In3[1] + Me2 * In3[2] + Me3 * In3[3];

    return Result;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self TMatrix4<T>::operator/(const Self& InMatrix) const noexcept
{
    ColumnTy const Me0 = this->Matrix[0];
    ColumnTy const Me1 = this->Matrix[1];
    ColumnTy const Me2 = this->Matrix[2];
    ColumnTy const Me3 = this->Matrix[3];

    ColumnTy const In0 = InMatrix.Matrix[0];
    ColumnTy const In1 = InMatrix.Matrix[1];
    ColumnTy const In2 = InMatrix.Matrix[2];
    ColumnTy const In3 = InMatrix.Matrix[3];

    Self Result = SkipInit;
    Result.Matrix[0] = Me0 / In0[0] + Me1 / In0[1] + Me2 / In0[2] + Me3 / In0[3];
    Result.Matrix[1] = Me0 / In1[0] + Me1 / In1[1] + Me2 / In1[2] + Me3 / In1[3];
    Result.Matrix[2] = Me0 / In2[0] + Me1 / In2[1] + Me2 / In2[2] + Me3 / In2[3];
    Result.Matrix[3] = Me0 / In3[0] + Me1 / In3[1] + Me2 / In3[2] + Me3 / In3[3];

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::operator+(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] += Scalar;
    Result.Matrix[1][1] += Scalar;
    Result.Matrix[2][2] += Scalar;
    Result.Matrix[3][3] += Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::operator-(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] -= Scalar;
    Result.Matrix[1][1] -= Scalar;
    Result.Matrix[2][2] -= Scalar;
    Result.Matrix[3][3] -= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::operator*(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] *= Scalar;
    Result.Matrix[1][1] *= Scalar;
    Result.Matrix[2][2] *= Scalar;
    Result.Matrix[3][3] *= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::operator/(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] /= Scalar;
    Result.Matrix[1][1] /= Scalar;
    Result.Matrix[2][2] /= Scalar;
    Result.Matrix[3][3] /= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::operator-() const noexcept
{
    Self Result = SkipInit;
    Result.Matrix[0][0] = -this->Matrix[0][0];
    Result.Matrix[0][1] = -this->Matrix[0][1];
    Result.Matrix[0][2] = -this->Matrix[0][2];
    Result.Matrix[0][3] = -this->Matrix[0][3];
    Result.Matrix[1][0] = -this->Matrix[1][0];
    Result.Matrix[1][1] = -this->Matrix[1][1];
    Result.Matrix[1][2] = -this->Matrix[1][2];
    Result.Matrix[1][3] = -this->Matrix[1][3];
    Result.Matrix[2][0] = -this->Matrix[2][0];
    Result.Matrix[2][1] = -this->Matrix[2][1];
    Result.Matrix[2][2] = -this->Matrix[2][2];
    Result.Matrix[2][3] = -this->Matrix[2][3];
    Result.Matrix[3][0] = -this->Matrix[3][0];
    Result.Matrix[3][1] = -this->Matrix[3][1];
    Result.Matrix[3][2] = -this->Matrix[3][2];
    Result.Matrix[3][3] = -this->Matrix[3][3];

    return Result;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator+=(const Self& InMatrix) noexcept
{
    for (i32 Col = 0; Col < 4; ++Col)
    {
        this->Matrix[Col][0] += InMatrix.Matrix[Col][0];
        this->Matrix[Col][1] += InMatrix.Matrix[Col][1];
        this->Matrix[Col][2] += InMatrix.Matrix[Col][2];
        this->Matrix[Col][3] += InMatrix.Matrix[Col][3];

        continue;
    }

    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator-=(const Self& InMatrix) noexcept
{
    for (i32 Col = 0; Col < 4; ++Col)
    {
        this->Matrix[Col][0] -= InMatrix.Matrix[Col][0];
        this->Matrix[Col][1] -= InMatrix.Matrix[Col][1];
        this->Matrix[Col][2] -= InMatrix.Matrix[Col][2];
        this->Matrix[Col][3] -= InMatrix.Matrix[Col][3];

        continue;
    }

    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator*=(const Self& InMatrix) noexcept
{
    *this = *this * InMatrix;
    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator/=(const Self& InMatrix) noexcept
{
    *this = *this / InMatrix;
    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator+=(const T Scalar) noexcept
{
    this->Matrix[0][0] += Scalar;
    this->Matrix[1][1] += Scalar;
    this->Matrix[2][2] += Scalar;
    this->Matrix[3][3] += Scalar;

    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator-=(const T Scalar) noexcept
{
    this->Matrix[0][0] -= Scalar;
    this->Matrix[1][1] -= Scalar;
    this->Matrix[2][2] -= Scalar;
    this->Matrix[3][3] -= Scalar;

    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator*=(const T Scalar) noexcept
{
    this->Matrix[0][0] *= Scalar;
    this->Matrix[1][1] *= Scalar;
    this->Matrix[2][2] *= Scalar;
    this->Matrix[3][3] *= Scalar;

    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator/=(const T Scalar) noexcept
{
    this->Matrix[0][0] /= Scalar;
    this->Matrix[1][1] /= Scalar;
    this->Matrix[2][2] /= Scalar;
    this->Matrix[3][3] /= Scalar;

    return *this;
}

template<typename T>
FORCEINLINE constexpr bool TMatrix4<T>::Equals(const Self& InMatrix, const T InTolerance) const noexcept
{
    for (i32 Col = 0; Col < 4; ++Col)
    {
        for (i32 Row = 0; Row < 4; ++Row)
        {
            if (Maths::Absolute(this->Matrix[Col][Row] - InMatrix.Matrix[Col][Row]) > InTolerance)
            {
                return false;
            }

            continue;
        }

        continue;
    }

    return true;
}

template<typename T>
FORCEINLINE bool TMatrix4<T>::operator==(const Self& InMatrix) const noexcept
{
    static_assert(sizeof(Self) == 16 * sizeof(T), "TMatrix4<T> is not 16 * sizeof(T) bytes large.");
    return ::memcmp(this->GetData(), InMatrix.GetData(), sizeof(Self)) == FALSE;
}

template<typename T>
FORCEINLINE bool TMatrix4<T>::operator!=(const Self& InMatrix) const noexcept
{
    return !(*this == InMatrix);
}

template <typename T>
FORCEINLINE void TMatrix4<T>::InlineTranslate(const TVector3<T>& InTranslation)
{
    this->Matrix[3][0] += InTranslation.X;
    this->Matrix[3][1] += InTranslation.Y;
    this->Matrix[3][2] += InTranslation.Z;

    return;
}

template<typename T>
LString TMatrix4<T>::ToString() const
{
    LString Result;
    for (i32 Row = 0; Row < 4; ++Row)
    {
        Result.Append(LString::SprintF("{:.2f} {:.2f} {:.2f} {:.2f}\n",
            this->Matrix[0][Row], this->Matrix[1][Row], this->Matrix[2][Row], this->Matrix[3][Row]));
    }

    return Result;
}

template <typename T>
FORCEINLINE void TMatrix4<T>::FastCopy(const Self& InMatrix, Self* OutMatrix)
{
    static_assert(sizeof(Self) == 16 * sizeof(T), "TMatrix4<T> is not 16 * sizeof(T) bytes large.");

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
#endif /* WITH_GCC */
    ::memcpy(OutMatrix, &InMatrix, sizeof(Self));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    return;
}

template<typename T>
FORCEINLINE void TMatrix4<T>::FastMove(Self* InMatrix, Self* OutMatrix)
{
    static_assert(sizeof(Self) == 16 * sizeof(T), "TMatrix4<T> is not 16 * sizeof(T) bytes large.");

    Self::FastCopy(*InMatrix, OutMatrix);
    InMatrix->SetIdentity();

    return;
}

} /* ~Namespace Jafg */
