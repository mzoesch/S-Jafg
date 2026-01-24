// Copyright mzoesch. All rights reserved.

#pragma once

namespace algo
{

using std::swap;

template <typename T> requires std::is_default_constructible_v<T>
FORCEINLINE constexpr void default_swap(T* Element) noexcept
{
    T Default;
    swap(*Element, Default);

    return;
}

} /* ~Namespace algo */
