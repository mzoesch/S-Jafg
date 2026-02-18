// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg::Algo
{

namespace Private
{

template <typename T>
void SortQuick(T* Begin, T* Slack);

} /* ~Namespace Private */

FORCEINLINE void SortQuick(auto* Container)
{
    static_assert(TIterator_IsContiguous_v<TIteratorTraits<decltype(Container->begin())>>);
    check(algo::size(*Container) > 0)
    Private::SortQuick(Container->data(), Container->data() + algo::size(*Container));
}

namespace Private
{

template <typename T>
T* SortQuick_Pivot(T* Begin, T* Slack)
{
    if (static_cast<const void*>(Begin + (Slack - Begin) / 2) != static_cast<const void*>(Slack - 1))
    {
        std::swap(*(Begin + (Slack - Begin) / 2), *(Slack - 1));
    }

    T* Pivot { Slack - 1 };
    T* Left { Begin };

    for (T* Right { Left }; Right < Pivot; ++Right)
    {
        if (*Right < *Pivot)
        {
            if (Left != Right)
            {
                std::swap(*Left, *Right);
            }

            ++Left;
        }

        continue;
    }

    if (Left != Pivot)
    {
        std::swap(*Left, *Pivot);
    }

    return Left;
}

template <typename T>
void SortQuick(T* Begin, T* Slack)
{
    if (Slack - Begin > 1)
    {
        T* Pivot { SortQuick_Pivot(Begin, Slack) };

        SortQuick<T>(Begin, Pivot);
        SortQuick<T>(Pivot + 1, Slack);
    }

    return;
}

} /* ~Namespace Private */

} /* ~Namespace Jafg::Algo */
