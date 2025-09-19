// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename T, T V>
struct IntegralConstant
{
    static constexpr T Value { V };

    typedef T ValueType;
    typedef IntegralConstant<T, V> Type;

    FORCEINLINE constexpr operator ValueType() const noexcept { return IntegralConstant::Value; }
    FORCEINLINE constexpr ValueType operator()() const noexcept { return IntegralConstant::Value; }
};

template <bool V>
using BoolConstant = IntegralConstant<bool, V>;

typedef BoolConstant<true > TrueType;
typedef BoolConstant<false> FalseType;

} /* Namespace Lal */
