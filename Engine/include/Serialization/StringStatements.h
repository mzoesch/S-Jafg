// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Serde
{

namespace Detail
{

struct StringStatementsBaseFn
{
    template<algo::input_range TRange> requires IsString_v<std::remove_cvref_t<std::decay_t<TRange>>>
    NODISCARD FORCEINLINE constexpr bool
    operator()(this auto&& Self, TRange&& Range) noexcept
    {
        return (Self)(algo::begin(Range), algo::end(Range));
    }
};

struct IsNumericFn : StringStatementsBaseFn
{
    using StringStatementsBaseFn::operator();

    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TIter Iter, TSent Sent) const noexcept
    {
        if (Iter != Sent)
        {
            if (*Iter == '-')
            {
                ++Iter;
            }
        }
        bool bDecimal{};
        for (; Iter != Sent; ++Iter)
        {
            if (*Iter == '.')
            {
                if (bDecimal)
                {
                    return false;
                }
                bDecimal = true;
                continue;
            }
            if (*Iter >= '0' && *Iter <= '9')
            {
                continue;
            }
            return false;
        }
        return true;
    }
};

struct IsIntegralFn : StringStatementsBaseFn
{
    using StringStatementsBaseFn::operator();

    template<std::input_iterator Iter, std::sentinel_for<Iter> Sent>
    NODISCARD FORCEINLINE constexpr bool
    operator()(Iter First, Sent Last) const noexcept
    {
        if (First != Last)
        {
            if (*First == '-')
            {
                ++First;
            }
        }
        for (; First != Last; ++First)
        {
            if (*First >= '0' && *First <= '9')
            {
                continue;
            }
            return false;
        }
        return true;
    }
};

struct IsUIntegralFn : StringStatementsBaseFn
{
    using StringStatementsBaseFn::operator();

    template<std::input_iterator Iter, std::sentinel_for<Iter> Sent>
    NODISCARD FORCEINLINE constexpr bool
    operator()(Iter First, Sent Last) const noexcept
    {
        for (; First != Last; ++First)
        {
            if (*First >= '0' && *First <= '9')
            {
                continue;
            }
            return false;
        }
        return true;
    }
};

} /* namespace Detail */

inline constexpr Detail::IsNumericFn IsNumeric{};
inline constexpr Detail::IsIntegralFn IsIntegral{};
inline constexpr Detail::IsUIntegralFn IsUIntegral{};

} /* ~Namespace Serde */
