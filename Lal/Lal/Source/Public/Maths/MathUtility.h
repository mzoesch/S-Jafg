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
        requires std::is_floating_point_v<Arg1>                                 \
              || std::is_floating_point_v<Arg2>                                 \
              || std::is_floating_point_v<Arg3>                                 \
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

template <typename T> NODISCARD FORCEINLINE constexpr T Absolute(const T A) { return ( A < static_cast<T>(0) ) ? -A : A; }
template <typename T> NODISCARD FORCEINLINE constexpr T Min(const T A, const T B) { return (B < A) ? B : A; }
template <typename T> NODISCARD FORCEINLINE constexpr T Max(const T A, const T B) { return (B < A) ? A : B; }

/** @return The sign of the value. -1 for negative, 1 for positive, 0 for zero. */
template <typename T> NODISCARD FORCEINLINE constexpr T Sign(const T Value) { return (Value < static_cast<T>(0.0)) ? -static_cast<T>(1) : (Value > static_cast<T>(0.0)) ? static_cast<T>(1.0) : static_cast<T>(0.0); }
/** @return The sign of the value. -1 for negative, 1 for positive or zero. */
template <typename T> NODISCARD FORCEINLINE constexpr T SignNoZero(const T Value) { return (Value < static_cast<T>(0.0)) ? -static_cast<T>(1.0) : static_cast<T>(1.0); }

template <typename T> NODISCARD FORCEINLINE constexpr T Invert(const T Value) { return Value * static_cast<T>(-1.0); }

template <typename T> NODISCARD FORCEINLINE constexpr T Squared(const T Value) { return Value * Value; }
template <typename T> NODISCARD FORCEINLINE constexpr T Cubed(const T Value)   { return Value * Value * Value; }
template <typename T> NODISCARD FORCEINLINE constexpr T Quartic(const T Value) { return Value * Value * Value * Value; }
template <typename T> NODISCARD FORCEINLINE constexpr T Quintic(const T Value) { return Value * Value * Value * Value * Value; }
template <typename T> NODISCARD FORCEINLINE constexpr T Sextic(const T Value)  { return Value * Value * Value * Value * Value * Value; }
/** For generic pow. But should be avoided when dealing with exponents less than seven due to performance. */
template <typename T> NODISCARD FORCEINLINE constexpr T Pow(const T Base, const T Exponent);

template <typename T> NODISCARD FORCEINLINE constexpr T Factorial(const T Value);

/** Inclusively clamp the value of type T between its minimum and maximum values. */
template <typename T>
NODISCARD FORCEINLINE constexpr T      Clamp(const T Value, const T MinValue, const T MaxValue);
MIX_FLOATING_POINT_ARGS_THREE_PARAMS(Clamp)
NODISCARD FORCEINLINE constexpr float  Clamp(const float Value, const float MinValue, const float MaxValue);
NODISCARD FORCEINLINE constexpr double Clamp(const double Value, const double MinValue, const double MaxValue);
/** Inclusively clamp the value of type U between its minimum and maximum values and cast the result to TRet. */
template <typename TRet, typename U>
NODISCARD FORCEINLINE constexpr TRet ClampRet(const U Value, const U MinValue, const U MaxValue);
MIX_FLOATING_POINT_ARGS_THREE_PARAMS(ClampRet)

template <typename T> NODISCARD FORCEINLINE constexpr T Floor(const T Value);
template <typename T> NODISCARD FORCEINLINE constexpr T Ceil(const T Value);

NODISCARD FORCEINLINE bool IsNearlyEqual(const float A, const float B, const float Tolerance = JAFG_FLOAT_SMALL_NUMBER);
NODISCARD FORCEINLINE bool IsNearlyEqual(const double A, const double B, const double Tolerance = JAFG_DOUBLE_SMALL_NUMBER);
NODISCARD FORCEINLINE bool IsNearlyZero(const float Value, const float Tolerance = JAFG_FLOAT_SMALL_NUMBER);
NODISCARD FORCEINLINE bool IsNearlyZero(const double Value, const double Tolerance = JAFG_DOUBLE_SMALL_NUMBER);

/** https://en.cppreference.com/w/c/numeric/math/fmod */
NODISCARD FORCEINLINE float  Fmod(const float Numerator, const float Denominator);
NODISCARD FORCEINLINE double Fmod(const double Numerator, const double Denominator);
MIX_FLOATING_POINT_ARGS_TWO_PARAMS(Fmod)
/** https://en.cppreference.com/w/cpp/numeric/math/modf */
NODISCARD FORCEINLINE float  ModF(const float Numerator);
          FORCEINLINE float  ModF(const float Numerator, float* OutIntegralPart);
NODISCARD FORCEINLINE double ModF(const double Numerator);
          FORCEINLINE double ModF(const double Numerator, double* OutIntegralPart);

template <typename T> NODISCARD FORCEINLINE constexpr bool IsPowerOfTwo(const T Value);
template <typename T> NODISCARD FORCEINLINE constexpr T    Sqrt(const T Value);
template <typename T> NODISCARD FORCEINLINE constexpr T    InverseSqrt(const T Value);

template <typename T> NODISCARD FORCEINLINE constexpr T ToRadians(const T Degrees);
template <typename T> NODISCARD FORCEINLINE constexpr T ToDegrees(const T Radians);
/** Normalize the radian value of type T to the range [0, 2*PI[. */
template <typename T> NODISCARD FORCEINLINE constexpr T ClampRadians(const T Radians);
/** Normalize the radian value of type T to the range ]-PI, PI[. */
template <typename T> NODISCARD FORCEINLINE constexpr T NormalizeRadians(const T Radians);
/** Normalize the degree  value of type T to the range [0, 360[. */
template <typename T> NODISCARD FORCEINLINE constexpr T ClampDegrees(const T Degrees);
/** Normalize the degree value of type T to the range ]-180, 180]. */
template <typename T> NODISCARD FORCEINLINE constexpr T NormalizeDegrees(const T Degrees);

template <typename T> NODISCARD FORCEINLINE auto Sin(const T Value) -> T;       // So this is all non constexpr, lol.
template <typename T> NODISCARD FORCEINLINE auto Asin(const T Value) -> T;      // The std is not constexpr because of
template <typename T> NODISCARD FORCEINLINE auto Sinh(const T Value) -> T;      // legacy code (at least until C++26).
template <typename T> NODISCARD FORCEINLINE auto ASinh(const T Value) -> T;     // So we either shit on that, or
template <typename T> NODISCARD FORCEINLINE auto Cos(const T Value) -> T;       // implement our own math - I prefer
template <typename T> NODISCARD FORCEINLINE auto Acos(const T Value) -> T;      // the latter as currently this is just
template <typename T> NODISCARD FORCEINLINE auto Cosh(const T Value) -> T;      // a wrapper around std - that will be
template <typename T> NODISCARD FORCEINLINE auto ACosh(const T Value) -> T;     // fun :).
template <typename T> NODISCARD FORCEINLINE auto Tan(const T Value) -> T;
template <typename T> NODISCARD FORCEINLINE auto Atan(const T Value) -> T;
template <typename T> NODISCARD FORCEINLINE auto Tanh(const T Value) -> T;
template <typename T> NODISCARD FORCEINLINE auto ATanh(const T Value) -> T;

template <typename T, typename U>
FORCEINLINE constexpr void SinCos(T* SinScalar, T* CosScalar, const U InValue);
template <typename T>
FORCEINLINE constexpr void SinCos(T*  SinScalar, T*  CosScalar, const T InValue);

/** Round with common sense: 0.3 => 0.0; 0.7 => 1.0; 0.5 => 1.0; -0.5 => -1.0. */
NODISCARD FORCEINLINE float  Round(const float  Value) { return ::roundf(Value); }
NODISCARD FORCEINLINE double Round(const double Value) { return ::round(Value); }
/** Also useful: 0.3 => 0.0; 0.7 => 0.0; 0.5 => 0.0; -0.5 => 0.0. */
NODISCARD FORCEINLINE float  RoundToZero(const float Value);
NODISCARD FORCEINLINE double RoundToZero(const double Value);

/**
 * An affine transformation to get the transformation for objects viewed as of Eye.
 *
 * @tparam T      The floating type.
 * @param  Eye    The location to calculate when "viewed" form this location.
 * @param  Center The forward center of the view.
 * @param  Up     The up vector of the view.
 * @return        The view matrix V:
 *         |    R.X    U'.X   -F.X    0 | Where R  is the normalized right vector by crossing F and Up.
 *     V = |    R.Y    U'.Y   -F.Y    0 |       U' is the normalized up vector by crossing R and F.
 *         |    R.Z    U'.Z   -F.Z    0 |       F  is the normalized forward vector by getting the delta
 *         | -R*Eye -U'*Eye  F*Eye    1 |       from Eye to Center.
 */
template <typename T>
NODISCARD FORCEINLINE constexpr TMatrix<T> MakeViewMatrix(const TVector<T>& Eye, const TVector<T>& Center, const TVector<T>& Up);
template <typename T>
FORCEINLINE constexpr void MakeViewMatrixInline(TMatrix<T>* Out, const TVector<T>& Eye, const TVector<T>& Center, const TVector<T>& Up);

/**
 * An affine transformation to get the transformation for objects to be projected perspectively while keeping the
 * Z depth buffer information.
 *
 * @tparam T          The floating type.
 * @param  RadYFov    Vertical field of view in radians.
 * @param  Ratio      Ratio of the perspective view for distortion correction.
 * @param  NearZPlane Near clipped Z plane for the perspective frustum view.
 * @param  FarZPlane  Far clipped Z plane for the perspective frustum view.
 * @return            The perspective projection matrix P:
 *         | (H*R)^-1    0                   0                     0 | Where H is the cotangent of the half of the
 *     P = |        0 H^-1                   0                     0 |       vertical radiant field of view.
 *         |        0    0 -(Fz+Nz)*(Fz-Nz)^-1 -(2*Fz*Nz)*(Fz-Nz)^-1 |       R is the ratio of the view.
 *         |        0    0                  -1                     0 |       Nz is the near Z plane.
 *                                                                           Fz is the far Z plane.
 */
template <typename T>
NODISCARD FORCEINLINE constexpr TMatrix<T> MakePerspectiveProjectionMatrix(const T RadYFov, const T Ratio, const T NearZPlane, const T FarZPlane);

/**
 * An affine transformation to get the transformation for objects to be projected orthographically.
 *
 * @tparam T           The floating type.
 * @param  RightBottom The two-dimensional coordinate of the right bottom corner.
 * @param  LeftTop     The two-dimensional coordinate of the left top corner.
 * @return             The orthographic projection matrix O:
 *          | 2*(R-L)^-1         0   0 -(R+L)*(R-L)^-1 | Where R is the right coordinate.
 *     O =  | 0          2*(B-T)^-1  0 -(B+T)*(B-T)^-1 |       L is the left coordinate.
 *          | 0                  0  -1               0 |       T is the top coordinate.
 *          | 0                  0   0               1 |       B is the bottom coordinate.
 */
template <typename T>
NODISCARD FORCEINLINE constexpr TMatrix<T> MakeOrthographicProjectionMatrix(const TVector2<T>& RightBottom, const TVector2<T>& LeftTop = TVector2<T>::Zero());
template <typename T, typename U = float>
NODISCARD FORCEINLINE constexpr TMatrix<U> MakeOrthographicProjectionMatrix(const TIntVector2<T>& RightBottom, const TIntVector2<T>& LeftTop = TIntVector2<T>::Zero());


/*----------------------------------------------------------------------------
    BEGIN IMPLEMENTATION.
----------------------------------------------------------------------------*/

#if PLATFORM_WINDOWS_WITH_MSVC // Missing constexpr!
    template <> FORCEINLINE float  Absolute(const float A)  { return ::fabsf(A); }
    template <> FORCEINLINE double Absolute(const double A) { return ::fabs(A); }
#elif PLATFORM_WINDOWS_WITH_GCC
    template <> FORCEINLINE float  Absolute(const float A)  { return ::abs(A); }
    template <> FORCEINLINE double Absolute(const double A) { return ::abs(A); }
#endif /* PLATFORM_WINDOWS_WITH_GCC */

template <> NODISCARD FORCEINLINE constexpr float  Min(const float A,  const float B)  { return (B < A) ? B : A; }
template <> NODISCARD FORCEINLINE constexpr double Min(const double A, const double B) { return (B < A) ? B : A; }
template <> NODISCARD FORCEINLINE constexpr float  Max(const float A,  const float B)  { return (B < A) ? A : B; }
template <> NODISCARD FORCEINLINE constexpr double Max(const double A, const double B) { return (B < A) ? A : B; }

template <> NODISCARD FORCEINLINE constexpr float  Squared(const float Value)  { return Value * Value; }
template <> NODISCARD FORCEINLINE constexpr double Squared(const double Value) { return Value * Value; }
template <> NODISCARD FORCEINLINE constexpr float  Cubed(const float Value)    { return Value * Value * Value; }
template <> NODISCARD FORCEINLINE constexpr double Cubed(const double Value)   { return Value * Value * Value; }
template <> NODISCARD FORCEINLINE constexpr float  Quartic(const float Value)  { return Value * Value * Value * Value; }
template <> NODISCARD FORCEINLINE constexpr double Quartic(const double Value) { return Value * Value * Value * Value; }
template <> NODISCARD FORCEINLINE constexpr float  Quintic(const float Value)  { return Value * Value * Value * Value * Value; }
template <> NODISCARD FORCEINLINE constexpr double Quintic(const double Value) { return Value * Value * Value * Value * Value; }
template <> NODISCARD FORCEINLINE constexpr float  Sextic(const float Value)   { return Value * Value * Value * Value * Value * Value; }
template <> NODISCARD FORCEINLINE constexpr double Sextic(const double Value)  { return Value * Value * Value * Value * Value * Value; }
template <typename T> FORCEINLINE constexpr T      Pow(const T Base, const T Exponent) { return std::pow(Base, Exponent); }
template <> NODISCARD FORCEINLINE           float  Pow(const float Base, const float Exponent) { return ::powf(Base, Exponent); }
template <> NODISCARD FORCEINLINE           double Pow(const double Base, const double Exponent) { return ::pow(Base, Exponent); }
template <typename T> NODISCARD FORCEINLINE constexpr T Factorial(const T Value)
{
    return (Value == static_cast<T>(0))
        ? static_cast<T>(1) /* We have to make this more performant by using a loop. */
        : Value * Factorial<T>(Value - static_cast<T>(1));
}

template <typename T>
NODISCARD FORCEINLINE constexpr T      Clamp(const T Value, const T MinValue, const T MaxValue) { return Maths::Max(Maths::Min(Value, MaxValue), MinValue); }
NODISCARD FORCEINLINE constexpr float  Clamp(const float Value, const float MinValue, const float MaxValue) { return Clamp<float>(Value, MinValue, MaxValue); }
NODISCARD FORCEINLINE constexpr double Clamp(const double Value, const double MinValue, const double MaxValue) { return Clamp<double>(Value, MinValue, MaxValue); }
template <typename TRet, typename U> NODISCARD FORCEINLINE constexpr TRet ClampRet(const U Value, const U MinValue, const U MaxValue) { return static_cast<TRet>(Clamp(Value, MinValue, MaxValue)); }

template <typename T> NODISCARD   constexpr T      Floor(const T Value) UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <> NODISCARD FORCEINLINE           float  Floor(const float Value)  { return ::floorf(Value); }
template <> NODISCARD FORCEINLINE           double Floor(const double Value) { return ::floor(Value);  }
template <typename T> NODISCARD   constexpr T      Ceil(const T Value)  UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <> NODISCARD FORCEINLINE           float  Ceil(const float Value)   { return ::ceilf(Value);  }
template <> NODISCARD FORCEINLINE           double Ceil(const double Value)  { return ::ceil(Value);   }

NODISCARD FORCEINLINE bool IsNearlyEqual(const float A, const float B, const float Tolerance /* = JAFG_FLOAT_SMALL_NUMBER */) { return Maths::Absolute(A - B) < Tolerance; }
NODISCARD FORCEINLINE bool IsNearlyEqual(const double A, const double B, const double Tolerance /* = JAFG_DOUBLE_SMALL_NUMBER */) { return Maths::Absolute(A - B) < Tolerance; }
NODISCARD FORCEINLINE bool IsNearlyZero(const float Value, const float Tolerance /* = JAFG_FLOAT_SMALL_NUMBER */) { return Maths::Absolute(Value) < Tolerance; }
NODISCARD FORCEINLINE bool IsNearlyZero(const double Value, const double Tolerance /* = JAFG_DOUBLE_SMALL_NUMBER */) { return Maths::Absolute(Value) < Tolerance; }

NODISCARD FORCEINLINE float Fmod(const float Numerator, const float Denominator)
{
#if LAL_DO_CHECKS
    const float AbsDenominator = Maths::Absolute(Denominator);
    if (AbsDenominator <= JAFG_FLOAT_SMALL_NUMBER)
    {
        checkNoEntry()
        return 0.0f;
    }
#endif /* LAL_DO_CHECKS */

    return ::fmodf(Numerator, Denominator);
}

NODISCARD FORCEINLINE double Fmod(const double Numerator, const double Denominator)
{
#if LAL_DO_CHECKS
    const double AbsDenominator = Maths::Absolute(Denominator);
    if (AbsDenominator <= JAFG_DOUBLE_SMALL_NUMBER)
    {
        checkNoEntry()
        return 0.0;
    }
#endif /* LAL_DO_CHECKS */

    return ::fmod(Numerator, Denominator);
}

NODISCARD FORCEINLINE float  ModF(const float Numerator) { float Dummy = 0.0; return ::modff(Numerator, &Dummy); }
          FORCEINLINE float  ModF(const float Numerator, float* OutIntegralPart) { return ::modff(Numerator, OutIntegralPart); }
NODISCARD FORCEINLINE double ModF(const double Numerator) { double Dummy = 0.0; return ::modf(Numerator, &Dummy); }
          FORCEINLINE double ModF(const double Numerator, double* OutIntegralPart) { return ::modf(Numerator, OutIntegralPart); }

template <typename T>
NODISCARD FORCEINLINE constexpr bool IsPowerOfTwo(const T Value)
{
    static_assert(std::is_integral_v<T>, "Value must be an integral type.");
    if (Value <= static_cast<T>(0)) { return false; }
    return (Value & (Value - static_cast<T>(1))) == static_cast<T>(0);
}

template <typename T>
NODISCARD FORCEINLINE constexpr T Sqrt(const T Value)
{
    static_assert(std::is_floating_point_v<T>, "Value must be a floating point type.");
    return static_cast<T>(::sqrt(Value));
}

template <typename T>
NODISCARD FORCEINLINE constexpr T InverseSqrt(const T Value)
{
    static_assert(std::is_floating_point_v<T>, "Value must be a floating point type.");
    return static_cast<T>(1) / Maths::Sqrt(Value);
}

template <typename T>
NODISCARD FORCEINLINE constexpr T ToRadians(const T Degrees)
{
    static_assert(std::is_floating_point_v<T>, "Degrees must be a floating point type.");
    return Degrees * static_cast<T>(0.01745329251994329576923690768489);
}

template <typename T>
NODISCARD FORCEINLINE constexpr T ToDegrees(const T Radians)
{
    static_assert(std::is_floating_point_v<T>, "Radians must be a floating point type.");
    return Radians * static_cast<T>(57.295779513082320876798154814105);
}

template <typename T>
NODISCARD FORCEINLINE constexpr T ClampRadians(const T Radians)
{
    static_assert(std::is_floating_point_v<T>, "Radians must be a floating point type.");
    return Radians - (static_cast<T>(JAFG_TWO_PI_D) * static_cast<T>(std::floor(Radians / JAFG_TWO_PI_D)));
}

template <typename T>
NODISCARD FORCEINLINE constexpr T NormalizeRadians(T Radians)
{
    static_assert(std::is_floating_point_v<T>, "Radians must be a floating point type.");
    Radians = Maths::ClampRadians(Radians);
    if (Radians > static_cast<T>(JAFG_PI_D)) { Radians -= static_cast<T>(JAFG_TWO_PI_D); }
    return Radians;
}

template <typename T>
NODISCARD FORCEINLINE constexpr T ClampDegrees(const T Degrees)
{
    static_assert(std::is_floating_point_v<T>, "Degrees must be a floating point type.");
    return Degrees - (static_cast<T>(JAFG_DEG_FULL_CIRCLE_D) * static_cast<T>(Maths::Floor(Degrees / JAFG_DEG_FULL_CIRCLE_D)));
}

template <typename T>
NODISCARD FORCEINLINE constexpr T NormalizeDegrees(T Degrees)
{
    static_assert(std::is_floating_point_v<T>, "Degrees must be a floating point type.");
    Degrees = Maths::ClampDegrees(Degrees);
    if (Degrees > static_cast<T>(JAFG_DEG_HALF_CIRCLE_D)) { Degrees -= static_cast<T>(JAFG_DEG_FULL_CIRCLE_D); }
    return Degrees;
}

template <> NODISCARD FORCEINLINE float  Sin(const float Value)     { return ::sinf(Value);   }
template <> NODISCARD FORCEINLINE double Sin(const double Value)    { return ::sin(Value);    }
template <> NODISCARD FORCEINLINE float  Asin(const float Value)    { return ::asinf(Value);  }
template <> NODISCARD FORCEINLINE double Asin(const double Value)   { return ::asin(Value);   }
template <> NODISCARD FORCEINLINE float  Sinh(const float Value)    { return ::sinhf(Value);  }
template <> NODISCARD FORCEINLINE double Sinh(const double Value)   { return ::sinh(Value);   }
template <> NODISCARD FORCEINLINE float  ASinh(const float Value)   { return ::asinhf(Value); }
template <> NODISCARD FORCEINLINE double ASinh(const double Value)  { return ::asinh(Value);  }
template <> NODISCARD FORCEINLINE float  Cos(const float Value)     { return ::cosf(Value);   }
template <> NODISCARD FORCEINLINE double Cos(const double Value)    { return ::cos(Value);    }
template <> NODISCARD FORCEINLINE float  Acos(const float Value)    { return ::acosf(Value);  }
template <> NODISCARD FORCEINLINE double Acos(const double Value)   { return ::acos(Value);   }
template <> NODISCARD FORCEINLINE float  Cosh(const float Value)    { return ::coshf(Value);  }
template <> NODISCARD FORCEINLINE double Cosh(const double Value)   { return ::cosh(Value);   }
template <> NODISCARD FORCEINLINE float  ACosh(const float Value)   { return ::acoshf(Value); }
template <> NODISCARD FORCEINLINE double ACosh(const double Value)  { return ::acosh(Value);  }
template <> NODISCARD FORCEINLINE float  Tan(const float Value)     { return ::tanf(Value);   }
template <> NODISCARD FORCEINLINE double Tan(const double Value)    { return ::tan(Value);    }
template <> NODISCARD FORCEINLINE float  Atan(const float Value)    { return ::atanf(Value);  }
template <> NODISCARD FORCEINLINE double Atan(const double Value)   { return ::atan(Value);   }
template <> NODISCARD FORCEINLINE float  Tanh(const float Value)    { return ::tanhf(Value);  }
template <> NODISCARD FORCEINLINE double Tanh(const double Value)   { return ::tanh(Value);   }
template <> NODISCARD FORCEINLINE float  ATanh(const float Value)   { return ::atanhf(Value); }
template <> NODISCARD FORCEINLINE double ATanh(const double Value)  { return ::atanh(Value);  }
template <typename T> NODISCARD FORCEINLINE T Sin(const T Value)   UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T Asin(const T Value)  UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T Sinh(const T Value)  UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T ASinh(const T Value) UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T Cos(const T Value)   UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T Acos(const T Value)  UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T Cosh(const T Value)  UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T ACosh(const T Value) UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T Tan(const T Value)   UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T Atan(const T Value)  UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T Tanh(const T Value)  UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })
template <typename T> NODISCARD FORCEINLINE T ATanh(const T Value) UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return T { })

template <typename T, typename U>
FORCEINLINE constexpr void SinCos(T* SinScalar, T* CosScalar, const U InValue)
{
    static_assert(std::is_floating_point_v<T> && std::is_floating_point_v<U>, "Scalar and InValue must be floating point types.");
    static_assert(std::is_same_v<T, U> == false, "Scalar and InValue must not be the same type.");
    Maths::SinCos(SinScalar, CosScalar, static_cast<T>(InValue));
    return;
}

template <typename T>
FORCEINLINE constexpr void SinCos(T* SinScalar, T* CosScalar, const T InValue)
{
    *SinScalar = Maths::Sin(InValue);
    *CosScalar = Maths::Cos(InValue);
    return;
}

NODISCARD FORCEINLINE float RoundToZero(const float Value)
{
    float IntegralPart = 0.0f;
    const float FractionalPart = Maths::ModF(Value, &IntegralPart);

    if (FractionalPart < 0.0f)
    {
        return FractionalPart < -0.5f ? IntegralPart - 1.0f : IntegralPart;
    }

    return FractionalPart > 0.5f ? IntegralPart + 1.0f : IntegralPart;
}

NODISCARD FORCEINLINE double RoundToZero(const double Value)
{
    double IntegralPart = 0.0f;
    const double FractionalPart = Maths::ModF(Value, &IntegralPart);

    if (FractionalPart < 0.0f)
    {
        return FractionalPart < -0.5f ? IntegralPart - 1.0f : IntegralPart;
    }

    return FractionalPart > 0.5f ? IntegralPart + 1.0f : IntegralPart;
}

template <typename T>
NODISCARD FORCEINLINE constexpr TMatrix<T> MakeViewMatrix(const TVector<T>& Eye, const TVector<T>& Center, const TVector<T>& Up)
{
    const TVector<T> F = (Center - Eye).NormalizeRet();
    const TVector<T> R = F.Cross(Up).NormalizeRet();
    const TVector<T> U = R.Cross(F);

    TMatrix<T> Result;
    Result.Matrix[0][0] =  R.X;
    Result.Matrix[1][0] =  R.Y;
    Result.Matrix[2][0] =  R.Z;
    Result.Matrix[0][1] =  U.X;
    Result.Matrix[1][1] =  U.Y;
    Result.Matrix[2][1] =  U.Z;
    Result.Matrix[0][2] = -F.X;
    Result.Matrix[1][2] = -F.Y;
    Result.Matrix[2][2] = -F.Z;
    Result.Matrix[3][0] = -(R | Eye);
    Result.Matrix[3][1] = -(U | Eye);
    Result.Matrix[3][2] =  (F | Eye);
    Result.Matrix[3][3] =  1.0f;

    return Result;
}

template <typename T>
FORCEINLINE constexpr void MakeViewMatrixInline(TMatrix<T>* Out, const TVector<T>& Eye, const TVector<T>& Center, const TVector<T>& Up)
{
    checkSlow( Out )

    const TVector<T> F = (Center - Eye).NormalizeRet();
    const TVector<T> R = F.Cross(Up).NormalizeRet();
    const TVector<T> U = R.Cross(F);

    Out->Matrix[0][0] =  R.X;
    Out->Matrix[1][0] =  R.Y;
    Out->Matrix[2][0] =  R.Z;
    Out->Matrix[0][1] =  U.X;
    Out->Matrix[1][1] =  U.Y;
    Out->Matrix[2][1] =  U.Z;
    Out->Matrix[0][2] = -F.X;
    Out->Matrix[1][2] = -F.Y;
    Out->Matrix[2][2] = -F.Z;
    Out->Matrix[3][0] = -(R | Eye);
    Out->Matrix[3][1] = -(U | Eye);
    Out->Matrix[3][2] =  (F | Eye);
    Out->Matrix[3][3] =  1.0f;

    return;
}

template <typename T>
NODISCARD FORCEINLINE constexpr TMatrix<T> MakePerspectiveProjectionMatrix(const T RadYFov, const T Ratio, const T NearZPlane, const T FarZPlane)
{
    check( NearZPlane > 0.0f && FarZPlane > NearZPlane )
    check( Maths::Absolute(Ratio - std::numeric_limits<T>::epsilon()) > static_cast<T>(0.0f) )

    const T TanHalfYFov = Maths::Tan(RadYFov * static_cast<T>(0.5f));

    TMatrix<T> Result = Matrix::Zero;
    Result.Matrix[0][0] = static_cast<T>(1.0f) / (Ratio * TanHalfYFov);
    Result.Matrix[1][1] = static_cast<T>(1.0f) / TanHalfYFov;
    Result.Matrix[2][2] = - (FarZPlane + NearZPlane) / (FarZPlane - NearZPlane);
    Result.Matrix[2][3] = -  static_cast<T>(1.0f);
    Result.Matrix[3][2] = - (static_cast<T>(2.0f) * FarZPlane * NearZPlane) / (FarZPlane - NearZPlane);
    return Result;
}

template <typename T>
NODISCARD FORCEINLINE constexpr TMatrix<T> MakeOrthographicProjectionMatrix(const TVector2<T>& RightBottom, const TVector2<T>& LeftTop /* = TVector2<T>::Zero() */)
{
    TMatrix<T> Result = Matrix::Identity;
    Result.Matrix[0][0] = static_cast<T>(2.0f) / (RightBottom.X - LeftTop.X);
    Result.Matrix[1][1] = static_cast<T>(2.0f) / (RightBottom.Y - LeftTop.Y);
    Result.Matrix[2][2] = - static_cast<T>(1.0f);
    Result.Matrix[3][1] = - (RightBottom.X + LeftTop.X) / (RightBottom.X - LeftTop.X);
    Result.Matrix[3][0] = - (RightBottom.Y + LeftTop.Y) / (RightBottom.Y - LeftTop.Y);
    return Result;
}

template <typename T, typename U>
NODISCARD FORCEINLINE constexpr TMatrix<U> MakeOrthographicProjectionMatrix(const TIntVector2<T>& RightBottom, const TIntVector2<T>& LeftTop /* = TIntVector2<T>::Zero() */)
{
    return Maths::MakeOrthographicProjectionMatrix(
        TVector2<U>(static_cast<U>(RightBottom.X), static_cast<U>(RightBottom.Y)),
        TVector2<U>(static_cast<U>(LeftTop.X), static_cast<U>(LeftTop.Y))
    );
}

} /* Namespace Jafg::Maths */
