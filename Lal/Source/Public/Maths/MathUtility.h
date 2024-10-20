// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg::Maths
{


/*----------------------------------------------------------------------------
    Declarations.
----------------------------------------------------------------------------*/

template <typename T>
NODISCARD constexpr FORCEINLINE auto Absolute(const T A) -> T { return ( A < static_cast<T>(0) ) ? -A : A; }

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
