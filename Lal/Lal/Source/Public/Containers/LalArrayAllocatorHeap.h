// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
struct TArrayBaseDefaultAllocatorHeapImpl
{
    typedef TTraits Traits;
    typedef TIn     T;

    typedef typename Traits::SizeType SizeType;

    typedef typename Traits::template Iterator<T>       Iterator;
    typedef typename Traits::template Iterator<T const> ConstIterator;
    typedef typename Traits::template Iterator<T, true> MoveIterator;

    typedef T*       Pointer;
    typedef T const* ConstPointer;
    typedef T&       Reference;
    typedef T const& ConstReference;

    NODISCARD
    FORCEINLINE consteval static bool IsOwningMemoryAllocator() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept { return true; }

    FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl() noexcept
        : Data{nullptr}, Slack{nullptr}, End{nullptr}
    {
        return;
    }

    FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl(TArrayBaseDefaultAllocatorHeapImpl const& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl& operator=(TArrayBaseDefaultAllocatorHeapImpl const& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl(TArrayBaseDefaultAllocatorHeapImpl&& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl& operator=(TArrayBaseDefaultAllocatorHeapImpl&& Other) noexcept;

    FORCEINLINE constexpr explicit TArrayBaseDefaultAllocatorHeapImpl(CRange auto& Range) noexcept
        requires std::constructible_from<T, Ranges::Value_t<decltype(Range)>>;
    FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl& operator=(CRange auto& Range) noexcept
        requires requires{std::constructible_from<T, Ranges::Value_t<decltype(Range)>>;};

    FORCEINLINE constexpr explicit TArrayBaseDefaultAllocatorHeapImpl(CContainer auto&& Container) noexcept
        requires std::is_constructible_v<TIn, Ranges::Value_t<decltype(Container)>>;
    FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl& operator=(CContainer auto&& Container) noexcept
        requires std::is_constructible_v<TIn, Ranges::Value_t<decltype(Container)>>;
    FORCEINLINE constexpr explicit TArrayBaseDefaultAllocatorHeapImpl(CRange auto&& Range) noexcept
        requires (requires{std::constructible_from<TIn, Ranges::Value_t<decltype(Range)>>;} && (!Ranges::HasEmpty_v<decltype(Range)>)) = delete;
    FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl& operator=(CRange auto&& Range) noexcept
        requires (requires{std::is_constructible_v<TIn, Ranges::Value_t<decltype(Range)>>;} && (!Ranges::HasEmpty_v<decltype(Range)>)) = delete;

    FORCEINLINE constexpr TArrayBaseDefaultAllocatorHeapImpl(ITERATOR Begin, ITERATOR End) noexcept
        requires std::is_constructible_v<T, typename TIteratorTraits<decltype(Begin)>::value_type>;

    FORCEINLINE constexpr ~TArrayBaseDefaultAllocatorHeapImpl() noexcept
    {
        this->OrphanImpl();
        return;
    }

     //#
    //# Growths the allocator to the next size specified by its traits.
    //#
    FORCEINLINE void Grow() noexcept;

    //#
    //# Growths the allocator so that it can hold at least the given number of elements.
    //# This operation cannot cause a shrink under the hood.
    //#
    FORCEINLINE void GrowTo(const SizeType Count) noexcept;

    //#
    //# Resizes the allocator so that it can hold at least the given number of elements.
    //# This operation may cause a shrink under the hood.
    //# If the allocator is bigger than the given count, it will be shrunk to the number of elements it currently holds.
    //#
    //# @return The number of elements reserved by this allocator.
    //#
    FORCEINLINE SizeType Resize(const SizeType Count) noexcept;

    //#
    //# Shrinks the allocator to current number of elements it currently holds.
    //#
    FORCEINLINE void ShrinkToFit() noexcept
    {
        this->ShrinkToImpl(static_cast<SizeType>(this->Slack - this->Data));
        return;
    }

    //#
    //# Shrinks the allocator to the given number of elements.
    //# @param Count The number of elements to shrink the allocator to. If this is less than the current number
    //#              of elements it holds, the allocator will be shrunk to the current number of elements it holds.
    //#
    FORCEINLINE void ShrinkTo(const SizeType Count) noexcept
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

    //#
    //# Destructs all elements and orphans the current allocator.
    //#
    FORCEINLINE void Orphan() noexcept
    {
        this->OrphanImpl();

        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;

        return;
    }

    FORCEINLINE void SwapBuffers(TArrayBaseDefaultAllocatorHeapImpl* Other) noexcept;

    FORCEINLINE bool Drop() noexcept;
    FORCEINLINE SizeType Drop(const SizeType Count) noexcept;

    FORCEINLINE constexpr Iterator begin() noexcept { return Iterator{ this->Data  }; }
    FORCEINLINE constexpr Iterator end() noexcept { return Iterator{ this->Slack }; }
    FORCEINLINE constexpr ConstIterator begin() const noexcept { return ConstIterator{ this->Data  }; }
    FORCEINLINE constexpr ConstIterator end() const noexcept { return ConstIterator{ this->Slack }; }

    Pointer Data;
    Pointer Slack;
    Pointer End;

private:

    FORCEINLINE void GrowImpl(const SizeType Count) noexcept;
    FORCEINLINE void ShrinkToImpl(const SizeType Count) noexcept;
    FORCEINLINE void ResizeEmtpy(const SizeType Count) noexcept;

    FORCEINLINE static Pointer AlignedAlloc(const SizeType Count) noexcept;
    FORCEINLINE void AlignedRealloc(const SizeType Count) noexcept;

    FORCEINLINE void OrphanImpl() noexcept
    {
        this->Destruct();
        std::free(this->Data);

        return;
    }

    FORCEINLINE constexpr void Destruct() noexcept
    {
        for (T* RESTRICT Bulk { this->Data }; Bulk != this->Slack; ++Bulk)
        {
            Bulk->~T();
        }

        this->Slack = this->Data;

        return;
    }
};

} /* ~Namespace Lal */
