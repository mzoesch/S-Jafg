// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

namespace Private
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
struct TArrayBaseDefaultAllocatorWeakImpl
{
    typedef TTraits Traits;
    typedef TIn     T;

    typedef typename Traits::SizeType SizeType;

    typedef _TIterator      Iterator;
    typedef _TConstIterator ConstIterator;
    typedef _TMoveIterator  MoveIterator;

    typedef _TPointer       Pointer;
    typedef _TConstPointer  ConstPointer;
    typedef _TReference     Reference;
    typedef _TConstReference ConstReference;

    NODISCARD
    FORCEINLINE consteval static bool IsOwningMemoryAllocator() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept { return false; }
    NODISCARD
    FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept = delete;

    FORCEINLINE explicit constexpr TArrayBaseDefaultAllocatorWeakImpl() noexcept
        : Data{nullptr}, Slack{nullptr}
    {
        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseDefaultAllocatorWeakImpl(TArrayBaseDefaultAllocatorWeakImpl const& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return;
    }
    FORCEINLINE constexpr TArrayBaseDefaultAllocatorWeakImpl& operator=(TArrayBaseDefaultAllocatorWeakImpl const& Other) noexcept
    {
        /* It is okay if this == &other. */
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return *this;
    }
    FORCEINLINE explicit constexpr TArrayBaseDefaultAllocatorWeakImpl(TArrayBaseDefaultAllocatorWeakImpl&& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;
        Other.Data = nullptr;
        Other.Slack = nullptr;

        return;
    }
    FORCEINLINE constexpr TArrayBaseDefaultAllocatorWeakImpl& operator=(TArrayBaseDefaultAllocatorWeakImpl&& Other) noexcept
    {
#if LAL_DO_CHECKS
        TArrayBaseDefaultAllocatorWeakImpl::_check([this, &Other](void) -> bool
        {
            return static_cast<void const*>(this) != static_cast<void const*>(&Other);
        });
#endif /* LAL_DO_CHECKS */

        this->Data = Other.Data;
        this->Slack = Other.Slack;
        Other.Data = nullptr;
        Other.Slack = nullptr;

        return *this;
    }

    FORCEINLINE constexpr explicit TArrayBaseDefaultAllocatorWeakImpl(CRange auto& Range) noexcept
        requires
               std::is_lvalue_reference_v<decltype((Range))>
            && std::is_convertible_v<decltype(std::to_address(std::ranges::begin(Range))), Pointer>
            && TIterator_IsContiguous_v<decltype(std::ranges::begin(Range))>
        : Data{std::to_address(std::ranges::begin(Range))}, Slack{std::to_address(std::ranges::end(Range))}
    {
#if LAL_DO_CHECKS
        TArrayBaseDefaultAllocatorWeakImpl::_check([this](void) -> bool { return this->Data <= this->Slack; });
#endif /* LAL_DO_CHECKS */
        return;
    }
    FORCEINLINE constexpr TArrayBaseDefaultAllocatorWeakImpl& operator=(CRange auto& Range) noexcept
        requires
               std::is_lvalue_reference_v<decltype((Range))>
            && std::is_convertible_v<decltype(std::to_address(std::ranges::begin(Range))), Pointer>
            && TIterator_IsContiguous_v<decltype(std::ranges::begin(Range))>
    {
        this->Data = std::to_address(std::ranges::begin(Range));
        this->Slack = std::to_address(std::ranges::end(Range));

#if LAL_DO_CHECKS
        TArrayBaseDefaultAllocatorWeakImpl::_check([this](void) -> bool { return this->Data <= this->Slack; });
#endif /* LAL_DO_CHECKS */
        return *this;
    }

    FORCEINLINE constexpr TArrayBaseDefaultAllocatorWeakImpl(ITERATOR Begin, ITERATOR End) noexcept
        requires std::is_convertible_v<typename TIteratorTraits<decltype(Begin)>::pointer, Pointer>
        : Data{std::to_address(Begin)}, Slack{std::to_address(End)}
    {
#if LAL_DO_CHECKS
        TArrayBaseDefaultAllocatorWeakImpl::_check([this](void) -> bool { return this->Data <= this->Slack; });
#endif /* LAL_DO_CHECKS */
        return;
    }

    FORCEINLINE constexpr ~TArrayBaseDefaultAllocatorWeakImpl() noexcept = default;

    FORCEINLINE constexpr bool Drop() noexcept
    {
        if (this->Data == this->Slack)
        {
            return false;
        }

        ++this->Data;
        return true;
    }

    constexpr SizeType Drop(const SizeType Count) noexcept;

    FORCEINLINE constexpr void DestroyAt(const Pointer Ptr) noexcept { return; }
    FORCEINLINE constexpr void DestroyAt(const SizeType Index) noexcept { return; }

    FORCEINLINE constexpr void Empty() noexcept
    {
        this->Data = nullptr;
        this->Slack = nullptr;

        return;
    }

    FORCEINLINE constexpr Iterator begin() noexcept { return Iterator{ this->Data  }; }
    FORCEINLINE constexpr Iterator end() noexcept { return Iterator{ this->Slack }; }
    FORCEINLINE constexpr ConstIterator begin() const noexcept { return ConstIterator{ this->Data  }; }
    FORCEINLINE constexpr ConstIterator end() const noexcept { return ConstIterator{ this->Slack }; }

    Pointer Data;
    union
    {
        Pointer Slack;
        Pointer End;
    };

private:

#if LAL_DO_CHECKS
    template <typename UPredicate>
    FORCEINLINE static void _check(UPredicate&& Predicate);
#endif /* ~LAL_DO_CHECKS */
};

} /* Namespace Private */

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseDefaultAllocatorViewImpl : public Private::TArrayBaseDefaultAllocatorWeakImpl<
    TIn,
    TTraits,
    typename TTraits::template Iterator<const TIn>,
    typename TTraits::template Iterator<const TIn>,
    typename TTraits::template Iterator<const TIn, true>,
    const TIn*,
    const TIn*,
    const TIn&,
    const TIn&
    >
{
    using Super = Private::TArrayBaseDefaultAllocatorWeakImpl<
        TIn,
        TTraits,
        typename TTraits::template Iterator<const TIn>,
        typename TTraits::template Iterator<const TIn>,
        typename TTraits::template Iterator<const TIn, true>,
        const TIn*,
        const TIn*,
        const TIn&,
        const TIn&
        >;

    using Super::Super;
    using Super::operator=;

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return false; }
};

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseDefaultAllocatorMutableViewImpl : public Private::TArrayBaseDefaultAllocatorWeakImpl<
    TIn,
    TTraits,
    typename TTraits::template Iterator<TIn>,
    typename TTraits::template Iterator<const TIn>,
    typename TTraits::template Iterator<const TIn, true>,
    TIn*,
    const TIn*,
    TIn&,
    const TIn&
    >
{
    using Super = Private::TArrayBaseDefaultAllocatorWeakImpl<
        TIn,
        TTraits,
        typename TTraits::template Iterator<TIn>,
        typename TTraits::template Iterator<const TIn>,
        typename TTraits::template Iterator<const TIn, true>,
        TIn*,
        const TIn*,
        TIn&,
        const TIn&
        >;

    using Super::Super;
    using Super::operator=;

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return true; }
};

} /* ~Namespace Lal */
