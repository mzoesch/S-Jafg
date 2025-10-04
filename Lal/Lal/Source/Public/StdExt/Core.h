// Copyright mzoesch. All rights reserved.

#pragma once

namespace std
{

template <typename T>
FORCEINLINE constexpr void default_swap(T* Element) noexcept
{
    T Default;
    swap(*Element, Default);

    return;
}

template <typename TContainer>
FORCEINLINE constexpr void orphan(TContainer* Container) noexcept
{
    Container->clear();
    default_swap(Container);

    LAL_FWD_CHECK([Container]{return Container->capacity() == 0;})

    return;
}

} /* ~Namespace std */
