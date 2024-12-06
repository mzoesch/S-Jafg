// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/** Jafg implementation of a four dimensional matrix with TMatrix[R][C] notation. */
template <typename T>
struct TMatrix final
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TMatrix must be a floating point type.");

    union
    {
        T Matrix[4][4];
        T Bits[16];
    };

    /** Global identity matrix constant. */
    static const TMatrix<T> Identity;

    FORCEINLINE TMatrix() noexcept : TMatrix(TMatrix<T>::Identity) { }
    FORCEINLINE TMatrix(const Matrix::Type InType);
    FORCEINLINE TMatrix(const TPlane<T>& InX, const TPlane<T>& InY, const TPlane<T>& InZ, const TPlane<T>& InW)
        : Matrix
        {
            { InX.X, InX.Y, InX.Z, InX.W },
            { InY.X, InY.Y, InY.Z, InY.W },
            { InZ.X, InZ.Y, InZ.Z, InZ.W },
            { InW.X, InW.Y, InW.Z, InW.W }
        }
    {
    }
    FORCEINLINE TMatrix(const TVector<T>& InX, const TVector<T>& InY, const TVector<T>& InZ, const TVector<T>& InW)
        : Matrix
        {
            { InX.X, InX.Y, InX.Z, 0.0f },
            { InY.X, InY.Y, InY.Z, 0.0f },
            { InZ.X, InZ.Y, InZ.Z, 0.0f },
            { InW.X, InW.Y, InW.Z, 1.0f }
        }
    {
    }
    FORCEINLINE TMatrix(const TMatrix<T>& InMatrix) noexcept
    {
        FastCopy(InMatrix, this);
    }
    FORCEINLINE TMatrix(TMatrix<T>&& InMatrix) noexcept
    {
        /* Copy not moving. I know. But moving we cannot do :). */
        FastCopy(InMatrix, this);
    }

    FORCEINLINE auto GetData()       ->       T* { return &this->Matrix[0][0]; }
    FORCEINLINE auto GetData() const -> const T* { return &this->Matrix[0][0]; }

    FORCEINLINE auto operator[](const int32 InIndex)       -> TVector4<T>&;
    FORCEINLINE auto operator[](const int32 InIndex) const -> const TVector4<T>&;

    /*
     * This is quite a large data structure. Do not copy it around as you like.
     * We make copying more explicit (more knowingly) and annoying with constructors.
     */
    FORCEINLINE TMatrix<T>& operator =(const TMatrix<T>&  InMatrix) noexcept = delete;
    FORCEINLINE TMatrix<T>& operator =(      TMatrix<T>&& InMatrix) noexcept = delete;
    FORCEINLINE TMatrix<T>& operator =(const TMatrix<T>&& InMatrix) noexcept = delete;

    FORCEINLINE void CopyFrom(const TMatrix<T>& InMatrix) { TMatrix::FastCopy(InMatrix, this); }

    FORCEINLINE void SetIdentity();

    FORCEINLINE TMatrix<T>  operator *(const TMatrix<T>& InMatrix) const;
    FORCEINLINE TMatrix<T>& operator*=(const TMatrix<T>& InMatrix);
    FORCEINLINE TMatrix<T>  operator +(const TMatrix<T>& InMatrix) const;
    FORCEINLINE TMatrix<T>& operator+=(const TMatrix<T>& InMatrix);

    FORCEINLINE bool Equals(const TMatrix<T>& InMatrix, const T InTolerance = JAFG_SMALL_NUMBER) const;
    FORCEINLINE bool operator==(const TMatrix<T>& InMatrix) const;
    FORCEINLINE bool operator!=(const TMatrix<T>& InMatrix) const;

    /** Translate this matrix inline by the given translation vector. */
    FORCEINLINE void InlineTranslate(const TVector<T>& InTranslation);

    LSimpleString ToString() const;

private:

    /**
     * @param A         Left-hand side matrix.
     * @param B         Right-hand side matrix.
     * @param OutMatrix Resulting matrix must fulfill OutMatrix != InMatrixA && OutMatrix != InMatrixB.
     */
    FORCEINLINE static void MultiplyMatrix(const TMatrix<T>& A, const TMatrix<T>& B, TMatrix<T>* OutMatrix);
    FORCEINLINE static void FastCopy(const TMatrix<T>& InMatrix, TMatrix<T>* OutMatrix);
};

template <typename T>
TMatrix<T>::TMatrix(const Matrix::Type InType)
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

    return;
}

template <typename T>
TVector4<T>& TMatrix<T>::operator[](const int32 InIndex)
{
    check( InIndex > INDEX_NONE && InIndex < 4 )
    return &this->Matrix[InIndex][0];
}

template <typename T>
const TVector4<T>& TMatrix<T>::operator[](const int32 InIndex) const
{
    check( InIndex > INDEX_NONE && InIndex < 4 )
    return &this->Matrix[InIndex][0];
}

template <typename T>
void TMatrix<T>::SetIdentity()
{
    /*
     * Do we want to memcpy the identity matrix? What is faster??
     */

    this->Matrix[0][0] = 1.0f;
    this->Matrix[0][1] = 0.0f;
    this->Matrix[0][2] = 0.0f;
    this->Matrix[0][3] = 0.0f;

    this->Matrix[1][0] = 0.0f;
    this->Matrix[1][1] = 1.0f;
    this->Matrix[1][2] = 0.0f;
    this->Matrix[1][3] = 0.0f;

    this->Matrix[2][0] = 0.0f;
    this->Matrix[2][1] = 0.0f;
    this->Matrix[2][2] = 1.0f;
    this->Matrix[2][3] = 0.0f;

    this->Matrix[3][0] = 0.0f;
    this->Matrix[3][1] = 0.0f;
    this->Matrix[3][2] = 0.0f;
    this->Matrix[3][3] = 1.0f;

    return;
}

template <typename T>
TMatrix<T> TMatrix<T>::operator*(const TMatrix<T>& InMatrix) const
{
    TMatrix<T> Result;
    MultiplyMatrix(*this, InMatrix, &Result);
    return Result;
}

template <typename T>
TMatrix<T>& TMatrix<T>::operator*=(const TMatrix<T>& InMatrix)
{
    /*
     * Mathematically, it is impossible to in-place multiply a matrix.
     * Therefore, we sadly have to copy the result to the left-hand side.
     */
    TMatrix<T> Result;
    MultiplyMatrix(*this, InMatrix, &Result);
    FastCopy(Result, this);
    return *this;
}

template <typename T>
TMatrix<T> TMatrix<T>::operator+(const TMatrix<T>& InMatrix) const
{
    TMatrix<T> Result;

    for (int32 Row = 0; Row < 4; ++Row)
    {
        Result.Matrix[Row][0] = this->Matrix[Row][0] + InMatrix.Matrix[Row][0];
        Result.Matrix[Row][1] = this->Matrix[Row][1] + InMatrix.Matrix[Row][1];
        Result.Matrix[Row][2] = this->Matrix[Row][2] + InMatrix.Matrix[Row][2];
        Result.Matrix[Row][3] = this->Matrix[Row][3] + InMatrix.Matrix[Row][3];

        continue;
    }

    return Result;
}

template <typename T>
TMatrix<T>& TMatrix<T>::operator+=(const TMatrix<T>& InMatrix)
{
    for (int32 Row = 0; Row < 4; ++Row)
    {
        this->Matrix[Row][0] += InMatrix.Matrix[Row][0];
        this->Matrix[Row][1] += InMatrix.Matrix[Row][1];
        this->Matrix[Row][2] += InMatrix.Matrix[Row][2];
        this->Matrix[Row][3] += InMatrix.Matrix[Row][3];

        continue;
    }

    return *this;
}

template <typename T>
bool TMatrix<T>::Equals(const TMatrix<T>& InMatrix, const T InTolerance) const
{
    for (int32 Row = 0; Row < 4; ++Row)
    {
        for (int32 Col = 0; Col < 4; ++Col)
        {
            if (Maths::Absolute(this->Matrix[Row][Col] - InMatrix.Matrix[Row][Col]) > InTolerance)
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
bool TMatrix<T>::operator==(const TMatrix<T>& InMatrix) const
{
    for (int32 Row = 0; Row < 4; ++Row)
    {
        for (int32 Col = 0; Col < 4; ++Col)
        {
            if (this->Matrix[Row][Col] != InMatrix.Matrix[Row][Col])
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
bool TMatrix<T>::operator!=(const TMatrix<T>& InMatrix) const
{
    return !(*this == InMatrix);
}

template <typename T>
void TMatrix<T>::InlineTranslate(const TVector<T>& InTranslation)
{
    this->Matrix[3][0] += InTranslation.X;
    this->Matrix[3][1] += InTranslation.Y;
    this->Matrix[3][2] += InTranslation.Z;
    return;
}

template <typename T>
LSimpleString TMatrix<T>::ToString() const
{
    LSimpleString Result;

    for (int32 Row = 0; Row < 4; ++Row)
    {
        Result.Append(LSimpleString::SprintF("{:.2f} {:.2f} {:.2f} {:.2f}\n",
            this->Matrix[Row][0], this->Matrix[Row][1], this->Matrix[Row][2], this->Matrix[Row][3]));
    }

    return Result;
}

template <typename T>
void TMatrix<T>::MultiplyMatrix(const TMatrix<T>& A, const TMatrix<T>& B, TMatrix<T>* OutMatrix)
{
    OutMatrix[0][0] = A[0][0] * B[0][0] + A[0][1] * B[1][0] + A[0][2] * B[2][0] + A[0][3] * B[3][0];
    OutMatrix[0][1] = A[0][0] * B[0][1] + A[0][1] * B[1][1] + A[0][2] * B[2][1] + A[0][3] * B[3][1];
    OutMatrix[0][2] = A[0][0] * B[0][2] + A[0][1] * B[1][2] + A[0][2] * B[2][2] + A[0][3] * B[3][2];
    OutMatrix[0][3] = A[0][0] * B[0][3] + A[0][1] * B[1][3] + A[0][2] * B[2][3] + A[0][3] * B[3][3];

    OutMatrix[1][0] = A[1][0] * B[0][0] + A[1][1] * B[1][0] + A[1][2] * B[2][0] + A[1][3] * B[3][0];
    OutMatrix[1][1] = A[1][0] * B[0][1] + A[1][1] * B[1][1] + A[1][2] * B[2][1] + A[1][3] * B[3][1];
    OutMatrix[1][2] = A[1][0] * B[0][2] + A[1][1] * B[1][2] + A[1][2] * B[2][2] + A[1][3] * B[3][2];
    OutMatrix[1][3] = A[1][0] * B[0][3] + A[1][1] * B[1][3] + A[1][2] * B[2][3] + A[1][3] * B[3][3];

    OutMatrix[2][0] = A[2][0] * B[0][0] + A[2][1] * B[1][0] + A[2][2] * B[2][0] + A[2][3] * B[3][0];
    OutMatrix[2][1] = A[2][0] * B[0][1] + A[2][1] * B[1][1] + A[2][2] * B[2][1] + A[2][3] * B[3][1];
    OutMatrix[2][2] = A[2][0] * B[0][2] + A[2][1] * B[1][2] + A[2][2] * B[2][2] + A[2][3] * B[3][2];
    OutMatrix[2][3] = A[2][0] * B[0][3] + A[2][1] * B[1][3] + A[2][2] * B[2][3] + A[2][3] * B[3][3];

    OutMatrix[3][0] = A[3][0] * B[0][0] + A[3][1] * B[1][0] + A[3][2] * B[2][0] + A[3][3] * B[3][0];
    OutMatrix[3][1] = A[3][0] * B[0][1] + A[3][1] * B[1][1] + A[3][2] * B[2][1] + A[3][3] * B[3][1];
    OutMatrix[3][2] = A[3][0] * B[0][2] + A[3][1] * B[1][2] + A[3][2] * B[2][2] + A[3][3] * B[3][2];
    OutMatrix[3][3] = A[3][0] * B[0][3] + A[3][1] * B[1][3] + A[3][2] * B[2][3] + A[3][3] * B[3][3];

    return;
}

template <typename T>
void TMatrix<T>::FastCopy(const TMatrix<T>& InMatrix, TMatrix<T>* OutMatrix)
{
    static_assert(sizeof(TMatrix<T>) == 16 * sizeof(T), "TMatrix<T> is not 16 * sizeof(T) bytes large.");
    ::memcpy(OutMatrix, &InMatrix, sizeof(TMatrix<T>));  // NOLINT(bugprone-undefined-memory-manipulation)
    return;
}

} /* ~Namespace Jafg */
