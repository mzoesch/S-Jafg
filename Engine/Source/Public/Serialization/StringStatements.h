// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Serde
{

template<typename T> struct TIsStringType : std::false_type {};

template<typename T>
inline constexpr bool IsStringType_v = TIsStringType<T>::value;

namespace Detail
{

struct IsNumericFn
{
    template<std::input_iterator TIter, std::sentinel_for<TIter> TSent>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TIter Iter, TSent Sent) const noexcept
    {
        for (; Iter != Sent; ++Iter)
        {
            if (*Iter != '.' && (*Iter < '0' || *Iter > '9'))
            {
                return false;
            }
        }
        return true;
    }

    template<algo::input_range TRange> requires IsStringType_v<std::remove_cvref_t<std::decay_t<TRange>>>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TRange&& Range) const noexcept
    {
        return (*this)(algo::begin(Range), algo::end(Range));
    }
};

struct IsIntegralFn
{
    template<std::input_iterator Iter, std::sentinel_for<Iter> Sent>
    NODISCARD FORCEINLINE constexpr bool
    operator()(Iter First, Sent Last) const noexcept
    {
        for (; First != Last; ++First)
        {
            if (*First < '0' || *First > '9')
            {
                return false;
            }
        }
        return true;
    }

    template<algo::input_range TRange> requires IsStringType_v<std::remove_cvref_t<std::decay_t<TRange>>>
    NODISCARD FORCEINLINE constexpr bool
    operator()(TRange&& Range) const noexcept
    {
        return (*this)(algo::begin(Range), algo::end(Range));
    }
};

} /* namespace Detail */

inline constexpr Detail::IsNumericFn IsNumeric{};
inline constexpr Detail::IsIntegralFn IsIntegral{};

template<> struct TIsStringType<LString> : std::true_type {};
template<> struct TIsStringType<Lu8String> : std::true_type {};
template<> struct TIsStringType<Lu16String> : std::true_type {};
template<> struct TIsStringType<Lu32String> : std::true_type {};
template<> struct TIsStringType<LStringView> : std::true_type {};
template<> struct TIsStringType<Lu8StringView> : std::true_type {};
template<> struct TIsStringType<Lu16StringView> : std::true_type {};
template<> struct TIsStringType<Lu32StringView> : std::true_type {};
template<> struct TIsStringType<char const*> : std::true_type {};
template<> struct TIsStringType<unsigned char const*> : std::true_type {};
template<> struct TIsStringType<signed char const*> : std::true_type {};
template<> struct TIsStringType<char8_t const*> : std::true_type {};
template<> struct TIsStringType<char16_t const*> : std::true_type {};
template<> struct TIsStringType<char32_t const*> : std::true_type {};

} /* ~Namespace Serde */
