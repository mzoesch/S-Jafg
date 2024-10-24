// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/**
 * An array container with a fixed or dynamic size policy allocated on the stack or heap.
 * Fixed size arrays have zero runtime overhead and are equivalent to c arrays.
 *
 * This array may not handle complex types that require move semantics as it was designed to be a fast and simple
 * container for extreme fast-paced memory read and write operations.
 *
 * Allows for OOB checks if CHECK_CONTAINER_BOUNDS is defined. If checks are enabled, the array will panic if
 * something fishy is going on.
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

    FORCEINLINE auto GetSize()     const noexcept -> SizeType { return this->Size;                       }
    FORCEINLINE auto IsEmpty()     const noexcept -> bool     { return this->GetSize() == 0;             }
    FORCEINLINE auto GetCapacity() const noexcept -> SizeType { return this->Capacity;                   }
    FORCEINLINE auto IsData()      const noexcept -> bool     { return this->GetData() != nullptr;       }
    FORCEINLINE auto IsSlack()     const noexcept -> bool     { return this->GetData() != nullptr;       }
    FORCEINLINE auto GetData()           noexcept -> T*       { return this->Data;                       }
    FORCEINLINE auto GetData()     const noexcept -> const T* { return this->Data;                       }
    FORCEINLINE auto GetSlack()          noexcept -> T*       { return this->GetData() + this->Size;     }
    FORCEINLINE auto GetSlack()    const noexcept -> const T* { return this->GetData() + this->Size;     }
    FORCEINLINE auto GetFirst()          noexcept -> T*       { return this->GetData();                  }
    FORCEINLINE auto GetFirst()    const noexcept -> const T* { return this->GetData();                  }
    FORCEINLINE auto GetLast()           noexcept -> T*       { return this->GetData() + this->Size - 1; }
    FORCEINLINE auto GetLast()     const noexcept -> const T* { return this->GetData() + this->Size - 1; }

    /**
     * Add a new element to the array while potentially reallocating the whole array to fit.
     */
    template <bool Condition = IsDynamic()>
    FORCEINLINE auto Add(const T& InElement)  noexcept -> TEnableIf<Condition, Self&>;
    template <bool Condition = IsDynamic()>
    FORCEINLINE auto Add(const T&& InElement) noexcept -> TEnableIf<Condition, Self&>;

    /**
     * Adds a new element to the array and zeroes the target memory while potentially
     * reallocating the whole array to fit. No constructor of T will be called.
     */
    FORCEINLINE auto AddZeroed() noexcept -> SizeType;
    FORCEINLINE auto AddZeroed(const SizeType InCount) noexcept -> SizeType;

    /**
     * Adds a new element to the array without touching the new target memory while potentially reallocating the whole
     * array to fit. No constructor of T will be called. It is undefined behavior of what the memory contains after
     * calling this function.
     */
    FORCEINLINE auto AddUninitialized() noexcept -> SizeType;
    FORCEINLINE auto AddUninitialized(const SizeType InCount) noexcept -> SizeType;

    /**
     * Adds a new element to the array and constructs it in place while potentially
     * reallocating the whole array to fit.
     */
    template <typename ... Args>
    FORCEINLINE auto Emplace(Args&&... InArgs) noexcept -> Self&;

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

    template <typename InOtherElement>
    FORCEINLINE auto Contains(const InOtherElement& InElement)         const noexcept -> bool;
    template <typename Predicate>
    FORCEINLINE auto ContainsByPredicate(const Predicate& InPredicate) const noexcept -> bool;
    FORCEINLINE auto Contains(const T& InElement)                      const noexcept -> bool;

    /**
     * Will reserve memory for the array such that the buffer can hold at least InReserve elements.
     * Bits will be zeroed out. This action cannot perform a shrink under the hood.
     */
    FORCEINLINE auto Reserve(const SizeType InReserve) noexcept -> void;

    /**
     * Will clear out all elements in the array and set the size to zero. It will not deallocate or reallocate
     * the current memory buffer unless the InReserve parameter is greater than the current capacity and growing
     * the current memory buffer is not possible.
     */
    FORCEINLINE auto Reset(const SizeType InReserve) noexcept -> void;

    /**
     * Completely empties the array and sets the size to zero. The memory buffer will be deallocated if it was
     * allocated on the heap.
     */
    FORCEINLINE auto Empty() noexcept -> void;

    /** @return True, if the array has to be reallocated to push / emplace a new element. */
    FORCEINLINE auto IsCapped()                           const noexcept -> bool;
    FORCEINLINE auto IsValidIndex(const SizeType InIndex) const noexcept -> bool;

    /**
     * Get an element reference for T by index. Panics if CHECK_CONTAINER_BOUNDS is true (usually only true in
     * development build configurations), and the inbound index is invalid. Otherwise, the behavior is undefined.
     */
    FORCEINLINE auto operator[](const SizeType InIndex)       noexcept ->       T&;
    FORCEINLINE auto operator[](const SizeType InIndex) const noexcept -> const T&;

    /** Makes a copy of the other array. No constructors will be called. */
    FORCEINLINE auto operator =(const Self& InOther) noexcept -> Self&;
    /** Moves the other array. No move semantics will be called. */
    FORCEINLINE auto operator =(      Self&& InOther) noexcept -> Self&;

    /** Swaps the contents of this array with the other array. */
    FORCEINLINE auto SwapBuffers(Self& InOther) noexcept -> void;

    /** Private iterator functions for range-based loops. Do not use these directly. */
    FORCEINLINE auto begin()       noexcept -> Iterator<T>       { return Iterator<T>      (this->GetData());  }
    FORCEINLINE auto begin() const noexcept -> Iterator<const T> { return Iterator<const T>(this->GetData());  }
    FORCEINLINE auto end()         noexcept -> Iterator<T>       { return Iterator<T>      (this->GetSlack()); }
    FORCEINLINE auto end()   const noexcept -> Iterator<const T> { return Iterator<const T>(this->GetSlack()); }

private:

    /**
     * Grows the array by **reallocating** the whole array to fit, forwarding elements and zeroing the new memory.
     * A std::realloc will never call copy or move constructors.
     */
    template <bool Condition = IsDynamic()>
    FORCEINLINE auto ZeroedGrow() noexcept -> TEnableIf<Condition, void>;
    /** Same as ZeroedGrow(void) but with a specific capacity. */
    template <bool Condition = IsDynamic()>
    FORCEINLINE auto ZeroedGrow(const SizeType InTotalCapacity) noexcept -> TEnableIf<Condition, void>;
    template <bool Condition = IsDynamic()>
    FORCEINLINE auto Shrink() noexcept -> TEnableIf<Condition, void>;
    template <bool Condition = IsDynamic()>
    FORCEINLINE auto Shrink(const SizeType InTotalCapacity) noexcept -> TEnableIf<Condition, void>;

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
        this->ZeroedGrow();
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
        this->ZeroedGrow();
    }

    *(this->Data + this->Size++) = std::move(InElement);

    return *this;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
SizeType TArray<T, ResizePolicy, AllocationPolicy, SizeType>::AddZeroed() noexcept
{
    if (this->IsCapped())
    {
        this->ZeroedGrow();
    }

    return ++this->Size - 1;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
SizeType TArray<T, ResizePolicy, AllocationPolicy, SizeType>::AddZeroed(const SizeType InCount) noexcept
{
    unimplemented()
    return 0;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
SizeType TArray<T, ResizePolicy, AllocationPolicy, SizeType>::AddUninitialized() noexcept
{
    unimplemented()
    return 0;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
SizeType TArray<T, ResizePolicy, AllocationPolicy, SizeType>::AddUninitialized(const SizeType InCount) noexcept
{
    unimplemented()
    return 0;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <typename ... Args>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>&
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Emplace(Args&&... InArgs) noexcept
{
    // !!! We want to use AddUninitialized() to safe some runtime overhead. But we need to implement it first. Lol. !!!
    SizeType Index = this->AddZeroed();
    new( this->GetData() + Index ) T(std::forward<Args>(InArgs)...);
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

    for (T* RESTRICT Bulk = this->GetData(), *RESTRICT End = Bulk + this->Size; Bulk != End; ++Bulk)
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

    for (const T* RESTRICT Bulk = this->GetData(), *RESTRICT End = Bulk + this->Size; Bulk != End; ++Bulk)
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

    for (const T* RESTRICT Bulk = this->GetData(), *RESTRICT End = Bulk + this->Size; Bulk != End; ++Bulk)
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
    return (const_cast<TArray*>(this))->FindRef(InElement);
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
template <typename InOtherElement>
bool TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Contains(const InOtherElement& InElement) const noexcept
{
    return this->Find(InElement) != INDEX_NONE;
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
void TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Reserve(const SizeType InReserve) noexcept
{
    if (InReserve > this->Capacity)
    {
        this->ZeroedGrow(InReserve);
    }

    return;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
void TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Reset(const SizeType InReserve) noexcept
{
    if (this->IsData() == false)
    {
        this->Reserve(InReserve);
        return;
    }

    for (T* RESTRICT Bulk = this->Data, *RESTRICT End = Bulk + this->Size; Bulk != End; ++Bulk)
    {
        Bulk->~T();
    }

    this->Size = 0;

    this->Reserve(InReserve);

    return;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
void TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Empty() noexcept
{
    if (this->IsData() == false)
    {
        return;
    }

    for (T* RESTRICT Bulk = this->Data, *RESTRICT End = Bulk + this->Size; Bulk != End; ++Bulk)
    {
        Bulk->~T();
    }

    this->Size = 0;

    this->Shrink(0);

    return;
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
TArray<T, ResizePolicy, AllocationPolicy, SizeType>&
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::operator=(const Self& InOther) noexcept
{
    if (this == &InOther)
    {
        checkNoEntry() // We probably want to do some logging here. But we don't have a logger yet.
        return *this;
    }

    if (this->Data)
    {
        if constexpr (AllocationPolicy == AllocationPolicy::Heap)
        {
            ::free(this->Data);
        }
    }

    this->Size      = InOther.Size;
    this->Capacity  = InOther.Capacity;
    this->Data      = static_cast<T*>(::malloc(this->Capacity * sizeof(T)));
    ::memset(this->Data, 0, this->Capacity * sizeof(T));
    ::memcpy(this->Data, InOther.Data, this->Size * sizeof(T));

    return *this;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>&
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::operator=(Self&& InOther) noexcept
{
    if (this == &InOther)
    {
        checkNoEntry() // We probably want to do some logging here. But we don't have a logger yet.
        return *this;
    }

    if (this->Data)
    {
        if constexpr (AllocationPolicy == AllocationPolicy::Heap)
        {
            ::free(this->Data);
        }
    }

    this->Size      = InOther.Size;
    this->Capacity  = InOther.Capacity;
    this->Data      = InOther.Data;

    InOther.Size     = 0;
    InOther.Capacity = 0;
    InOther.Data     = nullptr;

    return *this;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
void TArray<T, ResizePolicy, AllocationPolicy, SizeType>::SwapBuffers(Self& InOther) noexcept
{
    if (this == &InOther)
    {
        check( false && "Cannot switch contents with itself." )
        /*
         * It's okay. When encountering this in production, we'll just ignore it.
         * I mean, the program will crash anyway soon if that happens, probably.
         * But this array should not be the reason for the end of life.
         */
        return;
    }

    /*
     * We could use std::swap here?
     * But this will do move semantics, and currently this array implementation does
     * not support any move semantics anyway.
     */

    SizeType TempSize     = this->Size;
    SizeType TempCapacity = this->Capacity;
    T*       TempData     = this->Data;

    this->Size      = InOther.Size;
    this->Capacity  = InOther.Capacity;
    this->Data      = InOther.Data;

    InOther.Size     = TempSize;
    InOther.Capacity = TempCapacity;
    InOther.Data     = TempData;

    return;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <bool Condition>
typename TArray<T, ResizePolicy, AllocationPolicy, SizeType>::template TEnableIf<Condition, void>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::ZeroedGrow() noexcept
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
        this->Data = static_cast<T*>(::malloc(this->Capacity * sizeof(T)));
        checkSlow( sizeof(*this->Data) == sizeof(T) )

        ::memset(this->Data, 0, this->Capacity * sizeof(*this->Data));

        return;
    }

    SizeType NewCapacity = this->Capacity < SECOND_GROWTH ? SECOND_GROWTH : this->Capacity * 2;

    T* NewData = static_cast<T*>(::realloc(this->Data, NewCapacity * sizeof(T)));
#if !IN_SHIPPING /* This will anyway never happen. Trust me, bro. */
    if (NewData == nullptr)
    {
        panic( false && "Failed to reallocate memory for TArray." )
        return;
    }
#endif /* !IN_SHIPPING */

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
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::ZeroedGrow(const SizeType InTotalCapacity) noexcept
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
    check( InTotalCapacity > this->Capacity && InTotalCapacity > 0 )
    if (InTotalCapacity > std::numeric_limits<SizeType>::max())
    {
        jassertNoEntry()
        return;
    }
#endif /* IN_DEBUG */

    if (this->Data == nullptr)
    {
        checkSlow( this->Size == 0 )

        this->Capacity = InTotalCapacity;
        this->Data = static_cast<T*>(::malloc(this->Capacity * sizeof(T)));
        checkSlow( sizeof(*this->Data) == sizeof(T) )

        ::memset(this->Data, 0, this->Capacity * sizeof(*this->Data));

        return;
    }

    T* NewData = static_cast<T*>(::realloc(this->Data, InTotalCapacity * sizeof(T)));
#if !IN_SHIPPING /* This will anyway never happen. Trust me, bro. */
    if (NewData == nullptr)
    {
        panic( false && "Failed to reallocate memory for TArray." )
        return;
    }
#endif /* !IN_SHIPPING */

    ::memset(NewData + this->Capacity, 0, (InTotalCapacity - this->Capacity) * sizeof(T));

    this->Data      = NewData;
    this->Capacity  = InTotalCapacity;

    return;
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <bool Condition>
typename TArray<T, ResizePolicy, AllocationPolicy, SizeType>::template TEnableIf<Condition, void>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Shrink() noexcept
{
    unimplemented()
}

template <typename T, ResizePolicy::Type ResizePolicy, AllocationPolicy::Type AllocationPolicy, typename SizeType>
template <bool Condition>
typename TArray<T, ResizePolicy, AllocationPolicy, SizeType>::template TEnableIf<Condition, void>
TArray<T, ResizePolicy, AllocationPolicy, SizeType>::Shrink(const SizeType InTotalCapacity) noexcept
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

    check( this->IsData()                                           )
    check( InTotalCapacity < this->Capacity && InTotalCapacity >= 0 )
    check( InTotalCapacity <= this->Size                            )

#if !IN_SHIPPING

    T* NewData = static_cast<T*>(::realloc(this->Data, InTotalCapacity * sizeof(T)));
    if (NewData == nullptr)
    {
        panic( "Failed to reallocate memory for TArray." )
        return;
    }

    check( NewData == this->Data && "Failed to shrink memory for TArray. Got a new malloc instead." )

    this->Data      = NewData;

#else /* !IN_SHIPPING */

    /**
     * In SHIPPING just shrink it and hope for the best.
     * A memory failure will not happen. Trust me, bro.
     */
    this->Data = static_cast<T*>(::realloc(this->Data, InTotalCapacity * sizeof(T)));

#endif /* IN_SHIPPING */

    this->Capacity = InTotalCapacity;

    return;
}

} /* ~Namespace Jafg */
