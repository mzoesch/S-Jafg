// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg::Maths
{


/*----------------------------------------------------------------------------
    Declarations.
----------------------------------------------------------------------------*/

template <typename T>
NODISCARD constexpr FORCEINLINE auto Absolute(const T A) -> T { return ( A < static_cast<T>(0) ) ? -A : A; }
template <typename T>
NODISCARD constexpr FORCEINLINE auto Min(const T A, const T B) -> T { return (B < A) ? B : A; }
template <typename T>
NODISCARD constexpr FORCEINLINE auto Max(const T A, const T B) -> T { return (B < A) ? A : B; }

NODISCARD FORCEINLINE bool IsNearlyEqual(const float A, const float B, const float Tolerance = JAFG_FLOAT_SMALL_NUMBER);
NODISCARD FORCEINLINE bool IsNearlyEqual(const double A, const double B, const double Tolerance = JAFG_DOUBLE_SMALL_NUMBER);


/*----------------------------------------------------------------------------
    Specializations.
----------------------------------------------------------------------------*/

template <>
FORCEINLINE float Absolute(const float A)
{
    return fabsf(A);
}

template <>
FORCEINLINE double Absolute(const double A)
{
    return fabs(A);
}

template <>
FORCEINLINE float Min(const float A, const float B)
{
    return (B < A) ? B : A;
}

template <>
FORCEINLINE double Min(const double A, const double B)
{
    return (B < A) ? B : A;
}

template <>
FORCEINLINE float Max(const float A, const float B)
{
    return (B < A) ? A : B;
}

template <>
FORCEINLINE double Max(const double A, const double B)
{
    return (B < A) ? A : B;
}

/*----------------------------------------------------------------------------
    Definitions.
----------------------------------------------------------------------------*/

FORCEINLINE bool IsNearlyEqual(const float A, const float B, const float Tolerance)
{
    return Absolute(A - B) < Tolerance;
}

FORCEINLINE bool IsNearlyEqual(const double A, const double B, const double Tolerance)
{
    return Absolute(A - B) < Tolerance;
}

} /* Namespace Jafg::Maths */
