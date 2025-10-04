// Copyright mzoesch. All rights reserved.

#pragma once

/** Jafg implementation of a three-dimensional matrix with TMatrix3[C][R] notation. */
template <typename T>
struct TMatrix3 final
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TMatrix must be a floating point type.");

    enum EMatrixAxis : u8
    {
        X    = 0,
        Y    = 1,
        Z    = 2,
    };

    union
    {
        T Matrix[3][3];
        T Bits[9];
    };

    typedef TVector3<T> ColumnTy;
    typedef TVector3<T> RowTy;
    typedef TMatrix3<T> Self;

    /** Global identity matrix constant. */
    LAL_API static const Self Identity;

    /** Global zero matrix constant. */
    LAL_API static const Self Zero;

    FORCEINLINE           TMatrix3() noexcept { Self::FastCopy(Self::Identity, this); }
    FORCEINLINE           TMatrix3(EForceInit) noexcept { Self::FastCopy(Self::Identity, this); }
    FORCEINLINE           TMatrix3(EZeroInit) noexcept { Self::FastCopy(Self::Identity, this); }
    FORCEINLINE constexpr TMatrix3(ESkipInit) noexcept { }
    FORCEINLINE           TMatrix3(const Matrix::Type InType) noexcept;
    FORCEINLINE constexpr TMatrix3(const ColumnTy& InX, const ColumnTy& InY, const ColumnTy& InZ) noexcept
        : Matrix
        {
            { InX.X, InX.Y, InX.Z },
            { InY.X, InY.Y, InY.Z },
            { InZ.X, InZ.Y, InZ.Z }
        }
    {
    }
    FORCEINLINE TMatrix3(const TMatrix3&  InMatrix) noexcept { Self::FastCopy( InMatrix, this); return; }
    FORCEINLINE TMatrix3(      TMatrix3&& InMatrix) noexcept { Self::FastMove(&InMatrix, this); return; }

    FORCEINLINE constexpr       T* GetData()       noexcept { return static_cast<T*>(this); }
    FORCEINLINE constexpr const T* GetData() const noexcept { return reinterpret_cast<const T*>(this); }

    FORCEINLINE constexpr ColumnTy GetRow(const EMatrixAxis Axis) const noexcept { return { this->Matrix[Axis][0], this->Matrix[Axis][1], this->Matrix[Axis][2] }; }
    FORCEINLINE constexpr RowTy    GetColumn(const EMatrixAxis Axis) const noexcept { return { this->Matrix[0][Axis], this->Matrix[1][Axis], this->Matrix[2][Axis] }; }

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

    FORCEINLINE constexpr bool Equals(const Self& InMatrix, const T InTolerance = static_cast<T>(LAL_DOUBLE_SMALL_NUMBER)) const noexcept;
    FORCEINLINE           bool operator==(const Self& InMatrix) const noexcept;
    FORCEINLINE           bool operator!=(const Self& InMatrix) const noexcept;

    FORCEINLINE constexpr  T GetDeterminant() const noexcept;
    FORCEINLINE constexpr  T GetOneOverDeterminant() const noexcept;
    FORCEINLINE constexpr  T GetOneOverDeterminantChecked() const noexcept;
    FORCEINLINE constexpr  T GetOneOverDeterminantSafe() const noexcept;
    FORCEINLINE constexpr  Self GetInverse() const noexcept;

    LString ToString() const;

private:

    /**
     * @param A         Left-hand side matrix.
     * @param B         Right-hand side matrix.
     * @param OutMatrix Resulting matrix. Must fulfill OutMatrix != InMatrixA && OutMatrix != InMatrixB.
     */
    FORCEINLINE static void FastMultiply(const Self& A, const Self& B, Self* OutMatrix);
    FORCEINLINE static void FastCopy(const Self& InMatrix, Self* OutMatrix) noexcept;
    FORCEINLINE static void FastMove(Self* InMatrix, Self* OutMatrix) noexcept;
};

template <typename T>
FORCEINLINE TMatrix3<T>::TMatrix3(const Matrix::Type InType) noexcept
{
    if (InType == Matrix::Identity)
    {
        this->SetIdentity();
    }
    else if (InType == Matrix::Zero)
    {
        ::memset(this->GetData(), 0, sizeof(this->Matrix));
    }
    else
    {
        panic( "Unknown matrix initialization type." )
    }
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self TMatrix3<T>::operator+(const Self& InMatrix) const noexcept
{
    Self Result = SkipInit;
    for (i32 Col = 0; Col < 3; ++Col)
    {
        Result.Matrix[Col][0] = this->Matrix[Col][0] + InMatrix.Matrix[Col][0];
        Result.Matrix[Col][1] = this->Matrix[Col][1] + InMatrix.Matrix[Col][1];
        Result.Matrix[Col][2] = this->Matrix[Col][2] + InMatrix.Matrix[Col][2];

        continue;
    }

    return Result;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self TMatrix3<T>::operator-(const Self& InMatrix) const noexcept
{
    Self Result = SkipInit;
    for (i32 Col = 0; Col < 3; ++Col)
    {
        Result.Matrix[Col][0] = this->Matrix[Col][0] - InMatrix.Matrix[Col][0];
        Result.Matrix[Col][1] = this->Matrix[Col][1] - InMatrix.Matrix[Col][1];
        Result.Matrix[Col][2] = this->Matrix[Col][2] - InMatrix.Matrix[Col][2];

        continue;
    }

    return Result;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self TMatrix3<T>::operator*(const Self& InMatrix) const noexcept
{
    const T Me00 = this->Matrix[0][0];
    const T Me01 = this->Matrix[0][1];
    const T Me02 = this->Matrix[0][2];
    const T Me10 = this->Matrix[1][0];
    const T Me11 = this->Matrix[1][1];
    const T Me12 = this->Matrix[1][2];
    const T Me20 = this->Matrix[2][0];
    const T Me21 = this->Matrix[2][1];
    const T Me22 = this->Matrix[2][2];

    const T In00 = InMatrix.Matrix[0][0];
    const T In01 = InMatrix.Matrix[0][1];
    const T In02 = InMatrix.Matrix[0][2];
    const T In10 = InMatrix.Matrix[1][0];
    const T In11 = InMatrix.Matrix[1][1];
    const T In12 = InMatrix.Matrix[1][2];
    const T In20 = InMatrix.Matrix[2][0];
    const T In21 = InMatrix.Matrix[2][1];
    const T In22 = InMatrix.Matrix[2][2];

    Self Result = SkipInit;
    Result.Matrix[0][0] = Me00 * In00 + Me10 * In01 + Me20 * In02;
    Result.Matrix[0][1] = Me01 * In00 + Me11 * In01 + Me21 * In02;
    Result.Matrix[0][2] = Me02 * In00 + Me12 * In01 + Me22 * In02;
    Result.Matrix[1][0] = Me00 * In10 + Me10 * In11 + Me20 * In12;
    Result.Matrix[1][1] = Me01 * In10 + Me11 * In11 + Me21 * In12;
    Result.Matrix[1][2] = Me02 * In10 + Me12 * In11 + Me22 * In12;
    Result.Matrix[2][0] = Me00 * In20 + Me10 * In21 + Me20 * In22;
    Result.Matrix[2][1] = Me01 * In20 + Me11 * In21 + Me21 * In22;
    Result.Matrix[2][2] = Me02 * In20 + Me12 * In21 + Me22 * In22;
    return Result;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self TMatrix3<T>::operator/(const Self& InMatrix) const noexcept
{
    const T Me00 = this->Matrix[0][0];
    const T Me01 = this->Matrix[0][1];
    const T Me02 = this->Matrix[0][2];
    const T Me10 = this->Matrix[1][0];
    const T Me11 = this->Matrix[1][1];
    const T Me12 = this->Matrix[1][2];
    const T Me20 = this->Matrix[2][0];
    const T Me21 = this->Matrix[2][1];
    const T Me22 = this->Matrix[2][2];

    const T In00 = InMatrix.Matrix[0][0];
    const T In01 = InMatrix.Matrix[0][1];
    const T In02 = InMatrix.Matrix[0][2];
    const T In10 = InMatrix.Matrix[1][0];
    const T In11 = InMatrix.Matrix[1][1];
    const T In12 = InMatrix.Matrix[1][2];
    const T In20 = InMatrix.Matrix[2][0];
    const T In21 = InMatrix.Matrix[2][1];
    const T In22 = InMatrix.Matrix[2][2];

    Self Result = SkipInit;
    Result.Matrix[0][0] = Me00 / In00 + Me10 / In01 + Me20 / In02;
    Result.Matrix[0][1] = Me01 / In00 + Me11 / In01 + Me21 / In02;
    Result.Matrix[0][2] = Me02 / In00 + Me12 / In01 + Me22 / In02;
    Result.Matrix[1][0] = Me00 / In10 + Me10 / In11 + Me20 / In12;
    Result.Matrix[1][1] = Me01 / In10 + Me11 / In11 + Me21 / In12;
    Result.Matrix[1][2] = Me02 / In10 + Me12 / In11 + Me22 / In12;
    Result.Matrix[2][0] = Me00 / In20 + Me10 / In21 + Me20 / In22;
    Result.Matrix[2][1] = Me01 / In20 + Me11 / In21 + Me21 / In22;
    Result.Matrix[2][2] = Me02 / In20 + Me12 / In21 + Me22 / In22;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix3<T>::Self TMatrix3<T>::operator+(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] += Scalar;
    Result.Matrix[1][1] += Scalar;
    Result.Matrix[2][2] += Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix3<T>::Self TMatrix3<T>::operator-(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] -= Scalar;
    Result.Matrix[1][1] -= Scalar;
    Result.Matrix[2][2] -= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix3<T>::Self TMatrix3<T>::operator*(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] *= Scalar;
    Result.Matrix[1][1] *= Scalar;
    Result.Matrix[2][2] *= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix3<T>::Self TMatrix3<T>::operator/(const T Scalar) const noexcept
{
    Self Result = SkipInit;
    Self::FastCopy(*this, &Result);

    Result.Matrix[0][0] /= Scalar;
    Result.Matrix[1][1] /= Scalar;
    Result.Matrix[2][2] /= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE typename TMatrix3<T>::Self TMatrix3<T>::operator-() const noexcept
{
    Self Result = SkipInit;
    Result.Matrix[0][0] = -this->Matrix[0][0];
    Result.Matrix[0][1] = -this->Matrix[0][1];
    Result.Matrix[0][2] = -this->Matrix[0][2];
    Result.Matrix[1][0] = -this->Matrix[1][0];
    Result.Matrix[1][1] = -this->Matrix[1][1];
    Result.Matrix[1][2] = -this->Matrix[1][2];
    Result.Matrix[2][0] = -this->Matrix[2][0];
    Result.Matrix[2][1] = -this->Matrix[2][1];
    Result.Matrix[2][2] = -this->Matrix[2][2];

    return Result;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self& TMatrix3<T>::operator+=(const Self& InMatrix) noexcept
{
    for (i32 Col = 0; Col < 3; ++Col)
    {
        this->Matrix[Col][0] += InMatrix.Matrix[Col][0];
        this->Matrix[Col][1] += InMatrix.Matrix[Col][1];
        this->Matrix[Col][2] += InMatrix.Matrix[Col][2];

        continue;
    }

    return *this;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self& TMatrix3<T>::operator-=(const Self& InMatrix) noexcept
{
    for (i32 Col = 0; Col < 3; ++Col)
    {
        this->Matrix[Col][0] -= InMatrix.Matrix[Col][0];
        this->Matrix[Col][1] -= InMatrix.Matrix[Col][1];
        this->Matrix[Col][2] -= InMatrix.Matrix[Col][2];

        continue;
    }

    return *this;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self& TMatrix3<T>::operator*=(const Self& InMatrix) noexcept
{
    *this = *this * InMatrix;
    return *this;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self& TMatrix3<T>::operator/=(const Self& InMatrix) noexcept
{
    *this = *this / InMatrix;
    return *this;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self& TMatrix3<T>::operator+=(const T Scalar) noexcept
{
    this->Matrix[0][0] += Scalar;
    this->Matrix[1][1] += Scalar;
    this->Matrix[2][2] += Scalar;

    return *this;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self& TMatrix3<T>::operator-=(const T Scalar) noexcept
{
    this->Matrix[0][0] -= Scalar;
    this->Matrix[1][1] -= Scalar;
    this->Matrix[2][2] -= Scalar;

    return *this;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self& TMatrix3<T>::operator*=(const T Scalar) noexcept
{
    this->Matrix[0][0] *= Scalar;
    this->Matrix[1][1] *= Scalar;
    this->Matrix[2][2] *= Scalar;

    return *this;
}

template <typename T>
FORCEINLINE constexpr typename TMatrix3<T>::Self& TMatrix3<T>::operator/=(const T Scalar) noexcept
{
    this->Matrix[0][0] /= Scalar;
    this->Matrix[1][1] /= Scalar;
    this->Matrix[2][2] /= Scalar;

    return *this;
}

template <typename T>
FORCEINLINE constexpr bool TMatrix3<T>::Equals(const TMatrix3& InMatrix, const T InTolerance) const noexcept
{
    for (i32 Col = 0; Col < 3; ++Col)
    {
        for (i32 Row = 0; Row < 3; ++Row)
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

template <typename T>
FORCEINLINE bool TMatrix3<T>::operator==(const TMatrix3& InMatrix) const noexcept
{
    static_assert(sizeof(Self) == 9 * sizeof(T), "TMatrix3<T> is not 9 * sizeof(T) bytes large.");
    return ::memcmp(this->GetData(), InMatrix.GetData(), sizeof(TMatrix3)) == FALSE;
}

template <typename T>
FORCEINLINE bool TMatrix3<T>::operator!=(const TMatrix3& InMatrix) const noexcept
{
    return !(*this == InMatrix);
}

template <typename T>
FORCEINLINE constexpr T TMatrix3<T>::GetDeterminant() const noexcept
{
    return this->Matrix[0][0] * (this->Matrix[1][1] * this->Matrix[2][2] - this->Matrix[2][1] * this->Matrix[1][2])
         - this->Matrix[1][0] * (this->Matrix[0][1] * this->Matrix[2][2] - this->Matrix[2][1] * this->Matrix[0][2])
         + this->Matrix[2][0] * (this->Matrix[0][1] * this->Matrix[1][2] - this->Matrix[1][1] * this->Matrix[0][2]);
}

template <typename T>
FORCEINLINE constexpr T TMatrix3<T>::GetOneOverDeterminant() const noexcept
{
    return static_cast<T>(1.0) / this->GetDeterminant();
}

template <typename T>
FORCEINLINE constexpr T TMatrix3<T>::GetOneOverDeterminantChecked() const noexcept
{
    const T Determinant = this->GetDeterminant();
#if LAL_DO_CHECKS
    if (Maths::IsNearlyZero(Determinant))
    {
        panic( "Matrix determinant is zero." )
        return static_cast<T>(0.0);
    }
    return static_cast<T>(1.0) / Determinant;
#else /* LAL_DO_CHECKS */
    return Maths::IsNearlyZero(Determinant) ? static_cast<T>(0.0) : static_cast<T>(1.0) / Determinant;
#endif /* !LAL_DO_CHECKS */
}

template <typename T>
FORCEINLINE constexpr T TMatrix3<T>::GetOneOverDeterminantSafe() const noexcept
{
    const T Determinant = this->GetDeterminant();
    return Maths::IsNearlyZero(Determinant) ? static_cast<T>(0.0) : static_cast<T>(1.0) / Determinant;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T> TMatrix3<T>::GetInverse() const noexcept
{
    TMatrix3 Result = SkipInit;
    const T InvDet = this->GetOneOverDeterminantChecked();

    Result[0][0] = + (this->Matrix[1][1] * this->Matrix[2][2] - this->Matrix[2][1] * this->Matrix[1][2]) * InvDet;
    Result[1][0] = - (this->Matrix[1][0] * this->Matrix[2][2] - this->Matrix[2][0] * this->Matrix[1][2]) * InvDet;
    Result[2][0] = + (this->Matrix[1][0] * this->Matrix[2][1] - this->Matrix[2][0] * this->Matrix[1][1]) * InvDet;
    Result[0][1] = - (this->Matrix[0][1] * this->Matrix[2][2] - this->Matrix[2][1] * this->Matrix[0][2]) * InvDet;
    Result[1][1] = + (this->Matrix[0][0] * this->Matrix[2][2] - this->Matrix[2][0] * this->Matrix[0][2]) * InvDet;
    Result[2][1] = - (this->Matrix[0][0] * this->Matrix[2][1] - this->Matrix[2][0] * this->Matrix[0][1]) * InvDet;
    Result[0][2] = + (this->Matrix[0][1] * this->Matrix[1][2] - this->Matrix[1][1] * this->Matrix[0][2]) * InvDet;
    Result[1][2] = - (this->Matrix[0][0] * this->Matrix[1][2] - this->Matrix[1][0] * this->Matrix[0][2]) * InvDet;
    Result[2][2] = + (this->Matrix[0][0] * this->Matrix[1][1] - this->Matrix[1][0] * this->Matrix[0][1]) * InvDet;

    return Result;
}

template <typename T>
LString TMatrix3<T>::ToString() const
{
    LString Result;
    for (i32 Col { 0 }; Col < 3; ++Col)
    {
        Result.append(Lal::SprintF("{:.2f} {:.2f} {:.2f}\n",
            this->Matrix[Col][0], this->Matrix[Col][1], this->Matrix[Col][2]));
    }

    return Result;
}

template<typename T>
FORCEINLINE void TMatrix3<T>::FastMultiply(const Self& A, const Self& B, Self* OutMatrix)
{
    checkSlow( OutMatrix )

    OutMatrix[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0];
    OutMatrix[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1];
    OutMatrix[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2];

    OutMatrix[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0];
    OutMatrix[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1];
    OutMatrix[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2];

    OutMatrix[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0];
    OutMatrix[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1];
    OutMatrix[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2];

    return;
}

template <typename T>
FORCEINLINE void TMatrix3<T>::FastCopy(const Self& InMatrix, Self* OutMatrix) noexcept
{
    static_assert(sizeof(Self) == 9 * sizeof(T), "TMatrix3<T> is not 9 * sizeof(T) bytes large.");

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
#endif /* WITH_GCC */
    ::memcpy(OutMatrix, &InMatrix, sizeof(Self));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    return;
}

template<typename T>
FORCEINLINE void TMatrix3<T>::FastMove(Self* InMatrix, Self* OutMatrix) noexcept
{
    static_assert(sizeof(Self) == 16 * sizeof(T), "TMatrix<T> is not 16 * sizeof(T) bytes large.");

    Self::FastCopy(*InMatrix, OutMatrix);
    InMatrix->SetIdentity();

    return;
}
