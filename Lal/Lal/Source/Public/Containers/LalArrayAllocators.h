// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

namespace Private
{

template <typename T>
concept HasIsCurrentDataOnHeap = requires(T t)
{
    { t.IsCurrentDataOnHeap() } -> std::convertible_to<bool>;
};

//#
//# The default implementation to check if an allocator has its current data on the heap.
//#
template <TArrayBaseAllocatorConceptBase TAllocator>
    requires
    (
        requires(TAllocator Allocator)
        {
            { Allocator.IsCurrentDataOnHeap() } -> std::convertible_to<bool>;
        }
    )
FORCEINLINE constexpr bool IsCurrentDataOnHeapDefaultAllocatorImpl(const TAllocator& Allocator) noexcept
{
    return Allocator.IsCurrentDataOnHeap();
}

//#
//# The default implementation to check if an allocator has its current data on the heap but does not
//# itself specify any #IsCurrentDataOnHeap function.
//#
template <TArrayBaseAllocatorConceptBase TAllocator>
    requires
    (
        !HasIsCurrentDataOnHeap<TAllocator> &&
        requires(TAllocator Allocator)
        {

            { Allocator.Data };
            requires std::is_pointer_v<decltype(Allocator.Data)>;
        }
    )
FORCEINLINE constexpr bool IsCurrentDataOnHeapDefaultAllocatorImpl(const TAllocator& Allocator) noexcept
{
    return Allocator.Data != nullptr;
}

} /* ~Namespace Private */

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseDefaultAllocatorBase
{
    typedef TTraits Traits;
    typedef TIn     T;

    typedef typename Traits::SizeType SizeType;

    FORCEINLINE explicit constexpr TArrayBaseDefaultAllocatorBase() = default;

    NODISCARD
    FORCEINLINE consteval static bool IsOwningMemoryAllocator() noexcept { return true; }
};

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseMutableDefaultAllocatorBase : public TArrayBaseDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::Traits Traits;
    typedef typename _Super::T      T;

    typedef typename _Super::SizeType SizeType;

    typedef typename Traits::template Iterator<T>       Iterator;
    typedef typename Traits::template Iterator<const T> ConstIterator;
    typedef typename Traits::template Iterator<T, true> MoveIterator;

    typedef T*       Pointer;
    typedef const T* ConstPointer;
    typedef T&       Reference;
    typedef const T& ConstReference;

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return true; }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorBase() noexcept
        : Data{nullptr}, Slack{nullptr}, End{nullptr}
    {
        return;
    }

    //#
    //# Default implementation to reset this allocator to its default state.
    //# If you have special rules that need to be applied, you can override this function.
    //#
    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data = nullptr;
        this->Slack = nullptr;
        this->End = nullptr;

        return;
    }

    Pointer Data;
    Pointer Slack;
    Pointer End;
};

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
struct TArrayBaseMutableDefaultAllocatorStrongImpl : public TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits   Traits;

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return true; }

    using TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>::TArrayBaseMutableDefaultAllocatorBase;

    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(const TArrayBaseMutableDefaultAllocatorStrongImpl& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(const TArrayBaseMutableDefaultAllocatorStrongImpl& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(TArrayBaseMutableDefaultAllocatorStrongImpl&& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(TArrayBaseMutableDefaultAllocatorStrongImpl&& Other) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(const UAllocator& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(const UAllocator& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(UAllocator&& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(UAllocator&& Allocator) noexcept;

    template <typename U>
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>);

    template <typename U>
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl& operator=(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>);

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorStrongImpl(UIterator Begin, const VIterator End) noexcept
        requires(std::is_constructible_v<T, const typename TIteratorTraits<UIterator>::value_type&>);

    FORCEINLINE ~TArrayBaseMutableDefaultAllocatorStrongImpl() noexcept
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

    FORCEINLINE void SwapBuffers(TArrayBaseMutableDefaultAllocatorStrongImpl* Other) noexcept;

    FORCEINLINE bool Drop() noexcept;
    FORCEINLINE SizeType Drop(const SizeType Count) noexcept;

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

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseMutableDefaultAllocatorWeakImpl : public TArrayBaseDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::Traits Traits;
    typedef typename _Super::T      T;

    typedef typename _Super::SizeType SizeType;

    typedef typename Traits::template Iterator<T>       Iterator;
    typedef typename Traits::template Iterator<const T> ConstIterator;
    typedef typename Traits::template Iterator<T, true> MoveIterator;

    typedef T*       Pointer;
    typedef const T* ConstPointer;
    typedef T&       Reference;
    typedef const T& ConstReference;

    NODISCARD
    FORCEINLINE consteval static bool IsOwningMemoryAllocator() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return true; }

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept { return false; }

    NODISCARD
    FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept;

    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data = nullptr;
        this->Slack = nullptr;

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl() noexcept
        : Data{nullptr}, Slack{nullptr}
    {
        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(const TArrayBaseMutableDefaultAllocatorWeakImpl& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(const TArrayBaseMutableDefaultAllocatorWeakImpl& Other) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return *this;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(TArrayBaseMutableDefaultAllocatorWeakImpl&& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        Other._ResetToDefaultState();

        return;
    }
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(TArrayBaseMutableDefaultAllocatorWeakImpl&& Other) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Other.Data;
        this->Slack = Other.Slack;

        Other._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            std::is_same_v<typename UAllocator::Pointer, const typename UAllocator::T*> == false
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(const UAllocator& Allocator) noexcept
    {
        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            std::is_same_v<typename UAllocator::Pointer, const typename UAllocator::T*> == false
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(const UAllocator& Allocator) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            (UAllocator::IsOwningMemoryAllocator() == false)
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(UAllocator&& Allocator) noexcept
    {
        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        Allocator._ResetToDefaultState();

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
    requires
    (
        requires
        {
            requires std::same_as<T, typename UAllocator::T>;
        }
        &&
        (UAllocator::IsOwningMemoryAllocator() == false)
        &&
        requires(UAllocator Allocator)
        {
            { Allocator.Data };
            { Allocator.Slack };
        }
    )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        Allocator._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            UAllocator::IsOwningMemoryAllocator()
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(UAllocator&& Allocator) noexcept = delete;
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            UAllocator::IsOwningMemoryAllocator()
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept = delete;

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(const UIterator Begin, const VIterator End) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultAllocatorWeakImpl(const Pointer Begin, const Pointer End) noexcept;

    FORCEINLINE constexpr bool Drop() noexcept;
    FORCEINLINE constexpr SizeType Drop(const SizeType Count) noexcept;

    FORCEINLINE constexpr void DestroyAt(const Pointer Ptr) noexcept
    {
        return;
    }
    FORCEINLINE constexpr void DestroyAt(const SizeType Index) noexcept
    {
        return;
    }

    FORCEINLINE constexpr void Empty() noexcept
    {
        this->_ResetToDefaultState();

        return;
    }

    Pointer Data;
    union
    {
        Pointer Slack;
        Pointer End;
    };
};

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseConstDefaultAllocatorWeakImpl : public TArrayBaseDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::Traits Traits;
    typedef typename _Super::T      T;

    typedef typename _Super::SizeType SizeType;

    typedef typename Traits::template Iterator<const T>       Iterator;
    typedef typename Traits::template Iterator<const T>       ConstIterator;
    typedef typename Traits::template Iterator<const T, true> MoveIterator;

    typedef const T*  Pointer;
    typedef Pointer   ConstPointer;
    typedef const T&  Reference;
    typedef Reference ConstReference;

    NODISCARD
    FORCEINLINE consteval static bool IsOwningMemoryAllocator() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return false; }

    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept { return false; }

    NODISCARD
    FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept;

    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data = nullptr;
        this->Slack = nullptr;

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl() noexcept
        : Data{nullptr}, Slack{nullptr}
    {
        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(const TArrayBaseConstDefaultAllocatorWeakImpl& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return;
    }
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(const TArrayBaseConstDefaultAllocatorWeakImpl& Other) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Other.Data;
        this->Slack = Other.Slack;

        return *this;
    }

    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(TArrayBaseConstDefaultAllocatorWeakImpl&& Other) noexcept
    {
        this->Data = Other.Data;
        this->Slack = Other.Slack;

        Other._ResetToDefaultState();

        return;
    }
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(TArrayBaseConstDefaultAllocatorWeakImpl&& Other) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Other.Data;
        this->Slack = Other.Slack;

        Other._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
            &&
            std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(const UAllocator& Allocator) noexcept
    {
        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
            &&
            std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(const UAllocator& Allocator) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            (UAllocator::IsOwningMemoryAllocator() == false)
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(UAllocator&& Allocator) noexcept
    {
        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        Allocator._ResetToDefaultState();

        return;
    }
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            (UAllocator::IsOwningMemoryAllocator() == false)
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept
    {
        /* It is okay if this == &other. */

        this->Data = Allocator.Data;
        this->Slack = Allocator.Slack;

        Allocator._ResetToDefaultState();

        return *this;
    }

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            UAllocator::IsOwningMemoryAllocator()
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseConstDefaultAllocatorWeakImpl(UAllocator&& Allocator) noexcept = delete;
    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                requires std::same_as<T, typename UAllocator::T>;
            }
            &&
            UAllocator::IsOwningMemoryAllocator()
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept = delete;

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl(const UIterator Begin, const VIterator End) noexcept;
    FORCEINLINE constexpr TArrayBaseConstDefaultAllocatorWeakImpl(const Pointer Begin, const Pointer End) noexcept;

    FORCEINLINE constexpr bool Drop() noexcept;
    FORCEINLINE constexpr SizeType Drop(const SizeType Count) noexcept;

    FORCEINLINE constexpr void DestroyAt(const Pointer Ptr) noexcept
    {
        return;
    }
    FORCEINLINE constexpr void DestroyAt(const SizeType Index) noexcept
    {
        return;
    }

    FORCEINLINE constexpr void Empty() noexcept
    {
        this->_ResetToDefaultState();

        return;
    }

    Pointer Data;
    union
    {
        Pointer Slack;
        Pointer End;
    };
};

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
struct TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl : public TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits Traits;

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return false; }

    static constexpr SizeType SizeCapacity { TTraits::SizeCapacity };
};

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
struct TArrayBaseMutableDefaultFixedAllocatorWeakImpl : public TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits Traits;

    static_assert(_Super::SizeCapacity > 0);

    using TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>::TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl;

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(const TArrayBaseMutableDefaultFixedAllocatorWeakImpl& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(const TArrayBaseMutableDefaultFixedAllocatorWeakImpl& Other) noexcept;
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(TArrayBaseMutableDefaultFixedAllocatorWeakImpl&& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(TArrayBaseMutableDefaultFixedAllocatorWeakImpl&& Other) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
            &&
            std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(const UAllocator& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
            &&
            std::is_lvalue_reference_v<decltype((std::declval<UAllocator const&>()))>
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(const UAllocator& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(UAllocator&& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(UAllocator&& Allocator) noexcept;

    template <typename U>
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>);
    template <typename U>
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl& operator=(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>);

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE constexpr TArrayBaseMutableDefaultFixedAllocatorWeakImpl(UIterator Begin, const VIterator End) noexcept
        requires(std::is_constructible_v<T, const typename TIteratorTraits<UIterator>::value_type&>);

    FORCEINLINE ~TArrayBaseMutableDefaultFixedAllocatorWeakImpl() noexcept
    {
        this->OrphanImpl();
        return;
    }

    //#
    //# Tries to allocate the memory for the array if it is not allocated yet.
    //# This allocator uses an allocating on demand strategy, so it will not allocate memory if it is unnecessary,
    //# e.g. during default construction.
    //#
    FORCEINLINE void Grow() noexcept;

    //#
    //# Destructs all elements and orphans the current allocator.
    //#
    FORCEINLINE void Orphan() noexcept;

    FORCEINLINE bool Drop() noexcept;
    FORCEINLINE SizeType Drop(const SizeType Count) noexcept;

private:

    FORCEINLINE void Allocate() noexcept;
    FORCEINLINE void AllocateNoCheck() noexcept;

    FORCEINLINE void OrphanImpl() noexcept;

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

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
struct TArrayBaseMutableDefaultStackAllocatorWeakImpl : public TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultCappedAllocatorWeakBaseImpl<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits Traits;

    NODISCARD
    FORCEINLINE consteval bool IsCurrentDataOnHeap() const noexcept { return false; }

    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data = reinterpret_cast<T*>(this->_Data);
        this->Slack = this->Data;
        this->End = this->Data + _Super::SizeCapacity;

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl() noexcept
        : _Data{}
    {
        this->_ResetToDefaultState();

        return;
    }

    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl(const TArrayBaseMutableDefaultStackAllocatorWeakImpl& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl& operator=(const TArrayBaseMutableDefaultStackAllocatorWeakImpl& Other) noexcept;
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl(TArrayBaseMutableDefaultStackAllocatorWeakImpl&& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl& operator=(TArrayBaseMutableDefaultStackAllocatorWeakImpl&& Other) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl(const UAllocator& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl& operator=(const UAllocator& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl(UAllocator&& Allocator) noexcept;

    template <typename U>
    FORCEINLINE explicit TArrayBaseMutableDefaultStackAllocatorWeakImpl(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>);
    template <typename U>
    FORCEINLINE TArrayBaseMutableDefaultStackAllocatorWeakImpl& operator=(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>);

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackAllocatorWeakImpl(UIterator Begin, const VIterator End) noexcept
        requires(std::is_constructible_v<T, const typename TIteratorTraits<UIterator>::value_type&>);

    FORCEINLINE constexpr void Empty() noexcept;

    FORCEINLINE bool Drop() noexcept;
    FORCEINLINE SizeType Drop(const SizeType Count) noexcept;

    alignas(T)
    std::byte _Data[sizeof(T) * _Super::SizeCapacity];

private:

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

template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
struct TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl : public TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits>
{
    typedef TArrayBaseMutableDefaultAllocatorBase<TIn, TTraits> _Super;

    typedef typename _Super::SizeType  SizeType;
    typedef typename _Super::T         T;
    typedef typename _Super::Pointer   Pointer;
    typedef typename _Super::Reference Reference;

    typedef typename _Super::Traits Traits;

    NODISCARD
    FORCEINLINE consteval static bool IsWeak() noexcept { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsStrong() noexcept { return true; }

    NODISCARD
    FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept;

    static constexpr SizeType SizeCapacity { TTraits::SizeCapacity };
    static_assert(TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity > 0);

    FORCEINLINE constexpr void _ResetToDefaultState() noexcept
    {
        this->Data  = reinterpret_cast<Pointer>(this->_Data);
        this->Slack = this->Data;
        this->End   = this->Data + TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity;

        return;
    }

    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl() noexcept
        : _Data{}
    {
        this->_ResetToDefaultState();

        return;
    }

    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(const TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& operator=(const TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl&& Other) noexcept;
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& operator=(TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl&& Other) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(const UAllocator& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
        requires
        (
            requires
            {
                typename UAllocator::T;
                typename UAllocator::Pointer;
                requires std::constructible_from<T, typename UAllocator::T>;
            }
            &&
            requires(UAllocator Allocator)
            {
                { Allocator.Data };
                { Allocator.Slack };
            }
        )
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& operator=(const UAllocator& Allocator) noexcept;

    template <TArrayBaseAllocatorConceptBase UAllocator>
    requires
    (
        requires
        {
            typename UAllocator::T;
            typename UAllocator::Pointer;
            requires std::constructible_from<T, typename UAllocator::T>;
        }
        &&
        requires(UAllocator Allocator)
        {
            { Allocator.Data };
            { Allocator.Slack };
        }
    )
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(UAllocator&& Allocator) noexcept;

    template <typename U>
    FORCEINLINE explicit constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>);
    template <typename U>
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl& operator=(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<T, const U&>);

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE constexpr TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl(UIterator Begin, const VIterator End) noexcept
        requires(std::is_constructible_v<T, const typename TIteratorTraits<UIterator>::value_type&>);

    FORCEINLINE ~TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl() noexcept;

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
    FORCEINLINE void ShrinkTo(const SizeType Count) noexcept;

    //#
    //# Destructs all elements and orphans the current allocated memory if it is on the heap.
    //#
    FORCEINLINE void Orphan() noexcept;

    FORCEINLINE bool Drop() noexcept;
    FORCEINLINE SizeType Drop(const SizeType Count) noexcept;

    alignas(T)
    std::byte _Data[sizeof(T) * TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl::SizeCapacity];

private:

    FORCEINLINE void GrowImpl(const SizeType Count) noexcept;
    FORCEINLINE void ShrinkToImpl(const SizeType Count) noexcept;
    FORCEINLINE void ResizeEmtpy(const SizeType Count) noexcept;

    FORCEINLINE static Pointer AlignedAlloc(const SizeType Count) noexcept;
    FORCEINLINE void AlignedRealloc(const SizeType Count) noexcept;

    FORCEINLINE void OrphanImpl() noexcept;

    FORCEINLINE constexpr void Destruct() noexcept;
    FORCEINLINE constexpr void DestructImpl() noexcept;
};

} /* ~Namespace Lal */
