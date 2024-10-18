// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/**
 * An array container with a fixed or dynamic size policy allocated on the stack or heap.
 * Fixed size arrays have zero runtime overhead and are equivalent to c arrays.
 *
 * @tparam T                Element type.
 * @tparam ResizePolicy     Policy for resizing the array.
 * @tparam AllocationPolicy Policy for allocating the array.
 * @tparam SizeType         Integral type used for capacity and size. This determines the maximum size of the array.
 */
template
<
    typename                T               ,
    ResizePolicy::Type      ResizePolicy    ,
    AllocationPolicy::Type  AllocationPolicy,
    typename                SizeType
>
class TArray
{
    static_assert( std::is_integral_v<SizeType>, "SizeType must be an integral type."       );
    static_assert( std::is_signed_v<SizeType>,   "SizeType must be a signed integral type." );

    template <bool Condition, typename RetVal>
    using TEnableIf             = std::enable_if_t<Condition, RetVal>;
    using Self                  = TArray<T, ResizePolicy, AllocationPolicy, SizeType>;

    FORCEINLINE static constexpr bool IsDynamic() noexcept { return ResizePolicy     == ::Jafg::ResizePolicy::Dynamic;   }
    FORCEINLINE static constexpr bool IsStatic()  noexcept { return ResizePolicy     == ::Jafg::ResizePolicy::Static;    }
    FORCEINLINE static constexpr bool IsHeap()    noexcept { return AllocationPolicy == ::Jafg::AllocationPolicy::Heap;  }
    FORCEINLINE static constexpr bool IsStack()   noexcept { return AllocationPolicy == ::Jafg::AllocationPolicy::Stack; }

public:

    FORCEINLINE TArray() noexcept;
    FORCEINLINE ~TArray() noexcept;

    FORCEINLINE auto GetSize()     const noexcept -> SizeType { return this->Size;                   }
    FORCEINLINE auto GetCapacity() const noexcept -> SizeType { return this->Capacity;               }
    FORCEINLINE auto IsData()      const noexcept -> bool     { return this->GetData() != nullptr;   }
    FORCEINLINE auto IsSlack()     const noexcept -> bool     { return this->GetData() != nullptr;   }
    FORCEINLINE auto GetData()           noexcept -> T*       { return this->Data;                   }
    FORCEINLINE auto GetData()     const noexcept -> const T* { return this->Data;                   }
    FORCEINLINE auto GetSlack()          noexcept -> T*       { return this->GetData() + this->Size; }
    FORCEINLINE auto GetSlack()    const noexcept -> const T* { return this->GetData() + this->Size; }

    template <bool Condition = IsDynamic()>
    FORCEINLINE auto Add(const T& InElement)  noexcept -> TEnableIf<Condition, Self&>;
    template <bool Condition = IsDynamic()>
    FORCEINLINE auto Add(const T&& InElement) noexcept -> TEnableIf<Condition, Self&>;

    template <typename Predicate>
    FORCEINLINE auto FindByPredicate(const Predicate& InPredicate)       noexcept -> T*;
    template <typename Predicate>
    FORCEINLINE auto FindByPredicate(const Predicate& InPredicate) const noexcept -> const T*;

    template <typename InOtherElement>
    FORCEINLINE auto Find(const InOtherElement& InElement, SizeType& OutIndex) const noexcept -> bool;
    template <typename InOtherElement>
    FORCEINLINE auto Find(const InOtherElement& InElement)                     const noexcept -> SizeType;
    FORCEINLINE auto Find(const T& InElement, SizeType& OutIndex)              const noexcept -> bool;
    FORCEINLINE auto Find(const T& InElement)                                  const noexcept -> SizeType;

    template <typename InOtherElement>
    FORCEINLINE auto FindRef(const InOtherElement& InElement, SizeType& OutIndex)       noexcept ->       T*;
    template <typename InOtherElement>
    FORCEINLINE auto FindRef(const InOtherElement& InElement, SizeType& OutIndex) const noexcept -> const T*;
    template <typename InOtherElement>
    FORCEINLINE auto FindRef(const InOtherElement& InElement)                           noexcept ->       T*;
    template <typename InOtherElement>
    FORCEINLINE auto FindRef(const InOtherElement& InElement)                     const noexcept -> const T*;
    FORCEINLINE auto FindRef(const T& InElement, SizeType& OutIndex)                    noexcept ->       T*;
    FORCEINLINE auto FindRef(const T& InElement, SizeType& OutIndex)              const noexcept -> const T*;
    FORCEINLINE auto FindRef(const T& InElement)                                        noexcept ->       T*;
    FORCEINLINE auto FindRef(const T& InElement)                                  const noexcept -> const T*;

    template <typename Predicate>
    FORCEINLINE auto ContainsByPredicate(const Predicate& InPredicate) const noexcept -> bool;
    FORCEINLINE auto Contains(const T& InElement)                      const noexcept -> bool;

    /** @return True, if the array has to be reallocated to append / emplace a new element. */
    FORCEINLINE auto IsCapped()                           const noexcept -> bool;
    FORCEINLINE auto IsValidIndex(const SizeType InIndex) const noexcept -> bool;

    FORCEINLINE auto operator[](const SizeType InIndex)       noexcept ->       T&;
    FORCEINLINE auto operator[](const SizeType InIndex) const noexcept -> const T&;

private:

    template <bool Condition = IsDynamic()>
    FORCEINLINE auto Grow() noexcept -> TEnableIf<Condition, void>;
    template <bool Condition = IsDynamic()>
    FORCEINLINE auto Shrink() noexcept -> TEnableIf<Condition, void>;

    SizeType    Size;
    SizeType    Capacity;
    T*          Data;
};

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::TArray() noexcept
{
    this->Size        = 0;
    this->Capacity    = 0;
    this->Data        = nullptr;

    return;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::~TArray() noexcept
{
    if (this->Data)
    {
        if constexpr (AllocationPolicy == AllocationPolicy::Heap)
        {
            ::free(this->Data);
        }

        this->Data = nullptr;
    }

    return;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <bool Condition>
typename TArray<T, ResizePolicy, AllocationPolicy, SizeType>::template TEnableIf
<
    Condition,
    TArray<T, ResizePolicy, AllocationPolicy, SizeType>&
>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Add(const T& InElement) noexcept
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    *(this->Data + this->Size++) = InElement;

    return *this;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <bool Condition>
typename TArray<T, ResizePolicy, AllocationPolicy, SizeType>::template TEnableIf
<
    Condition,
    TArray<T, ResizePolicy, AllocationPolicy, SizeType>&
>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Add(const T&& InElement) noexcept
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    *(this->Data + this->Size++) = std::move(InElement);

    return *this;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename Predicate>
T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindByPredicate(const Predicate& InPredicate) noexcept
{
    if (this->IsData() == false)
    {
        return nullptr;
    }

    for (T* RESTRICT Bulk = this->GetData(), *RESTRICT End = Bulk + this->GetSize(); Bulk != End; ++Bulk)
    {
        if (InPredicate(*Bulk))
        {
            return Bulk;
        }

        continue;
    }

    return nullptr;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename Predicate>
const T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindByPredicate(const Predicate& InPredicate) const noexcept
{
    return this->FindByPredicate(InPredicate);
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename InOtherElement>
bool TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Find(const InOtherElement& InElement, SizeType& OutIndex) const noexcept
{
    OutIndex = this->Find(InElement);
    return OutIndex != INDEX_NONE;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename InOtherElement>
SizeType TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Find(const InOtherElement& InElement) const noexcept
{
    if (this->IsData() == false)
    {
        return INDEX_NONE;
    }

    for (const T* RESTRICT Bulk = this->GetData(), *RESTRICT End = Bulk + this->GetSize(); Bulk != End; ++Bulk)
    {
        if (*Bulk == InElement)
        {
            return static_cast<SizeType>(Bulk - this->GetData());
        }

        continue;
    }

    return INDEX_NONE;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
bool TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Find(const T& InElement, SizeType& OutIndex) const noexcept
{
    OutIndex = this->Find(InElement);
    return OutIndex != INDEX_NONE;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
SizeType TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Find(const T& InElement) const noexcept
{
    if (this->IsData() == false)
    {
        return INDEX_NONE;
    }

    for (const T* RESTRICT Bulk = this->GetData(), *RESTRICT End = Bulk + this->GetSize(); Bulk != End; ++Bulk)
    {
        if (*Bulk == InElement)
        {
            return static_cast<SizeType>(Bulk - this->GetData());
        }

        continue;
    }

    return INDEX_NONE;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename InOtherElement>
T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindRef(const InOtherElement& InElement, SizeType& OutIndex) noexcept
{
    if (this->Find(InElement, OutIndex))
    {
        return &(*this)[OutIndex];
    }

    return nullptr;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename InOtherElement>
const T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindRef(const InOtherElement& InElement, SizeType& OutIndex) const noexcept
{
    return this->FindRef(InElement, OutIndex);
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename InOtherElement>
T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindRef(const InOtherElement& InElement) noexcept
{
    if (SizeType Index = this->Find(InElement); Index != INDEX_NONE)
    {
        return &(*this)[Index];
    }

    return nullptr;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename InOtherElement>
const T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindRef(const InOtherElement& InElement) const noexcept
{
    return this->FindRef(InElement);
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindRef(const T& InElement, SizeType& OutIndex) noexcept
{
    if (this->Find(InElement, OutIndex))
    {
        return &(*this)[OutIndex];
    }

    return nullptr;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
const T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindRef(const T& InElement, SizeType& OutIndex) const noexcept
{
    return (const_cast<TArray*>(this))->FindRef(InElement, OutIndex);
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindRef(const T& InElement) noexcept
{
    if (SizeType Index = this->Find(InElement); Index != INDEX_NONE)
    {
        return &(*this)[Index];
    }

    return nullptr;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
const T* TArray<T, ResizePolicy, AllocationPolicy, SizeType>::FindRef(const T& InElement) const noexcept
{
    return (const_cast<TArray*>(this))->FindRef(InElement);
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename Predicate>
bool TArray<T, ResizePolicy, AllocationPolicy, SizeType>::ContainsByPredicate(const Predicate& InPredicate) const noexcept
{
    return (const_cast<TArray*>(this))->FindByPredicate(InPredicate) != nullptr;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
bool TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Contains(const T& InElement) const noexcept
{
    return this->Find(InElement) != INDEX_NONE;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
bool TArray<T, ResizePolicy, AllocationPolicy, SizeType>::IsCapped() const noexcept
{
    if constexpr (std::is_unsigned_v<SizeType>)
    {
        if (this->Capacity == 0)
        {
            return true;
        }
    }

    return this->Size > this->Capacity - 1;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
bool TArray<T, ResizePolicy, AllocationPolicy, SizeType>::IsValidIndex(const SizeType InIndex) const noexcept
{
    return InIndex > -1 && InIndex < this->Size;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
T& TArray<T, ResizePolicy, AllocationPolicy, SizeType>::operator[](const SizeType InIndex) noexcept
{
#if CHECK_CONTAINER_BOUNDS
    if (this->IsValidIndex(InIndex) == false)
    {
        panic(false && "Index out of bounds.")
    }
#endif /* CHECK_CONTAINER_BOUNDS */

    return this->Data[InIndex];
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
const T& TArray<T, ResizePolicy, AllocationPolicy, SizeType>::operator[](const SizeType InIndex) const noexcept
{
#if CHECK_CONTAINER_BOUNDS
    if (this->IsValidIndex(InIndex) == false)
    {
        panic(false && "Index out of bounds.")
    }
#endif /* CHECK_CONTAINER_BOUNDS */

    return this->Data[InIndex];
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <bool Condition>
typename TArray<T, ResizePolicy, AllocationPolicy, SizeType>::template TEnableIf<Condition, void>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Grow() noexcept
{
    static_assert(ResizePolicy == ResizePolicy::Dynamic, "Only dynamic arrays may grow.");

    if constexpr (AllocationPolicy == AllocationPolicy::Stack)
    {
        /*
         * Not yet :). But there is a way to do this. Currently not used by Jafg but maybe in the future we'll
         * need this.
         */
        static_assert(AllocationPolicy != AllocationPolicy::Heap, "Cannot grow a stack allocated array.");
        return;
    }

#if IN_DEBUG
    if (this->Capacity > std::numeric_limits<SizeType>::max() / 2)
    {
        jassertNoEntry()
        return;
    }
#endif /* IN_DEBUG */

#define FIRST_GROWTH    5
#define SECOND_GROWTH   20

    if (this->Data == nullptr)
    {
        checkSlow( this->Size == 0 )

        this->Capacity = FIRST_GROWTH;
        this->Data = reinterpret_cast<T*>(::malloc(this->Capacity * sizeof(T)));
        checkSlow( sizeof(*this->Data) == sizeof(T) )

        ::memset(this->Data, 0, this->Capacity * sizeof(*this->Data));

        return;
    }

    SizeType NewCapacity = this->Capacity < SECOND_GROWTH ? SECOND_GROWTH : this->Capacity * 2;

    T* NewData = reinterpret_cast<T*>(::realloc(this->Data, NewCapacity * sizeof(T)));
    if (NewData == nullptr)
    {
        panic(false && "Failed to reallocate memory for TArray.")
        return;
    }

    ::memset(NewData + this->Capacity, 0, (NewCapacity - this->Capacity) * sizeof(T));

    this->Data      = NewData;
    this->Capacity  = NewCapacity;

#undef FIRST_GROWTH
#undef SECOND_GROWTH

    return;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <bool Condition>
typename TArray<T, ResizePolicy, AllocationPolicy, SizeType>::template TEnableIf<Condition, void>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Shrink() noexcept
{
    unimplemented()
}

} /* Namespace Jafg */
