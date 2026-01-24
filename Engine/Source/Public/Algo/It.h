// Copyright mzoesch. All rights reserved.

#pragma once

namespace algo
{

using std::predicate;
template<typename F, typename... TArgs>
concept void_predicate = std::regular_invocable<F, TArgs...> && std::same_as<std::invoke_result_t<F, TArgs...>, void>;
template<typename TFn, typename TWhat>
concept void_mutable_predicate = requires(TFn&& Fn, TWhat What) { std::invoke(std::forward<TFn>(Fn), What); };

//# Trivial distance.
using std::distance;
template<std::ranges::input_range TRange, typename TIt>
FORCEINLINE constexpr auto distance(TRange Range, TIt It) noexcept
{
    return std::ranges::distance(std::ranges::begin(Range), It);
}

//# Distance between ranges.
inline constexpr std::ranges::__distance_fn ranged_distance{};

inline constexpr decltype(LString::npos) npos{ LString::npos };

} /* ~Namespace algo */

static_assert(sizeof(algo::npos) == sizeof(std::string::npos));
static_assert(sizeof(algo::npos) == sizeof(LString::npos));
static_assert(sizeof(algo::npos) == sizeof(LStringView::npos));
static_assert(algo::npos == std::string::npos);
static_assert(algo::npos == LString::npos);
static_assert(algo::npos == LStringView::npos);
static_assert(std::is_same_v<decltype(algo::npos), decltype(std::string::npos)>);
static_assert(std::is_same_v<decltype(algo::npos), decltype(LString::npos)>);
static_assert(std::is_same_v<decltype(algo::npos), decltype(LStringView::npos)>);

namespace algo::detail
{


} /* ~Namespace algo::detail */
