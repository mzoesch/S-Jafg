// Copyright mzoesch. All rights reserved.

#pragma once

#if CHECK_CONTAINER_BOUNDS
    #define JAFG_CHECK_ARRAY(Expr)      jassert(Expr)
#else /* CHECK_CONTAINER_BOUNDS */
    #define JAFG_CHECK_ARRAY(Expr)
#endif /* !CHECK_CONTAINER_BOUNDS */

namespace Jafg
{

template <typename InSizeType>
struct TArrayAllocatorTraits
{
    typedef InSizeType SizeType;

    static_assert(std::is_integral_v<SizeType>, "SizeType must be an integral type.");
    static_assert(std::is_signed_v<SizeType>,   "SizeType must be a signed integral type.");

    FORCEINLINE static SizeType GetGrowSize(const SizeType InCurrent)
    {
        return InCurrent < 5 ? 5 : InCurrent < 20 ? 20 : InCurrent * 2;
    }
};

template <typename InT, typename InSizeType, typename InTraits>
struct TArrayAllocator
{
    typedef InT        T;
    typedef InSizeType SizeType;
    typedef InTraits   Traits;

    using Self = TArrayAllocator<T, SizeType, Traits>;

    static_assert(std::is_integral_v<SizeType>, "SizeType must be an integral type.");
    static_assert(std::is_signed_v<SizeType>,   "SizeType must be a signed integral type.");

    FORCEINLINE TArrayAllocator() noexcept : Data(), Slack(), End() { }
    FORCEINLINE TArrayAllocator(const Self& Other) noexcept;
    FORCEINLINE TArrayAllocator(Self&& Other) noexcept;

    FORCEINLINE Self& operator=(const Self& Other) noexcept;
    FORCEINLINE Self& operator=(Self&& Other) noexcept;

    void Grow() noexcept;
    void Grow(const SizeType InAmount) noexcept;

    void Shrink() noexcept;

    void Orphan() noexcept;

    /** The first element of the data or nullptr. */
    T* Data;
    /** The first element of the slack or nullptr. */
    T* Slack;
    /** The first element of unowned memory or nullptr. */
    T* End;
};

/**
 * An array container that may use any form of allocated memory.
 *
 * This container checks OOB accesses during debug and development builds but will not check in release builds.
 *
 * This array may not handle complex types that require move semantics as it was designed to be a fast and simple
 * container for extreme fast-paced memory read and write operations.
 */
template <typename InT, typename InAlloc>
class TArrayBase
{
public:

    using T        = InT;
    using Alloc    = InAlloc;
    using SizeType = typename Alloc::SizeType;
    using Traits   = typename Alloc::Traits;
    using Self     = TArrayBase<T, Alloc>;

    static_assert(std::is_integral_v<SizeType>, "SizeType must be an integral type.");
    static_assert(std::is_signed_v<SizeType>,   "SizeType must be a signed integral type.");

    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TMemberField* MemberField);
    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TArray<TMemberField>* MemberField);

    FORCEINLINE  TArrayBase() noexcept = default;
    FORCEINLINE  TArrayBase(const Self& InOther) noexcept : Impl() { Self::Copy(*this, InOther); }
    FORCEINLINE  TArrayBase(Self&& InOther) noexcept : Impl() { Self::Move(*this, std::move(InOther)); }
    FORCEINLINE  TArrayBase(std::initializer_list<T> InList) noexcept;
    FORCEINLINE ~TArrayBase() noexcept { this->Empty(); }

    FORCEINLINE Self& operator=(const Self& InOther) noexcept { Self::Copy(*this, InOther); return *this; }
    FORCEINLINE Self& operator=(Self&& InOther) noexcept { Self::Move(*this, std::move(InOther)); return *this; }
    FORCEINLINE Self& operator=(std::initializer_list<T> InList) noexcept;

    FORCEINLINE Self& CopyFrom(const Self& InOther) noexcept { Self::Copy(*this, InOther); return *this; }
    FORCEINLINE Self& CopyFrom(const Self& InOther, const SizeType InCount) noexcept;
    FORCEINLINE Self& CopyFrom(const Self& InOther, const SizeType InOffset, const SizeType InCount) noexcept;

    NODISCARD FORCEINLINE SizeType GetSize()     const noexcept { return this->Impl.Slack - this->Impl.Data;  }
    NODISCARD FORCEINLINE SizeType GetByteSize() const noexcept { return this->GetSize() * static_cast<SizeType>(sizeof(T)); }
    NODISCARD FORCEINLINE bool     IsEmpty()     const noexcept { return this->Impl.Data == this->Impl.Slack; }
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return this->Impl.End - this->Impl.Data;   }
    NODISCARD FORCEINLINE bool     IsData()      const noexcept { return this->Impl.Data  != nullptr; }
    NODISCARD FORCEINLINE bool     IsSlack()     const noexcept { return this->Impl.Slack != nullptr; }
    NODISCARD FORCEINLINE       T* GetData()           noexcept { return this->Impl.Data; }
    NODISCARD FORCEINLINE const T* GetData()     const noexcept { return this->Impl.Data; }
    NODISCARD FORCEINLINE       T* GetSlack()          noexcept { return this->Impl.Slack; }
    NODISCARD FORCEINLINE const T* GetSlack()    const noexcept { return this->Impl.Slack; }
    NODISCARD FORCEINLINE       T* GetFirst()          noexcept { return this->GetSize() > 0 ? this->Impl.Data  : nullptr; }
    NODISCARD FORCEINLINE const T* GetFirst()    const noexcept { return this->GetSize() > 0 ? this->Impl.Data  : nullptr; }
    NODISCARD FORCEINLINE       T* GetLast()           noexcept { return this->GetSize() > 0 ? this->Impl.Slack - 1: nullptr; }
    NODISCARD FORCEINLINE const T* GetLast()     const noexcept { return this->GetSize() > 0 ? this->Impl.Slack - 1: nullptr; }
    NODISCARD FORCEINLINE bool     IsCapped()    const noexcept { return this->Impl.Slack == this->Impl.End; }

    FORCEINLINE bool IsValidIndex(const SizeType InIndex) const noexcept { return InIndex > INDEX_NONE && InIndex < this->GetSize(); }

    FORCEINLINE       T& operator[](const SizeType InIndex)       noexcept;
    FORCEINLINE const T& operator[](const SizeType InIndex) const noexcept;

    /**
     * Will reserve memory for the array such that the buffer can hold at least InAmount elements.
     * This action cannot perform a shrink under the hood.
     */
    FORCEINLINE void Reserve(const SizeType InAmount) noexcept { this->Grow(InAmount); }

    /**
     * Will clear out all elements in the array and set the size to zero. It will not deallocate or reallocate
     * the current memory buffer unless the InReserve parameter is greater than the current capacity and growing
     * the current memory buffer is not possible.
     */
    FORCEINLINE void Reset(const SizeType InAmount) noexcept;

    /**
     * Try to shrink the array to the current size or reallocate the array to the new size.
     */
    FORCEINLINE void Shrink() noexcept { this->Impl.Shrink(); }

    /**
     * Resize the array to the new size. The new size has to be less or equal to the current size.
     * @param bInShrinkToFit Whether to shrink the array buffer to fit the new size.
     */
    FORCEINLINE void Resize(const SizeType InSize, const bool bInShrinkToFit);

    /**
     * Completely empties the array and sets the size to zero. The memory buffer will be orphaned.
     */
    FORCEINLINE void Empty() noexcept { this->Impl.Orphan(); }

    /**
     * Swaps the content buffers of this array with the other array.
     */
    FORCEINLINE void SwapBuffers(Self& InOther) noexcept;

    /**
     * Swap two indices in the array.
     */
    FORCEINLINE void SwapIndices(const SizeType InIndexA, const SizeType InIndexB) noexcept;

    /**
     * Do not use std operators as ...
     *    - they are ambiguous in terms of meaning (compare by size, value or reference?).
     *    - we might accidentally do comparisons inside templated paths of arrays that are very expensive in terms of
     *      runtime performance.
     * Better be explicit about it with the named functions #EqualSize, #IsSameArray and #IsDataEqual.
     */
    FORCEINLINE bool operator==(      Self& InOther)       noexcept = delete;
    FORCEINLINE bool operator==(const Self& InOther) const noexcept = delete;
    FORCEINLINE bool operator!=(      Self& InOther)       noexcept = delete;
    FORCEINLINE bool operator!=(const Self& InOther) const noexcept = delete;

    FORCEINLINE bool operator <(const Self& InOther) const noexcept { return this->GetSize()  < InOther.GetSize(); }
    FORCEINLINE bool operator >(const Self& InOther) const noexcept { return this->GetSize()  > InOther.GetSize(); }
    FORCEINLINE bool operator<=(const Self& InOther) const noexcept { return this->GetSize() <= InOther.GetSize(); }
    FORCEINLINE bool operator>=(const Self& InOther) const noexcept { return this->GetSize() >= InOther.GetSize(); }
    FORCEINLINE bool EqualSize (const Self& InOther) const noexcept { return this->GetSize() == InOther.GetSize(); }

    /**
     * Checks if both instances point to the same memory location.
     */
    NODISCARD FORCEINLINE bool IsSameArray(const Self& InOther) const noexcept { return this->Impl.Data == InOther.Impl.Data; }

    /**
     * Checks if both instances have the same meaningful data. The capacity is not checked.
     */
    NODISCARD FORCEINLINE bool IsDataEqual(const Self& InOther) const noexcept;
    NODISCARD FORCEINLINE bool IsDataUnequal(const Self& InOther) const noexcept { return !this->IsDataEqual(InOther); }

    /**
     * Add a new element to the array while potentially reallocating the whole array to fit.
     * @return The index of the newly added element.
     */
    SizeType Add(const T& InElement) noexcept;
    SizeType Add(T&& InElement) noexcept;
    void     AddAt(const SizeType InIndex, const T& InElement) noexcept;
    void     AddAt(const SizeType InIndex, T&& InElement) noexcept;
    SizeType AddDefault() noexcept;
    void     AddDefault(SizeType InCount) noexcept;
    SizeType AddZeroed() noexcept;
    void     AddZeroed(const SizeType InCount) noexcept;
    SizeType AddUninitialized() noexcept;
    void     AddUninitialized(const SizeType InCount) noexcept;

    /**
     * Adds a new element to the array and constructs it in place while potentially
     * reallocating the whole array to fit.
     * @return The index of the newly added element.
     */
    template <typename ... InTArgs>
    FORCEINLINE SizeType Emplace(InTArgs&&... InArgs) noexcept;
    template <typename ... InTArgs>
    FORCEINLINE void EmplaceAt(const SizeType InIndex, InTArgs&&... InArgs) noexcept;

    /**
     * Appends new elements to the array while potentially reallocating the whole array to fit.
     */
    void Append(const Self& InOther) noexcept;
    void Append(Self&& InOther) noexcept;
    void Append(const T* InElements, const SizeType InCount) noexcept;
    void AppendAt(const SizeType InIndex, const Self& InOther) noexcept;
    void AppendAt(const SizeType InIndex, Self&& InOther) noexcept;
    void AppendAt(      SizeType InIndex, const T* InElements, const SizeType InCount) noexcept;

    FORCEINLINE void RemoveAt(const SizeType InIndex) noexcept;
    FORCEINLINE void RemoveAt(const SizeType InIndex, const SizeType InCount) noexcept;

    FORCEINLINE SizeType Remove(const T& InElement) noexcept;
    FORCEINLINE bool     RemoveOnce(const T& InElement) noexcept;
    FORCEINLINE bool     RemoveOnceChecked(const T& InElement) noexcept { const bool bOut = this->RemoveOnce(InElement); check(bOut); return bOut; }
    FORCEINLINE bool     RemoveOnceAsserted(const T& InElement) noexcept { const bool bOut = this->RemoveOnce(InElement); jassert(bOut); return bOut; }

    template <typename InOtherElement>
    FORCEINLINE SizeType Remove(const InOtherElement& InElement) noexcept;
    template <typename InOtherElement>
    FORCEINLINE bool     RemoveOnce(const InOtherElement& InElement) noexcept;
    template <typename InOtherElement>
    FORCEINLINE bool     RemoveOnceChecked(const InOtherElement& InElement) noexcept { const bool bOut = this->RemoveOnce(InElement); check(bOut); return bOut; }
    template <typename InOtherElement>
    FORCEINLINE bool     RemoveOnceAsserted(const InOtherElement& InElement) noexcept { const bool bOut = this->RemoveOnce(InElement); jassert(bOut); return bOut; }

    template <typename Predicate>
    FORCEINLINE SizeType RemoveByPredicate(const Predicate& InPredicate) noexcept;
    template <typename Predicate>
    FORCEINLINE bool     RemoveOnceByPredicate(const Predicate& InPredicate) noexcept;
    template <typename Predicate>
    FORCEINLINE bool     RemoveOnceByPredicateChecked(const Predicate& InPredicate) noexcept { const bool bOut = this->RemoveOnceByPredicate(InPredicate); check(bOut); return bOut; }
    template <typename Predicate>
    FORCEINLINE bool     RemoveOnceByPredicateAsserted(const Predicate& InPredicate) noexcept { const bool bOut = this->RemoveOnceByPredicate(InPredicate); jassert(bOut); return bOut; }

    FORCEINLINE SizeType Find(const T& InElement, T** OutElement) noexcept;
    FORCEINLINE SizeType Find(const T& InElement, const T** OutElement) const noexcept { return const_cast<Self*>(this)->Find(InElement, OutElement); }
    FORCEINLINE SizeType Find(const T& InElement) const noexcept;

    template <typename InOtherElement>
    FORCEINLINE SizeType Find(const InOtherElement& InElement, T** OutElement) noexcept;
    template <typename InOtherElement>
    FORCEINLINE SizeType Find(const InOtherElement& InElement, const T** OutElement) const noexcept { return const_cast<Self*>(this)->Find(InElement, OutElement); }
    template <typename InOtherElement>
    FORCEINLINE SizeType Find(const InOtherElement& InElement) const noexcept;

    template <typename Predicate>
    FORCEINLINE SizeType FindByPredicate(const Predicate& InPredicate, T** OutElement) const noexcept;
    template <typename Predicate>
    FORCEINLINE SizeType FindByPredicate(const Predicate& InPredicate, const T** OutElement) const noexcept { return const_cast<Self*>(this)->FindByPredicate(InPredicate, OutElement); }
    template <typename Predicate>
    FORCEINLINE SizeType FindByPredicate(const Predicate& InPredicate) const noexcept;

    FORCEINLINE       T* FindRef(const T& InElement, SizeType* OutIndex) noexcept;
    FORCEINLINE const T* FindRef(const T& InElement, SizeType* OutIndex) const noexcept { return const_cast<Self*>(this)->FindRef(InElement, OutIndex); }
    FORCEINLINE       T* FindRef(const T& InElement) noexcept;
    FORCEINLINE const T* FindRef(const T& InElement) const noexcept { return const_cast<Self*>(this)->FindRef(InElement); }

    template <typename InOtherElement>
    FORCEINLINE      T* FindRef(const InOtherElement& InElement, SizeType* OutIndex) noexcept;
    template <typename InOtherElement>
    FORCEINLINE const T* FindRef(const InOtherElement& InElement, SizeType* OutIndex) const noexcept { return const_cast<Self*>(this)->FindRef(InElement, OutIndex); }
    template <typename InOtherElement>
    FORCEINLINE       T* FindRef(const InOtherElement& InElement) noexcept;
    template <typename InOtherElement>
    FORCEINLINE const T* FindRef(const InOtherElement& InElement) const noexcept { return const_cast<Self*>(this)->FindRef(InElement); }

    template <typename Predicate>
    FORCEINLINE       T* FindRefByPredicate(const Predicate& InPredicate, SizeType* OutIndex) noexcept;
    template <typename Predicate>
    FORCEINLINE const T* FindRefByPredicate(const Predicate& InPredicate, SizeType* OutIndex) const noexcept { return const_cast<Self*>(this)->FindRefByPredicate(InPredicate, OutIndex); }
    template <typename Predicate>
    FORCEINLINE       T* FindRefByPredicate(const Predicate& InPredicate) noexcept;
    template <typename Predicate>
    FORCEINLINE const T* FindRefByPredicate(const Predicate& InPredicate) const noexcept { return const_cast<Self*>(this)->FindRefByPredicate(InPredicate); }

    FORCEINLINE bool Contains(const T& InElement) const noexcept;
    template <typename InOtherElement>
    FORCEINLINE bool Contains(const InOtherElement& InElement) const noexcept;
    template <typename Predicate>
    FORCEINLINE bool ContainsByPredicate(const Predicate& InPredicate) const noexcept;

    /** Peeks at the last element in the array. Returns nullptr if the array is empty. */
    FORCEINLINE       T* Peek()       noexcept { return this->GetLast(); }
    FORCEINLINE const T* Peek() const noexcept { return this->GetLast(); }
    /** Removes the last element in the array. */
    FORCEINLINE bool Pop() noexcept;
    FORCEINLINE void Pop(SizeType InCount) noexcept;

    FORCEINLINE Iterator<T>       begin()       noexcept { return Iterator<T>      (this->Impl.Data);  }
    FORCEINLINE Iterator<const T> begin() const noexcept { return Iterator<const T>(this->Impl.Data);  }
    FORCEINLINE Iterator<T>       end()         noexcept { return Iterator<T>      (this->Impl.Slack); }
    FORCEINLINE Iterator<const T> end()   const noexcept { return Iterator<const T>(this->Impl.Slack); }

    FORCEINLINE       Alloc& GetUnderlyingDataStructure()       noexcept { return this->Impl; }
    FORCEINLINE const Alloc& GetUnderlyingDataStructure() const noexcept { return this->Impl; }

private:

    FORCEINLINE void Grow() noexcept { this->Impl.Grow(); }
    FORCEINLINE void Grow(const SizeType InAmount) noexcept { this->Impl.Grow(InAmount); }

    FORCEINLINE static void Copy(Self& Dst, const Self& Src) noexcept { Dst.Impl = Src.Impl; }
    FORCEINLINE static void Move(Self& Dst, Self&& Src) noexcept { Dst.Impl = std::move(Src.Impl); }

    FORCEINLINE void DestroyAt(const SizeType InIndex) noexcept;
    FORCEINLINE void DestroyAt(T* InAddress) noexcept;

    Alloc Impl;
};

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE TArrayAllocator<InT, InSizeType, InTraits>::TArrayAllocator(const Self& Other) noexcept : Data(), Slack(), End()
{
    JAFG_CHECK_ARRAY( this != &Other )
    this->Grow(Other.Slack - Other.Data);

    T* Me = this->Data;
    for (const T* RESTRICT Bulk = Other.Data; Bulk != Other.Slack ; ++Bulk)
    {
        std::construct_at(Me, *Bulk);
        ++Me;
    }
    this->Slack = Me;

    checkSlow( this->Slack <= this->End )

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE TArrayAllocator<InT, InSizeType, InTraits>::TArrayAllocator(Self&& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;
    this->End   = Other.End;

    Other.Data  = nullptr;
    Other.Slack = nullptr;
    Other.End   = nullptr;

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE typename TArrayAllocator<InT, InSizeType, InTraits>::Self& TArrayAllocator<InT, InSizeType, InTraits>::operator=(const Self& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    for (T* RESTRICT Bulk = this->Data; Bulk != this->Slack ; ++Bulk)
    {
        Bulk->~T();
    }
    this->Slack = this->Data;

    this->Grow(Other.Slack - Other.Data);

    T* Me = this->Data;
    for (const T* RESTRICT Bulk = Other.Data; Bulk != Other.Slack ; ++Bulk)
    {
        std::construct_at(Me, *Bulk);
        ++Me;
    }
    this->Slack = Me;

    checkSlow( this->Slack <= this->End )

    return *this;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE typename TArrayAllocator<InT, InSizeType, InTraits>::Self& TArrayAllocator<InT, InSizeType, InTraits>::operator=(Self&& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )
    this->Orphan();

    this->Data  = Other.Data;
    this->Slack = Other.Slack;
    this->End   = Other.End;

    Other.Data  = nullptr;
    Other.Slack = nullptr;
    Other.End   = nullptr;

    return *this;
}

template<typename InT, typename InSizeType, typename InTraits>
void TArrayAllocator<InT, InSizeType, InTraits>::Grow() noexcept
{
    const SizeType NewCapacity = Traits::GetGrowSize(this->End - this->Data);
    JAFG_CHECK_ARRAY( NewCapacity > 0 && NewCapacity >= this->End - this->Data )

    if (this->Data == nullptr)
    {
#if WITH_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
        #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        this->Data  = static_cast<T*>(::malloc(NewCapacity * sizeof(T)));
#if WITH_GCC
        #pragma GCC diagnostic pop
#endif /* WITH_GCC */

        this->Slack = this->Data;
        this->End   = this->Data + NewCapacity;

        JAFG_CHECK_ARRAY( this->Data )

        return;
    }

    const SizeType UsedSize = this->Slack - this->Data;

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    this->Data  = static_cast<T*>(::realloc(this->Data, NewCapacity * sizeof(T)));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    this->Slack = this->Data + UsedSize;
    this->End   = this->Data + NewCapacity;

    JAFG_CHECK_ARRAY( this->Data )

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
void TArrayAllocator<InT, InSizeType, InTraits>::Grow(const SizeType InAmount) noexcept
{
    if (InAmount <= this->End - this->Data)
    {
        return;
    }

    if (this->Data == nullptr)
    {
#if WITH_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
        #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        this->Data  = static_cast<T*>(::malloc(InAmount * sizeof(T)));
#if WITH_GCC
        #pragma GCC diagnostic pop
#endif /* WITH_GCC */

        this->Slack = this->Data;
        this->End   = this->Data + InAmount;

        JAFG_CHECK_ARRAY(this->Data)

        return;
    }

    const SizeType UsedSize = this->Slack - this->Data;

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    this->Data  = static_cast<T*>(::realloc(this->Data, InAmount * sizeof(T)));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    this->Slack = this->Data + UsedSize;
    this->End   = this->Data + InAmount;

    JAFG_CHECK_ARRAY(this->Data)

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
void TArrayAllocator<InT, InSizeType, InTraits>::Shrink() noexcept
{
    if (this->Data == nullptr)
    {
        return;
    }

    const SizeType UsedSize = this->Slack - this->Data;

    if (UsedSize == 0)
    {
        this->Orphan();
        return;
    }

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    this->Data  = static_cast<T*>(::realloc(this->Data, UsedSize * sizeof(T)));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    this->Slack = this->Data + UsedSize;
    this->End   = this->Data + UsedSize;

    JAFG_CHECK_ARRAY( this->Data )

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
void TArrayAllocator<InT, InSizeType, InTraits>::Orphan() noexcept
{
    if (this->Data == nullptr)
    {
        return;
    }

    for (T* RESTRICT Bulk = this->Data; Bulk != this->Slack ; ++Bulk)
    {
        Bulk->~T();
    }

    ::free(this->Data);

    this->Data  = nullptr;
    this->Slack = nullptr;
    this->End   = nullptr;

    return;
}

template<typename InT, typename InAlloc>
TArrayBase<InT, InAlloc>::TArrayBase(std::initializer_list<T> InList) noexcept : Impl()
{
    this->Reserve(InList.size());

    T* Me = this->Impl.Data;
    for (const T& Element : InList)
    {
        *Me = Element;
        ++Me;
    }

    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
typename TArrayBase<InT, InAlloc>::Self& TArrayBase<InT, InAlloc>::operator=(std::initializer_list<T> InList) noexcept
{
    this->Reset(InList.size());

    T* Me = this->Impl.Data;
    for (const T& Element : InList)
    {
        *Me = Element;
        ++Me;
    }

    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return *this;
}

template<typename InT, typename InAlloc>
typename TArrayBase<InT, InAlloc>::Self& TArrayBase<InT, InAlloc>::CopyFrom(const Self& InOther, const SizeType InCount) noexcept
{
    this->Reset(InCount);

    T* Me = this->Impl.Data;
    SizeType Added = 0;
    for (const T* RESTRICT Bulk = InOther.Impl.Data; Bulk != InOther.Impl.Slack && Added < InCount; ++Bulk, ++Added, ++Me)
    {
        *Me = *Bulk;
    }

    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return *this;
}

template<typename InT, typename InAlloc>
typename TArrayBase<InT, InAlloc>::Self& TArrayBase<InT, InAlloc>::CopyFrom(const Self& InOther, const SizeType InOffset, const SizeType InCount) noexcept
{
    this->Reset(InCount);

    T* Me = this->Impl.Data;
    SizeType Added = 0;
    for (const T* RESTRICT Bulk = InOther.Impl.Data + InOffset; Bulk != InOther.Impl.Slack && Added < InCount; ++Bulk, ++Added, ++Me)
    {
        *Me = *Bulk;
    }

    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return *this;
}

template<typename InT, typename InAlloc>
FORCEINLINE typename TArrayBase<InT, InAlloc>::T& TArrayBase<InT, InAlloc>::operator[](const SizeType InIndex) noexcept
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )
    return this->Impl.Data[InIndex];
}

template<typename InT, typename InAlloc>
FORCEINLINE const typename TArrayBase<InT, InAlloc>::T& TArrayBase<InT, InAlloc>::operator[](const SizeType InIndex) const noexcept
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )
    return this->Impl.Data[InIndex];
}

template<typename InT, typename InAlloc>
FORCEINLINE void TArrayBase<InT, InAlloc>::Reset(const SizeType InAmount) noexcept
{
    this->Reserve(InAmount);

    for (T* RESTRICT Bulk = this->Impl.Data; Bulk != this->Impl.Slack ; ++Bulk)
    {
        Bulk->~T();
    }

    this->Impl.Slack = this->Impl.Data;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
FORCEINLINE void TArrayBase<InT, InAlloc>::Resize(const SizeType InSize, const bool bInShrinkToFit)
{
    JAFG_CHECK_ARRAY( InSize >= 0 && InSize <= this->GetSize() )

    T* NewSlack = this->Impl.Data + InSize;
    for (T* RESTRICT Bulk = NewSlack; Bulk != this->Impl.Slack ; ++Bulk)
    {
        Bulk->~T();
    }
    this->Impl.Slack = NewSlack;

    if (bInShrinkToFit)
    {
        this->Shrink();
    }

    return;
}

template<typename InT, typename InAlloc>
FORCEINLINE void TArrayBase<InT, InAlloc>::SwapBuffers(Self& InOther) noexcept
{
    JAFG_CHECK_ARRAY( this != &InOther )

    T* TempData  = this->Impl.Data;
    T* TempSlack = this->Impl.Slack;
    T* TempEnd   = this->Impl.End;

    this->Impl.Data  = InOther.Impl.Data;
    this->Impl.Slack = InOther.Impl.Slack;
    this->Impl.End   = InOther.Impl.End;

    InOther.Impl.Data  = TempData;
    InOther.Impl.Slack = TempSlack;
    InOther.Impl.End   = TempEnd;

    return;
}

template<typename InT, typename InAlloc>
FORCEINLINE void TArrayBase<InT, InAlloc>::SwapIndices(const SizeType InIndexA, const SizeType InIndexB) noexcept
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndexA) && this->IsValidIndex(InIndexB) )

    if (InIndexA == InIndexB)
    {
        return;
    }

    alignas(T)
    u8 Temp[sizeof(T)];

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memcpy(Temp,                       this->Impl.Data + InIndexA, sizeof(T));
    ::memcpy(this->Impl.Data + InIndexA, this->Impl.Data + InIndexB, sizeof(T));
    ::memcpy(this->Impl.Data + InIndexB, Temp,                       sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    return;
}

template<typename InT, typename InAlloc>
FORCEINLINE bool TArrayBase<InT, InAlloc>::IsDataEqual(const Self& InOther) const noexcept
{
    if (this->GetSize() != InOther.GetSize())
    {
        return false;
    }

    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] != InOther.Impl.Data[Index])
        {
            return false;
        }

        continue;
    }

    return true;
}

template<typename InT, typename InAlloc>
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Add(const T& InElement) noexcept
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    std::construct_at(this->Impl.Slack++, InElement);

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InT, typename InAlloc>
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Add(T&& InElement) noexcept
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    std::construct_at(this->Impl.Slack++, std::forward<T>(InElement));

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::AddAt(const SizeType InIndex, const T& InElement) noexcept
{
    if (this->GetSize() == InIndex)
    {
        this->Add(InElement);
        return;
    }

    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )

    this->AddUninitialized();

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + 1, this->Impl.Data + InIndex, (this->GetSize() - InIndex - 1) * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    std::construct_at(this->Impl.Data + InIndex, InElement);

    return;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::AddAt(const SizeType InIndex, T&& InElement) noexcept
{
    if (this->GetSize() == InIndex)
    {
        this->Add(std::forward<T>(InElement));
        return;
    }

    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )

    this->AddUninitialized();

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + 1, this->Impl.Data + InIndex, (this->GetSize() - InIndex - 1) * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    std::construct_at(this->Impl.Data + InIndex, std::forward<T>(InElement));

    return;
}

template<typename InT, typename InAlloc>
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::AddDefault() noexcept
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    std::construct_at(this->Impl.Slack++);

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::AddDefault(SizeType InCount) noexcept
{
    this->Reserve(InCount);

    while (InCount > 0)
    {
        std::construct_at(this->Impl.Slack++);
        --InCount;
    }

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::AddZeroed() noexcept
{
    if (this->IsCapped())
    {
        this->Grow();
    }

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memset(this->Impl.Slack++, 0, sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::AddZeroed(const SizeType InCount) noexcept
{
    this->Reserve(InCount);

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memset(this->Impl.Slack, 0, InCount * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    this->Impl.Slack += InCount;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::AddUninitialized() noexcept
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    ++this->Impl.Slack;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::AddUninitialized(const SizeType InCount) noexcept
{
    this->Reserve(InCount);
    this->Impl.Slack += InCount;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
template<typename ... InTArgs>
FORCEINLINE typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Emplace(InTArgs&&... InArgs) noexcept
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    std::construct_at(this->Impl.Slack++, std::forward<InTArgs>(InArgs)...);

    return this->GetSize() - 1;
}

template<typename InT, typename InAlloc>
template<typename ... InTArgs>
FORCEINLINE void TArrayBase<InT, InAlloc>::EmplaceAt(const SizeType InIndex, InTArgs&&... InArgs) noexcept
{
    if (this->GetSize() == InIndex)
    {
        this->Emplace(std::forward<InTArgs>(InArgs)...);
        return;
    }

    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )

    this->AddUninitialized();

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + 1, this->Impl.Data + InIndex, (this->GetSize() - InIndex - 1) * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    std::construct_at(this->Impl.Data + InIndex, std::forward<InTArgs>(InArgs)...);

    return;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::Append(const Self& InOther) noexcept
{
    this->Reserve(this->GetSize() + InOther.GetSize());

    T* Me = this->Impl.Slack;
    for (const T* RESTRICT Bulk = InOther.Impl.Data; Bulk != InOther.Impl.Slack; ++Bulk)
    {
        std::construct_at(Me, *Bulk);
        ++Me;
    }
    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::Append(Self&& InOther) noexcept
{
    if (InOther.GetSize() > 0)
    {
        this->Reserve(this->GetSize() + InOther.GetSize());

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        ::memcpy(this->Impl.Slack, InOther.Impl.Data, InOther.GetSize() * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

        this->Impl.Slack += InOther.GetSize();

        InOther.Impl.Slack = InOther.Impl.Data;
        InOther.Impl.Orphan();
    }

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::Append(const T* InElements, const SizeType InCount) noexcept
{
    this->Reserve(this->GetSize() + InCount);

    T* Me = this->Impl.Slack;
    for (const T* RESTRICT Bulk = InElements; Bulk != InElements + InCount; ++Bulk)
    {
        std::construct_at(Me, *Bulk);
        ++Me;
    }
    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::AppendAt(const SizeType InIndex, const Self& InOther) noexcept
{
    if (InOther.IsEmpty())
    {
        return;
    }

    this->Reserve(this->GetSize() + InOther.GetSize());

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + InOther.GetSize(), this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    T* Me = this->Impl.Data + InIndex;
    for (const T* RESTRICT Bulk = InOther.Impl.Data; Bulk != InOther.Impl.Slack; ++Bulk)
    {
        std::construct_at(Me, *Bulk);
        ++Me;
    }
    this->Impl.Slack = this->Impl.Data + this->GetSize() + InOther.GetSize();

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::AppendAt(const SizeType InIndex, Self&& InOther) noexcept
{
    if (InOther.IsEmpty())
    {
        return;
    }

    this->Reserve(this->GetSize() + InOther.GetSize());

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + InOther.GetSize(), this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
    ::memcpy(this->Impl.Data + InIndex, InOther.Impl.Data, InOther.GetSize() * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    this->Impl.Slack = this->Impl.Data + this->GetSize() + InOther.GetSize();

    InOther.Impl.Slack = InOther.Impl.Data;
    InOther.Impl.Orphan();

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::AppendAt(SizeType InIndex, const T* InElements, const SizeType InCount) noexcept
{
    if (InCount == 0)
    {
        return;
    }

    this->Reserve(this->GetSize() + InCount);

#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + InCount, this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    for (const T* RESTRICT Bulk = InElements; Bulk != InElements + InCount; ++Bulk)
    {
        std::construct_at(this->Impl.Data + InIndex, *Bulk);
        ++InIndex;
    }
    this->Impl.Slack = this->Impl.Data + this->GetSize() + InCount;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
FORCEINLINE void TArrayBase<InT, InAlloc>::RemoveAt(const SizeType InIndex) noexcept
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )

    this->DestroyAt(InIndex);

    if (InIndex < this->GetSize() - 1)
    {
#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        ::memmove(this->Impl.Data + InIndex, this->Impl.Data + InIndex + 1, (this->GetSize() - InIndex - 1) * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */
    }

    --this->Impl.Slack;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
FORCEINLINE void TArrayBase<InT, InAlloc>::RemoveAt(const SizeType InIndex, const SizeType InCount) noexcept
{
    JAFG_CHECK_ARRAY( InCount > 0 && InIndex >= 0 && this->IsValidIndex(InIndex + InCount) )

    for (SizeType Index = InIndex; Index < InIndex + InCount; ++Index)
    {
        this->DestroyAt(Index);
    }

    if (InIndex + InCount < this->GetSize())
    {
#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        ::memmove(this->Impl.Data + InIndex, this->Impl.Data + InIndex + InCount, (this->GetSize() - InIndex - InCount) * sizeof(T));
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */
    }

    this->Impl.Slack -= InCount;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InT, typename InAlloc>
FORCEINLINE typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Remove(const T& InElement) noexcept
{
    SizeType Removed = 0;

    for (SizeType Index = 0; Index < this->GetSize();)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            this->RemoveAt(Index);
            ++Removed;
        }
        else
        {
            ++Index;
        }

        continue;
    }

    return Removed;
}

template<typename InT, typename InAlloc>
FORCEINLINE bool TArrayBase<InT, InAlloc>::RemoveOnce(const T& InElement) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            this->RemoveAt(Index);
            return true;
        }

        continue;
    }

    return false;
}

template<typename InT, typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Remove(const InOtherElement& InElement) noexcept
{
    SizeType Removed = 0;

    for (SizeType Index = 0; Index < this->GetSize();)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            this->RemoveAt(Index);
            ++Removed;
        }
        else
        {
            ++Index;
        }

        continue;
    }

    return Removed;
}

template<typename InT, typename InAlloc>
template<typename InOtherElement>
FORCEINLINE bool TArrayBase<InT, InAlloc>::RemoveOnce(const InOtherElement& InElement) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            this->RemoveAt(Index);
            return true;
        }

        continue;
    }

    return false;
}

template<typename InT, typename InAlloc>
template<typename Predicate>
FORCEINLINE typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::RemoveByPredicate(const Predicate& InPredicate) noexcept
{
    SizeType Removed = 0;

    for (SizeType Index = 0; Index < this->GetSize();)
    {
        if (InPredicate(this->Impl.Data[Index]))
        {
            this->RemoveAt(Index);
            ++Removed;
        }
        else
        {
            ++Index;
        }

        continue;
    }

    return Removed;
}

template<typename InT, typename InAlloc>
template<typename Predicate>
FORCEINLINE bool TArrayBase<InT, InAlloc>::RemoveOnceByPredicate(const Predicate& InPredicate) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (InPredicate(this->Impl.Data[Index]))
        {
            this->RemoveAt(Index);
            return true;
        }

        continue;
    }

    return false;
}

template<typename InT, typename InAlloc>
FORCEINLINE typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Find(const T& InElement, T** OutElement) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            if (OutElement)
            {
                *OutElement = this->Impl.Data[Index];
            }

            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename InT, typename InAlloc>
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Find(const T& InElement) const noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename InT, typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Find(const InOtherElement& InElement, T** OutElement) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            if (OutElement)
            {
                *OutElement = this->Impl.Data[Index];
            }

            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename InT, typename InAlloc>
template<typename InOtherElement>
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Find(const InOtherElement& InElement) const noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename InT, typename InAlloc>
template<typename Predicate>
FORCEINLINE typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::FindByPredicate(const Predicate& InPredicate, T** OutElement) const noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (InPredicate(this->Impl.Data[Index]))
        {
            if (OutElement)
            {
                *OutElement = this->Impl.Data[Index];
            }

            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename InT, typename InAlloc>
template<typename Predicate>
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::FindByPredicate(const Predicate& InPredicate) const noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (InPredicate(this->Impl.Data[Index]))
        {
            return Index;
        }

        continue;
    }

    return INDEX_NONE;
}

template<typename InT, typename InAlloc>
FORCEINLINE typename TArrayBase<InT, InAlloc>::T* TArrayBase<InT, InAlloc>::FindRef(const T& InElement, SizeType* OutIndex) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            if (OutIndex)
            {
                *OutIndex = Index;
            }

            return this->Impl.Data + Index;
        }

        continue;
    }

    return nullptr;
}

template<typename InT, typename InAlloc>
FORCEINLINE typename TArrayBase<InT, InAlloc>::T* TArrayBase<InT, InAlloc>::FindRef(const T& InElement) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            return this->Impl.Data + Index;
        }

        continue;
    }

    return nullptr;
}

template<typename InT, typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InT, InAlloc>::T* TArrayBase<InT, InAlloc>::FindRef(const InOtherElement& InElement, SizeType* OutIndex) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            if (OutIndex)
            {
                *OutIndex = Index;
            }

            return this->Impl.Data + Index;
        }

        continue;
    }

    return nullptr;
}

template<typename InT, typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InT, InAlloc>::T* TArrayBase<InT, InAlloc>::FindRef(const InOtherElement& InElement) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            return this->Impl.Data + Index;
        }

        continue;
    }

    return nullptr;
}

template<typename InT, typename InAlloc>
template<typename Predicate>
typename TArrayBase<InT, InAlloc>::T* TArrayBase<InT, InAlloc>::FindRefByPredicate(const Predicate& InPredicate, SizeType* OutIndex) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (InPredicate(this->Impl.Data[Index]))
        {
            if (OutIndex)
            {
                *OutIndex = Index;
            }

            return this->Impl.Data + Index;
        }

        continue;
    }

    return nullptr;
}

template<typename InT, typename InAlloc>
template<typename Predicate>
typename TArrayBase<InT, InAlloc>::T* TArrayBase<InT, InAlloc>::FindRefByPredicate(const Predicate& InPredicate) noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (InPredicate(this->Impl.Data[Index]))
        {
            return this->Impl.Data + Index;
        }

        continue;
    }

    return nullptr;
}

template<typename InT, typename InAlloc>
FORCEINLINE bool TArrayBase<InT, InAlloc>::Contains(const T& InElement) const noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            return true;
        }

        continue;
    }

    return false;
}

template<typename InT, typename InAlloc>
template<typename InOtherElement>
FORCEINLINE bool TArrayBase<InT, InAlloc>::Contains(const InOtherElement& InElement) const noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (this->Impl.Data[Index] == InElement)
        {
            return true;
        }

        continue;
    }

    return false;
}

template<typename InT, typename InAlloc>
template<typename Predicate>
FORCEINLINE bool TArrayBase<InT, InAlloc>::ContainsByPredicate(const Predicate& InPredicate) const noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        if (InPredicate(this->Impl.Data[Index]))
        {
            return true;
        }

        continue;
    }

    return false;
}

template<typename InT, typename InAlloc>
FORCEINLINE bool TArrayBase<InT, InAlloc>::Pop() noexcept
{
    if (this->GetSize() > 0)
    {
        this->DestroyAt(this->Impl.Slack - 1);
        --this->Impl.Slack;
        return true;
    }

    return false;
}

template<typename InT, typename InAlloc>
FORCEINLINE void TArrayBase<InT, InAlloc>::Pop(SizeType InCount) noexcept
{
    while (this->GetSize() > 0 && InCount > 0)
    {
        this->DestroyAt(this->Impl.Slack - 1);
        --this->Impl.Slack;
        --InCount;

        continue;
    }

    return;
}

template<typename InT, typename InAlloc>
FORCEINLINE void TArrayBase<InT, InAlloc>::DestroyAt(const SizeType InIndex) noexcept
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )
    this->Impl.Data[InIndex].~T();
    return;
}

template<typename InT, typename InAlloc>
FORCEINLINE void TArrayBase<InT, InAlloc>::DestroyAt(T* InAddress) noexcept
{
    JAFG_CHECK_ARRAY( InAddress >= this->Impl.Data && InAddress < this->Impl.Slack )
    InAddress->~T();
    return;
}

} /* ~Namespace Jafg */

#undef JAFG_CHECK_ARRAY
