// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/** Jafg implementation of a three-dimensional matrix with TMatrix3[C][R] notation. */
template <typename T>
struct TMatrix3 final
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TMatrix must be a floating point type.");

    enum EMatrixAxis : uint8
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

    typedef TVector<T> ColumnTy;
    typedef TVector<T> RowTy;

    /** Global identity matrix constant. */
    LAL_API static const TMatrix3<T> Identity;

    FORCEINLINE constexpr TMatrix3() noexcept : TMatrix3(TMatrix3<T>::Identity) { }
    FORCEINLINE constexpr TMatrix3(EForceInit) noexcept : TMatrix3(Matrix::Identity) { }
    FORCEINLINE constexpr TMatrix3(EZeroInit) noexcept : TMatrix3(Matrix::Zero) { }
    FORCEINLINE constexpr TMatrix3(ESkipInit) noexcept { }
    FORCEINLINE           TMatrix3(const Matrix::Type InType) noexcept;
    FORCEINLINE constexpr TMatrix3(const TVector<T>& InX, const TVector<T>& InY, const TVector<T>& InZ) noexcept
        : Matrix
        {
            { InX.X, InX.Y, InX.Z },
            { InY.X, InY.Y, InY.Z },
            { InZ.X, InZ.Y, InZ.Z }
        }
    {
    }
    FORCEINLINE constexpr TMatrix3(const TMatrix3& InMatrix) noexcept { TMatrix3::FastCopy(InMatrix, this); return; }
    FORCEINLINE constexpr TMatrix3(TMatrix3&& InMatrix) noexcept { TMatrix3::FastCopy(InMatrix, this); return; }

    FORCEINLINE constexpr       T* GetData()       noexcept { return static_cast<T*>(this); }
    FORCEINLINE constexpr const T* GetData() const noexcept { return static_cast<T*>(this); }

    FORCEINLINE constexpr TVector<T> GetRow(const EMatrixAxis Axis) const;
    FORCEINLINE constexpr TVector<T> GetColumn(const EMatrixAxis Axis) const;

    FORCEINLINE void SetIdentity() noexcept;

    FORCEINLINE           TMatrix3& operator =(const TMatrix3&  InMatrix) noexcept;
    FORCEINLINE           TMatrix3& operator =(      TMatrix3&& InMatrix) noexcept;

    FORCEINLINE constexpr TMatrix3  operator +(const TMatrix3& InMatrix) const noexcept;
    FORCEINLINE constexpr TMatrix3  operator -(const TMatrix3& InMatrix) const noexcept;
    FORCEINLINE constexpr TMatrix3  operator *(const TMatrix3& InMatrix) const noexcept;
    FORCEINLINE constexpr TMatrix3  operator /(const TMatrix3& InMatrix) const noexcept;
    FORCEINLINE           TMatrix3  operator +(const T Scalar) const noexcept;
    FORCEINLINE           TMatrix3  operator -(const T Scalar) const noexcept;
    FORCEINLINE           TMatrix3  operator *(const T Scalar) const noexcept;
    FORCEINLINE           TMatrix3  operator /(const T Scalar) const noexcept;
    FORCEINLINE           TMatrix3  operator -() const noexcept;
    FORCEINLINE constexpr TMatrix3& operator+=(const TMatrix3& InMatrix) noexcept;
    FORCEINLINE constexpr TMatrix3& operator-=(const TMatrix3& InMatrix) noexcept;
    FORCEINLINE constexpr TMatrix3& operator*=(const TMatrix3& InMatrix) noexcept;
    FORCEINLINE constexpr TMatrix3& operator/=(const TMatrix3& InMatrix) noexcept;
    FORCEINLINE constexpr TMatrix3& operator+=(const T Scalar) noexcept;
    FORCEINLINE constexpr TMatrix3& operator-=(const T Scalar) noexcept;
    FORCEINLINE constexpr TMatrix3& operator*=(const T Scalar) noexcept;
    FORCEINLINE constexpr TMatrix3& operator/=(const T Scalar) noexcept;

    FORCEINLINE constexpr bool Equals(const TMatrix3& InMatrix, const T InTolerance = JAFG_SMALL_NUMBER) const noexcept;
    FORCEINLINE constexpr bool operator==(const TMatrix3& InMatrix) const noexcept;
    FORCEINLINE constexpr bool operator!=(const TMatrix3& InMatrix) const noexcept;

    FORCEINLINE constexpr  T GetDeterminant() const noexcept;
    FORCEINLINE constexpr  T GetOneOverDeterminant() const noexcept;
    FORCEINLINE constexpr  T GetOneOverDeterminantChecked() const noexcept;
    FORCEINLINE constexpr  T GetOneOverDeterminantSafe() const noexcept;
    FORCEINLINE constexpr  TMatrix3 GetInverse() const noexcept;

    LString ToString() const;

private:

    FORCEINLINE static void FastCopy(const TMatrix3<T>& InMatrix, TMatrix3<T>* OutMatrix) noexcept;
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
FORCEINLINE constexpr TVector<T> TMatrix3<T>::GetRow(const EMatrixAxis Axis) const
{
    return { this->Matrix[Axis][0], this->Matrix[Axis][1], this->Matrix[Axis][2] };
}

template <typename T>
FORCEINLINE constexpr TVector<T> TMatrix3<T>::GetColumn(const EMatrixAxis Axis) const
{
    return { *reinterpret_cast<TVector<T>*>(&this->Matrix[Axis]) };
}

template <typename T>
FORCEINLINE void TMatrix3<T>::SetIdentity() noexcept
{
    ::memcpy(this->GetData(), &TMatrix3<T>::Identity, sizeof(TMatrix3));
}

template <typename T>
FORCEINLINE TMatrix3<T>& TMatrix3<T>::operator=(const TMatrix3& InMatrix) noexcept
{
    TMatrix3::FastCopy(InMatrix, this);
    return *this;
}

template <typename T>
FORCEINLINE TMatrix3<T>& TMatrix3<T>::operator=(TMatrix3&& InMatrix) noexcept
{
    TMatrix3::FastCopy(InMatrix, this);
    return *this;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T> TMatrix3<T>::operator+(const TMatrix3& InMatrix) const noexcept
{
    TMatrix3 Result = SkipInit;
    for (int32 Col = 0; Col < 3; ++Col)
    {
        for (int32 Row = 0; Row < 3; ++Row)
        {
            Result[Col][Row] = this->Matrix[Col][Row] + InMatrix.Matrix[Col][Row];
        }
    }

    return Result;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T> TMatrix3<T>::operator-(const TMatrix3& InMatrix) const noexcept
{
    TMatrix3 Result = SkipInit;
    for (int32 Col = 0; Col < 3; ++Col)
    {
        for (int32 Row = 0; Row < 3; ++Row)
        {
            Result[Col][Row] = this->Matrix[Col][Row] - InMatrix.Matrix[Col][Row];
        }
    }

    return Result;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T> TMatrix3<T>::operator*(const TMatrix3& InMatrix) const noexcept
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

    TMatrix3 Result = SkipInit;
    Result[0][0] = Me00 * In00 + Me10 * In01 + Me20 * In02;
    Result[0][1] = Me01 * In00 + Me11 * In01 + Me21 * In02;
    Result[0][2] = Me02 * In00 + Me12 * In01 + Me22 * In02;
    Result[1][0] = Me00 * In10 + Me10 * In11 + Me20 * In12;
    Result[1][1] = Me01 * In10 + Me11 * In11 + Me21 * In12;
    Result[1][2] = Me02 * In10 + Me12 * In11 + Me22 * In12;
    Result[2][0] = Me00 * In20 + Me10 * In21 + Me20 * In22;
    Result[2][1] = Me01 * In20 + Me11 * In21 + Me21 * In22;
    Result[2][2] = Me02 * In20 + Me12 * In21 + Me22 * In22;
    return Result;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T> TMatrix3<T>::operator/(const TMatrix3& InMatrix) const noexcept
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

    TMatrix3 Result = SkipInit;
    Result[0][0] = Me00 / In00 + Me10 / In01 + Me20 / In02;
    Result[0][1] = Me01 / In00 + Me11 / In01 + Me21 / In02;
    Result[0][2] = Me02 / In00 + Me12 / In01 + Me22 / In02;
    Result[1][0] = Me00 / In10 + Me10 / In11 + Me20 / In12;
    Result[1][1] = Me01 / In10 + Me11 / In11 + Me21 / In12;
    Result[1][2] = Me02 / In10 + Me12 / In11 + Me22 / In12;
    Result[2][0] = Me00 / In20 + Me10 / In21 + Me20 / In22;
    Result[2][1] = Me01 / In20 + Me11 / In21 + Me21 / In22;
    Result[2][2] = Me02 / In20 + Me12 / In21 + Me22 / In22;

    return Result;
}

template <typename T>
FORCEINLINE TMatrix3<T> TMatrix3<T>::operator+(const T Scalar) const noexcept
{
    TMatrix3 Result = SkipInit;
    TMatrix3::FastCopy(*this, &Result);

    Result[0][0] += Scalar;
    Result[1][1] += Scalar;
    Result[2][2] += Scalar;

    return Result;
}

template <typename T>
FORCEINLINE TMatrix3<T> TMatrix3<T>::operator-(const T Scalar) const noexcept
{
    TMatrix3 Result = SkipInit;
    TMatrix3::FastCopy(*this, &Result);

    Result[0][0] -= Scalar;
    Result[1][1] -= Scalar;
    Result[2][2] -= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE TMatrix3<T> TMatrix3<T>::operator*(const T Scalar) const noexcept
{
    TMatrix3 Result = SkipInit;
    TMatrix3::FastCopy(*this, &Result);

    Result[0][0] *= Scalar;
    Result[1][1] *= Scalar;
    Result[2][2] *= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE TMatrix3<T> TMatrix3<T>::operator/(const T Scalar) const noexcept
{
    TMatrix3 Result = SkipInit;
    TMatrix3::FastCopy(*this, &Result);

    Result[0][0] /= Scalar;
    Result[1][1] /= Scalar;
    Result[2][2] /= Scalar;

    return Result;
}

template <typename T>
FORCEINLINE TMatrix3<T> TMatrix3<T>::operator-() const noexcept
{
    TMatrix3 Result = SkipInit;
    TMatrix3::FastCopy(*this, &Result);

    Result[0][0] = -Result[0][0];
    Result[0][1] = -Result[0][1];
    Result[0][2] = -Result[0][2];
    Result[1][0] = -Result[1][0];
    Result[1][1] = -Result[1][1];
    Result[1][2] = -Result[1][2];
    Result[2][0] = -Result[2][0];
    Result[2][1] = -Result[2][1];
    Result[2][2] = -Result[2][2];

    return Result;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T>& TMatrix3<T>::operator+=(const TMatrix3& InMatrix) noexcept
{
    for (int32 Col = 0; Col < 3; ++Col)
    {
        for (int32 Row = 0; Row < 3; ++Row)
        {
            this->Matrix[Col][Row] += InMatrix.Matrix[Col][Row];
        }
    }

    return *this;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T>& TMatrix3<T>::operator-=(const TMatrix3& InMatrix) noexcept
{
    for (int32 Col = 0; Col < 3; ++Col)
    {
        for (int32 Row = 0; Row < 3; ++Row)
        {
            this->Matrix[Col][Row] -= InMatrix.Matrix[Col][Row];
        }
    }

    return *this;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T>& TMatrix3<T>::operator*=(const TMatrix3& InMatrix) noexcept
{
    *this = *this * InMatrix;
    return *this;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T>& TMatrix3<T>::operator/=(const TMatrix3& InMatrix) noexcept
{
    *this = *this / InMatrix;
    return *this;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T>& TMatrix3<T>::operator+=(const T Scalar) noexcept
{
    this->Matrix[0][0] += Scalar;
    this->Matrix[1][1] += Scalar;
    this->Matrix[2][2] += Scalar;

    return *this;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T>& TMatrix3<T>::operator-=(const T Scalar) noexcept
{
    this->Matrix[0][0] -= Scalar;
    this->Matrix[1][1] -= Scalar;
    this->Matrix[2][2] -= Scalar;

    return *this;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T>& TMatrix3<T>::operator*=(const T Scalar) noexcept
{
    this->Matrix[0][0] *= Scalar;
    this->Matrix[1][1] *= Scalar;
    this->Matrix[2][2] *= Scalar;

    return *this;
}

template <typename T>
FORCEINLINE constexpr TMatrix3<T>& TMatrix3<T>::operator/=(const T Scalar) noexcept
{
    this->Matrix[0][0] /= Scalar;
    this->Matrix[1][1] /= Scalar;
    this->Matrix[2][2] /= Scalar;

    return *this;
}

template <typename T>
FORCEINLINE constexpr bool TMatrix3<T>::Equals(const TMatrix3& InMatrix, const T InTolerance) const noexcept
{
    for (int32 Col = 0; Col < 3; ++Col)
    {
        for (int32 Row = 0; Row < 3; ++Row)
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
FORCEINLINE constexpr bool TMatrix3<T>::operator==(const TMatrix3& InMatrix) const noexcept
{
    return ::memcmp(this->GetData(), InMatrix.GetData(), sizeof(TMatrix3)) == FALSE;
}

template <typename T>
FORCEINLINE constexpr bool TMatrix3<T>::operator!=(const TMatrix3& InMatrix) const noexcept
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
#if DO_CHECKS
    if (Maths::IsNearlyZero(Determinant))
    {
        panic( "Matrix determinant is zero." )
        return static_cast<T>(0.0);
    }
    return static_cast<T>(1.0) / Determinant;
#else /* DO_CHECKS */
    return Maths::IsNearlyZero(Determinant) ? static_cast<T>(0.0) : static_cast<T>(1.0) / Determinant;
#endif /* !DO_CHECKS */
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
    for (int32 Col = 0; Col < 3; ++Col)
    {
        Result.Append(LString::SprintF("{:.2f} {:.2f} {:.2f}\n",
            this->Matrix[Col][0], this->Matrix[Col][1], this->Matrix[Col][2]));
    }

    return Result;
}

template <typename T>
FORCEINLINE void TMatrix3<T>::FastCopy(const TMatrix3<T>& InMatrix, TMatrix3<T>* OutMatrix) noexcept
{
    static_assert(sizeof(TMatrix3<T>) == 9 * sizeof(T), "TMatrix3<T> is not 9 * sizeof(T) bytes large.");
    ::memcpy(OutMatrix, &InMatrix, sizeof(TMatrix3<T>));  // NOLINT(bugprone-undefined-memory-manipulation)
    return;
}

} /* ~Namespace Jafg */
