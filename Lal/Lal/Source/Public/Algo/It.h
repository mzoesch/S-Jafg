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

} /* ~Namespace algo */
