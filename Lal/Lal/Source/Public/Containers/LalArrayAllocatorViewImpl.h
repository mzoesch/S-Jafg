// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal::Private
{

template <
    typename TIn,
    TArrayBaseAllocatorTraitsConceptBase TTraits,
    typename _TIterator,
    typename _TConstIterator,
    typename _TMoveIterator,
    typename _TPointer,
    typename _TConstPointer,
    typename _TReference,
    typename _TConstReference
    >
FORCEINLINE constexpr typename TArrayBaseDefaultAllocatorWeakImpl<
    TIn,
    TTraits,
    _TIterator,
    _TConstIterator,
    _TMoveIterator,
    _TPointer,
    _TConstPointer,
    _TReference,
    _TConstReference
    >::SizeType
TArrayBaseDefaultAllocatorWeakImpl<TIn, TTraits,
    _TIterator,
    _TConstIterator,
    _TMoveIterator,
    _TPointer,
    _TConstPointer,
    _TReference,
    _TConstReference
    >::Drop(const SizeType Count) noexcept
{
    if (Count == 0 || this->Data == this->Slack)
    {
        return 0;
    }

    const SizeType Dropped { Maths::Min(Count, static_cast<SizeType>(this->Slack - this->Data)) };
    this->Data += Dropped;

    return Dropped;
}

#if LAL_DO_CHECKS
template <
    typename TIn,
    TArrayBaseAllocatorTraitsConceptBase TTraits,
    typename _TIterator,
    typename _TConstIterator,
    typename _TMoveIterator,
    typename _TPointer,
    typename _TConstPointer,
    typename _TReference,
    typename _TConstReference
    >
template <typename UPredicate>
FORCEINLINE void TArrayBaseDefaultAllocatorWeakImpl<
    TIn,
    TTraits,
    _TIterator,
    _TConstIterator,
    _TMoveIterator,
    _TPointer,
    _TConstPointer,
    _TReference,
    _TConstReference
    >::_check(UPredicate&& Predicate)
{
    check( Predicate() )
    return;
}
#endif /* LAL_DO_CHECKS */

} /* ~Namespace Lal::Private */
