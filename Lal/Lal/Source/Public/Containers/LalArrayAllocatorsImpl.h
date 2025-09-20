// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorStrongImpl(const TArrayBaseMutableDefaultAllocatorStrongImpl& Other) noexcept
{
    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Used > 0)
    {
        this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(Used);
        this->Slack = this->Data;

        T* RESTRICT Cursor { Other.Data };
        while (Cursor != Other.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        this->End = this->Slack;
        LAL_CHECK_ARRAY( this->End == this->Data + Used )
    }
    else
    {
        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>& TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::operator=(const TArrayBaseMutableDefaultAllocatorStrongImpl& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Used > 0)
    {
        this->Destruct();
        this->ResizeEmtpy(Used);

        LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

        Pointer Cursor { Other.Data };
        while (Cursor != Other.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorStrongImpl(TArrayBaseMutableDefaultAllocatorStrongImpl&& Other) noexcept
{
    this->Data = Other.Data;
    this->Slack = Other.Slack;
    this->End = Other.End;

    Other._ResetToDefaultState();

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>& TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::operator=(TArrayBaseMutableDefaultAllocatorStrongImpl&& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->OrphanImpl();

    this->Data = Other.Data;
    this->Slack = Other.Slack;
    this->End = Other.End;

    Other._ResetToDefaultState();

    return *this;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorStrongImpl(const UAllocator& Allocator) noexcept
{
    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(Used);
        this->Slack = this->Data;

        typename UAllocator::Pointer Cursor { Allocator.Data };
        while (Cursor != Allocator.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        this->End = this->Slack;
        LAL_CHECK_ARRAY( this->End == this->Data + Used )
    }
    else
    {
        this->_ResetToDefaultState();
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>& TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::operator=(const UAllocator& Allocator) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        this->Destruct();
        this->ResizeEmtpy(Used);

        LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

        typename UAllocator::Pointer Cursor { Allocator.Data };
        while (Cursor != Allocator.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorStrongImpl(UAllocator&& Allocator) noexcept
{
    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
        {
            static_assert
            (
                /* Only required for heap-based allocators. */
                requires(UAllocator Allocator)
                {
                    { Allocator.End };
                }
            );

            this->Data = Allocator.Data;
            this->Slack = Allocator.Slack;
            this->End = Allocator.End;
        }
        else
        {
            this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(Used);

            if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
            {
                this->Slack = this->Data + Used;
                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"
            }
            else
            {
                this->Slack = this->Data;

                for (T* RESTRICT Cursor { Allocator.Data }; Cursor != Allocator.Slack; ++Cursor)
                {
                    std::construct_at(this->Slack++, std::move(*Cursor));
                    std::destroy_at(Cursor);

                    continue;
                }

                LAL_CHECK_ARRAY( this->Data + Used == this->Slack )
            }

            this->End = this->Slack;
        }

        Allocator._ResetToDefaultState();
        LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
    }
    else
    {
        this->_ResetToDefaultState();
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>& TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::operator=(UAllocator&& Allocator) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
        {
            this->OrphanImpl();

            static_assert
            (
                /* Only required for heap-based allocators. */
                requires(UAllocator Allocator)
                {
                    { Allocator.End };
                }
            );

            this->Data = Allocator.Data;
            this->Slack = Allocator.Slack;
            this->End = Allocator.End;
        }
        else
        {
            this->Destruct();
            this->ResizeEmtpy(Used);

            if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
            {
                this->Slack = this->Data + Used;
                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"
            }
            else
            {
                for (T* RESTRICT Cursor { Allocator.Data }; Cursor != Allocator.Slack; ++Cursor)
                {
                    std::construct_at(this->Slack++, std::move(*Cursor));
                    std::destroy_at(Cursor);

                    continue;
                }

                LAL_CHECK_ARRAY( this->Data + Used == this->Slack )
            }

            LAL_CHECK_ARRAY( this->Slack <= this->End )
        }

        Allocator._ResetToDefaultState();
        LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
template <typename U>
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorStrongImpl(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, const U&>)
{
    if (LAL_LIKELY(List.size() > 0))
    {
        this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(List.size());
        this->Slack = this->Data;

        for (const U& Element : List)
        {
            std::construct_at(this->Slack++, Element);
        }

        this->End = this->Slack;
        LAL_CHECK_ARRAY( this->End == this->Data + List.size() )
    }
    else
    {
        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
template <typename U>
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>& TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::operator=(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, const U&>)
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&List) )

    if (LAL_LIKELY(List.size() > 0))
    {
        this->Destruct();
        this->ResizeEmtpy(List.size());

        LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

        for (const U& Element : List)
        {
            std::construct_at(this->Slack++, Element);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorStrongImpl(UIterator Begin, const VIterator End) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, const typename TIteratorTraits<UIterator>::value_type&>)
{
    LAL_CHECK_ARRAY( Begin <= End )

    const SizeType Count { static_cast<SizeType>(End - Begin) };

    if (Count > 0)
    {
        this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(Count);
        this->Slack = this->Data;

        while (Begin != End)
        {
            std::construct_at(this->Slack++, *Begin++);
        }

        this->End = this->Slack;
        LAL_CHECK_ARRAY( this->End == this->Data + Count )
    }
    else
    {
        this->_ResetToDefaultState();
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::Grow() noexcept
{
    const SizeType NewCapacity { Traits::GetGrowSize(this->End - this->Data) };
    LAL_CHECK_ARRAY( NewCapacity > 0ull && NewCapacity >= static_cast<SizeType>(this->End - this->Data) )

    this->GrowImpl(NewCapacity);

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::GrowTo(const SizeType Count) noexcept
{
    if (Count > static_cast<SizeType>(this->End - this->Data))
    {
        this->GrowImpl(Count);
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE typename TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::SizeType
TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::Resize(const SizeType Count) noexcept
{
    if
    (
        const SizeType Diff { static_cast<SizeType>(this->Slack - this->Data) };
        Count <= Diff
    )
    {
        this->ShrinkToImpl(Diff);
    }
    else if (Count > static_cast<SizeType>(this->End - this->Data))
    {
        this->GrowImpl(Count);
    }
    else
    {
        this->ShrinkToImpl(Count);
    }

    return this->End - this->Data;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::SwapBuffers(TArrayBaseMutableDefaultAllocatorStrongImpl* Other) noexcept
{
    LAL_CHECK_ARRAY( this != Other )

    Pointer TempData  { this->Data };
    Pointer TempSlack { this->Slack };
    Pointer TempEnd   { this->End };

    this->Data  = Other->Data;
    this->Slack = Other->Slack;
    this->End   = Other->End;

    Other->Data  = TempData;
    Other->Slack = TempSlack;
    Other->End   = TempEnd;

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE bool TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::Drop() noexcept
{
    if (this->Data == this->Slack)
    {
        return false;
    }

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        std::destroy_at(this->Data);
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Data, this->Data + 1, (this->Slack - this->Data - 1) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        std::move(this->Data + 1, this->Slack, this->Data);
        std::destroy_at(this->Slack - 1);
    }

    --this->Slack;

    return true;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE typename TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::SizeType
TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::Drop(const SizeType Count) noexcept
{
    const SizeType Dropped { Maths::Min(Count, static_cast<SizeType>(this->Slack - this->Data)) };

    std::destroy_n(this->Data, Dropped);

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Data, this->Data + Dropped, (this->Slack - this->Data - Dropped) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        SizeType _Count { 0 };
        for (T* /* RESTRICT */ Bulk { this->Data + Dropped }; Bulk != this->Slack; ++Bulk)
        {
            std::construct_at(this->Data + _Count++, std::move(*Bulk));
            std::destroy_at(Bulk);

            continue;
        }
    }

    this->Slack -= Dropped;

    return Dropped;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::GrowImpl(const SizeType Count) noexcept
{
    if (this->Data)
    {
        this->AlignedRealloc(Count);
    }
    else
    {
        this->Data = TArrayBaseMutableDefaultAllocatorStrongImpl::AlignedAlloc(Count);
        this->Slack = this->Data;
        this->End = this->Data + Count;
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::ShrinkToImpl(const SizeType Count) noexcept
{
    if (Count == 0)
    {
        this->Orphan();
    }
    else if (Count < static_cast<SizeType>(this->End - this->Data))
    {
        this->AlignedRealloc(Count);
    }
    else
    {
        LAL_CHECK_ARRAY( Count == static_cast<SizeType>(this->End - this->Data) )
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::ResizeEmtpy(const SizeType Count) noexcept
{
    LAL_CHECK_ARRAY( this->Slack == this->Data )

    if (Count > static_cast<SizeType>(this->End - this->Data))
    {
        this->GrowImpl(Count);
    }
    else
    {
        this->ShrinkToImpl(Count);
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE typename TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::Pointer
TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::AlignedAlloc(const SizeType Count) noexcept
{
    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
#if LAL_CHECK_CONTAINER_BOUNDS
    Pointer Out { static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T))) };
    jassert( Out )
    return Out;
#else /* LAL_CHECK_CONTAINER_BOUNDS */
    return static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T)));
#endif /* !LAL_CHECK_CONTAINER_BOUNDS */
    #include "Definitions/PopDiagnostics.h"
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultAllocatorStrongImpl<TIn, TTraits>::AlignedRealloc(const SizeType Count) noexcept
{
    LAL_CHECK_ARRAY( Count > 0 && Count >= static_cast<SizeType>(this->Slack - this->Data) )

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    Pointer NewData { static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T))) };
    #include "Definitions/PopDiagnostics.h"
    LAL_CHECK_ARRAY( NewData && this->Data != NewData )

    const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memcpy(NewData, this->Data, Used * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        std::uninitialized_move(this->Data, this->Slack, NewData);
        std::destroy_n(this->Data, Used);
    }

    std::free(this->Data);

    this->Data = NewData;
    this->Slack = this->Data + Used;
    this->End = this->Data + Count;

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
FORCEINLINE constexpr bool TArrayBaseMutableDefaultAllocatorWeakImpl<TIn, TTraits>::IsCurrentDataOnHeap() const noexcept
{
    checkNoEntry()
    return false;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorWeakImpl(const UIterator Begin, const VIterator End) noexcept
{
    static_assert(std::is_base_of_v<std::contiguous_iterator_tag, typename TIteratorTraits<UIterator>::iterator_category>);

    this->Data = std::to_address(Begin);
    this->Slack = std::to_address(End);

    LAL_CHECK_ARRAY( this->Data <= this->Slack )

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorWeakImpl(const Pointer Begin, const Pointer End) noexcept
{
    LAL_CHECK_ARRAY( Begin <= End )

    this->Data = Begin;
    this->Slack = End;

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
FORCEINLINE constexpr bool TArrayBaseMutableDefaultAllocatorWeakImpl<TIn, TTraits>::Drop() noexcept
{
    if (this->Data == this->Slack)
    {
        return false;
    }

    ++this->Data;
    return true;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
FORCEINLINE constexpr typename TArrayBaseMutableDefaultAllocatorWeakImpl<TIn, TTraits>::SizeType
TArrayBaseMutableDefaultAllocatorWeakImpl<TIn, TTraits>::Drop(const SizeType Count) noexcept
{
    if (Count == 0 || this->Data == this->Slack)
    {
        return 0;
    }

    const SizeType Dropped { Maths::Min(Count, static_cast<SizeType>(this->Slack - this->Data)) };
    this->Data += Dropped;

    return Dropped;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
FORCEINLINE constexpr bool TArrayBaseConstDefaultAllocatorWeakImpl<TIn, TTraits>::IsCurrentDataOnHeap() const noexcept
{
    checkNoEntry()
    return false;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl<TIn, TTraits>::
TArrayBaseConstDefaultAllocatorWeakImpl(const UIterator Begin, const VIterator End) noexcept
{
    static_assert(std::is_base_of_v<std::contiguous_iterator_tag, typename TIteratorTraits<UIterator>::iterator_category>);

    this->Data = std::to_address(Begin);
    this->Slack = std::to_address(End);

    LAL_CHECK_ARRAY( this->Data <= this->Slack )

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl<TIn, TTraits>::TArrayBaseConstDefaultAllocatorWeakImpl(const Pointer Begin, const Pointer End) noexcept
{
    LAL_CHECK_ARRAY( Begin <= End )

    this->Data = Begin;
    this->Slack = End;

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
FORCEINLINE constexpr bool TArrayBaseConstDefaultAllocatorWeakImpl<TIn, TTraits>::Drop() noexcept
{
    if (this->Data == this->Slack)
    {
        return false;
    }

    ++this->Data;
    return true;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
FORCEINLINE constexpr typename TArrayBaseConstDefaultAllocatorWeakImpl<TIn, TTraits>::SizeType
TArrayBaseConstDefaultAllocatorWeakImpl<TIn, TTraits>::Drop(const SizeType Count) noexcept
{
    if (Count == 0 || this->Data == this->Slack)
    {
        return 0;
    }

    const SizeType Dropped { Maths::Min(Count, static_cast<SizeType>(this->Slack - this->Data)) };
    this->Data += Dropped;

    return Dropped;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultFixedAllocatorWeakImpl(const TArrayBaseMutableDefaultFixedAllocatorWeakImpl& Other) noexcept
{
    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Used > 0)
    {
        this->AllocateNoCheck();

        Pointer Cursor { Other.Data };
        while (Cursor != Other.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )
    }
    else
    {
        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>&
TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::operator=(const TArrayBaseMutableDefaultFixedAllocatorWeakImpl& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Used > 0)
    {
        if (this->Data == nullptr)
        {
            this->Allocate();
        }
        else
        {
            this->Destruct();
        }

        LAL_CHECK_ARRAY( this->Data == this->Slack )

        Pointer Cursor { Other.Data };
        while (Cursor != Other.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultFixedAllocatorWeakImpl(TArrayBaseMutableDefaultFixedAllocatorWeakImpl&& Other) noexcept
{
    this->Data = Other.Data;
    this->Slack = Other.Slack;
    this->End = Other.End;

    Other._ResetToDefaultState();

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>&
TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::operator=(TArrayBaseMutableDefaultFixedAllocatorWeakImpl&& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->OrphanImpl();

    this->Data = Other.Data;
    this->Slack = Other.Slack;
    this->End = Other.End;

    Other._ResetToDefaultState();

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; } && std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultFixedAllocatorWeakImpl(const UAllocator& Allocator) noexcept
{
    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        this->AllocateNoCheck();

        typename UAllocator::Pointer Cursor { Allocator.Data };
        while (Cursor != Allocator.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
    }
    else
    {
        this->_ResetToDefaultState();
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; } && std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>&
TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::operator=(const UAllocator& Allocator) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        if (this->Data == nullptr)
        {
            this->Allocate();
        }
        else
        {
            this->Destruct();
        }

        LAL_CHECK_ARRAY( this->Data == this->Slack )

        typename UAllocator::Pointer Cursor { Allocator.Data };
        while (Cursor != Allocator.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultFixedAllocatorWeakImpl(UAllocator&& Allocator) noexcept
{
    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        /* Assume that the using developer is not a JavaScript coder... */
        if (LAL_UNLIKELY(Used > _Super::SizeCapacity))
        {
            PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                "Array overflow: Tried to allocate more elements than the array can hold; Capacity [{}], Allocated [{}].",
                _Super::SizeCapacity, Used
            )
        }

        if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
        {
            static_assert
            (
                /* Only required for heap-based allocators. */
                requires(UAllocator Allocator)
                {
                    { Allocator.End };
                }
            );

            this->Data = Allocator.Data;
            this->Slack = Allocator.Slack;
            this->End = Allocator.End;
        }
        else
        {
            this->AllocateNoCheck();

            if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
            {
                this->Slack = this->Data + Used;
                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"
            }
            else
            {
                for (T* RESTRICT Cursor { Allocator.Data }; Cursor != Allocator.Slack; ++Cursor)
                {
                    std::construct_at(this->Slack++, std::move(*Cursor));
                    std::destroy_at(Cursor);

                    continue;
                }

                LAL_CHECK_ARRAY( this->Data + Used == this->Slack )
            }

            LAL_CHECK_ARRAY( this->Slack <= this->End )
        }

        Allocator._ResetToDefaultState();
        LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
    }
    else
    {
        this->_ResetToDefaultState();
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>&
TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::operator=(UAllocator&& Allocator) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        if (Used > _Super::SizeCapacity)
        {
            PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                "Array overflow: Tried to allocate more elements than the array can hold; Capacity [{}], Allocated [{}].",
                _Super::SizeCapacity, Used
            )
        }

        if (this->Data == nullptr)
        {
            this->Allocate();
        }
        else
        {
            this->Destruct();
        }

        if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
        {
            this->Slack = this->Data + Used;
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }
        else
        {
            for (T* RESTRICT Cursor { Allocator.Data }; Cursor != Allocator.Slack; ++Cursor)
            {
                std::construct_at(this->Slack++, std::move(*Cursor));
                std::destroy_at(Cursor);

                continue;
            }

            LAL_CHECK_ARRAY( this->Data + Used == this->Slack )
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )

        if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
        {
            static_assert
            (
                /* Only required for heap-based allocators. */
                requires(UAllocator Allocator)
                {
                    { Allocator.Orphan() };
                }
            );

            Allocator.Orphan();
        }
        else
        {
            Allocator._ResetToDefaultState();
        }

        LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <typename U>
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultFixedAllocatorWeakImpl(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, const U&>)
{
    if (LAL_LIKELY(List.size() > 0))
    {
        this->AllocateNoCheck();

        for (const U& Element : List)
        {
            /* Assume that the using developer is not a vibe coder... */
            if (LAL_UNLIKELY(this->Slack >= this->End))
            {
                const SizeType ListSize { static_cast<SizeType>(List.size()) };
                PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                    "Array overflow: Tried to add more elements than the array can hold; Capacity [{}], std::initializer_list [{}].",
                    _Super::SizeCapacity, ListSize
                )
            }

            std::construct_at(this->Slack++, Element);

            continue;
        }
    }
    else
    {
        this->_ResetToDefaultState();
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <typename U>
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>&
TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::operator=(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, const U&>)
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&List) )

    if (LAL_LIKELY(List.size() > 0))
    {
        if (this->Data == nullptr)
        {
            this->Allocate();
        }
        else
        {
            this->Destruct();
        }

        for (const U& Element : List)
        {
            /* Assume that the using developer is not a vibe coder... */
            if (LAL_UNLIKELY(this->Slack >= this->End))
            {
                const SizeType ListSize { static_cast<SizeType>(List.size()) };
                PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                                           "Array overflow: Tried to add more elements than the array can hold; Capacity [{}], std::initializer_list [{}].",
                                           _Super::SizeCapacity, ListSize
                )

                break;
            }

            std::construct_at(this->Slack++, Element);

            continue;
        }
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultFixedAllocatorWeakImpl(UIterator Begin, const VIterator End) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, const typename TIteratorTraits<UIterator>::value_type&>)
{
    LAL_CHECK_ARRAY( Begin <= End )

    const SizeType Used { static_cast<SizeType>(End - Begin) };

    if (Used > 0)
    {
        /* Assume that the using developer is using LLMs to code. */
        if (LAL_UNLIKELY(Used > _Super::SizeCapacity))
        {
            PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                "Array overflow: Tried to allocate more elements than the array can hold; Capacity [{}], Allocated [{}].",
                _Super::SizeCapacity, Used
            )
        }

        this->AllocateNoCheck();

        while (Begin != End)
        {
            std::construct_at(this->Slack++, *Begin++);
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )
    }
    else
    {
        this->_ResetToDefaultState();
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE void TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::Grow() noexcept
{
    /* Assume that the using developer is not a vibe coder... */
    if (LAL_LIKELY(this->Data == nullptr))
    {
        this->Allocate();
    }
    else
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Array overflow: Tried to grow a fixed array that is already allocated; Capacity [{}].", _Super::SizeCapacity)
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE void TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::Orphan() noexcept
{
    this->OrphanImpl();
    this->_ResetToDefaultState();

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE bool TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::Drop() noexcept
{
    if (this->Data == this->Slack)
    {
        return false;
    }

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        std::destroy_at(this->Data);

        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Data, this->Data + 1, (this->Slack - this->Data - 1) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        std::move(this->Data + 1, this->Slack, this->Data);
        std::destroy_at(this->Slack - 1);
    }

    --this->Slack;

    return true;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE typename TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::SizeType
TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::Drop(const SizeType Count) noexcept
{
    const SizeType Dropped { Maths::Min(Count, static_cast<SizeType>(this->Slack - this->Data)) };

    std::destroy_n(this->Data, Dropped);

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Data, this->Data + Dropped, (this->Slack - this->Data - Dropped) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        SizeType _Count { 0 };
        for (T* /* RESTRICT */ Bulk { this->Data + Dropped }; Bulk != this->Slack; ++Bulk)
        {
            std::construct_at(this->Data + _Count++, std::move(*Bulk));
            std::destroy_at(Bulk);

            continue;
        }
    }

    this->Slack -= Dropped;

    return Dropped;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE void TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::Allocate() noexcept
{
    LAL_CHECK_ARRAY( this->Data == nullptr && this->Slack == nullptr && this->End == nullptr )
    this->AllocateNoCheck();

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE void TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::AllocateNoCheck() noexcept
{
    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    this->Data = static_cast<Pointer>(std::aligned_alloc(alignof(T), _Super::SizeCapacity * sizeof(T)));
    #include "Definitions/PopDiagnostics.h"

    this->Slack = this->Data;
    this->End = this->Data + _Super::SizeCapacity;

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE void TArrayBaseMutableDefaultFixedAllocatorWeakImpl<TIn, TTraits>::OrphanImpl() noexcept
{
    this->Destruct();
    std::free(this->Data);

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackAllocatorWeakImpl(const TArrayBaseMutableDefaultStackAllocatorWeakImpl& Other) noexcept
    : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
{
    Pointer Cursor { Other.Data };
    while (Cursor != Other.Slack)
    {
        std::construct_at(this->Slack++, *Cursor++);
    }

    LAL_CHECK_ARRAY( this->Slack <= this->End )

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>&
TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::operator=(const TArrayBaseMutableDefaultStackAllocatorWeakImpl& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->Destruct();

    Pointer Cursor { Other.Data };
    while (Cursor != Other.Slack)
    {
        std::construct_at(this->Slack++, *Cursor++);
    }

    LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackAllocatorWeakImpl(TArrayBaseMutableDefaultStackAllocatorWeakImpl&& Other) noexcept
    : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
{
    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Used > 0)
    {
        if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
        {
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, Other.Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }
        else
        {
            std::uninitialized_move(Other.Data, Other.Slack, this->Data);
            std::destroy_n(Other.Data, Used);
        }

        this->Slack = this->Data + Used;
        LAL_CHECK_ARRAY( this->Slack <= this->End )

        Other.Slack = Other.Data;
        LAL_CHECK_ARRAY( static_cast<const void*>(&Other) == static_cast<const void*>(Other.Slack) )
    }

    LAL_CHECK_ARRAY( this->Slack <= this->End )

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>&
TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::operator=(TArrayBaseMutableDefaultStackAllocatorWeakImpl&& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->Destruct();

    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Used > 0)
    {
        if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
        {
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, Other.Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }
        else
        {
            std::uninitialized_move(Other.Data, Other.Slack, this->Data);
            std::destroy_n(Other.Data, Used);
        }

        this->Slack = this->Data + Used;
        LAL_CHECK_ARRAY( this->Slack <= this->End )

        Other.Slack = Other.Data;
    }

    LAL_CHECK_ARRAY( this->Slack <= this->End )

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
       requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, typename UAllocator::T>; }
    && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
    )
FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::
TArrayBaseMutableDefaultStackAllocatorWeakImpl(const UAllocator& Allocator) noexcept
    : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > _Super::SizeCapacity)
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
            "Array overflow: Stack allocated array has reached its capacity; Capacity [{}], Allocated [{}].",
            _Super::SizeCapacity, Used
        )
    }

    typename UAllocator::Pointer Cursor { Allocator.Data };
    while (Cursor != Allocator.Slack)
    {
        std::construct_at(this->Slack++, *Cursor++);
    }

    LAL_CHECK_ARRAY( this->Slack <= this->End )

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>&
TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::operator=(const UAllocator& Allocator) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

    this->Destruct();

    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > _Super::SizeCapacity)
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
            "Array overflow: Stack allocated array has reached its capacity; Capacity [{}], Allocated [{}].",
            _Super::SizeCapacity, Used
        )
    }

    typename UAllocator::Pointer Cursor { Allocator.Data };
    while (Cursor != Allocator.Slack)
    {
        std::construct_at(this->Slack++, *Cursor++);
    }

    LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackAllocatorWeakImpl(UAllocator&& Allocator) noexcept
    : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (LAL_UNLIKELY(Used > _Super::SizeCapacity))
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
            "Array overflow: Stack allocated array has reached its capacity; Capacity [{}], Allocated [{}].",
            _Super::SizeCapacity, Used
        )
    }

    if (Used > 0)
    {
        if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
        {
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }
        else
        {
            std::uninitialized_move(Allocator.Data, Allocator.Slack, this->Data);
            std::destroy_n(Allocator.Data, Used);
        }

        Allocator.Slack = Allocator.Data;

        if constexpr (requires(UAllocator Allocator) { Allocator.Orphan(); })
        {
            Allocator.Orphan();
        }
        else
        {
            static_assert
            (
                requires(UAllocator Allocator)
                {
                    { Allocator._ResetToDefaultState() };
                }
            );
            Allocator.Empty();
        }

        this->Slack += Used;
        LAL_CHECK_ARRAY( this->Slack <= this->End )
    }

    LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <typename U>
FORCEINLINE TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackAllocatorWeakImpl(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, const U&>)
    : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
{
    LAL_CHECK_ARRAY( this->Data != nullptr )

    for (const U& Element : List)
    {
        /* Assume that the using developer has a brain... */
        if (LAL_UNLIKELY(this->Slack >= this->End))
        {
            const SizeType ListSize { static_cast<SizeType>(List.size()) };
            PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                "Array overflow: Tried to add more elements than the array can hold: Capacity [{}], std::initializer_list [{}].",
                _Super::SizeCapacity, ListSize
            )

            break;
        }

        std::construct_at(this->Slack++, Element);

        continue;
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <typename U>
FORCEINLINE TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>&
TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::operator=(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, const U&>)
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&List) )

    this->Destruct();

    for (const U& Element : List)
    {
        /* Assume that the using developer has a brain... */
        if (LAL_UNLIKELY(this->Slack >= this->End))
        {
            const SizeType ListSize { static_cast<SizeType>(List.size()) };
            PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
                "Array overflow: Tried to add more elements than the array can hold: Capacity [{}], std::initializer_list [{}].",
                _Super::SizeCapacity, ListSize
            )

            break;
        }

        std::construct_at(this->Slack++, Element);

        continue;
    }

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackAllocatorWeakImpl(UIterator Begin, const VIterator End) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::T, const typename TIteratorTraits<UIterator>::value_type&>)
    : TArrayBaseMutableDefaultStackAllocatorWeakImpl{}
{
    LAL_CHECK_ARRAY( Begin <= End )

    const SizeType Used { static_cast<SizeType>(End - Begin) };

    if (Used > _Super::SizeCapacity)
    {
        PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel,
            "Array overflow: Stack allocated array has reached its capacity; Capacity [{}], Allocated [{}].",
            _Super::SizeCapacity, Used
        )
    }

    while (Begin != End)
    {
        std::construct_at(this->Slack++, *Begin++);
    }

    LAL_CHECK_ARRAY( this->Slack <= this->End )

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE constexpr void TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::Empty() noexcept
{
    this->Destruct();
    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE bool TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::Drop() noexcept
{
    if (this->Data == this->Slack)
    {
        return false;
    }

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        std::destroy_at(this->Data);
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Data, this->Data + 1, (this->Slack - this->Data - 1) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        std::move(this->Data + 1, this->Slack, this->Data);
        std::destroy_at(this->Slack - 1);
    }

    --this->Slack;

    return true;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
FORCEINLINE typename TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::SizeType
TArrayBaseMutableDefaultStackAllocatorWeakImpl<TIn, TTraits>::Drop(const SizeType Count) noexcept
{
    const SizeType Dropped { Maths::Min(Count, static_cast<SizeType>(this->Slack - this->Data)) };

    std::destroy_n(this->Data, Dropped);

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Data, this->Data + Dropped, (this->Slack - this->Data - Dropped) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        SizeType _Count { 0 };
        for (T* /* RESTRICT */ Bulk { this->Data + Dropped }; Bulk != this->Slack; ++Bulk)
        {
            std::construct_at(this->Data + _Count++, std::move(*Bulk));
            std::destroy_at(Bulk);

            continue;
        }
    }

    this->Slack -= Dropped;

    return Dropped;
}


template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr bool TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::IsCurrentDataOnHeap() const noexcept
{
    LAL_CHECK_ARRAY( this->Data != nullptr )
    return this->Data != reinterpret_cast<const T*>(this->_Data);
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(const TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& Other) noexcept
{
    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Other.IsCurrentDataOnHeap())
    {
        LAL_CHECK_ARRAY( Used > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )

        this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(Used);
        this->Slack = this->Data;
        this->End = this->Data + Used;
    }
    else
    {
        this->_ResetToDefaultState();
    }

    Pointer Cursor { Other.Data };
    while (Cursor != Other.Slack)
    {
        std::construct_at(this->Slack++, *Cursor++);
    }

    LAL_CHECK_ARRAY( this->Slack <= this->End )

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>&
TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::operator=(const TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Used > 0)
    {
        this->Destruct();
        this->ResizeEmtpy(Used);

        LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

        Pointer Cursor { Other.Data };
        while (Cursor != Other.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl&& Other) noexcept
{
    if (Other.IsCurrentDataOnHeap())
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;
        this->End = Other.End;
    }
    else
    {
        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        std::memcpy(this->_Data, Other._Data, Used * sizeof(T));

        this->Data = reinterpret_cast<Pointer>(this->_Data);
        this->Slack = this->Data + Used;
        this->End = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;
    }

    Other._ResetToDefaultState();

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>&
TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::operator=(TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl&& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->DestructImpl();
    if (this->IsCurrentDataOnHeap())
    {
        std::free(this->Data);
    }

    if (Other.IsCurrentDataOnHeap())
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;
        this->End = Other.End;
    }
    else
    {
        const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

        std::memcpy(this->_Data, Other._Data, Used * sizeof(T));

        this->Data = reinterpret_cast<Pointer>(this->_Data);
        this->Slack = this->Data + Used;
        this->End = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;
    }

    Other._ResetToDefaultState();

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(const UAllocator& Allocator) noexcept
{
    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        if (Used > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
        {
            this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(Used);
            this->Slack = this->Data;

            typename UAllocator::Pointer Cursor { Allocator.Data };
            while (Cursor != Allocator.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }

            this->End = this->Slack;
            LAL_CHECK_ARRAY( this->End == this->Data + Used )
        }
        else
        {
            this->_ResetToDefaultState();

            typename UAllocator::Pointer Cursor { Allocator.Data };
            while (Cursor != Allocator.Slack)
            {
                std::construct_at(this->Slack++, *Cursor++);
            }
        }
    }
    else
    {
        this->_ResetToDefaultState();
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>&
TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::operator=(const UAllocator& Allocator) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Allocator) )

    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        this->Destruct();
        this->ResizeEmtpy(Used);

        LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

        typename UAllocator::Pointer Cursor { Allocator.Data };
        while (Cursor != Allocator.Slack)
        {
            std::construct_at(this->Slack++, *Cursor++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
template <TArrayBaseAllocatorConceptBase UAllocator>
    requires(
           requires { typename UAllocator::T; typename UAllocator::Pointer; requires std::constructible_from<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, typename UAllocator::T>; }
        && requires(UAllocator Allocator) { { Allocator.Data }; { Allocator.Slack }; }
        )
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(UAllocator&& Allocator) noexcept
{
    const SizeType Used { static_cast<SizeType>(Allocator.Slack - Allocator.Data) };

    if (Used > 0)
    {
        if (Used > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
        {
            if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
            {
                static_assert
                (
                    /* Only required for heap-based allocators. */
                    requires(UAllocator Allocator)
                    {
                        { Allocator.End };
                    }
                );

                this->Data = Allocator.Data;
                this->Slack = Allocator.Slack;
                this->End = Allocator.End;
            }
            else
            {
                this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(Used);

                if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
                {
                    this->Slack = this->Data + Used;
                    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                    std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                    #include "Definitions/PopDiagnostics.h"
                }
                else
                {
                    this->Slack = this->Data;

                    for (T* RESTRICT Cursor { Allocator.Data }; Cursor != Allocator.Slack; ++Cursor)
                    {
                        std::construct_at(this->Slack++, std::move(*Cursor));
                        std::destroy_at(Cursor);

                        continue;
                    }

                    LAL_CHECK_ARRAY( this->Data + Used == this->Slack )
                }

                this->End = this->Slack;
            }

            Allocator._ResetToDefaultState();
        }
        else
        {
            this->_ResetToDefaultState();

            if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
            {
                this->Slack = this->Data + Used;
                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->Data, Allocator.Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"
            }
            else
            {
                for (T* RESTRICT Cursor { Allocator.Data }; Cursor != Allocator.Slack; ++Cursor)
                {
                    std::construct_at(this->Slack++, std::move(*Cursor));
                    std::destroy_at(Cursor);

                    continue;
                }

                LAL_CHECK_ARRAY( this->Data + Used == this->Slack )
            }

            if (Private::IsCurrentDataOnHeapDefaultAllocatorImpl<UAllocator>(Allocator))
            {
                static_assert
                (
                    /* Only required for heap-based allocators. */
                    requires(UAllocator Allocator)
                    {
                        { Allocator.Orphan() };
                    }
                );

                Allocator.Slack = Allocator.Data;
                Allocator.Orphan();
            }
            else
            {
                Allocator._ResetToDefaultState();
            }
        }

        LAL_CHECK_ARRAY( Allocator.Data == Allocator.Slack )
    }
    else
    {
        this->_ResetToDefaultState();
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
template <typename U>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, const U&>)
{
    if (List.size() > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
    {
        this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(List.size());
        this->Slack = this->Data;

        for (const U& Element : List)
        {
            std::construct_at(this->Slack++, Element);
        }

        this->End = this->Slack;
        LAL_CHECK_ARRAY(this->End == this->Data + List.size())
    }
    else
    {
        this->_ResetToDefaultState();

        for (const U& Element : List)
        {
            std::construct_at(this->Slack++, Element);
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
template <typename U>
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>&
TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::operator=(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, const U&>)
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&List) )

    if (LAL_LIKELY(List.size() > 0))
    {
        if (List.size() > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
        {
            this->Destruct();
            this->Resize(List.size());
        }
        else
        {
            if (this->IsCurrentDataOnHeap())
            {
                this->OrphanImpl();
                this->_ResetToDefaultState();
            }
            else
            {
                this->Destruct();
                LAL_CHECK_ARRAY( this->Data == reinterpret_cast<Pointer>(this->_Data) )
                LAL_CHECK_ARRAY( this->Slack == this->Data )
                LAL_CHECK_ARRAY( this->End == this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )
            }
        }

        for (const U& Element : List)
        {
            std::construct_at(this->Slack++, Element);
        }

        LAL_CHECK_ARRAY( this->Slack <= this->End )
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(UIterator Begin, const VIterator End) noexcept
    requires(std::is_constructible_v<typename TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::T, const typename TIteratorTraits<UIterator>::value_type&>)
{
    LAL_CHECK_ARRAY( Begin <= End )

    const SizeType Used { static_cast<SizeType>(End - Begin) };

    if (Used > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
    {
        this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(Used);
        this->Slack = this->Data;

        while (Begin != End)
        {
            std::construct_at(this->Slack++, *Begin++);
        }

        this->End = this->Slack;
        LAL_CHECK_ARRAY( this->End == this->Data + Used )
    }
    else
    {
        this->_ResetToDefaultState();

        while (Begin != End)
        {
            std::construct_at(this->Slack++, *Begin++);
        }
    }

    LAL_CHECK_ARRAY( this->Slack <= this->End )

    return;
}


template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::~TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl() noexcept
{
    if (this->IsCurrentDataOnHeap())
    {
        this->OrphanImpl();
    }
    else
    {
        this->DestructImpl();
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::Grow() noexcept
{
    const SizeType NewCapacity { Traits::GetGrowSize(this->End - this->Data) };
    LAL_CHECK_ARRAY( NewCapacity > 0ull && NewCapacity >= static_cast<SizeType>(this->End - this->Data) )

    if (this->IsCurrentDataOnHeap())
    {
        this->AlignedRealloc(NewCapacity);
    }
    else
    {
        const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };

        this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(NewCapacity);

        if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
        {
            this->Slack = this->Data + Used;
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, this->_Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }
        else
        {
            this->Slack = this->Data;

            for (T* RESTRICT Cursor { this->_Data }; Cursor != this->_Data + Used; ++Cursor)
            {
                std::construct_at(this->Slack++, std::move(*Cursor));
                std::destroy_at(Cursor);

                continue;
            }

            LAL_CHECK_ARRAY( this->Data + Used == this->Slack )
        }

        this->End = this->Data + NewCapacity;
    }

    LAL_CHECK_ARRAY( this->Data )

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::GrowTo(const SizeType Count) noexcept
{
    if (Count > static_cast<SizeType>(this->End - this->Data))
    {
        this->GrowImpl(Count);
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE typename TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::SizeType
TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::Resize(const SizeType Count) noexcept
{
    if
    (
        const SizeType Diff { static_cast<SizeType>(this->Slack - this->Data) };
        Count <= Diff
    )
    {
        this->ShrinkToImpl(Diff);
    }
    else if (Count > static_cast<SizeType>(this->End - this->Data))
    {
        this->GrowImpl(Count);
    }
    else
    {
        this->ShrinkToImpl(Count);
    }

    return this->End - this->Data;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::ShrinkTo(const SizeType Count) noexcept
{
    if (Count <= static_cast<SizeType>(this->Slack - this->Data))
    {
        this->ShrinkToFit();
    }
    else if (Count < static_cast<SizeType>(this->End - this->Data))
    {
        this->ShrinkToImpl(Count);
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::Orphan() noexcept
{
    if (this->IsCurrentDataOnHeap())
    {
        this->OrphanImpl();

        this->Data = reinterpret_cast<Pointer>(this->_Data);
        this->Slack = this->Data;
        this->End = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;
    }
    else
    {
        this->Destruct();
        LAL_CHECK_ARRAY( this->Data == reinterpret_cast<Pointer>(this->_Data) )
        LAL_CHECK_ARRAY( this->Slack == this->Data )
        LAL_CHECK_ARRAY( this->End == this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE bool TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::Drop() noexcept
{
    if (this->Data == this->Slack)
    {
        return false;
    }

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        std::destroy_at(this->Data);
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Data, this->Data + 1, (this->Slack - this->Data - 1) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        std::move(this->Data + 1, this->Slack, this->Data);
        std::destroy_at(this->Slack - 1);
    }

    --this->Slack;

    return true;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE typename TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::SizeType
TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::Drop(const SizeType Count) noexcept
{
    const SizeType Dropped { Maths::Min(Count, static_cast<SizeType>(this->Slack - this->Data)) };

    std::destroy_n(this->Data, Dropped);

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Data, this->Data + Dropped, (this->Slack - this->Data - Dropped) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        SizeType _Count { 0 };
        for (T* /* RESTRICT */ Bulk { this->Data + Dropped }; Bulk != this->Slack; ++Bulk)
        {
            std::construct_at(this->Data + _Count++, std::move(*Bulk));
            std::destroy_at(Bulk);

            continue;
        }
    }

    this->Slack -= Dropped;

    return Dropped;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::GrowImpl(const SizeType Count) noexcept
{
    if (this->IsCurrentDataOnHeap())
    {
        this->AlignedRealloc(Count);
    }
    else
    {
        LAL_CHECK_ARRAY( Count > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )

        const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };

        this->Data = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::AlignedAlloc(Count);

        if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
        {
            this->Slack = this->Data + Used;
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Data, this->_Data, Used * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }
        else
        {
            this->Slack = this->Data;

            for (T* RESTRICT Cursor { this->_Data }; Cursor != this->_Data + Used; ++Cursor)
            {
                std::construct_at(this->Slack++, std::move(*Cursor));
                std::destroy_at(Cursor);

                continue;
            }

            LAL_CHECK_ARRAY( this->Data + Used == this->Slack )
        }

        this->End = this->Data + Count;
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::ShrinkToImpl(const SizeType Count) noexcept
{
    LAL_CHECK_ARRAY( Count >= static_cast<SizeType>(this->Slack - this->Data) )

    if (this->IsCurrentDataOnHeap() == false)
    {
        return;
    }

    if (Count == 0)
    {
        this->OrphanImpl();
        this->_ResetToDefaultState();
    }
    else if (Count < static_cast<SizeType>(this->End - this->Data))
    {
        if (Count > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity)
        {
            this->AlignedRealloc(Count);
        }
        else
        {
            const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };

            if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
            {
                #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
                std::memcpy(this->_Data, this->Data, Used * sizeof(T));
                #include "Definitions/PopDiagnostics.h"
            }
            else
            {
                SizeType _Count { 0 };
                for (T* RESTRICT Cursor { this->Data }; Cursor != this->Slack; ++Cursor)
                {
                    std::construct_at(this->_Data + Count++, std::move(*Cursor));
                    std::destroy_at(Cursor);

                    continue;
                }

                LAL_CHECK_ARRAY( _Count == Used )
            }

            std::free(this->Data);

            this->Data = reinterpret_cast<Pointer>(this->_Data);
            this->Slack = this->Data + Used;
            this->End = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;
        }
    }
    else
    {
        LAL_CHECK_ARRAY( Count == static_cast<SizeType>(this->End - this->Data) )
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::ResizeEmtpy(const SizeType Count) noexcept
{
    LAL_CHECK_ARRAY( this->Slack == this->Data )

    if (Count > static_cast<SizeType>(this->End - this->Data))
    {
        this->GrowImpl(Count);
    }
    else
    {
        this->ShrinkToImpl(Count);
    }

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE typename TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::Pointer
TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::AlignedAlloc(const SizeType Count) noexcept
{
    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
#if LAL_CHECK_CONTAINER_BOUNDS
    Pointer Out { static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T))) };
    jassert( Out )
    return Out;
#else /* LAL_CHECK_CONTAINER_BOUNDS */
    return static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T)));
#endif /* !LAL_CHECK_CONTAINER_BOUNDS */
    #include "Definitions/PopDiagnostics.h"
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::AlignedRealloc(const SizeType Count) noexcept
{
    LAL_CHECK_ARRAY( Count > 0 && Count >= static_cast<SizeType>(this->Slack - this->Data) )
    LAL_CHECK_ARRAY( Count > TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity )

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    Pointer NewData { static_cast<Pointer>(std::aligned_alloc(alignof(T), Count * sizeof(T))) };
    #include "Definitions/PopDiagnostics.h"

    LAL_CHECK_ARRAY( NewData )

    const SizeType Used { static_cast<SizeType>(this->Slack - this->Data) };

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memcpy(NewData, this->Data, Used * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        SizeType _Count { 0 };
        for (T* RESTRICT Bulk { this->Data }; Bulk != this->Slack; ++Bulk)
        {
            std::construct_at(NewData + _Count++, std::move(*Bulk));
            std::destroy_at(Bulk);

            continue;
        }

        LAL_CHECK_ARRAY( _Count == Used )
    }

    LAL_CHECK_ARRAY( this->IsCurrentDataOnHeap() )
    std::free(this->Data);

    this->Data = NewData;
    this->Slack = this->Data + Used;
    this->End = this->Data + Count;

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::OrphanImpl() noexcept
{
    LAL_CHECK_ARRAY( this->IsCurrentDataOnHeap() )

    this->Destruct();
    std::free(this->Data);

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::Destruct() noexcept
{
    this->DestructImpl();
    this->Slack = this->Data;

    return;
}

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr void TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<TIn, TTraits>::DestructImpl() noexcept
{
    for (T* RESTRICT Bulk { this->Data }; Bulk != this->Slack; ++Bulk)
    {
        Bulk->~T();
    }

    return;
}

} /* ~Namespace Lal */
