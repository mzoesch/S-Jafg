// Copyright mzoesch. All rights reserved.

#pragma once

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
    FORCEINLINE TMatrix4(const TPlane<T>& InX, const TPlane<T>& InY, const TPlane<T>& InZ, const TPlane<T>& InW)
        : Matrix
        {
                { InX.X, InX.Y, InX.Z, InX.W },
                { InY.X, InY.Y, InY.Z, InY.W },
                { InZ.X, InZ.Y, InZ.Z, InZ.W },
                { InW.X, InW.Y, InW.Z, InW.W }
        }
    {
    }
    FORCEINLINE TMatrix4(const Self&  InMatrix) noexcept { Self::FastCopy( InMatrix, this); return; }
    FORCEINLINE TMatrix4(      Self&& InMatrix) noexcept { Self::FastMove(&InMatrix, this); return; }

    FORCEINLINE constexpr       T* GetData()       noexcept { return reinterpret_cast<T*>(this); }
    FORCEINLINE constexpr const T* GetData() const noexcept { return reinterpret_cast<const T*>(this); }

    FORCEINLINE constexpr ColumnTy GetRow(const EMatrixAxis Axis) const noexcept { return { this->Matrix[Axis][0], this->Matrix[Axis][1], this->Matrix[Axis][2], this->Matrix[Axis][3] }; }
    FORCEINLINE constexpr RowTy    GetColumn(const EMatrixAxis Axis) const noexcept { return { this->Matrix[0][Axis], this->Matrix[1][Axis], this->Matrix[2][Axis], this->Matrix[3][Axis] }; }

    FORCEINLINE void SetIdentity() noexcept { Self::FastCopy(Self::Identity, this); }
    FORCEINLINE void SetZero() noexcept { Self::FastCopy(Self::Zero, this); }

    FORCEINLINE Self& operator =(const Self&  InMatrix) noexcept { Self::FastCopy( InMatrix, this); return *this; }
    FORCEINLINE Self& operator =(      Self&& InMatrix) noexcept { Self::FastMove(&InMatrix, this); return *this; }

    FORCEINLINE constexpr Self     operator +(const Self& InMatrix) const noexcept;
    FORCEINLINE constexpr Self     operator -(const Self& InMatrix) const noexcept;
    FORCEINLINE constexpr Self     operator *(const Self& InMatrix) const noexcept;
    FORCEINLINE constexpr Self     operator /(const Self& InMatrix) const noexcept;
    FORCEINLINE           Self     operator +(const T Scalar) const noexcept;
    FORCEINLINE           Self     operator -(const T Scalar) const noexcept;
    FORCEINLINE           Self     operator *(const T Scalar) const noexcept;
    FORCEINLINE           Self     operator /(const T Scalar) const noexcept;
    FORCEINLINE constexpr ColumnTy operator *(const ColumnTy& Vector) const noexcept;
    FORCEINLINE           Self     operator -() const noexcept;
    FORCEINLINE constexpr Self&    operator+=(const Self& InMatrix) noexcept;
    FORCEINLINE constexpr Self&    operator-=(const Self& InMatrix) noexcept;
    FORCEINLINE constexpr Self&    operator*=(const Self& InMatrix) noexcept;
    FORCEINLINE constexpr Self&    operator/=(const Self& InMatrix) noexcept;
    FORCEINLINE constexpr Self&    operator+=(const T Scalar) noexcept;
    FORCEINLINE constexpr Self&    operator-=(const T Scalar) noexcept;
    FORCEINLINE constexpr Self&    operator*=(const T Scalar) noexcept;
    FORCEINLINE constexpr Self&    operator/=(const T Scalar) noexcept;

    FORCEINLINE constexpr bool Equals(const Self& InMatrix, const T InTolerance = static_cast<T>(LAL_DOUBLE_SMALL_NUMBER)) const noexcept;
    FORCEINLINE           bool operator==(const Self& InMatrix) const noexcept;
    FORCEINLINE           bool operator!=(const Self& InMatrix) const noexcept;

    /** Translate this matrix inline by the given translation vector. */
    FORCEINLINE void InlineTranslate(const TVector3<T>& InTranslation);

    FORCEINLINE void InlineScale(const TVector3<T>& InScale) noexcept;

    FORCEINLINE Self GetInverse() const;

    LString ToString() const;

private:

    FORCEINLINE static void FastCopy(const Self& InMatrix, Self* OutMatrix);
    FORCEINLINE static void FastMove(Self* InMatrix, Self* OutMatrix);
    FORCEINLINE        void SetColumn(const EMatrixAxis Axis, const ColumnTy& Vector);
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
    const ColumnTy Me0 = this->Matrix[0];
    const ColumnTy Me1 = this->Matrix[1];
    const ColumnTy Me2 = this->Matrix[2];
    const ColumnTy Me3 = this->Matrix[3];

    const ColumnTy In0 = InMatrix.Matrix[0];
    const ColumnTy In1 = InMatrix.Matrix[1];
    const ColumnTy In2 = InMatrix.Matrix[2];
    const ColumnTy In3 = InMatrix.Matrix[3];

    Self Result = SkipInit;
    Result.SetColumn(X, Me0 * In0[0] + Me1 * In0[1] + Me2 * In0[2] + Me3 * In0[3]);
    Result.SetColumn(Y, Me0 * In1[0] + Me1 * In1[1] + Me2 * In1[2] + Me3 * In1[3]);
    Result.SetColumn(Z, Me0 * In2[0] + Me1 * In2[1] + Me2 * In2[2] + Me3 * In2[3]);
    Result.SetColumn(W, Me0 * In3[0] + Me1 * In3[1] + Me2 * In3[2] + Me3 * In3[3]);

    return Result;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self TMatrix4<T>::operator/(const Self& InMatrix) const noexcept
{
    const ColumnTy Me0 = this->Matrix[0];
    const ColumnTy Me1 = this->Matrix[1];
    const ColumnTy Me2 = this->Matrix[2];
    const ColumnTy Me3 = this->Matrix[3];

    const ColumnTy In0 = InMatrix.Matrix[0];
    const ColumnTy In1 = InMatrix.Matrix[1];
    const ColumnTy In2 = InMatrix.Matrix[2];
    const ColumnTy In3 = InMatrix.Matrix[3];

    Self Result = SkipInit;
    Result.SetColumn(X, Me0 / In0[0] + Me1 / In0[1] + Me2 / In0[2] + Me3 / In0[3]);
    Result.SetColumn(Y, Me0 / In1[0] + Me1 / In1[1] + Me2 / In1[2] + Me3 / In1[3]);
    Result.SetColumn(Z, Me0 / In2[0] + Me1 / In2[1] + Me2 / In2[2] + Me3 / In2[3]);
    Result.SetColumn(W, Me0 / In3[0] + Me1 / In3[1] + Me2 / In3[2] + Me3 / In3[3]);

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::operator+(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] += Scalar;
    Result.Matrix[0][1] += Scalar;
    Result.Matrix[0][2] += Scalar;
    Result.Matrix[0][3] += Scalar;
    Result.Matrix[1][0] += Scalar;
    Result.Matrix[1][1] += Scalar;
    Result.Matrix[1][2] += Scalar;
    Result.Matrix[1][3] += Scalar;
    Result.Matrix[2][0] += Scalar;
    Result.Matrix[2][1] += Scalar;
    Result.Matrix[2][2] += Scalar;
    Result.Matrix[2][3] += Scalar;
    Result.Matrix[3][0] += Scalar;
    Result.Matrix[3][1] += Scalar;
    Result.Matrix[3][2] += Scalar;
    Result.Matrix[3][3] += Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::operator-(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] -= Scalar;
    Result.Matrix[0][1] -= Scalar;
    Result.Matrix[0][2] -= Scalar;
    Result.Matrix[0][3] -= Scalar;
    Result.Matrix[1][0] -= Scalar;
    Result.Matrix[1][1] -= Scalar;
    Result.Matrix[1][2] -= Scalar;
    Result.Matrix[1][3] -= Scalar;
    Result.Matrix[2][0] -= Scalar;
    Result.Matrix[2][1] -= Scalar;
    Result.Matrix[2][2] -= Scalar;
    Result.Matrix[2][3] -= Scalar;
    Result.Matrix[3][0] -= Scalar;
    Result.Matrix[3][1] -= Scalar;
    Result.Matrix[3][2] -= Scalar;
    Result.Matrix[3][3] -= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::operator*(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] *= Scalar;
    Result.Matrix[0][1] *= Scalar;
    Result.Matrix[0][2] *= Scalar;
    Result.Matrix[0][3] *= Scalar;
    Result.Matrix[1][0] *= Scalar;
    Result.Matrix[1][1] *= Scalar;
    Result.Matrix[1][2] *= Scalar;
    Result.Matrix[1][3] *= Scalar;
    Result.Matrix[2][0] *= Scalar;
    Result.Matrix[2][1] *= Scalar;
    Result.Matrix[2][2] *= Scalar;
    Result.Matrix[2][3] *= Scalar;
    Result.Matrix[3][0] *= Scalar;
    Result.Matrix[3][1] *= Scalar;
    Result.Matrix[3][2] *= Scalar;
    Result.Matrix[3][3] *= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::operator/(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] /= Scalar;
    Result.Matrix[0][1] /= Scalar;
    Result.Matrix[0][2] /= Scalar;
    Result.Matrix[0][3] /= Scalar;
    Result.Matrix[1][0] /= Scalar;
    Result.Matrix[1][1] /= Scalar;
    Result.Matrix[1][2] /= Scalar;
    Result.Matrix[1][3] /= Scalar;
    Result.Matrix[2][0] /= Scalar;
    Result.Matrix[2][1] /= Scalar;
    Result.Matrix[2][2] /= Scalar;
    Result.Matrix[2][3] /= Scalar;
    Result.Matrix[3][0] /= Scalar;
    Result.Matrix[3][1] /= Scalar;
    Result.Matrix[3][2] /= Scalar;
    Result.Matrix[3][3] /= Scalar;

    return Result;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::ColumnTy TMatrix4<T>::operator*(const ColumnTy& Vector) const noexcept
{
    const ColumnTy Mov0(Vector[0]);
    const ColumnTy Mov1(Vector[1]);
    const ColumnTy Mul0 = ColumnTy(this->Matrix[0]) * Mov0;
    const ColumnTy Mul1 = ColumnTy(this->Matrix[1]) * Mov1;
    const ColumnTy Add0 = Mul0 + Mul1;
    const ColumnTy Mov2(Vector[2]);
    const ColumnTy Mov3(Vector[3]);
    const ColumnTy Mul2 = ColumnTy(this->Matrix[2]) * Mov2;
    const ColumnTy Mul3 = ColumnTy(this->Matrix[3]) * Mov3;
    const ColumnTy Add1 = Mul2 + Mul3;
    const ColumnTy Add2 = Add0 + Add1;

    return Add2;
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
    this->Matrix[0][1] += Scalar;
    this->Matrix[0][2] += Scalar;
    this->Matrix[0][3] += Scalar;
    this->Matrix[1][0] += Scalar;
    this->Matrix[1][1] += Scalar;
    this->Matrix[1][2] += Scalar;
    this->Matrix[1][3] += Scalar;
    this->Matrix[2][0] += Scalar;
    this->Matrix[2][1] += Scalar;
    this->Matrix[2][2] += Scalar;
    this->Matrix[2][3] += Scalar;
    this->Matrix[3][0] += Scalar;
    this->Matrix[3][1] += Scalar;
    this->Matrix[3][2] += Scalar;
    this->Matrix[3][3] += Scalar;

    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator-=(const T Scalar) noexcept
{
    this->Matrix[0][0] -= Scalar;
    this->Matrix[0][1] -= Scalar;
    this->Matrix[0][2] -= Scalar;
    this->Matrix[0][3] -= Scalar;
    this->Matrix[1][0] -= Scalar;
    this->Matrix[1][1] -= Scalar;
    this->Matrix[1][2] -= Scalar;
    this->Matrix[1][3] -= Scalar;
    this->Matrix[2][0] -= Scalar;
    this->Matrix[2][1] -= Scalar;
    this->Matrix[2][2] -= Scalar;
    this->Matrix[2][3] -= Scalar;
    this->Matrix[3][0] -= Scalar;
    this->Matrix[3][1] -= Scalar;
    this->Matrix[3][2] -= Scalar;
    this->Matrix[3][3] -= Scalar;

    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator*=(const T Scalar) noexcept
{
    this->Matrix[0][0] *= Scalar;
    this->Matrix[0][1] *= Scalar;
    this->Matrix[0][2] *= Scalar;
    this->Matrix[0][3] *= Scalar;
    this->Matrix[1][0] *= Scalar;
    this->Matrix[1][1] *= Scalar;
    this->Matrix[1][2] *= Scalar;
    this->Matrix[1][3] *= Scalar;
    this->Matrix[2][0] *= Scalar;
    this->Matrix[2][1] *= Scalar;
    this->Matrix[2][2] *= Scalar;
    this->Matrix[2][3] *= Scalar;
    this->Matrix[3][0] *= Scalar;
    this->Matrix[3][1] *= Scalar;
    this->Matrix[3][2] *= Scalar;
    this->Matrix[3][3] *= Scalar;

    return *this;
}

template<typename T>
FORCEINLINE constexpr typename TMatrix4<T>::Self& TMatrix4<T>::operator/=(const T Scalar) noexcept
{
    this->Matrix[0][0] /= Scalar;
    this->Matrix[0][1] /= Scalar;
    this->Matrix[0][2] /= Scalar;
    this->Matrix[0][3] /= Scalar;
    this->Matrix[1][0] /= Scalar;
    this->Matrix[1][1] /= Scalar;
    this->Matrix[1][2] /= Scalar;
    this->Matrix[1][3] /= Scalar;
    this->Matrix[2][0] /= Scalar;
    this->Matrix[2][1] /= Scalar;
    this->Matrix[2][2] /= Scalar;
    this->Matrix[2][3] /= Scalar;
    this->Matrix[3][0] /= Scalar;
    this->Matrix[3][1] /= Scalar;
    this->Matrix[3][2] /= Scalar;
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
FORCEINLINE void TMatrix4<T>::InlineScale(const TVector3<T>& InScale) noexcept
{
    TMatrix4<T> ScaleMatrix { ZeroInit };
    ScaleMatrix.Matrix[0][0] = InScale.X;
    ScaleMatrix.Matrix[1][1] = InScale.Y;
    ScaleMatrix.Matrix[2][2] = InScale.Z;
    ScaleMatrix.Matrix[3][3] = 1.0f;

    *this *= ScaleMatrix;

    return;
}

template<typename T>
FORCEINLINE typename TMatrix4<T>::Self TMatrix4<T>::GetInverse() const
{
    T Coef00 = this->Matrix[2][2] * this->Matrix[3][3] - this->Matrix[3][2] * this->Matrix[2][3];
    T Coef02 = this->Matrix[1][2] * this->Matrix[3][3] - this->Matrix[3][2] * this->Matrix[1][3];
    T Coef03 = this->Matrix[1][2] * this->Matrix[2][3] - this->Matrix[2][2] * this->Matrix[1][3];

    T Coef04 = this->Matrix[2][1] * this->Matrix[3][3] - this->Matrix[3][1] * this->Matrix[2][3];
    T Coef06 = this->Matrix[1][1] * this->Matrix[3][3] - this->Matrix[3][1] * this->Matrix[1][3];
    T Coef07 = this->Matrix[1][1] * this->Matrix[2][3] - this->Matrix[2][1] * this->Matrix[1][3];

    T Coef08 = this->Matrix[2][1] * this->Matrix[3][2] - this->Matrix[3][1] * this->Matrix[2][2];
    T Coef10 = this->Matrix[1][1] * this->Matrix[3][2] - this->Matrix[3][1] * this->Matrix[1][2];
    T Coef11 = this->Matrix[1][1] * this->Matrix[2][2] - this->Matrix[2][1] * this->Matrix[1][2];

    T Coef12 = this->Matrix[2][0] * this->Matrix[3][3] - this->Matrix[3][0] * this->Matrix[2][3];
    T Coef14 = this->Matrix[1][0] * this->Matrix[3][3] - this->Matrix[3][0] * this->Matrix[1][3];
    T Coef15 = this->Matrix[1][0] * this->Matrix[2][3] - this->Matrix[2][0] * this->Matrix[1][3];

    T Coef16 = this->Matrix[2][0] * this->Matrix[3][2] - this->Matrix[3][0] * this->Matrix[2][2];
    T Coef18 = this->Matrix[1][0] * this->Matrix[3][2] - this->Matrix[3][0] * this->Matrix[1][2];
    T Coef19 = this->Matrix[1][0] * this->Matrix[2][2] - this->Matrix[2][0] * this->Matrix[1][2];

    T Coef20 = this->Matrix[2][0] * this->Matrix[3][1] - this->Matrix[3][0] * this->Matrix[2][1];
    T Coef22 = this->Matrix[1][0] * this->Matrix[3][1] - this->Matrix[3][0] * this->Matrix[1][1];
    T Coef23 = this->Matrix[1][0] * this->Matrix[2][1] - this->Matrix[2][0] * this->Matrix[1][1];

    TVector4<T> Fac0(Coef00, Coef00, Coef02, Coef03);
    TVector4<T> Fac1(Coef04, Coef04, Coef06, Coef07);
    TVector4<T> Fac2(Coef08, Coef08, Coef10, Coef11);
    TVector4<T> Fac3(Coef12, Coef12, Coef14, Coef15);
    TVector4<T> Fac4(Coef16, Coef16, Coef18, Coef19);
    TVector4<T> Fac5(Coef20, Coef20, Coef22, Coef23);

    TVector4<T> Vec0(this->Matrix[1][0], this->Matrix[0][0], this->Matrix[0][0], this->Matrix[0][0]);
    TVector4<T> Vec1(this->Matrix[1][1], this->Matrix[0][1], this->Matrix[0][1], this->Matrix[0][1]);
    TVector4<T> Vec2(this->Matrix[1][2], this->Matrix[0][2], this->Matrix[0][2], this->Matrix[0][2]);
    TVector4<T> Vec3(this->Matrix[1][3], this->Matrix[0][3], this->Matrix[0][3], this->Matrix[0][3]);

    TVector4<T> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
    TVector4<T> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
    TVector4<T> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
    TVector4<T> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

    TVector4<T> SignA(+1, -1, +1, -1);
    TVector4<T> SignB(-1, +1, -1, +1);
    Self        Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

    TVector4<T> Row0(Inverse.Matrix[0][0], Inverse.Matrix[1][0], Inverse.Matrix[2][0], Inverse.Matrix[3][0]);

    TVector4<T> Dot0(TVector4<T>(this->Matrix[0]) * Row0);
    T Dot1 = (Dot0.X + Dot0.Y) + (Dot0.Z + Dot0.W);

    T OneOverDeterminant = static_cast<T>(1) / Dot1;

    return Inverse * OneOverDeterminant;
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

#if LAL_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wnontrivial-memcall"
#endif /* LAL_WITH_CLANG */
    ::memcpy(OutMatrix, &InMatrix, sizeof(Self));
#if LAL_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* LAL_WITH_CLANG */

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

template<typename T>
FORCEINLINE void TMatrix4<T>::SetColumn(const EMatrixAxis Axis, const ColumnTy& Vector)
{
    ::memcpy(this->Matrix[Axis], &Vector, sizeof(ColumnTy));
}
