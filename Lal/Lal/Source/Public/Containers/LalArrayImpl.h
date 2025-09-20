// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const TArrayBase& Other) noexcept
    requires(std::is_constructible_v<TAllocator, const TAllocator&>)
    : Impl{Other.Impl}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(const TArrayBase& Other) noexcept
    requires(std::assignable_from<TAllocator&, const TAllocator&>)
{
    this->Impl = Other.Impl;
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(TArrayBase&& Other) noexcept
    requires(std::is_constructible_v<TAllocator, TAllocator&&>)
    : Impl{std::move(Other.Impl)}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(TArrayBase&& Other) noexcept
    requires(std::assignable_from<TAllocator&, TAllocator&&>)
{
    this->Impl = std::move(Other.Impl);
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const TArrayBase<UAllocator>& Other) noexcept
    requires(std::is_constructible_v<TAllocator, const UAllocator&>)
    : Impl{Other.Impl}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(const TArrayBase<UAllocator>& Other) noexcept
    requires(std::assignable_from<TAllocator&, const UAllocator&>)
{
    this->Impl = Other.Impl;
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(TArrayBase<UAllocator>&& Other) noexcept
    requires(std::is_constructible_v<TAllocator, UAllocator&&>)
    : Impl{std::move(Other.Impl)}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(TArrayBase<UAllocator>&& Other) noexcept
    requires(std::assignable_from<TAllocator&, UAllocator&&>)
{
    this->Impl = std::move(Other.Impl);
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const std::initializer_list<T> List) noexcept
    requires(std::is_constructible_v<Allocator, std::initializer_list<typename Allocator::T>>)
    : Impl{List}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const std::initializer_list<U> List) noexcept
    requires(std::is_constructible_v<Allocator, std::initializer_list<U>>)
    : Impl{List}
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
FORCEINLINE constexpr TArrayBase<TAllocator>& TArrayBase<TAllocator>::operator=(const std::initializer_list<U> List) noexcept
    requires(std::assignable_from<TAllocator&, std::initializer_list<U>>)
{
    this->Impl = List;
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const UIterator Begin, const VIterator End) noexcept
    requires(std::is_constructible_v<TAllocator, UIterator, VIterator>)
    : Impl{ Begin, End }
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator>
FORCEINLINE constexpr TArrayBase<TAllocator>::TArrayBase(const UIterator Begin, const SizeType Length) noexcept
    requires(std::is_constructible_v<TAllocator, UIterator, UIterator>)
    : Impl{ Begin, Begin + Length }
{
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Reference TArrayBase<TAllocator>::operator[](const SizeType Index) noexcept
    requires(TArrayBase::IsContentMutable())
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )
    return this->Impl.Data[Index];
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::ConstReference TArrayBase<TAllocator>::operator[](const SizeType Index) const noexcept
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )
    return this->Impl.Data[Index];
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr auto TArrayBase<TAllocator>::Iter() noexcept
    requires(requires { typename Iterator::Factory; })
{
    return typename Iterator::Factory{ this->Impl.Data, this->Impl.Slack };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr auto TArrayBase<TAllocator>::CIter() const noexcept
    requires(requires { typename ConstIterator::Factory; })
{
    return typename ConstIterator::Factory{ this->Impl.Data, this->Impl.Slack };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr auto TArrayBase<TAllocator>::MIter() noexcept
    requires(TArrayBase::IsContentMutable() && requires { typename MoveIterator::Factory; })
{
    return typename MoveIterator::Factory{ this->Impl.Data, this->Impl.Slack };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::ToIndex(ITERATOR It) const noexcept
{
    check( this->IsValidIterator(It) )
    return static_cast<SizeType>(std::to_address(It) - this->GetDataPointer());
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::GetFirstChecked() noexcept requires(TArrayBase::IsContentMutable())
{
    check( this->GetSize() > 0 )
    return this->GetDataPointer();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::GetFirstAsserted() noexcept requires(TArrayBase::IsContentMutable())
{
    jassert( this->GetSize() > 0 )
    return this->GetDataPointer();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::GetFirstChecked() const noexcept
{
    check( this->GetSize() > 0 )
    return this->GetDataPointer();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::GetFirstAsserted() const noexcept
{
    jassert( this->GetSize() > 0 )
    return this->GetDataPointer();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::GetLastChecked() noexcept requires(TArrayBase::IsContentMutable())
{
    check( this->GetSize() > 0 )
    return this->GetSlackPointer() - 1;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::GetLastAsserted() noexcept requires(TArrayBase::IsContentMutable())
{
    jassert( this->GetSize() > 0 )
    return this->GetSlackPointer() - 1;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::GetLastChecked() const noexcept
{
    check( this->GetSize() > 0 )
    return this->GetSlackPointer() - 1;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::GetLastAsserted() const noexcept
{
    jassert( this->GetSize() > 0 )
    return this->GetSlackPointer() - 1;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Reserve(const SizeType Count) noexcept
    requires(TArrayBase::IsStronglyAllocated())
{
    this->GrowToImpl(Count);
    LAL_CHECK_ARRAY( Count == 0 || (this->Impl.Data && this->Impl.Slack <= this->Impl.End) )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Reset(const SizeType Count, const bool bAllowShrink /* = true */) noexcept
    requires(TArrayBase::IsStronglyAllocated())
{
    this->Destruct();

    if (LAL_LIKELY(bAllowShrink))
    {
        this->Resize(Count);
    }
    else
    {
        this->Reserve(Count);
    }

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::Resize(const SizeType Count) noexcept
    requires(TArrayBase::IsStronglyAllocated())
{
    return this->ResizeImpl(Count);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::ShrinkToFit() noexcept
    requires(TArrayBase::IsStronglyAllocated())
{
    this->ShrinkToFitImpl();
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Empty() noexcept
{
    this->EmptyImpl();
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE void TArrayBase<TAllocator>::SwapBuffers(TArrayBase* Other) noexcept
    requires(requires(Allocator _AllocatorA, Allocator _AllocatorB) { _AllocatorA.SwapBuffers(&_AllocatorB); })
{
    check( Other )

    this->Impl.SwapBuffers(&Other->Impl);
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE void TArrayBase<TAllocator>::SwapIndices(const UIterator InA, const VIterator InB) noexcept
    requires(TAllocator::IsContentMutable())
{
    LAL_CHECK_ARRAY( this->IsValidIterator(InA) && this->IsValidIterator(InB) )

    /* Assume the correct input. */
    if (LAL_UNLIKELY(InA == InB))
    {
        return;
    }

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        alignas(T)
        u8 Temp[sizeof(T)];

        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memcpy(Temp,                 std::to_address(InA), sizeof(T));
        std::memcpy(std::to_address(InA), std::to_address(InB), sizeof(T));
        std::memcpy(std::to_address(InB), Temp,                 sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        std::swap(*InA, *InB);
    }

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE void TArrayBase<TAllocator>::SwapIndices(const SizeType InA, const SizeType InB) noexcept
    requires(TAllocator::IsContentMutable())
{
    this->SwapIndices(this->begin() + InA, this->begin() + InB);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE constexpr bool TArrayBase<TAllocator>::IsDataEqual(const TArrayBase<UAllocator>& Other) const noexcept
{
    if (this->GetSize() != Other.GetSize())
    {
        return false;
    }

    for (const T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk != Other[Bulk - this->GetDataPointer()])
        {
            return false;
        }

        continue;
    }

    return true;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Add(const T& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    std::construct_at(this->Impl.Slack++, Element);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Add(T&& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    std::construct_at(this->Impl.Slack++, std::move(Element));

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::AddDefaulted() noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_default_constructible_v<typename TAllocator::T>)
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    std::construct_at(this->Impl.Slack++);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::AddDefaulted(const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_default_constructible_v<typename TAllocator::T>)
{
    if (LAL_UNLIKELY(Count == 0))
    {
        return;
    }

    this->Reserve(this->GetSize() + Count);

    for (SizeType Index { 0 }; Index < Count; ++Index)
    {
        std::construct_at(this->Impl.Slack++);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::AddZeroed() noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    std::memset(this->Impl.Slack++, 0, sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::AddZeroed(const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (LAL_UNLIKELY(Count == 0))
    {
        return;
    }

    this->Reserve(this->GetSize() + Count);

    #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
    std::memset(this->Impl.Slack, 0, Count * sizeof(T));
    #include "Definitions/PopDiagnostics.h"

    this->Impl.Slack += Count;
    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::AddUninitialized() noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    ++this->Impl.Slack;

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::AddUninitialized(const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (LAL_UNLIKELY(Count == 0))
    {
        return;
    }

    this->Reserve(this->GetSize() + Count);

    this->Impl.Slack += Count;
    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AddAt(const UIterator It, const T& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    return this->AddAt(std::to_address(It) - this->Impl.Data, Element);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AddAt(const SizeType Index, const T& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (Index == this->GetSize())
    {
        this->Add(Element);
        return --this->end();
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->AddUninitialized();

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Impl.Data + Index + 1, this->Impl.Data + Index, (this->GetSize() - Index - 1) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"

        std::construct_at(this->Impl.Data + Index, Element);
    }
    else
    {
        LAL_CHECK_ARRAY( this->Impl.Data <= this->Impl.Slack - 2 )
        LAL_CHECK_ARRAY( this->Impl.Data + Index <= this->Impl.Slack - 2 )

        std::construct_at(this->Impl.Slack - 1, std::move(*(this->Impl.Slack - 2)));
        std::move_backward
        (
            this->Impl.Data + Index,
            this->Impl.Data + this->GetSize() - /*From #AddUninitialized*/1 - /*last element*/1,
            this->Impl.Data + this->GetSize() - /*From #AddUninitialized*/1
        );

        *(this->Impl.Data + Index) = Element;
    }

    return Iterator{ this->Impl.Data + Index };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AddAt(const UIterator It, T&& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    return this->AddAt(std::to_address(It) - this->Impl.Data, std::move(Element));
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AddAt(const SizeType Index, T&& Element) noexcept
    requires(TArrayBase::IsAllowedToPushItems())
{
    if (Index == this->GetSize())
    {
        this->Add(std::move(Element));
        return --this->end();
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->AddUninitialized();

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Impl.Data + Index + 1, this->Impl.Data + Index, (this->GetSize() - Index - 1) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"

        std::construct_at(this->Impl.Data + Index, std::move(Element));
    }
    else
    {
        LAL_CHECK_ARRAY( this->Impl.Data <= this->Impl.Slack - 2 )
        LAL_CHECK_ARRAY( this->Impl.Data + Index <= this->Impl.Slack - 2 )

        std::construct_at(this->Impl.Slack - 1, std::move(*(this->Impl.Slack - 2)));
        std::move_backward
        (
            this->Impl.Data + Index,
            this->Impl.Data + this->GetSize() - /*From #AddUninitialized*/1 - /*last element*/1,
            this->Impl.Data + this->GetSize() - /*From #AddUninitialized*/1
        );

        *(this->Impl.Data + Index) = std::move(Element);
    }

    return Iterator{ this->Impl.Data + Index };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
FORCEINLINE void TArrayBase<TAllocator>::Emplace(TArgs&&... Args) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    if (this->HasReachedCapacity())
    {
        this->GrowImpl();
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    std::construct_at(this->Impl.Slack++, std::forward<TArgs>(Args)...);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
FORCEINLINE void TArrayBase<TAllocator>::EmplaceMinimalGrowth(TArgs&&... Args) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    if (this->HasReachedCapacity())
    {
        this->ReserveAdditionally(1);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack < this->Impl.End )
    std::construct_at(this->Impl.Slack++, std::forward<TArgs>(Args)...);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, typename... TArgs>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::EmplaceAt(const UIterator It, TArgs&&... Args) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    return this->EmplaceAt(std::to_address(It) - this->Impl.Data, std::forward<TArgs>(Args)...);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::EmplaceAt(const SizeType Index, TArgs&&... Args) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    if (Index == this->GetSize())
    {
        this->Emplace(std::forward<TArgs>(Args)...);
        return --this->end();
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->AddUninitialized();

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Impl.Data + Index + 1, this->Impl.Data + Index, (this->GetSize() - Index - 1) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
    }
    else
    {
        LAL_CHECK_ARRAY( this->Impl.Data <= this->Impl.Slack - 2 )
        LAL_CHECK_ARRAY( this->Impl.Data + Index <= this->Impl.Slack - 2 )

        std::construct_at(this->Impl.Slack - 1, std::move(*(this->Impl.Slack - 2)));
        std::move_backward
        (
            this->Impl.Data + Index,
            this->Impl.Data + this->GetSize() - /*From #AddUninitialized*/1 - /*last element*/1,
            this->Impl.Data + this->GetSize() - /*From #AddUninitialized*/1
        );

        std::destroy_at(this->Impl.Data + Index);
    }

    std::construct_at(this->Impl.Data + Index, std::forward<TArgs>(Args)...);

    return Iterator{ this->Impl.Data + Index };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE void TArrayBase<TAllocator>::Append(const TArrayBase<UAllocator>& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const typename UAllocator::T&>)
{
    this->Reserve(this->GetSize() + Other.GetSize());

    LAL_CHECK_ARRAY( this->Impl.Data )

    for (const typename UAllocator::T& Element : Other)
    {
        std::construct_at(this->Impl.Slack++, Element);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE void TArrayBase<TAllocator>::Append(TArrayBase<UAllocator>&& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T&&>)
{
    LAL_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    if (Other.GetSize() == 0)
    {
        return;
    }

    this->Reserve(this->GetSize() + Other.GetSize());

    LAL_CHECK_ARRAY( this->Impl.Data )

    if constexpr (std::is_same_v<T, typename UAllocator::T> && TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memcpy(this->Impl.Slack, Other.Impl.Data, Other.GetSize() * sizeof(T));
        #include "Definitions/PopDiagnostics.h"
        this->Impl.Slack += Other.GetSize();

        Other.GetMutableAllocator().Slack = Other.GetMutableAllocator().Data;
    }
    else
    {
        for (typename UAllocator::T& Element : Other)
        {
            std::construct_at(this->Impl.Slack++, std::move(Element));
        }
    }

    Other.Empty();

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
FORCEINLINE void TArrayBase<TAllocator>::Append(const std::initializer_list<U> List) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>)
{
    this->Append(List.begin(), List.end());
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE void TArrayBase<TAllocator>::Append(UIterator Begin, const VIterator End) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<UIterator>::reference>)
{
    LAL_CHECK_ARRAY( std::to_address(Begin) <= std::to_address(End) )

    this->Reserve(this->GetSize() + (End - Begin));

    while (Begin != End)
    {
        std::construct_at(this->Impl.Slack++, *Begin++);
    }

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator>
FORCEINLINE void TArrayBase<TAllocator>::Append(const UIterator Begin, const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<UIterator>::reference>)
{
    this->Append(Begin, Begin + Count);
    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const UIterator It, const TArrayBase<UAllocator>& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const typename UAllocator::T&>)
{
    return this->AppendAt(std::to_address(It) - this->Impl.Data, Other);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const UIterator It, TArrayBase<UAllocator>&& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T&&>)
{
    return this->AppendAt(std::to_address(It) - this->Impl.Data, std::move(Other));
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, typename U>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const UIterator It, const std::initializer_list<U> List) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>)
{
    return this->AppendAt(std::to_address(It) - this->Impl.Data, List);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator>)
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const UIterator It, const VIterator Begin, const WIterator End) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<VIterator>::reference>)
{
    return this->AppendAt(std::to_address(It) - this->Impl.Data, Begin, End);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, TIteratorConcept VIterator>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const UIterator It, const VIterator Begin, const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<VIterator>::reference>)
{
    return this->AppendAt(It, Begin, Begin + Count);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const SizeType Index, const TArrayBase<UAllocator>& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const typename UAllocator::T&>)
{
    return this->AppendAt(Index, Other.begin(), Other.end());
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TArrayBaseAllocatorConceptBase UAllocator>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator
TArrayBase<TAllocator>::AppendAt(const SizeType Index, TArrayBase<UAllocator>&& Other) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T&&>)
{
    check( this != &Other )

    const SizeType Count { Other.GetSize() };

    if (Count == 0)
    {
        return this->end();
    }

    if (Index == this->GetSize())
    {
        this->Append(std::move(Other));
        return this->end() - Count;
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->ReserveAdditionally(Count);

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Impl.Data + Index + Count, this->Impl.Data + Index, (this->GetSize() - Index) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"

        if constexpr (std::is_same_v<T, typename UAllocator::T>)
        {
            static_assert(sizeof(T) == sizeof(typename UAllocator::T));

            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memcpy(this->Impl.Data + Index, Other.GetAllocator().Data, Count * sizeof(T));
            #include "Definitions/PopDiagnostics.h"

            Other.GetMutableAllocator().Slack = Other.GetMutableAllocator().Data;
        }
        else
        {
            SizeType MutableIndex { Index };
            for (typename UAllocator::T& Element : Other)
            {
                std::construct_at(this->Impl.Data + MutableIndex++, std::move(Element));
            }
        }
    }
    else
    {
        Pointer InsertPos = this->Impl.Data + Index;

        const SizeType TailCount { static_cast<SizeType>(this->Impl.Slack - InsertPos) };

        const SizeType NToMoveBackwards {  TailCount < Count ? 0 : TailCount - Count };
        LAL_CHECK_ARRAY( TailCount >= NToMoveBackwards )
        const SizeType NToMoveConstruct { TailCount - NToMoveBackwards };

        for (SizeType Idx { 0 }; Idx < NToMoveConstruct; ++Idx)
        {
            Pointer Src = this->Impl.Slack - 1 - Idx;
            Pointer Dst = Src + Count;
            std::construct_at(Dst, std::move(*Src));

            continue;
        }

        for (SizeType Idx { 0 }; Idx < NToMoveBackwards; ++Idx)
        {
            Pointer Src = this->Impl.Slack - 1 - Idx - NToMoveConstruct;
            Pointer Dst = Src + Count;
            *Dst = std::move(*Src);

            continue;
        }

        for (typename UAllocator::T& Element : Other)
        {
            if (InsertPos < this->Impl.Slack)
            {
                *InsertPos = std::move(Element);
            }
            else
            {
                std::construct_at(InsertPos, std::move(Element));
            }

            ++InsertPos;
        }
    }

    Other.Empty();

    this->Impl.Slack += Count;
    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return Iterator{ this->Impl.Data + Index };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator
TArrayBase<TAllocator>::AppendAt(const SizeType Index, const std::initializer_list<U> List) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>)
{
    return this->AppendAt(Index, List.begin(), List.end());
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const SizeType Index, UIterator Begin, const VIterator End) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<UIterator>::reference>)
{
    check( this->IsValidIterator(Begin) == false && this->IsValidIterator(End) == false )

    const SizeType Count { static_cast<SizeType>(End - Begin) };

    if (Count == 0)
    {
        return this->end();
    }

    if (Index == this->GetSize())
    {
        this->Append(Begin, End);
        return this->end() - Count;
    }

    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->ReserveAdditionally(Count);

    if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
    {
        #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
        std::memmove(this->Impl.Data + Index + Count, this->Impl.Data + Index, (this->GetSize() - Index) * sizeof(T));
        #include "Definitions/PopDiagnostics.h"

        // TODO Also memcpy this if T and U is the same
        SizeType MutableIndex { Index };
        while (Begin != End)
        {
            std::construct_at(this->Impl.Data + MutableIndex++, std::forward<decltype(*Begin)>(*Begin++));
            continue;
        }
    }
    else
    {
        Pointer InsertPos = this->Impl.Data + Index;

        const SizeType TailCount { static_cast<SizeType>(this->Impl.Slack - InsertPos) };

        const SizeType NToMoveBackwards {  TailCount < Count ? 0 : TailCount - Count };
        LAL_CHECK_ARRAY( TailCount >= NToMoveBackwards )
        const SizeType NToMoveConstruct { TailCount - NToMoveBackwards };

        for (SizeType Idx { 0 }; Idx < NToMoveConstruct; ++Idx)
        {
            Pointer Src = this->Impl.Slack - 1 - Idx;
            Pointer Dst = Src + Count;
            std::construct_at(Dst, std::move(*Src));

            continue;
        }

        for (SizeType Idx { 0 }; Idx < NToMoveBackwards; ++Idx)
        {
            Pointer Src = this->Impl.Slack - 1 - Idx - NToMoveConstruct;
            Pointer Dst = Src + Count;
            *Dst = std::move(*Src);

            continue;
        }

        for (; Begin != End; ++Begin, ++InsertPos)
        {
            if (InsertPos < this->Impl.Slack)
            {
                *InsertPos = std::forward<decltype(*Begin)>(*Begin);
            }
            else
            {
                std::construct_at(InsertPos, std::forward<decltype(*Begin)>(*Begin));
            }

            continue;
        }
    }

    this->Impl.Slack += Count;

    LAL_CHECK_ARRAY( this->Impl.Slack <= this->Impl.End )

    return Iterator{ this->Impl.Data + Index };
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::AppendAt(const SizeType Index, const UIterator Begin, const SizeType Count) noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<UIterator>::reference>)
{
    return this->AppendAt(Index, Begin, Begin + Count);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator>
FORCEINLINE void TArrayBase<TAllocator>::RemoveAt(const UIterator It) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    this->RemoveAt(std::to_address(It) - this->Impl.Data);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
FORCEINLINE void TArrayBase<TAllocator>::RemoveAt(const UIterator Begin, const VIterator End) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    this->RemoveAt(std::to_address(Begin) - this->Impl.Data, std::to_address(End) - this->Impl.Data);

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
void TArrayBase<TAllocator>::RemoveAt(const SizeType Index) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    this->DestroyAt(Index);

    if (Index < this->GetSize() - 1)
    {
        if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
        {
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memmove(this->Impl.Data + Index, this->Impl.Data + Index + 1, (this->GetSize() - Index - 1) * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }
        else
        {
            unimplemented()
        }
    }

    --this->Impl.Slack;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
void TArrayBase<TAllocator>::RemoveAt(const SizeType Begin, const SizeType End) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Begin) && Begin <= End && (this->IsValidIndex(Begin == End || End - 1)) )

    if (Begin == End)
    {
        return;
    }

    for (SizeType Index { Begin }; Index < End; ++Index)
    {
        this->DestroyAt(Index);
    }

    if (End <= this->GetSize())
    {
        if constexpr (TArrayBaseAllowTrivialMemoryBufferMove_v<T>)
        {
            #include "Definitions/PushDynamicNonTrivialMemoryAccess.h"
            std::memmove(this->Impl.Data + Begin, this->Impl.Data + End, (this->GetSize() - (End - 1) - 1) * sizeof(T));
            #include "Definitions/PopDiagnostics.h"
        }
        else
        {
            unimplemented()
        }
    }

    this->Impl.Slack -= End - Begin;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::Remove(const U& What) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    SizeType Removed { 0 };

    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer();)
    {
        if (*Bulk == What)
        {
            this->RemoveAt(Bulk - this->GetDataPointer());
            ++Removed;
        }
        else
        {
            ++Bulk;
        }

        continue;
    }

    return Removed;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::RemoveAtLeastOnceChecked(const U& What) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(What) };
    check( Count > 0 )
    return Count;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::RemoveAtLeastOnceAsserted(const U& What) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->Remove(What) };
    jassert( Count > 0 )
    return Count;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::RemoveByPredicate(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    SizeType Removed { 0 };

    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer();)
    {
        if (Predicate(*Bulk))
        {
            this->RemoveAt(Bulk - this->GetDataPointer());
            ++Removed;
        }
        else
        {
            ++Bulk;
        }

        continue;
    }

    return Removed;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::RemoveByPredicateAtLeastOnceChecked(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->RemoveByPredicate<TPredicate>(Predicate) };
    check( Count > 0 )
    return Count;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::RemoveByPredicateAtLeastOnceAsserted(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const SizeType Count { this->RemoveByPredicate<TPredicate>(Predicate) };
    jassert( Count > 0 )
    return Count;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
bool TArrayBase<TAllocator>::RemoveOnce(const U& What) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk == What)
        {
            this->RemoveAt(Bulk - this->GetDataPointer());
            return true;
        }

        continue;
    }

    return false;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE bool TArrayBase<TAllocator>::RemoveOnceChecked(const U& What) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const bool bRemoved { this->RemoveOnce(What) };
    check( bRemoved )
    return bRemoved;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE bool TArrayBase<TAllocator>::RemoveOnceAsserted(const U& What) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const bool bRemoved { this->RemoveOnce(What) };
    jassert( bRemoved )
    return bRemoved;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
bool TArrayBase<TAllocator>::RemoveOnceByPredicate(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (Predicate(*Bulk))
        {
            this->RemoveAt(Bulk - this->GetDataPointer());
            return true;
        }

        continue;
    }

    return false;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE bool TArrayBase<TAllocator>::RemoveOnceByPredicateChecked(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const bool bRemoved { this->RemoveOnceByPredicate<TPredicate>(Predicate) };
    check( bRemoved )
    return bRemoved;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE bool TArrayBase<TAllocator>::RemoveOnceByPredicateAsserted(const TPredicate& Predicate) noexcept
    requires(TArrayBase::IsAllowedToPopItemsInBetween())
{
    const bool bRemoved { this->RemoveOnceByPredicate<TPredicate>(Predicate) };
    jassert( bRemoved )
    return bRemoved;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::Find(const U& What) noexcept
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk == What)
        {
            return Iterator{ Bulk };
        }

        continue;
    }

    return this->end();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::Find(const U& What) const noexcept
{
    for (const T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk == What)
        {
            return ConstIterator{ Bulk };
        }

        continue;
    }

    return this->cend();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindChecked(const U& What) noexcept
{
    const Iterator It { this->Find(What) };
    check( It != this->end() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindChecked(const U& What) const noexcept
{
    const ConstIterator It { this->Find(What) };
    check( It != this->cend() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindAsserted(const U& What) noexcept
{
    const Iterator It { this->Find(What) };
    jassert( It != this->end() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindAsserted(const U& What) const noexcept
{
    const ConstIterator It { this->Find(What) };
    jassert( It != this->cend() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndex(const U& What) noexcept
{
    if (Iterator It { this->Find(What) }; It != this->end())
    {
        return std::to_address(It) - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndex(const U& What) const noexcept
{
    if (ConstIterator It { this->Find(What) }; It != this->cend())
    {
        return std::to_address(It) - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexChecked(const U& What) noexcept
{
    const SizeType Index { this->FindIndex(What) };
    check( Index != this->GetSize() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexChecked(const U& What) const noexcept
{
    const SizeType Index { this->FindIndex(What) };
    check( Index != this->GetSize() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexAsserted(const U& What) noexcept
{
    const SizeType Index { this->FindIndex(What) };
    jassert( Index != this->GetSize() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexAsserted(const U& What) const noexcept
{
    const SizeType Index { this->FindIndex(What) };
    jassert( Index != this->GetSize() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindRef(const U& What) noexcept
{
    if (Iterator It { this->Find(What) }; It != this->end())
    {
        return std::to_address(It);
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindRef(const U& What) const noexcept
{
    if (ConstIterator It { this->Find(What) }; It != this->cend())
    {
        return std::to_address(It);
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindRefChecked(const U& What) noexcept
{
    const Pointer Ptr { this->FindRef(What) };
    check( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindRefChecked(const U& What) const noexcept
{
    const ConstPointer Ptr { this->FindRef(What) };
    check( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindRefAsserted(const U& What) noexcept
{
    const Pointer Ptr { this->FindRef(What) };
    jassert( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindRefAsserted(const U& What) const noexcept
{
    const ConstPointer Ptr { this->FindRef(What) };
    jassert( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindByPredicate(const TPredicate& Predicate) noexcept
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (Predicate(*Bulk))
        {
            return Iterator{ Bulk };
        }

        continue;
    }

    return this->end();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindByPredicate(const TPredicate& Predicate) const noexcept
{
    for (const T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (Predicate(*Bulk))
        {
            return ConstIterator{ Bulk };
        }

        continue;
    }

    return this->cend();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindByPredicateChecked(const TPredicate& Predicate) noexcept
{
    const Iterator It { this->FindByPredicate(Predicate) }; check( It != this->end() ) return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindByPredicateChecked(const TPredicate& Predicate) const noexcept
{
    const ConstIterator It { this->FindByPredicate(Predicate) }; check( It != this->cend() ) return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindByPredicateAsserted(const TPredicate& Predicate) noexcept
{
    const Iterator It { this->FindByPredicate(Predicate) }; jassert( It != this->end() ) return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindByPredicateAsserted(const TPredicate& Predicate) const noexcept
{
    const ConstIterator It { this->FindByPredicate(Predicate) }; jassert( It != this->cend() ) return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexByPredicate(const TPredicate& Predicate) noexcept
{
    if (Iterator It { this->FindByPredicate(Predicate) }; It != this->end())
    {
        return std::to_address(It) - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexByPredicate(const TPredicate& Predicate) const noexcept
{
    if (ConstIterator It { this->FindByPredicate(Predicate) }; It != this->cend())
    {
        return std::to_address(It) - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexByPredicateChecked(const TPredicate& Predicate) noexcept
{
    const SizeType Index { this->FindIndexByPredicate(Predicate) };
    check( Index != this->end() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexByPredicateChecked(const TPredicate& Predicate) const noexcept
{
    const SizeType Index { this->FindIndexByPredicate(Predicate) };
    check( Index != this->cend() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexByPredicateAsserted(const TPredicate& Predicate) noexcept
{
    const SizeType Index { this->FindIndexByPredicate(Predicate) };
    jassert( Index != this->end() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindIndexByPredicateAsserted(const TPredicate& Predicate) const noexcept
{
    const SizeType Index { this->FindIndexByPredicate(Predicate) };
    jassert( Index != this->cend() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindRefByPredicate(const TPredicate& Predicate) noexcept
{
    if (Iterator It { this->FindByPredicate(Predicate) }; It != this->end())
    {
        return std::to_address(It);
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindRefByPredicate(const TPredicate& Predicate) const noexcept
{
    if (ConstIterator It { this->FindByPredicate(Predicate) }; It != this->cend())
    {
        return std::to_address(It);
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindRefByPredicateChecked(const TPredicate& Predicate) noexcept
{
    const Pointer Ptr { this->FindRefByPredicate(Predicate) };
    check( Ptr)
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindRefByPredicateChecked(const TPredicate& Predicate) const noexcept
{
    const ConstPointer Ptr { this->FindRefByPredicate(Predicate) };
    check( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindRefByPredicateAsserted(const TPredicate& Predicate) noexcept
{
    const Pointer Ptr { this->FindRefByPredicate(Predicate) };
    jassert( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindRefByPredicateAsserted(const TPredicate& Predicate) const noexcept
{
    const ConstPointer Ptr { this->FindRefByPredicate(Predicate) };
    jassert( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindLast(const U& What) noexcept
{
    for (T* RESTRICT Bulk { this->GetSlackPointer() - 1 }; Bulk >= this->GetDataPointer(); --Bulk)
    {
        if (*Bulk == What)
        {
            return Iterator{ Bulk };
        }

        continue;
    }

    return this->end();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindLast(const U& What) const noexcept
{
    for (const T* RESTRICT Bulk { this->GetSlackPointer() - 1 }; Bulk >= this->GetDataPointer(); --Bulk)
    {
        if (*Bulk == What)
        {
            return ConstIterator{ Bulk };
        }

        continue;
    }

    return this->cend();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindLastChecked(const U& What) noexcept
{
    const Iterator It { this->FindLast(What) };
    check( It != this->end() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindLastChecked(const U& What) const noexcept
{
    const ConstIterator It { this->FindLast(What) };
    check( It != this->cend() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindLastAsserted(const U& What) noexcept
{
    const Iterator It { this->FindLast(What) };
    jassert( It != this->end() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindLastAsserted(const U& What) const noexcept
{
    const ConstIterator It { this->FindLast(What) };
    jassert( It != this->cend() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndex(const U& What) noexcept
{
    if (Iterator It { this->FindLast(What) }; It != this->end())
    {
        return std::to_address(It) - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndex(const U& What) const noexcept
{
    if (ConstIterator It { this->FindLast(What) }; It != this->cend())
    {
        return std::to_address(It) - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexChecked(const U& What) noexcept
{
    const SizeType Index { this->FindLastIndex(What) }; check( Index != this->GetSize() ) return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexChecked(const U& What) const noexcept
{
    const SizeType Index { this->FindLastIndex(What) }; check( Index != this->GetSize() ) return Index;
}
    template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexAsserted(const U& What) noexcept
{
    const SizeType Index { this->FindLastIndex(What) }; jassert( Index != this->GetSize() ) return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexAsserted(const U& What) const noexcept
{
     const SizeType Index { this->FindLastIndex(What) }; jassert( Index != this->GetSize() ) return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindLastRef(const U& What) noexcept
{
    if (Iterator It { this->FindLast(What) }; It != this->end())
    {
        return std::to_address(It);
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U>
    requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindLastRef(const U& What) const noexcept
{
    if (ConstIterator It { this->FindLast(What) }; It != this->cend())
    {
        return std::to_address(It);
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindLastRefChecked(const U& What) noexcept
{
    const Pointer Ptr { this->FindLastRef(What) };
    check( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindLastRefChecked(const U& What) const noexcept
{
    const ConstPointer Ptr { this->FindLastRef(What) };
    check( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindLastRefAsserted(const U& What) noexcept
{
    const Pointer Ptr { this->FindLastRef(What) };
    jassert( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindLastRefAsserted(const U& What) const noexcept
{
    const ConstPointer Ptr { this->FindLastRef(What) };
    jassert( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindLastByPredicate(const TPredicate& Predicate) noexcept
{
    for (T* RESTRICT Bulk { this->GetSlackPointer() - 1 }; Bulk >= this->GetDataPointer(); --Bulk)
    {
        if (Predicate(*Bulk))
        {
            return Iterator{ Bulk };
        }

        continue;
    }

    return this->end();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindLastByPredicate(const TPredicate& Predicate) const noexcept
{
    for (const T* RESTRICT Bulk { this->GetSlackPointer() - 1 }; Bulk >= this->GetDataPointer(); --Bulk)
    {
        if (Predicate(*Bulk))
        {
            return ConstIterator{ Bulk };
        }

        continue;
    }

    return this->cend();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindLastByPredicateChecked(const TPredicate& Predicate) noexcept
{
    const Iterator It { this->FindLastByPredicate(Predicate) };
    check( It != this->end() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindLastByPredicateChecked(const TPredicate& Predicate) const noexcept
{
    const ConstIterator It { this->FindLastByPredicate(Predicate) };
    check( It != this->cend() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Iterator TArrayBase<TAllocator>::FindLastByPredicateAsserted(const TPredicate& Predicate) noexcept
{
    const Iterator It { this->FindLastByPredicate(Predicate) };
    jassert( It != this->end() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstIterator TArrayBase<TAllocator>::FindLastByPredicateAsserted(const TPredicate& Predicate) const noexcept
{
    const ConstIterator It { this->FindLastByPredicate(Predicate) };
    jassert( It != this->cend() )
    return It;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexByPredicate(const TPredicate& Predicate) noexcept
{
    if (Iterator It { this->FindLastByPredicate(Predicate) }; It != this->end())
    {
        return std::to_address(It) - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexByPredicate(const TPredicate& Predicate) const noexcept
{
    if (ConstIterator It { this->FindLastByPredicate(Predicate) }; It != this->cend())
    {
        return std::to_address(It) - this->GetDataPointer();
    }

    return this->GetSize();
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexByPredicateChecked(const TPredicate& Predicate) noexcept
{
    const SizeType Index { this->FindLastIndexByPredicate(Predicate) };
    check( Index != this->end() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexByPredicateChecked(const TPredicate& Predicate) const noexcept
{
    const SizeType Index { this->FindLastIndexByPredicate(Predicate) };
    check( Index != this->cend() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexByPredicateAsserted(const TPredicate& Predicate) noexcept
{
    const SizeType Index { this->FindLastIndexByPredicate(Predicate) };
    jassert( Index != this->end() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::FindLastIndexByPredicateAsserted(const TPredicate& Predicate) const noexcept
{
    const SizeType Index { this->FindLastIndexByPredicate(Predicate) };
    jassert( Index != this->cend() )
    return Index;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindLastRefByPredicate(const TPredicate& Predicate) noexcept
{
    if (Iterator It { this->FindLastByPredicate(Predicate) }; It != this->end())
    {
        return std::to_address(It);
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindLastRefByPredicate(const TPredicate& Predicate) const noexcept
{
    if (ConstIterator It { this->FindLastByPredicate(Predicate) }; It != this->cend())
    {
        return std::to_address(It);
    }

    return nullptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindLastRefByPredicateChecked(const TPredicate& Predicate) noexcept
{
    const Pointer Ptr { this->FindLastRefByPredicate(Predicate) };
    check( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindLastRefByPredicateChecked(const TPredicate& Predicate) const noexcept
{
    const ConstPointer Ptr { this->FindLastRefByPredicate(Predicate) };
    check( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::Pointer TArrayBase<TAllocator>::FindLastRefByPredicateAsserted(const TPredicate& Predicate) noexcept
{
    const Pointer Ptr { this->FindLastRefByPredicate(Predicate) };
    jassert( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE typename TArrayBase<TAllocator>::ConstPointer TArrayBase<TAllocator>::FindLastRefByPredicateAsserted(const TPredicate& Predicate) const noexcept
{
    const ConstPointer Ptr { this->FindLastRefByPredicate(Predicate) };
    jassert( Ptr )
    return Ptr;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::Count(const U& What) const noexcept
{
    SizeType Count { 0 };

    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk == What)
        {
            ++Count;
        }

        continue;
    }

    return Count;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename U, typename V> requires(Lal::TEqualityComparableLeft<typename TAllocator::T, U> && std::assignable_from<typename TAllocator::T&, V>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::Replace(const U& What, const V& Replacement) noexcept
    requires(TArrayBase::IsContentMutable())
{
    SizeType Replaced { 0 };

    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (*Bulk == What)
        {
            *Bulk = Replacement;
            ++Replaced;
        }

        continue;
    }

    return Replaced;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate, typename V> requires(std::assignable_from<typename TAllocator::T&, V>)
FORCEINLINE typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::ReplaceByPredicate(const TPredicate& Predicate, const V& Replacement) noexcept
    requires(TArrayBase::IsContentMutable())
{
    SizeType Replaced { 0 };

    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        if (Predicate(*Bulk))
        {
            *Bulk = Replacement;
            ++Replaced;
        }

        continue;
    }

    return Replaced;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename TPredicate> requires(std::invocable<TPredicate, typename TAllocator::T> && std::is_void_v<std::invoke_result_t<TPredicate, typename TAllocator::T>>)
FORCEINLINE void TArrayBase<TAllocator>::ForEach(const TPredicate& Predicate) noexcept requires(TArrayBase::IsContentMutable())
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        Predicate(*Bulk);

        continue;
    }

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <std::predicate<typename TAllocator::T> TPredicate>
FORCEINLINE void TArrayBase<TAllocator>::ForEach(const TPredicate& Predicate) const noexcept
    requires(!TArrayBase::IsContentMutable())
{
    for (const T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        Predicate(*Bulk);

        continue;
    }

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename TPredicate>
    requires(std::invocable<TPredicate, typename TArrayBase<TAllocator>::T> && std::is_void_v<std::invoke_result_t<TPredicate, typename TArrayBase<TAllocator>::T>>)
FORCEINLINE void TArrayBase<TAllocator>::ForEach(const TPredicate& Predicate) const noexcept
    requires(TArrayBase::IsContentMutable())
{
    for (const T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        Predicate(*Bulk);

        continue;
    }

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
FORCEINLINE TArrayBase<TAllocator>& TArrayBase<TAllocator>::Push(TArgs&&... Args) & noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    this->Emplace(std::forward<TArgs>(Args)...);
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
FORCEINLINE TArrayBase<TAllocator>&& TArrayBase<TAllocator>::Push(TArgs&&... Args) && noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    this->Emplace(std::forward<TArgs>(Args)...);
    return std::move(*this);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
FORCEINLINE TArrayBase<TAllocator>& TArrayBase<TAllocator>::PushMinimalGrowth(TArgs&&... Args) & noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    this->EmplaceMinimalGrowth(std::forward<TArgs>(Args)...);
    return *this;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <typename... TArgs>
FORCEINLINE TArrayBase<TAllocator>&& TArrayBase<TAllocator>::PushMinimalGrowth(TArgs&&... Args) && noexcept
    requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<typename TAllocator::T, TArgs...>)
{
    this->EmplaceMinimalGrowth(std::forward<TArgs>(Args)...);
    return std::move(*this);
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE bool TArrayBase<TAllocator>::Pop() noexcept
{
    if (this->GetSize() > 0)
    {
        this->DestroyAt(this->GetSize() - 1);
        --this->Impl.Slack;

        LAL_CHECK_ARRAY( this->Impl.Slack >= this->Impl.Data && this->Impl.Slack <= this->Impl.End )

        return true;
    }

    return false;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE void TArrayBase<TAllocator>::Pop(SizeType Count) noexcept
{
    while (this->GetSize() > 0 && Count > 0)
    {
        this->DestroyAt(this->Impl.Slack - 1);
        --this->Impl.Slack;
        --Count;

        continue;
    }

    LAL_CHECK_ARRAY( this->Impl.Slack >= this->Impl.Data && this->Impl.Slack <= this->Impl.End )

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::GrowImpl() noexcept
    requires(!TArrayBase::IsStronglyAllocated() && !requires(Allocator _Allocator) { _Allocator.Grow(); })
{
    PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::GrowToImpl(const SizeType Count) noexcept
    requires(!TArrayBase::IsStronglyAllocated())
{
    PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr typename TArrayBase<TAllocator>::SizeType TArrayBase<TAllocator>::ResizeImpl(const SizeType Count) noexcept
    requires(!TArrayBase::IsStronglyAllocated())
{
    PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::ShrinkToFitImpl() noexcept
    requires(!TArrayBase::IsStronglyAllocated())
{
    PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::ShrinkToImpl(const SizeType Count) noexcept
    requires(!TArrayBase::IsStronglyAllocated())
{
    PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on weakly allocated array.")
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::OrphanImpl() noexcept
    requires(!TArrayBase::IsStronglyAllocated() && !requires(Allocator _Allocator) { _Allocator.Orphan(); })
{
    PRIVATE_LAL_LOG_FATAL_CORE(LogLowLevel, "Called on non orphanable array.")
}

template <TArrayBaseAllocatorConceptBase TAllocator>
template <TIteratorConcept UIterator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::DestroyAt(const UIterator It) noexcept
    requires(!requires(Allocator _Allocator) { _Allocator.DestroyAt(std::to_address(It)); } && TArrayBase::IsContentMutable())
{
    LAL_CHECK_ARRAY( this->IsValidIterator(It) )

    (std::to_address(It))->~T();

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::DestroyAt(const SizeType Index) noexcept
    requires(!requires(Allocator _Allocator) { _Allocator.DestroyAt(Index); } && TArrayBase::IsContentMutable())
{
    LAL_CHECK_ARRAY( this->IsValidIndex(Index) )

    (this->GetDataPointer() + Index)->~T();

    return;
}

template <TArrayBaseAllocatorConceptBase TAllocator>
FORCEINLINE constexpr void TArrayBase<TAllocator>::Destruct() noexcept
    requires(TArrayBase::IsContentMutable() && TArrayBase::IsStronglyAllocated())
{
    for (T* RESTRICT Bulk { this->GetDataPointer() }; Bulk != this->GetSlackPointer(); ++Bulk)
    {
        Bulk->~T();

        continue;
    }

    this->Impl.Slack = this->Impl.Data;

    return;
}

} /* ~Namespace Lal */
