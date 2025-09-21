// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename T, T V>
struct IntegralConstant
{
    static constexpr T value { V };

    typedef T value_type;
    typedef IntegralConstant<T, V> type;

    FORCEINLINE constexpr operator value_type() const noexcept { return IntegralConstant::value; }
    FORCEINLINE constexpr value_type operator()() const noexcept { return IntegralConstant::value; }
};

template <bool V>
using BoolConstant = IntegralConstant<bool, V>;

typedef BoolConstant<true > TrueType;
typedef BoolConstant<false> FalseType;

} /* Namespace Lal */
