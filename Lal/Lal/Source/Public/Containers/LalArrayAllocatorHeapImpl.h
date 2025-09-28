// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::TArrayBaseDefaultAllocatorHeapImpl(TArrayBaseDefaultAllocatorHeapImpl const& Other) noexcept
{
    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Used > 0)
    {
        this->Data = TArrayBaseDefaultAllocatorHeapImpl::AlignedAlloc(Used);
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
FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>& TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::operator=(TArrayBaseDefaultAllocatorHeapImpl const& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<void const*>(this) != static_cast<void const*>(&Other) )

    const SizeType Used { static_cast<SizeType>(Other.Slack - Other.Data) };

    if (Used > 0)
    {
        this->Destruct();
        this->ResizeEmtpy(Used);

        LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

        T* RESTRICT Cursor { Other.Data };
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
FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::TArrayBaseDefaultAllocatorHeapImpl(TArrayBaseDefaultAllocatorHeapImpl&& Other) noexcept
{
    this->Data = Other.Data;
    this->Slack = Other.Slack;
    this->End = Other.End;

    Other.Data = nullptr;
    Other.Slack = nullptr;
    Other.End = nullptr;

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>&
TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::operator=(TArrayBaseDefaultAllocatorHeapImpl&& Other) noexcept
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->OrphanImpl();

    this->Data = Other.Data;
    this->Slack = Other.Slack;
    this->End = Other.End;

    Other.Data = nullptr;
    Other.Slack = nullptr;
    Other.End = nullptr;

    return *this;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::TArrayBaseDefaultAllocatorHeapImpl(CRange auto& Range) noexcept
    requires std::constructible_from<TIn, Ranges::Value_t<decltype(Range)>>
{
    auto Begin { std::ranges::begin(Range) };
    auto End { std::ranges::end(Range) };

    const SizeType Used { static_cast<SizeType>(std::distance(Begin, End)) };

    if (Used > 0)
    {
        this->Data = TArrayBaseDefaultAllocatorHeapImpl::AlignedAlloc(Used);
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
        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>&
TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::operator=(CRange auto& Range) noexcept
    requires requires{std::constructible_from<TIn, Ranges::Value_t<decltype(Range)>>;}
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Range) )

    auto Begin { std::ranges::begin(Range) };
    auto End { std::ranges::end(Range) };

    const SizeType Used { static_cast<SizeType>(std::distance(Begin, End)) };

    if (Used > 0)
    {
        this->Destruct();
        this->ResizeEmtpy(Used);

        LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

        while (Begin != End)
        {
            std::construct_at(this->Slack++, *Begin++);
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
FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::TArrayBaseDefaultAllocatorHeapImpl(CContainer auto&& Container) noexcept
    requires std::is_constructible_v<TIn, Ranges::Value_t<decltype(Container)>>
{
    auto Begin { std::make_move_iterator(std::ranges::begin(Container)) };
    auto End { std::make_move_iterator(std::ranges::end(Container)) };

    const SizeType Used { static_cast<SizeType>(std::distance(Begin, End)) };

    if (Used > 0)
    {
        this->Data = TArrayBaseDefaultAllocatorHeapImpl::AlignedAlloc(Used);
        this->Slack = this->Data;

        while (Begin != End)
        {
            std::construct_at(this->Slack++, *Begin++);
        }

        this->End = this->Slack;
        LAL_CHECK_ARRAY( this->End == this->Data + Used )

        Ranges::Empty(Container);
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
FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>&
TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::operator=(CContainer auto&& Container) noexcept
    requires std::is_constructible_v<TIn, Ranges::Value_t<decltype(Container)>>
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Container) )

    auto Begin { std::make_move_iterator(std::ranges::begin(Container)) };
    auto End { std::make_move_iterator(std::ranges::end(Container)) };

    const SizeType Used { static_cast<SizeType>(std::distance(Begin, End)) };

    if (Used > 0)
    {
        LAL_CHECK_ARRAY( std::to_address(Begin) < this->Data || std::to_address(Begin) >= this->End )

        this->Destruct();
        this->ResizeEmtpy(Used);

        LAL_CHECK_ARRAY( this->Slack == this->Data && this->End > this->Data )

        while (Begin != End)
        {
            std::construct_at(this->Slack++, *Begin++);
        }

        LAL_CHECK_ARRAY( this->Data && this->Slack <= this->End )

        Ranges::Empty(Container);
    }
    else
    {
        this->Orphan();
    }

    return *this;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::TArrayBaseDefaultAllocatorHeapImpl(ITERATOR Begin, ITERATOR End) noexcept
    requires std::is_constructible_v<TIn, typename TIteratorTraits<decltype(Begin)>::value_type>
{
    const SizeType Used { static_cast<SizeType>(std::distance(Begin, End)) };

    if (Used > 0)
    {
        this->Data = TArrayBaseDefaultAllocatorHeapImpl::AlignedAlloc(Used);
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
        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::Grow() noexcept
{
    const SizeType NewCapacity { Traits::GetGrowSize(this->End - this->Data) };
    LAL_CHECK_ARRAY( NewCapacity > 0ull && NewCapacity >= static_cast<SizeType>(this->End - this->Data) )

    this->GrowImpl(NewCapacity);

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::GrowTo(const SizeType Count) noexcept
{
    if (Count > static_cast<SizeType>(this->End - this->Data))
    {
        this->GrowImpl(Count);
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE typename TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::SizeType
TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::Resize(const SizeType Count) noexcept
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
FORCEINLINE void TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::SwapBuffers(TArrayBaseDefaultAllocatorHeapImpl* Other) noexcept
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
FORCEINLINE bool TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::Drop() noexcept
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
FORCEINLINE typename TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::SizeType
TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::Drop(const SizeType Count) noexcept
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
FORCEINLINE void TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::GrowImpl(const SizeType Count) noexcept
{
    if (this->Data)
    {
        this->AlignedRealloc(Count);
    }
    else
    {
        this->Data = TArrayBaseDefaultAllocatorHeapImpl::AlignedAlloc(Count);
        this->Slack = this->Data;
        this->End = this->Data + Count;
    }

    return;
}

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
FORCEINLINE void TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::ShrinkToImpl(const SizeType Count) noexcept
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
FORCEINLINE void TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::ResizeEmtpy(const SizeType Count) noexcept
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
FORCEINLINE typename TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::Pointer
TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::AlignedAlloc(const SizeType Count) noexcept
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
FORCEINLINE void TArrayBaseDefaultAllocatorHeapImpl<TIn, TTraits>::AlignedRealloc(const SizeType Count) noexcept
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

} /* ~Namespace Lal */
