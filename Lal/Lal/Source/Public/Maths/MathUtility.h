// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg::Maths
{


/*----------------------------------------------------------------------------
    Macros.
----------------------------------------------------------------------------*/

#define MIX_FLOATING_POINT_ARGS_TWO_PARAMS(Func)                                   \
    template                                                                       \
    <                                                                              \
        typename Arg1,                                                             \
        typename Arg2                                                              \
    >                                                                              \
    FORCEINLINE static auto Func(Arg1 X, Arg2 Y) -> decltype(X * Y)                \
    {                                                                              \
        static_assert(                                                             \
               (std::is_floating_point_v<Arg1> || std::is_floating_point_v<Arg2>)  \
            && (std::is_same_v<Arg1, Arg2>     == false                         ), \
            "Arguments must be floating point types and not the same."             \
        );                                                                         \
        using ArgType = decltype(X * Y);                                           \
        return Func(static_cast<ArgType>(X), static_cast<ArgType>(Y));             \
    }

#define MIX_FLOATING_POINT_ARGS_THREE_PARAMS(Func)                              \
    template                                                                    \
    <                                                                           \
        typename Arg1,                                                          \
        typename Arg2,                                                          \
        typename Arg3                                                           \
    >                                                                           \
    FORCEINLINE static auto Func(Arg1 X, Arg2 Y, Arg3 Z) -> decltype(X * Y * Z) \
    {                                                                           \
        static_assert(                                                          \
               (                                                                \
                  std::is_floating_point_v<Arg1>                                \
               || std::is_floating_point_v<Arg2>                                \
               || std::is_floating_point_v<Arg3>                                \
               )                                                                \
            &&                                                                  \
               (                                                                \
                  std::is_same_v<Arg1, Arg2> == false                           \
               || std::is_same_v<Arg1, Arg3> == false                           \
               || std::is_same_v<Arg2, Arg3> == false                           \
               ),                                                               \
            "Arguments must be floating point types and not the same."          \
        );                                                                      \
        using ArgType = decltype(X * Y * Z);                                    \
        return Func(                                                            \
            static_cast<ArgType>(X),                                            \
            static_cast<ArgType>(Y),                                            \
            static_cast<ArgType>(Z)                                             \
        );                                                                      \
    }


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

/** https://en.cppreference.com/w/c/numeric/math/fmod */
NODISCARD FORCEINLINE float  Fmod(const float Numerator, const float Denominator);
NODISCARD FORCEINLINE double Fmod(const double Numerator, const double Denominator);
MIX_FLOATING_POINT_ARGS_TWO_PARAMS(Fmod);


/*----------------------------------------------------------------------------
    Specializations.
----------------------------------------------------------------------------*/

template <>
FORCEINLINE float Absolute(const float A)
{
    return ::fabsf(A);
}

template <>
FORCEINLINE double Absolute(const double A)
{
    return ::fabs(A);
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

FORCEINLINE float Fmod(const float Numerator, const float Denominator)
{
#if DO_CHECKS
    const float AbsDenominator = Maths::Absolute(Denominator);
    if (AbsDenominator <= JAFG_FLOAT_SMALL_NUMBER)
    {
        checkNoEntry()
        return 0.0f;
    }
#endif /* DO_CHECKS */

    return ::fmodf(Numerator, Denominator);
}

FORCEINLINE double Fmod(const double Numerator, const double Denominator)
{
#if DO_CHECKS
    const double AbsDenominator = Maths::Absolute(Denominator);
    if (AbsDenominator <= JAFG_DOUBLE_SMALL_NUMBER)
    {
        checkNoEntry()
        return 0.0;
    }
#endif /* DO_CHECKS */

    return ::fmod(Numerator, Denominator);
}

} /* Namespace Jafg::Maths */
