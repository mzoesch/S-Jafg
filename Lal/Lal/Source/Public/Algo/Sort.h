// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg::Algo
{

namespace Private
{

template <typename T>
void SortQuick(T* Begin, T* Slack);

} /* ~Namespace Private */

template <typename TAlloc> requires (TArrayBase<TAlloc>::IsContentMutable())
FORCEINLINE void SortQuick(TArrayBase<TAlloc>* Container);

template <typename TAlloc> requires (TArrayBase<TAlloc>::IsContentMutable())
FORCEINLINE void SortQuick(TArrayBase<TAlloc>* Container)
{
    Private::SortQuick(Container->GetData(), Container->GetSlack());
}

namespace Private
{

template <typename T>
T* SortQuick_Pivot(T* Begin, T* Slack)
{
    std::swap(*(Begin + (Slack - Begin) / 2), *(Slack - 1));
    T* Pivot { Slack - 1 };

    T* Left { Begin };

    for (T* Right { Left }; Right < Pivot; ++Right)
    {
        if (*Right < *Pivot)
        {
            std::swap(*Left, *Right);
            ++Left;
        }

        continue;
    }

    std::swap(*Left, *Pivot);

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
