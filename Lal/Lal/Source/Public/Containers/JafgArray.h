// Copyright mzoesch. All rights reserved.

#pragma once

#ifndef JAFG_CHECK_ARRAY
    #define JAFG_CHECK_ARRAY(Expr)      check(Expr)
#endif /* JAFG_CHECK_ARRAY */

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

template <typename InT, typename InSizeType, typename InTraits = TArrayAllocatorTraits<InSizeType>>
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

    void Orphan() noexcept;

    T* Data;
    T* Slack;
    T* End;
};

template <typename InT, typename InAlloc = TArrayAllocator<InT, i32>>
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

    FORCEINLINE  TArrayBase() noexcept = default;
    FORCEINLINE  TArrayBase(const Self& InOther) noexcept : Impl() { Self::Copy(*this, InOther); }
    FORCEINLINE  TArrayBase(Self&& InOther) noexcept : Impl() { Self::Move(*this, std::move(InOther)); }
    FORCEINLINE  TArrayBase(std::initializer_list<T> InList) noexcept;
    FORCEINLINE ~TArrayBase() noexcept { this->Empty(); }

    FORCEINLINE Self& operator=(const Self& InOther) noexcept { Self::Copy(*this, InOther); return *this; }
    FORCEINLINE Self& operator=(Self&& InOther) noexcept { Self::Move(*this, std::move(InOther)); return *this; }
    FORCEINLINE Self& operator=(std::initializer_list<T> InList) noexcept;

    FORCEINLINE SizeType GetSize()     const noexcept { return this->Impl.Slack - this->Impl.Data;  }
    FORCEINLINE bool     IsEmpty()     const noexcept { return this->Impl.Data == this->Impl.Slack; }
    FORCEINLINE SizeType GetCapacity() const noexcept { return this->Impl.End - this->Impl.Data;   }
    FORCEINLINE bool     IsData()      const noexcept { return this->Impl.Data  != nullptr; }
    FORCEINLINE bool     IsSlack()     const noexcept { return this->Impl.Slack != nullptr; }
    FORCEINLINE       T* GetData()           noexcept { return this->Impl.Data; }
    FORCEINLINE const T* GetData()     const noexcept { return this->Impl.Data; }
    FORCEINLINE       T* GetSlack()          noexcept { return this->Impl.Slack; }
    FORCEINLINE const T* GetSlack()    const noexcept { return this->Impl.Slack; }
    FORCEINLINE       T* GetFirst()          noexcept { return this->GetSize() > 0 ? this->Impl.Data  : nullptr; }
    FORCEINLINE const T* GetFirst()    const noexcept { return this->GetSize() > 0 ? this->Impl.Data  : nullptr; }
    FORCEINLINE       T* GetLast()           noexcept { return this->GetSize() > 0 ? this->Impl.Slack - 1: nullptr; }
    FORCEINLINE const T* GetLast()     const noexcept { return this->GetSize() > 0 ? this->Impl.Slack - 1: nullptr; }
    FORCEINLINE bool     IsCapped()    const noexcept { return this->Impl.Slack == this->Impl.End; }

    FORCEINLINE bool IsValidIndex(const SizeType InIndex) const noexcept { return InIndex > INDEX_NONE && InIndex < this->GetSize(); }

    FORCEINLINE       T& operator[](const SizeType InIndex)       noexcept;
    FORCEINLINE const T& operator[](const SizeType InIndex) const noexcept;

    /**
     * Will reserve memory for the array such that the buffer can hold at least InAmount elements.
     * This action cannot perform a shrink under the hood.
     */
    void Reserve(const SizeType InAmount) noexcept { this->Grow(InAmount); }

    /**
     * Will clear out all elements in the array and set the size to zero. It will not deallocate or reallocate
     * the current memory buffer unless the InReserve parameter is greater than the current capacity and growing
     * the current memory buffer is not possible.
     */
    void Reset(const SizeType InAmount) noexcept;

    /**
     * Completely empties the array and sets the size to zero. The memory buffer will be orphaned.
     */
    void Empty() noexcept { this->Impl.Orphan(); }

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
    SizeType Emplace(InTArgs&&... InArgs) noexcept;
    template <typename ... InTArgs>
    void EmplaceAt(const SizeType InIndex, InTArgs&&... InArgs) noexcept;

    /**
     * Appends new elements to the array while potentially reallocating the whole array to fit.
     */
    void Append(const Self& InOther) noexcept;
    void Append(Self&& InOther) noexcept;
    void Append(const T* InElements, const SizeType InCount) noexcept;
    void AppendAt(const SizeType InIndex, const Self& InOther) noexcept;
    void AppendAt(const SizeType InIndex, Self&& InOther) noexcept;
    void AppendAt(      SizeType InIndex, const T* InElements, const SizeType InCount) noexcept;

    /** Private iterator functions for range-based loops. Do not use these directly. */
    FORCEINLINE Iterator<T>       begin()       noexcept { return Iterator<T>      (this->Impl.Data);  }
    FORCEINLINE Iterator<const T> begin() const noexcept { return Iterator<const T>(this->Impl.Data);  }
    FORCEINLINE Iterator<T>       end()         noexcept { return Iterator<T>      (this->Impl.Slack); }
    FORCEINLINE Iterator<const T> end()   const noexcept { return Iterator<const T>(this->Impl.Slack); }

private:

    void Grow() noexcept { this->Impl.Grow(); }
    void Grow(const SizeType InAmount) noexcept { this->Impl.Grow(InAmount); }

    static void Copy(Self& Dst, const Self& Src) noexcept { Dst.Impl = Src.Impl; }
    static void Move(Self& Dst, Self&& Src) noexcept { Dst.Impl = std::move(Src.Impl); }

    Alloc Impl;
};

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE TArrayAllocator<InT, InSizeType, InTraits>::TArrayAllocator(const Self& Other) noexcept : Data(), Slack(), End()
{
    check( this != &Other )
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
    check( this != &Other )

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
    check( this != &Other )

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
    check( this != &Other )
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
    check( NewCapacity > 0 && NewCapacity >= this->End - this->Data )

    if (this->Data == nullptr)
    {
        this->Data  = static_cast<T*>(::malloc(NewCapacity * sizeof(T)));
        this->Slack = this->Data;
        this->End   = this->Data + NewCapacity;

        JAFG_CHECK_ARRAY(this->Data)

        return;
    }

    const SizeType UsedSize = this->Slack - this->Data;

    this->Data  = static_cast<T*>(::realloc(this->Data, NewCapacity * sizeof(T)));
    this->Slack = this->Data + UsedSize;
    this->End   = this->Data + NewCapacity;

    JAFG_CHECK_ARRAY(this->Data)

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
        this->Data  = static_cast<T*>(::malloc(InAmount * sizeof(T)));
        this->Slack = this->Data;
        this->End   = this->Data + InAmount;

        JAFG_CHECK_ARRAY(this->Data)

        return;
    }

    const SizeType UsedSize = this->Slack - this->Data;

    this->Data  = static_cast<T*>(::realloc(this->Data, InAmount * sizeof(T)));
    this->Slack = this->Data + UsedSize;
    this->End   = this->Data + InAmount;

    JAFG_CHECK_ARRAY(this->Data)

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
FORCEINLINE typename TArrayBase<InT, InAlloc>::T& TArrayBase<InT, InAlloc>::operator[](const SizeType InIndex) noexcept
{
    check( this->IsValidIndex(InIndex) )
    return this->Impl.Data[InIndex];
}

template<typename InT, typename InAlloc>
FORCEINLINE const typename TArrayBase<InT, InAlloc>::T& TArrayBase<InT, InAlloc>::operator[](const SizeType InIndex) const noexcept
{
    check( this->IsValidIndex(InIndex) )
    return this->Impl.Data[InIndex];
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::Reset(const SizeType InAmount) noexcept
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
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Add(const T& InElement) noexcept
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

    check( this->IsValidIndex(InIndex) )

    this->AddUninitialized();

    ::memmove(this->Impl.Data + InIndex + 1, this->Impl.Data + InIndex, (this->GetSize() - InIndex - 1) * sizeof(T));
    std::construct_at(this->Impl.Data + InIndex, std::forward<T>(InElement));

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

    check( this->IsValidIndex(InIndex) )

    this->AddUninitialized();

    ::memmove(this->Impl.Data + InIndex + 1, this->Impl.Data + InIndex, (this->GetSize() - InIndex - 1) * sizeof(T));
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

    ::memset(this->Impl.Slack++, 0, sizeof(T));

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InT, typename InAlloc>
void TArrayBase<InT, InAlloc>::AddZeroed(const SizeType InCount) noexcept
{
    this->Reserve(InCount);

    ::memset(this->Impl.Slack, 0, InCount * sizeof(T));
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
typename TArrayBase<InT, InAlloc>::SizeType TArrayBase<InT, InAlloc>::Emplace(InTArgs&&... InArgs) noexcept
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
void TArrayBase<InT, InAlloc>::EmplaceAt(const SizeType InIndex, InTArgs&&... InArgs) noexcept
{
    if (this->GetSize() == InIndex)
    {
        this->Emplace(std::forward<InTArgs>(InArgs)...);
        return;
    }

    check( this->IsValidIndex(InIndex) )

    this->AddUninitialized();

    ::memmove(this->Impl.Data + InIndex + 1, this->Impl.Data + InIndex, (this->GetSize() - InIndex - 1) * sizeof(T));
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

        ::memcpy(this->Impl.Slack, InOther.Impl.Data, InOther.GetSize() * sizeof(T));
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

    ::memmove(this->Impl.Data + InIndex + InOther.GetSize(), this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
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

    ::memmove(this->Impl.Data + InIndex + InOther.GetSize(), this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
    ::memcpy(this->Impl.Data + InIndex, InOther.Impl.Data, InOther.GetSize() * sizeof(T));
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

    ::memmove(this->Impl.Data + InIndex + InCount, this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
    for (const T* RESTRICT Bulk = InElements; Bulk != InElements + InCount; ++Bulk)
    {
        std::construct_at(this->Impl.Data + InIndex, *Bulk);
        ++InIndex;
    }
    this->Impl.Slack = this->Impl.Data + this->GetSize() + InCount;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

} /* ~Namespace Jafg */
