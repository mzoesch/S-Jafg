// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

//# Weak comparison for left-hand side comparison.
template <typename TLhs, typename TRhs>
concept TEqualityComparableLeft = requires(const TLhs& Lhs, const TRhs& Rhs)
{
    { Lhs == Rhs } -> std::convertible_to<bool>;
};

//# Weak comparison for right-hand side comparison.
template <typename TLhs, typename TRhs>
concept TEqualityComparableRight = requires(const TLhs& LHs, const TRhs& Rhs)
{
    { Rhs == LHs } -> std::convertible_to<bool>;
};

//# Weak comparison for two-way comparison.
template <typename TLhs, typename TRhs>
concept TEqualityComparableTwoWay = TEqualityComparableLeft<TLhs, TRhs> && TEqualityComparableRight<TLhs, TRhs>;

//# Weak comparison for spaceships.
template <typename TLhs, typename TRhs>
concept TEqualityComparableThreeWay = TEqualityComparableTwoWay<TLhs, TRhs> && requires(const TLhs& Lhs, const TRhs& Rhs)
{
    { Lhs <=> Rhs } -> std::convertible_to<std::strong_ordering>;
};

//# Weak assignable clause. TRhs may be decayed to an l-value.
template<typename TLhs, typename TRhs>
concept AssignableFromWeak
    =  std::is_lvalue_reference_v<TLhs>
    && requires(TLhs Lhs, TRhs&& Rhs)
    {
        { Lhs = std::forward<TRhs>(Rhs) } -> std::same_as<TLhs>;
    };

// Strong assignable clause. TRhs must be an l-value reference.
template<typename TLhs, typename TRhs>
concept AssignableFromL
    =  AssignableFromWeak<TLhs, TRhs>
    && std::is_lvalue_reference_v<TRhs>;

// Strong assignable clause. TRhs must be an r-value reference.
template<typename TLhs, typename TRhs>
concept AssignableFromR
    =  AssignableFromWeak<TLhs, TRhs>
    && std::is_rvalue_reference_v<TRhs>;

template <typename T> struct TIsChar : std::false_type { };
template <typename T> struct TIsChar<const T> : TIsChar<T> { };
template <> struct TIsChar<char> : std::true_type { };
template <> struct TIsChar<wchar_t> : std::true_type { };
template <> struct TIsChar<char8_t> : std::true_type { };
template <> struct TIsChar<char16_t> : std::true_type { };
template <> struct TIsChar<char32_t> : std::true_type { };
template <typename T>
inline constexpr bool TIsChar_v = TIsChar<T>::value;

} /* ~Namespace Lal */
