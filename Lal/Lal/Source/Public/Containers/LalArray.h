// Copyright mzoesch. All rights reserved.

#pragma once

namespace Lal
{

//# @see Containers/ContainerForward.h
template <TArrayBaseAllocatorConceptBase TAllocator>
class TArrayBase
{
public:

    typedef TAllocator                         Allocator;
    typedef typename Allocator::Traits         Traits;
    typedef typename Allocator::T              T;
    typedef typename Allocator::SizeType       SizeType;
    typedef typename Allocator::Iterator       Iterator;
    typedef typename Allocator::ConstIterator  ConstIterator;
    typedef typename Allocator::MoveIterator   MoveIterator;
    typedef typename Allocator::Pointer        Pointer;
    typedef typename Allocator::ConstPointer   ConstPointer;
    typedef typename Allocator::Reference      Reference;
    typedef typename Allocator::ConstReference ConstReference;

    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TMemberField* MemberField);
    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TArrayBase<TMemberField>* MemberField);

    template <TArrayBaseAllocatorConceptBase UAllocator>
    friend class TArrayBase;

    NODISCARD
    FORCEINLINE consteval static bool IsStronglyAllocated() noexcept requires(requires { Allocator::IsStrong; }) { return Allocator::IsStrong(); }
    NODISCARD
    FORCEINLINE consteval static bool IsStronglyAllocated() noexcept requires(!requires { Allocator::IsStrong; }) { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsWeaklyAllocated() noexcept requires(requires { Allocator::IsWeak; }) { return Allocator::IsWeak(); }
    NODISCARD
    FORCEINLINE consteval static bool IsWeaklyAllocated() noexcept requires(!requires { Allocator::IsWeak; }) { return false; }
    static_assert(TArrayBase::IsStronglyAllocated() != TArrayBase::IsWeaklyAllocated());

    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept requires(requires { Allocator::IsContentConst; }) { return Allocator::IsContentConst(); }
    NODISCARD
    FORCEINLINE consteval static bool IsContentConst() noexcept requires(!requires { Allocator::IsContentConst; }) { return false; }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept requires(requires { Allocator::IsContentMutable; }) { return Allocator::IsContentMutable(); }
    NODISCARD
    FORCEINLINE consteval static bool IsContentMutable() noexcept requires(!requires { Allocator::IsContentMutable; }) { return false; }
    static_assert(TArrayBase::IsContentConst() != TArrayBase::IsContentMutable());

    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept requires(requires { Allocator::IsAllowedToPushItems; }) { return Allocator::IsAllowedToPushItems(); }
    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPushItems() noexcept requires(!requires { Allocator::IsAllowedToPushItems; }) { return true; }
    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPopItemsInBetween() noexcept requires(requires { Allocator::IsAllowedToPopItemsInBetween; }) { return Allocator::IsAllowedToPopItemsInBetween(); }
    NODISCARD
    FORCEINLINE consteval static bool IsAllowedToPopItemsInBetween() noexcept requires(!requires { Allocator::IsAllowedToPopItemsInBetween; }) { return IsAllowedToPushItems(); }

    FORCEINLINE constexpr TArrayBase() noexcept = default;

    FORCEINLINE constexpr TArrayBase(const TArrayBase& Other) noexcept
        requires(std::is_constructible_v<Allocator, const Allocator&>);
    FORCEINLINE constexpr TArrayBase& operator=(const TArrayBase& Other) noexcept
        requires(std::assignable_from<Allocator&, const Allocator&>);

    FORCEINLINE constexpr TArrayBase(TArrayBase&& Other) noexcept
        requires(std::is_constructible_v<Allocator, Allocator&&>);
    FORCEINLINE constexpr TArrayBase& operator=(TArrayBase&& Other) noexcept
        requires(std::assignable_from<Allocator&, Allocator&&>);

    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE explicit constexpr TArrayBase(const TArrayBase<UAllocator>& Other) noexcept
        requires(std::is_constructible_v<Allocator, const UAllocator&>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE constexpr TArrayBase& operator=(const TArrayBase<UAllocator>& Other) noexcept
        requires(std::assignable_from<Allocator&, const UAllocator&>);

    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE explicit constexpr TArrayBase(TArrayBase<UAllocator>&& Other) noexcept
        requires(std::is_constructible_v<Allocator, UAllocator&&>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE constexpr TArrayBase& operator=(TArrayBase<UAllocator>&& Other) noexcept
        requires(std::assignable_from<Allocator&, UAllocator&&>);

    //#
    //# Delete any r-value move conversion constructors and assignment operators that would otherwise implicitly
    //# convert to l-value references of the allocator type, which may cause double implicit conversions of the
    //# underlying allocator type. Notably, moves stack pointers to the heap or double owned heap pointers from
    //# extraordinary weak allocators.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE explicit constexpr TArrayBase(TArrayBase<UAllocator>&& Other) noexcept
        requires(std::is_constructible_v<Allocator, UAllocator&&> == false) = delete;
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE constexpr TArrayBase& operator=(TArrayBase<UAllocator>&& Other) noexcept
       requires(std::assignable_from<Allocator&, UAllocator&&> == false) = delete;

    //# Non templated ctor for recursive implicit bracket initialization.
    FORCEINLINE constexpr TArrayBase(const std::initializer_list<T> List) noexcept
        requires(std::is_constructible_v<Allocator, std::initializer_list<T>>);
    template <typename U>
    FORCEINLINE constexpr TArrayBase(const std::initializer_list<U> List) noexcept
        requires(std::is_constructible_v<Allocator, std::initializer_list<U>>);
    template <typename U>
    FORCEINLINE constexpr TArrayBase& operator=(const std::initializer_list<U> List) noexcept
        requires(std::assignable_from<Allocator&, std::initializer_list<U>>);

    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE constexpr TArrayBase(const UIterator Begin, const VIterator End) noexcept
        requires(std::is_constructible_v<Allocator, UIterator, VIterator>);
    template <TIteratorConcept UIterator>
    FORCEINLINE constexpr TArrayBase(const UIterator Begin, const SizeType Length) noexcept
        requires(std::is_constructible_v<Allocator, UIterator, UIterator>);

    FORCEINLINE constexpr ~TArrayBase() noexcept = default;

    NODISCARD FORCEINLINE constexpr SizeType GetSize() const noexcept { return this->Impl.Slack - this->Impl.Data; }
    NODISCARD FORCEINLINE constexpr bool     IsEmpty() const noexcept { return this->Impl.Data == this->Impl.Slack; }
    NODISCARD FORCEINLINE constexpr SizeType GetSizeInBytes() const noexcept { return this->GetSize() * static_cast<SizeType>(sizeof(T)); }
    NODISCARD FORCEINLINE constexpr SizeType GetCapacity() const noexcept { return this->Impl.End - this->Impl.Data; }
    NODISCARD FORCEINLINE constexpr SizeType GetAllocatedByteSize() const noexcept { return this->GetCapacity() * static_cast<SizeType>(sizeof(T)); }
    NODISCARD FORCEINLINE constexpr bool IsDataValid() const noexcept { return this->Impl.Data != nullptr; }
    NODISCARD FORCEINLINE constexpr bool IsSlackValid() const noexcept { return this->Impl.Slack != nullptr; }
    NODISCARD FORCEINLINE constexpr bool IsCapacityValid() const noexcept { return this->Impl.End != nullptr; }
    NODISCARD FORCEINLINE constexpr bool HasReachedCapacity() const noexcept { return this->Impl.Slack == this->Impl.End; }
    NODISCARD FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept requires(requires(Allocator _Allocator) { _Allocator.IsCurrentDataOnHeap(); }) { return this->Impl.IsCurrentDataOnHeap(); }
    NODISCARD FORCEINLINE constexpr bool IsCurrentDataOnHeap() const noexcept requires(!requires(Allocator _Allocator) { _Allocator.IsCurrentDataOnHeap(); }) { return this->GetDataPointer() != nullptr; }

    NODISCARD FORCEINLINE constexpr ConstPointer GetDataPointer() const noexcept { return this->Impl.Data; }
    NODISCARD FORCEINLINE constexpr Pointer      GetDataPointer() noexcept requires(TArrayBase::IsContentMutable()) { return this->Impl.Data; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetSlackPointer() const noexcept { return this->Impl.Slack; }
    NODISCARD FORCEINLINE constexpr Pointer      GetSlackPointer() noexcept requires(TArrayBase::IsContentMutable()) { return this->Impl.Slack; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetCapacityPointer() const noexcept { return this->Impl.End; }
    NODISCARD FORCEINLINE constexpr Pointer      GetCapacityPointer() noexcept requires(TArrayBase::IsContentMutable()) { return this->Impl.End; }

    NODISCARD FORCEINLINE constexpr bool IsValidIterator(const ITERATOR It) const noexcept { return std::to_address(It) >= this->GetDataPointer() && std::to_address(It) < this->GetSlackPointer(); }
    NODISCARD FORCEINLINE constexpr bool IsValidIndex(const SizeType Index) const noexcept requires( std::is_signed_v<SizeType>) { return Index >= 0 && Index < this->GetSize(); }
    NODISCARD FORCEINLINE constexpr bool IsValidIndex(const SizeType Index) const noexcept requires(!std::is_signed_v<SizeType>) { return Index < this->GetSize(); }
    NODISCARD FORCEINLINE constexpr bool IsValidEndIterator(const ITERATOR It) const noexcept { return std::to_address(It) >= this->GetDataPointer() && std::to_address(It) <= this->GetSlackPointer(); }

    NODISCARD FORCEINLINE constexpr bool IsIteratorInCapacityRange(const ITERATOR It) const noexcept { return std::to_address(It) >= this->GetDataPointer() && std::to_address(It) < this->GetCapacityPointer(); }

    NODISCARD FORCEINLINE constexpr Reference      operator[](const SizeType Index) noexcept requires(TArrayBase::IsContentMutable());
    NODISCARD FORCEINLINE constexpr ConstReference operator[](const SizeType Index) const noexcept;

    FORCEINLINE constexpr Iterator begin() noexcept { return Iterator{ this->Impl.Data  }; }
    FORCEINLINE constexpr Iterator end() noexcept { return Iterator{ this->Impl.Slack }; }
    FORCEINLINE constexpr ConstIterator begin() const noexcept { return ConstIterator{ this->Impl.Data  }; }
    FORCEINLINE constexpr ConstIterator end() const noexcept { return ConstIterator{ this->Impl.Slack }; }
    FORCEINLINE constexpr ConstIterator cbegin() const noexcept { return ConstIterator{ this->Impl.Data  }; }
    FORCEINLINE constexpr ConstIterator cend() const noexcept { return ConstIterator{ this->Impl.Slack }; }
    FORCEINLINE constexpr MoveIterator move_begin() noexcept requires(TArrayBase::IsContentMutable()) { return MoveIterator{ this->Impl.Data }; }
    FORCEINLINE constexpr MoveIterator move_end() noexcept requires(TArrayBase::IsContentMutable()) { return MoveIterator{ this->Impl.Slack }; }

    FORCEINLINE constexpr Pointer begin_ptr() noexcept { return this->GetDataPointer(); }
    FORCEINLINE constexpr Pointer end_ptr() noexcept { return this->GetSlackPointer(); }
    FORCEINLINE constexpr ConstPointer begin_ptr() const noexcept { return this->GetDataPointer(); }
    FORCEINLINE constexpr ConstPointer end_ptr() const noexcept { return this->GetSlackPointer(); }

    FORCEINLINE constexpr SizeType end_idx() const noexcept { return this->GetSize(); }

    FORCEINLINE constexpr auto Iter() noexcept requires(requires { typename Iterator::Factory; });
    FORCEINLINE constexpr auto CIter() const noexcept requires(requires { typename ConstIterator::Factory; });
    FORCEINLINE constexpr auto MIter() noexcept requires(TArrayBase::IsContentMutable() && requires { typename MoveIterator::Factory; });

    //# @return First element or nullptr.
    NODISCARD FORCEINLINE constexpr Pointer GetFirst()         noexcept requires(TArrayBase::IsContentMutable()) { return this->GetSize() > 0 ? this->GetDataPointer() : nullptr; }
    NODISCARD FORCEINLINE constexpr Pointer GetFirstChecked()  noexcept requires(TArrayBase::IsContentMutable());
    NODISCARD FORCEINLINE constexpr Pointer GetFirstAsserted() noexcept requires(TArrayBase::IsContentMutable());
    NODISCARD FORCEINLINE constexpr ConstPointer GetFirst()         const noexcept { return this->GetSize() > 0 ? this->GetDataPointer() : nullptr; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetFirstChecked()  const noexcept;
    NODISCARD FORCEINLINE constexpr ConstPointer GetFirstAsserted() const noexcept;

    //# @return Last element or nullptr.
    NODISCARD FORCEINLINE constexpr Pointer GetLast()         noexcept requires(TArrayBase::IsContentMutable()) { return this->GetSize() > 0 ? this->GetSlackPointer() - 1 : nullptr; }
    NODISCARD FORCEINLINE constexpr Pointer GetLastChecked()  noexcept requires(TArrayBase::IsContentMutable());
    NODISCARD FORCEINLINE constexpr Pointer GetLastAsserted() noexcept requires(TArrayBase::IsContentMutable()) ;
    NODISCARD FORCEINLINE constexpr ConstPointer GetLast()         const noexcept { return this->GetSize() > 0 ? this->GetSlackPointer() - 1 : nullptr; }
    NODISCARD FORCEINLINE constexpr ConstPointer GetLastChecked()  const noexcept;
    NODISCARD FORCEINLINE constexpr ConstPointer GetLastAsserted() const noexcept;

    //#
    //# Growths the array so that it can hold at least the given number of elements.
    //# This operation cannot cause a shrink under the hood.
    //#
    FORCEINLINE constexpr void Reserve(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated());
    FORCEINLINE constexpr void ReserveAdditionally(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated()) { this->Reserve(this->GetSize() + Count); }

    //#
    //# Will clear out all elements in the array and set the size to zero.
    //# Then it will try to grow or shrink the array to the given number of elements.
    //#
    FORCEINLINE constexpr void Reset(const SizeType Count, const bool bAllowShrink = true) noexcept requires(TArrayBase::IsStronglyAllocated());

    //#
    //# Resizes the array so that it can hold at least the given number of elements.
    //# This operation may cause a shrink under the hood.
    //# If the array is bigger than the given count, it will be shrunk to the number of elements it currently holds.
    //#
    //# @return The number of elements reserved by this array.
    //#
    FORCEINLINE constexpr SizeType Resize(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated());

    //#
    //# Shrinks the array to current number of elements it currently holds.
    //#
    FORCEINLINE constexpr void ShrinkToFit() noexcept requires(TArrayBase::IsStronglyAllocated());

    //#
    //# Completely empties the array, sets its size to zero, and if possible, orphans the underlying memory.
    //#
    FORCEINLINE constexpr void Empty() noexcept;

    //#
    //# Swaps the underlying buffers of this array with the other array. If the buffers are stack allocated, then this
    //# operation may cause a cross-copy of the data.
    //#
    FORCEINLINE void SwapBuffers(TArrayBase* Other) noexcept requires(requires(Allocator _AllocatorA, Allocator _AllocatorB) { _AllocatorA.SwapBuffers(&_AllocatorB); });

    //#
    //# Swap two indices in the array.
    //#
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void SwapIndices(const UIterator InA, const VIterator InB) noexcept requires(Allocator::IsContentMutable());
    FORCEINLINE void SwapIndices(const SizeType  InA, const SizeType  InB) noexcept requires(Allocator::IsContentMutable());

    //#
    //# Do not use std operators as...
    //#    - they are ambiguous in terms of meaning (compare by size, value or reference?).
    //#    - we might accidentally do comparisons inside templated paths of arrays that are very expensive in terms of
    //#      runtime performance.
    //# Better be explicit about it with the named functions #EqualInSizeTo, #IsSameArray and #IsDataEqual.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE bool operator==(const TArrayBase<UAllocator>& Other) const noexcept = delete;
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE bool operator!=(const TArrayBase<UAllocator>& Other) const noexcept = delete;
    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering operator<=>(const TArrayBase<UAllocator>& Other) const noexcept = delete;

    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD
    FORCEINLINE constexpr std::strong_ordering CompareSize(const TArrayBase<UAllocator>& Other) const noexcept { return this->GetSize() <=> Other.GetSize(); }

    //#
    //# Checks if both instances point to the same memory location.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD
    FORCEINLINE constexpr bool IsSameArray(const TArrayBase<UAllocator>& Other) const noexcept { return this->GetDataPointer() != nullptr && this->GetDataPointer() == Other.GetDataPointer(); }

    //#
    //# Checks if both instances have the same meaningful data. The capacity is not checked.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD FORCEINLINE constexpr bool IsDataEqual(const TArrayBase<UAllocator>& Other) const noexcept;
    template <TArrayBaseAllocatorConceptBase UAllocator>
    NODISCARD FORCEINLINE constexpr bool IsDataUnequal(const TArrayBase<UAllocator>& Other) const noexcept { return this->IsDataEqual(Other) == false; }

    //# Pushes the specified element to the array.
    constexpr void Add(const T& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr void Add(T&& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());

    // Pushes the default constructed element to the array.
    constexpr void AddDefaulted() noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_default_constructible_v<T>);
    constexpr void AddDefaulted(const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_default_constructible_v<T>);

    //# Pushes an element to the array that is zeroed out.
    constexpr void AddZeroed() noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr void AddZeroed(const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems());

    //# Pushes an uninitialized element to the array. The memory's content is undefined and must be initialized by hand afterward.
    constexpr void AddUninitialized() noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr void AddUninitialized(const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems());

    //# @return The iterator of the newly added element.
    template <TIteratorConcept UIterator>
    FORCEINLINE constexpr Iterator AddAt(const UIterator It, const T& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr Iterator AddAt(const SizeType Index, const T& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());

    //# @return The iterator of the newly added element.
    template <TIteratorConcept UIterator>
    FORCEINLINE constexpr Iterator AddAt(const UIterator It, T&& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());
    constexpr Iterator AddAt(const SizeType Index, T&& Element) noexcept requires(TArrayBase::IsAllowedToPushItems());

    //#
    //# Adds a new element to the array and constructs it in place while potentially reallocating the whole
    //# array to fit.
    //#
    template <typename... TArgs>
    void Emplace(TArgs&&... Args) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);
    template <typename... TArgs>
    void EmplaceMinimalGrowth(TArgs&&... Args) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);

    //#
    //# Adds a new element to the array and constructs it in place while potentially reallocating the whole
    //# array to fit.
    //# @return The iterator of the newly added element.
    //#
    template <TIteratorConcept UIterator, typename... TArgs>
    FORCEINLINE Iterator EmplaceAt(const UIterator It, TArgs&&... Args) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);
    template <typename... TArgs>
    Iterator EmplaceAt(const SizeType Index, TArgs&&... Args) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);

    //#
    //# Appends new elements to the array while potentially reallocating the whole array to fit.
    //#
    template <TArrayBaseAllocatorConceptBase UAllocator>
    void Append(const TArrayBase<UAllocator>& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const typename UAllocator::T&>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    void Append(TArrayBase<UAllocator>&& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T&&>);
    template <typename U>
    void Append(const std::initializer_list<U> List) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>);
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    void Append(UIterator Begin, const VIterator End) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<UIterator>::reference>);
    template <TIteratorConcept UIterator>
    FORCEINLINE void Append(const UIterator Begin, const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<UIterator>::reference>);

    //#
    //# Inserts new elements to the array at the specified location, while potentially reallocating the whole
    //# array to fit.
    //# @return The iterator to the first newly inserted element. If the range is empty, the iterator will point
    //#         to the slack.
    //#
    template <TIteratorConcept UIterator, TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE Iterator AppendAt(const UIterator It, const TArrayBase<UAllocator>& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const typename UAllocator::T&>);
    template <TIteratorConcept UIterator, TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE Iterator AppendAt(const UIterator It, TArrayBase<UAllocator>&& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T&&>);
    template <TIteratorConcept UIterator, typename U>
    FORCEINLINE Iterator AppendAt(const UIterator It, const std::initializer_list<U> List) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>);
    template <TIteratorConcept UIterator, TIteratorConcept VIterator, TIteratorConcept WIterator> requires(TIteratorPairConcept<VIterator, WIterator>)
    FORCEINLINE Iterator AppendAt(const UIterator It, const VIterator Begin, const WIterator End) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<VIterator>::reference>);
    template <TIteratorConcept UIterator, TIteratorConcept VIterator>
    FORCEINLINE Iterator AppendAt(const UIterator It, const VIterator Begin, const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<VIterator>::reference>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    FORCEINLINE Iterator AppendAt(const SizeType Index, const TArrayBase<UAllocator>& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const typename UAllocator::T&>);
    template <TArrayBaseAllocatorConceptBase UAllocator>
    Iterator AppendAt(const SizeType Index, TArrayBase<UAllocator>&& Other) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename UAllocator::T&&>);
    template <typename U>
    FORCEINLINE Iterator AppendAt(const SizeType Index, const std::initializer_list<U> List) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, const U&>);
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    Iterator AppendAt(const SizeType Index, UIterator Begin, const VIterator End) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<UIterator>::reference>);
    template <TIteratorConcept UIterator>
    FORCEINLINE Iterator AppendAt(const SizeType Index, const UIterator Begin, const SizeType Count) noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, typename TIteratorTraits<UIterator>::reference>);

    template <TIteratorConcept UIterator>
    FORCEINLINE void RemoveAt(const UIterator It) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    //# Inclusive Begin and exclusive End.
    template <TIteratorConcept UIterator, TIteratorConcept VIterator> requires(TIteratorPairConcept<UIterator, VIterator>)
    FORCEINLINE void RemoveAt(const UIterator Begin, const VIterator End) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    FORCEINLINE void RemoveAt(const SizeType Index) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    //# Inclusive Begin and exclusive End.
    FORCEINLINE void RemoveAt(const SizeType Begin, const SizeType End) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());

    //# @return The number of elements removed.
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType Remove(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType RemoveAtLeastOnceChecked(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType RemoveAtLeastOnceAsserted(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());

    template <std::predicate<T> TPredicate>
    SizeType RemoveByPredicate(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType RemoveByPredicateAtLeastOnceChecked(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType RemoveByPredicateAtLeastOnceAsserted(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());

    //#
    //# Removes the first occurrence of the specified element from the array.
    //# @return True if the element was found and removed, false otherwise.
    //#
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    bool RemoveOnce(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool RemoveOnceChecked(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool RemoveOnceAsserted(const U& What) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());

    template <std::predicate<T> TPredicate>
    bool RemoveOnceByPredicate(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <std::predicate<T> TPredicate>
    FORCEINLINE bool RemoveOnceByPredicateChecked(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());
    template <std::predicate<T> TPredicate>
    FORCEINLINE bool RemoveOnceByPredicateAsserted(const TPredicate& Predicate) noexcept requires(TArrayBase::IsAllowedToPopItemsInBetween());

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    Iterator Find(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    ConstIterator Find(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Iterator FindChecked(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstIterator FindChecked(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Iterator FindAsserted(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstIterator FindAsserted(const U& What) const noexcept;

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndex(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndex(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndexChecked(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndexChecked(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndexAsserted(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindIndexAsserted(const U& What) const noexcept;

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindRef(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindRef(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindRefChecked(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindRefChecked(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindRefAsserted(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindRefAsserted(const U& What) const noexcept;

    template <std::predicate<T> TPredicate>
    Iterator FindByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    ConstIterator FindByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindByPredicateChecked(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindByPredicateChecked(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindByPredicateAsserted(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindByPredicateAsserted(const TPredicate& Predicate) const noexcept;

    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicateChecked(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicateChecked(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicateAsserted(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindIndexByPredicateAsserted(const TPredicate& Predicate) const noexcept;

    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindRefByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindRefByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindRefByPredicateChecked(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindRefByPredicateChecked(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindRefByPredicateAsserted(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindRefByPredicateAsserted(const TPredicate& Predicate) const noexcept;

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    Iterator FindLast(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    ConstIterator FindLast(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Iterator FindLastChecked(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstIterator FindLastChecked(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Iterator FindLastAsserted(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstIterator FindLastAsserted(const U& What) const noexcept;

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndex(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndex(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndexChecked(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndexChecked(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndexAsserted(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType FindLastIndexAsserted(const U& What) const noexcept;

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindLastRef(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindLastRef(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindLastRefChecked(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindLastRefChecked(const U& What) const noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE Pointer FindLastRefAsserted(const U& What) noexcept;
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE ConstPointer FindLastRefAsserted(const U& What) const noexcept;

    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindLastByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindLastByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindLastByPredicateChecked(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindLastByPredicateChecked(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Iterator FindLastByPredicateAsserted(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstIterator FindLastByPredicateAsserted(const TPredicate& Predicate) const noexcept;

    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicateChecked(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicateChecked(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicateAsserted(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE SizeType FindLastIndexByPredicateAsserted(const TPredicate& Predicate) const noexcept;

    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindLastRefByPredicate(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindLastRefByPredicate(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindLastRefByPredicateChecked(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindLastRefByPredicateChecked(const TPredicate& Predicate) const noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE Pointer FindLastRefByPredicateAsserted(const TPredicate& Predicate) noexcept;
    template <std::predicate<T> TPredicate>
    FORCEINLINE ConstPointer FindLastRefByPredicateAsserted(const TPredicate& Predicate) const noexcept;

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool Contains(const U& What) const noexcept { return this->Find(What) != this->cend(); }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool ContainsChecked(const U& What) const noexcept { const ConstIterator It { this->FindChecked(What) }; return It != this->cend(); }
    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE bool ContainsAsserted(const U& What) const noexcept { const ConstIterator It { this->FindAsserted(What) }; return It != this->cend(); }

    template <std::predicate<T> TPredicate>
    FORCEINLINE bool ContainsByPredicate(const TPredicate& Predicate) const noexcept { return this->FindByPredicate(Predicate) != this->cend(); }
    template <std::predicate<T> TPredicate>
    FORCEINLINE bool ContainsByPredicateChecked(const TPredicate& Predicate) const noexcept { const ConstIterator It { this->FindByPredicateChecked(Predicate) }; return It != this->cend(); }
    template <std::predicate<T> TPredicate>
    FORCEINLINE bool ContainsByPredicateAsserted(const TPredicate& Predicate) const noexcept { const ConstIterator It { this->FindByPredicateAsserted(Predicate) }; return It != this->cend(); }

    template <typename U> requires(Lal::TEqualityComparableLeft<T, U>)
    FORCEINLINE SizeType Count(const U& What) const noexcept;

    //# @return The number of elements replaced.
    template <typename U, typename V> requires(Lal::TEqualityComparableLeft<T, U> && std::assignable_from<T&, V>)
    FORCEINLINE SizeType Replace(const U& What, const V& Replacement) noexcept requires(TArrayBase::IsContentMutable());
    template <std::predicate<T> TPredicate, typename V> requires(std::assignable_from<T&, V>)
    FORCEINLINE SizeType ReplaceByPredicate(const TPredicate& Predicate, const V& Replacement) noexcept requires(TArrayBase::IsContentMutable());

    template <typename TPredicate> requires(std::invocable<TPredicate, T> && std::is_void_v<std::invoke_result_t<TPredicate, T>>)
    FORCEINLINE void ForEach(const TPredicate& Predicate) noexcept requires(TArrayBase::IsContentMutable());
    template <std::predicate<T> TPredicate>
    FORCEINLINE void ForEach(const TPredicate& Predicate) const noexcept;

    ///////////////////////////////////////////////////////////////////////////////
    // Stack Operations

    //# Peeks at the last element in the array. Returns nullptr if the array is empty.
    NODISCARD FORCEINLINE constexpr Pointer Peek()         noexcept requires(TArrayBase::IsContentMutable()) { return this->GetLast(); }
    NODISCARD FORCEINLINE constexpr Pointer PeekChecked()  noexcept requires(TArrayBase::IsContentMutable()) { return this->GetLastChecked(); }
    NODISCARD FORCEINLINE constexpr Pointer PeekAsserted() noexcept requires(TArrayBase::IsContentMutable()) { return this->GetLastAsserted(); }
    NODISCARD FORCEINLINE constexpr ConstPointer Peek()         const noexcept { return this->GetLast(); }
    NODISCARD FORCEINLINE constexpr ConstPointer PeekChecked()  const noexcept { return this->GetLastChecked(); }
    NODISCARD FORCEINLINE constexpr ConstPointer PeekAsserted() const noexcept { return this->GetLastAsserted(); }

    template <typename... TArgs>
    FORCEINLINE TArrayBase& Push(TArgs&&... Args) & noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);
    template <typename... TArgs>
    FORCEINLINE TArrayBase&& Push(TArgs&&... Args) && noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);
    template <typename... TArgs>
    FORCEINLINE TArrayBase& PushMinimalGrowth(TArgs&&... Args) & noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);
    template <typename... TArgs>
    FORCEINLINE TArrayBase&& PushMinimalGrowth(TArgs&&... Args) && noexcept requires(TArrayBase::IsAllowedToPushItems() && std::is_constructible_v<T, TArgs...>);

    //#
    //# Removes the last element from the array.
    //# @return True if an element was popped, false if the array was empty.
    //#
    //# @note This operation cannot cause a shrink under the hood.
    //#
    FORCEINLINE bool Pop() noexcept;
    //# Pops #Count elements from the end of the array.
    FORCEINLINE void Pop(SizeType Count) noexcept;

    // ~Stack Operations
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    // Fifo

    //#
    //# Drops the first element from the array.
    //# @return True if an element was dropped, false if the array was empty.
    //#
    //# @note This operation cannot cause a shrink under the hood.
    //#
    FORCEINLINE bool Drop() noexcept { return this->Impl.Drop(); }
    //# Drops #Count elements from the beginning of the array.
    FORCEINLINE SizeType Drop(const SizeType Count) noexcept { return this->Impl.Drop(Count); }

    // ~Fifo
    ///////////////////////////////////////////////////////////////////////////////

    NODISCARD
    FORCEINLINE const Allocator& GetAllocator() const noexcept { return this->Impl; }
    NODISCARD
    FORCEINLINE Allocator& GetMutableAllocator() noexcept { return this->Impl; }

private:

    //#
    //# We could use the TArrayBase::IsStronglyAllocated required clause here, but we want to give weakly allocated
    //# arrays also the chance to grow if they hunt a creation on demand allocation strategy.
    //#
    FORCEINLINE constexpr void GrowImpl() noexcept
        requires(TArrayBase::IsStronglyAllocated() || requires(Allocator _Allocator) { _Allocator.Grow(); })
    {
        this->Impl.Grow();
        return;
    }
    FORCEINLINE constexpr void GrowImpl() noexcept
        requires(!TArrayBase::IsStronglyAllocated() && !requires(Allocator _Allocator) { _Allocator.Grow(); });

    FORCEINLINE constexpr void GrowToImpl(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated())
    {
        this->Impl.GrowTo(Count);
        return;
    }
    FORCEINLINE constexpr void GrowToImpl(const SizeType Count) noexcept requires(!TArrayBase::IsStronglyAllocated());

    FORCEINLINE constexpr SizeType ResizeImpl(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated())
    {
        return this->Impl.Resize(Count);
    }
    FORCEINLINE constexpr SizeType ResizeImpl(const SizeType Count) noexcept requires(!TArrayBase::IsStronglyAllocated());

    FORCEINLINE constexpr void ShrinkToFitImpl() noexcept requires(TArrayBase::IsStronglyAllocated())
    {
        this->Impl.ShrinkToFit();
        return;
    }
    FORCEINLINE constexpr void ShrinkToFitImpl() noexcept requires(!TArrayBase::IsStronglyAllocated());

    FORCEINLINE constexpr void ShrinkToImpl(const SizeType Count) noexcept requires(TArrayBase::IsStronglyAllocated())
    {
        this->Impl.ShrinkTo(Count);
        return;
    }
    FORCEINLINE constexpr void ShrinkToImpl(const SizeType Count) noexcept requires(!TArrayBase::IsStronglyAllocated());

    FORCEINLINE constexpr void OrphanImpl() noexcept
        requires(TArrayBase::IsStronglyAllocated() || requires(Allocator _Allocator) { _Allocator.Orphan(); })
    {
        this->Impl.Orphan();
        return;
    }
    FORCEINLINE constexpr void OrphanImpl() noexcept
        requires(!TArrayBase::IsStronglyAllocated() && !requires(Allocator _Allocator) { _Allocator.Orphan(); });

    FORCEINLINE constexpr void EmptyImpl() noexcept
        requires(TArrayBase::IsStronglyAllocated() || requires(Allocator _Allocator) { _Allocator.Orphan(); })
    {
        this->Impl.Orphan();
        return;
    }
    FORCEINLINE constexpr void EmptyImpl() noexcept
        requires(!TArrayBase::IsStronglyAllocated() && !requires(Allocator _Allocator) { _Allocator.Orphan(); })
    {
        this->Impl.Empty();
        return;
    }

    template <TIteratorConcept UIterator>
    FORCEINLINE constexpr void DestroyAt(const UIterator It) noexcept
        requires(requires(Allocator _Allocator) { _Allocator.DestroyAt(std::to_address(It)); })
    {
        this->Impl.DestroyAt(std::to_address(It));
        return;
    }
    template <TIteratorConcept UIterator>
    FORCEINLINE constexpr void DestroyAt(const UIterator It) noexcept
        requires(!requires(Allocator _Allocator) { _Allocator.DestroyAt(std::to_address(It)); } && TArrayBase::IsContentMutable());

    FORCEINLINE constexpr void DestroyAt(const SizeType Index) noexcept
        requires(requires(Allocator _Allocator) { _Allocator.DestroyAt(Index); })
    {
        this->Impl.DestroyAt(Index);
        return;
    }
    FORCEINLINE constexpr void DestroyAt(const SizeType Index) noexcept
        requires(!requires(Allocator _Allocator) { _Allocator.DestroyAt(Index); } && TArrayBase::IsContentMutable());

    FORCEINLINE constexpr void DestroyAt(const Pointer Ptr) noexcept
    {
        this->DestroyAt(Iterator{ Ptr });
        return;
    }

    FORCEINLINE constexpr void Destruct() noexcept requires(TArrayBase::IsContentMutable() && TArrayBase::IsStronglyAllocated());

    Allocator Impl;
};

static_assert(TArrayBaseAllocatorTraitsConceptStrong<LArrayBaseAllocatorDefaultTraitsStrong>);
static_assert(TArrayBaseAllocatorTraitsConceptWeak<LArrayBaseAllocatorDefaultTraitsWeak>);
static_assert(TArrayBaseCappedAllocatorTraitsConceptWeak<TArrayBaseCappedAllocatorDefaultTraitsWeak<64>>);
static_assert(TArrayBaseCappedAllocatorTraitsConceptStrong<TArrayBaseCappedAllocatorDefaultTraitsStrong<64>>);

static_assert(TArrayBaseMutableAllocatorConceptStrong<TArrayBaseMutableDefaultAllocatorStrong<LPlatformTypes::LSize>>);
static_assert(TArrayBaseConstAllocatorConceptWeak<TArrayBaseConstDefaultAllocatorWeak<LPlatformTypes::LSize>>);
static_assert(TArrayBaseMutableAllocatorConceptWeak<TArrayBaseMutableDefaultAllocatorWeak<LPlatformTypes::LSize>>);
static_assert(TArrayBaseMutableCappedAllocatorConceptWeak<TArrayBaseMutableDefaultFixedAllocatorWeak<LPlatformTypes::LSize, 64>>);
static_assert(TArrayBaseMutableCappedAllocatorConceptWeak<TArrayBaseMutableDefaultStackAllocatorWeak<LPlatformTypes::LSize, 64>>);
static_assert(TArrayBaseMutableCapacityAllocatorConceptStrong<TArrayBaseMutableDefaultStackOptimizedAllocatorStrong<LPlatformTypes::LSize, 64>>);

} /* ~Namespace Lal */

#undef LAL_CHECK_ARRAY

static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<u8>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<u16>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<u32>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<u64>);

static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<i8>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<i16>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<i32>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<i64>);

static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<f32>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<f64>);

static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<LSize>);

static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<LAsciiChar>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<LJafgChar>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<LChar>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<LWideChar>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<LStringLegacy> == false);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<LWideString> == false);

template <typename T>
struct Lal::TArrayBaseAllowTrivialMemoryBufferMove<TArray<T>> : Lal::TrueType { };
template <typename T>
struct Lal::TArrayBaseAllowTrivialMemoryBufferMove<TArrayView<T>> : Lal::TrueType { };
template <typename T>
struct Lal::TArrayBaseAllowTrivialMemoryBufferMove<TMutableArrayView<T>> : Lal::TrueType { };
template <typename T, LSize TSizeCapacity>
struct Lal::TArrayBaseAllowTrivialMemoryBufferMove<TFixedArray<T, TSizeCapacity>> : Lal::TrueType { };

static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<TArray<LSize>>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<TArrayView<LSize>>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<TMutableArrayView<LSize>>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<TFixedArray<LSize, 64>>);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<TStackArray<LSize, 64>> == false);
static_assert(Lal::TArrayBaseAllowTrivialMemoryBufferMove_v<TStackOptimizedArray<LSize, 64>> == false);
