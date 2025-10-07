// Copyright mzoesch. All rights reserved.

#pragma once

namespace algo
{

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
