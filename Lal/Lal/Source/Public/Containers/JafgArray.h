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

    FORCEINLINE constexpr static SizeType GetGrowSize(const SizeType InCurrent) noexcept
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

    FORCEINLINE consteval static bool IsDynamic() noexcept { return true; }

    FORCEINLINE TArrayAllocator() noexcept : Data(), Slack(), End() { }
    FORCEINLINE TArrayAllocator(const Self& Other) noexcept;
    FORCEINLINE TArrayAllocator(Self&& Other) noexcept;
    template <typename TOtherAlloc>
    FORCEINLINE TArrayAllocator(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, T> == false);

    FORCEINLINE Self& operator=(const Self& Other) noexcept;
    FORCEINLINE Self& operator=(Self&& Other) noexcept;
    template <typename TOtherAlloc>
    FORCEINLINE Self& operator=(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, T> == false);

    void Grow() noexcept;
    void Grow(const SizeType InAmount) noexcept;

    void Shrink() noexcept;

    void Orphan() noexcept;

    //# The first element of the data or nullptr.
    T* Data;
    //# The first element of the slack or nullptr.
    T* Slack;
    //# The first element of unowned memory or nullptr.
    T* End;
};

template <typename InSizeType>
struct TArrayViewAllocatorTraits : public TArrayAllocatorTraits<InSizeType>
{
    typedef typename TArrayAllocatorTraits<InSizeType>::SizeType SizeType;
};

template <typename InT, typename InSizeType, typename InTraits>
struct TArrayViewAllocator
{
    typedef InT                       T;
    typedef InTraits                  Traits;
    typedef typename Traits::SizeType SizeType;

    using Self = TArrayViewAllocator<T, SizeType, Traits>;

    static_assert(std::is_integral_v<SizeType>, "SizeType must be an integral type.");
    static_assert(std::is_signed_v<SizeType>,   "SizeType must be a signed integral type.");

    FORCEINLINE consteval static bool IsWeakAlloc()    noexcept { return true; }
    FORCEINLINE consteval static bool IsContentConst() noexcept { return true; }

    FORCEINLINE TArrayViewAllocator() noexcept : Data(), Slack() { }
    FORCEINLINE TArrayViewAllocator(const Self& Other) noexcept;
    FORCEINLINE TArrayViewAllocator(Self&& Other) noexcept;
    template <typename TOtherAlloc>
    FORCEINLINE TArrayViewAllocator(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, T> == false);
    template <typename TOtherAlloc>
    FORCEINLINE TArrayViewAllocator(const TOtherAlloc&& Other) noexcept requires (std::is_same_v<TOtherAlloc, T> == false) = delete;

    FORCEINLINE Self& operator=(const Self& Other) noexcept;
    FORCEINLINE Self& operator=(Self&& Other) noexcept;
    template <typename TOtherAlloc>
    FORCEINLINE Self& operator=(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, T> == false);
    template <typename TOtherAlloc>
    FORCEINLINE Self& operator=(const TOtherAlloc&& Other) noexcept requires (std::is_same_v<TOtherAlloc, T> == false) = delete;

    FORCEINLINE void Invalidate() noexcept;

    //# The first element of the data or nullptr.
    const T* Data;

    //# The first element of the end or nullptr.
    union
    {
        const T* Slack;
        const T* End;
    };
};


template <typename InSizeType>
struct TMutableArrayViewAllocatorTraits : public TArrayAllocatorTraits<InSizeType>
{
    typedef InSizeType SizeType;
};

template <typename InT, typename InSizeType, typename InTraits>
struct TMutableArrayViewAllocator
{
    typedef InT        T;
    typedef InSizeType SizeType;
    typedef InTraits   Traits;

    using Self = TMutableArrayViewAllocator<T, SizeType, Traits>;

    static_assert(std::is_integral_v<SizeType>, "SizeType must be an integral type.");
    static_assert(std::is_signed_v<SizeType>,   "SizeType must be a signed integral type.");

    FORCEINLINE consteval static bool IsWeakAlloc()    noexcept { return true; }

    FORCEINLINE TMutableArrayViewAllocator() noexcept : Data(), Slack() { }
    FORCEINLINE TMutableArrayViewAllocator(const Self& Other) noexcept;
    FORCEINLINE TMutableArrayViewAllocator(Self&& Other) noexcept;

    FORCEINLINE Self& operator=(const Self& Other) noexcept;
    FORCEINLINE Self& operator=(Self&& Other) noexcept;
    template <typename TOtherAlloc>
    FORCEINLINE Self& operator=(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, T> == false);
    template <typename TOtherAlloc>
    FORCEINLINE Self& operator=(const TOtherAlloc&& Other) noexcept requires (std::is_same_v<TOtherAlloc, T> == false) = delete;

    FORCEINLINE void Invalidate() noexcept;

    //# The first element of the data or nullptr.
    T* Data;

    //# The first element of the end or nullptr.
    union
    {
        T* Slack;
        T* End;
    };
};

//#
//# An array container that may use any form of stack or heap allocated memory.
//#
//# This container checks OOB accesses during debug and development builds but will not check in release builds.
//#
//# This array may not handle complex types that require move semantics as it was designed to be a fast and simple
//# container for extreme fast-paced memory read and write operations.
//#
template <typename InAlloc>
class TArrayBase
{
public:

    using Alloc    = InAlloc;
    using T        = typename Alloc::T;
    using SizeType = typename Alloc::SizeType;
    using Traits   = typename Alloc::Traits;
    using Self     = TArrayBase<Alloc>;

    static_assert(std::is_integral_v<SizeType>, "SizeType must be an integral type.");
    static_assert(std::is_signed_v<SizeType>,   "SizeType must be a signed integral type.");

    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TMemberField* MemberField);
    template <typename TMemberField>
    friend void OnDefaultOnlyMallocMember(TArray<TMemberField>* MemberField);

    template <typename TOtherAlloc>
    friend class TArrayBase;

    //# Whether this array is dynamic. Meaning elements can be added and removed at runtime.
    FORCEINLINE consteval static bool IsDynamic() noexcept requires ( requires { Alloc::IsDynamic; }) { return Alloc::IsDynamic(); }
    FORCEINLINE consteval static bool IsDynamic() noexcept requires (!requires { Alloc::IsDynamic; }) { return false; }

    //#
    //# Whether this array is a weak allocator. Meaning it does not own the memory. Elements may be changed but
    //# not added or removed.
    //#
    FORCEINLINE consteval static bool IsWeakAlloc() noexcept requires ( requires { Alloc::IsWeakAlloc; }) { return Alloc::IsWeakAlloc(); }
    FORCEINLINE consteval static bool IsWeakAlloc() noexcept requires (!requires { Alloc::IsWeakAlloc; }) { return false; }

    //#
    //# Whether this array is a strong allocator. Meaning it owns the memory. Elements may be added and removed.
    //#
    FORCEINLINE consteval static bool IsStrongAlloc() noexcept { return !Self::IsWeakAlloc(); }

    //#
    //# Whether the data can be changed.
    //#
    FORCEINLINE consteval static bool IsContentConst() noexcept requires ( requires { Alloc::IsContentConst; }) { return Alloc::IsContentConst(); }
    FORCEINLINE consteval static bool IsContentConst() noexcept requires (!requires { Alloc::IsContentConst; }) { return false; }
    FORCEINLINE consteval static bool IsContentMutable() noexcept { return !Self::IsContentConst(); }

    static_assert(Self::IsWeakAlloc()    ? Self::IsDynamic()      == false : true, "Weak allocators must not be dynamic.");
    static_assert(Self::IsDynamic()      ? Self::IsWeakAlloc()    == false : true, "Dynamic allocators must not be weak.");
    static_assert(Self::IsDynamic()      ? Self::IsContentConst() == false : true, "Dynamic allocators must not be content.");
    static_assert(Self::IsContentConst() ? Self::IsDynamic()      == false : true, "Const content allocators must not be dynamic.");

    FORCEINLINE  TArrayBase() noexcept = default;
    FORCEINLINE  TArrayBase(const Self& InOther) noexcept : Impl() { Self::Copy(*this, InOther); }
    FORCEINLINE  TArrayBase(Self&& InOther) noexcept : Impl() { Self::Move(*this, std::move(InOther)); }
    FORCEINLINE  TArrayBase(std::initializer_list<T> InList) noexcept requires (Self::IsStrongAlloc() && (std::is_copy_assignable_v<T> || std::is_copy_constructible_v<T>));
    template <typename InOtherElement>
    FORCEINLINE  TArrayBase(std::initializer_list<InOtherElement> InList) noexcept requires (Self::IsStrongAlloc() && (std::is_same_v<T, InOtherElement> == false) && std::is_convertible_v<InOtherElement, T>);
    template <typename ... InTArgs>
    FORCEINLINE  TArrayBase(InTArgs&&... InArgs) noexcept requires (Self::IsStrongAlloc() && std::is_move_constructible_v<T>);
    FORCEINLINE ~TArrayBase() noexcept;
    template <typename TOtherAlloc>
    FORCEINLINE TArrayBase(const TArrayBase<TOtherAlloc>& Other) noexcept requires (Self::IsWeakAlloc() && std::is_same_v<TOtherAlloc, Alloc> == false);
    template <typename TOtherAlloc>
    FORCEINLINE TArrayBase(const TArrayBase<TOtherAlloc>& Other) noexcept requires (Self::IsStrongAlloc() && std::is_same_v<TOtherAlloc, Alloc> == false);

    FORCEINLINE Self& operator=(const Self& InOther) noexcept { Self::Copy(*this, InOther); return *this; }
    FORCEINLINE Self& operator=(Self&& InOther) noexcept { Self::Move(*this, std::move(InOther)); return *this; }
    FORCEINLINE Self& operator=(std::initializer_list<T> InList) noexcept requires (Self::IsStrongAlloc());
    template <typename TOtherAlloc>
    FORCEINLINE Self& operator=(const TArrayBase<TOtherAlloc>& Other) noexcept requires (Self::IsWeakAlloc() && std::is_same_v<TOtherAlloc, Alloc> == false);
    template <typename TOtherAlloc>
    FORCEINLINE Self& operator=(const TArrayBase<TOtherAlloc>& Other) noexcept requires (Self::IsStrongAlloc() && std::is_same_v<TOtherAlloc, Alloc> == false);

    //#
    //# Invalidate the array. This will not deallocate the memory but set the pointers to null.
    //#
    FORCEINLINE void Invalidate() requires (Self::IsWeakAlloc());

    //#
    //# Move the data pointer up to maximal the given offset or the slack pointer.
    //# @return The actual new data pointer.
    //#
    FORCEINLINE T* MoveDataPointerUp(T* NewDataPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable());
    FORCEINLINE const T* MoveDataPointerUp(const T* NewDataPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst());
    //#
    //# Move the data pointer up one unit if possible.
    //# @return Whether the data pointer was moved.
    //#
    FORCEINLINE bool MoveDataPointerUp() noexcept requires (Self::IsWeakAlloc());
    //#
    //# Move the data pointer up to maximal the given offset or the slack pointer.
    //# @return The actual offset that was moved.
    //#
    FORCEINLINE SizeType MoveDataPointerUp(const SizeType InOffset) noexcept requires (Self::IsWeakAlloc());

    //#
    //# Move the slack pointer down to maximal the given offset or the data pointer.
    //# @return The actual new slack pointer.
    //#
    FORCEINLINE T* MoveSlackPointerDown(T* NewSlackPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable());
    FORCEINLINE const T* MoveSlackPointerDown(const T* NewSlackPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst());
    //#
    //# Move the slack pointer down one unit if possible.
    //# @return Whether the slack pointer was moved.
    //#
    FORCEINLINE bool MoveSlackPointerDown() noexcept requires (Self::IsWeakAlloc());
    //#
    //# Move the slack pointer down to maximal the given offset or the data pointer.
    //# @return The actual offset that was moved.
    //#
    FORCEINLINE SizeType MoveSlackPointerDown(const SizeType InOffset) noexcept requires (Self::IsWeakAlloc());

    FORCEINLINE Self& CopyFrom(const Self& InOther) noexcept requires (Self::IsStrongAlloc()) { Self::Copy(*this, InOther); return *this; }
    FORCEINLINE Self& CopyFrom(const Self& InOther, const SizeType InCount) noexcept requires (Self::IsStrongAlloc());
    FORCEINLINE Self& CopyFrom(const Self& InOther, const SizeType InOffset, const SizeType InCount) noexcept requires (Self::IsStrongAlloc());

    NODISCARD FORCEINLINE SizeType GetSize()     const noexcept { return this->Impl.Slack - this->Impl.Data;  }
    NODISCARD FORCEINLINE SizeType GetByteSize() const noexcept { return this->GetSize() * static_cast<SizeType>(sizeof(T)); }
    NODISCARD FORCEINLINE bool     IsEmpty()     const noexcept { return this->Impl.Data == this->Impl.Slack; }
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return this->Impl.End - this->Impl.Data;   }
    NODISCARD FORCEINLINE bool     IsData()      const noexcept { return this->Impl.Data  != nullptr; }
    NODISCARD FORCEINLINE bool     IsSlack()     const noexcept { return this->Impl.Slack != nullptr; }
    NODISCARD FORCEINLINE       T* GetData()           noexcept requires (Self::IsContentMutable()) { return this->Impl.Data; }
    NODISCARD FORCEINLINE const T* GetData()     const noexcept { return this->Impl.Data; }
    NODISCARD FORCEINLINE       T* GetSlack()          noexcept requires (Self::IsContentMutable()) { return this->Impl.Slack; }
    NODISCARD FORCEINLINE const T* GetSlack()    const noexcept { return this->Impl.Slack; }
    NODISCARD FORCEINLINE       T* GetFirst()          noexcept requires (Self::IsContentMutable()) { return this->GetSize() > 0 ? this->Impl.Data : nullptr; }
    NODISCARD FORCEINLINE const T* GetFirst()    const noexcept { return this->GetSize() > 0 ? this->Impl.Data : nullptr; }
    NODISCARD FORCEINLINE       T* GetLast()           noexcept requires (Self::IsContentMutable()) { return this->GetSize() > 0 ? this->Impl.Slack - 1 : nullptr; }
    NODISCARD FORCEINLINE const T* GetLast()     const noexcept { return this->GetSize() > 0 ? this->Impl.Slack - 1 : nullptr; }
    NODISCARD FORCEINLINE bool     IsCapped()    const noexcept requires (Self::IsStrongAlloc()) { return this->Impl.Slack == this->Impl.End; }

    FORCEINLINE bool IsValidIndex(const SizeType InIndex) const noexcept { return InIndex > INDEX_NONE && InIndex < this->GetSize(); }

    FORCEINLINE       T& operator[](const SizeType InIndex)       noexcept requires (Self::IsContentMutable());
    FORCEINLINE const T& operator[](const SizeType InIndex) const noexcept;

    //#
    //# Will reserve memory for the array such that the buffer can hold at least InAmount elements.
    //# This action cannot perform a shrink under the hood.
    //#
    FORCEINLINE void Reserve(const SizeType InAmount) noexcept requires (Self::IsDynamic()) { this->Grow(InAmount); }

    //#
    //# Will clear out all elements in the array and set the size to zero. It will not deallocate or reallocate
    //# the current memory buffer unless the InReserve parameter is greater than the current capacity and growing
    //# the current memory buffer is not possible.
    //#
    FORCEINLINE void Reset(const SizeType InAmount) noexcept requires (Self::IsDynamic());

    //#
    //# Try to shrink the array to the current size or reallocate the array to the new size.
    //#
    FORCEINLINE void Shrink() noexcept requires (Self::IsDynamic()) { this->Impl.Shrink(); }

    //#
    //# Resize the array to the new size. The new size has to be less or equal to the current size.
    //# @param bInShrinkToFit Whether to shrink the array buffer to fit the new size.
    //#
    FORCEINLINE void Resize(const SizeType InSize, const bool bInShrinkToFit) noexcept requires (Self::IsDynamic());

    //#
    //# Completely empties the array and sets the size to zero. The memory buffer will be orphaned.
    //#
    FORCEINLINE void Empty() noexcept requires (Self::IsDynamic()) { this->Impl.Orphan(); }

    //#
    //# Swaps the content buffers of this array with the other array.
    //#
    FORCEINLINE void SwapBuffers(Self& InOther) noexcept;

    //#
    //# Swap two indices in the array.
    //#
    FORCEINLINE void SwapIndices(const SizeType InIndexA, const SizeType InIndexB) noexcept requires (Self::IsContentMutable());

    //#
    //# Do not use std operators as ...
    //#    - they are ambiguous in terms of meaning (compare by size, value or reference?).
    //#    - we might accidentally do comparisons inside templated paths of arrays that are very expensive in terms of
    //#      runtime performance.
    //# Better be explicit about it with the named functions #EqualSize, #IsSameArray and #IsDataEqual.
    //#
    FORCEINLINE bool operator==(      Self& InOther)       noexcept = delete;
    FORCEINLINE bool operator==(const Self& InOther) const noexcept = delete;
    FORCEINLINE bool operator!=(      Self& InOther)       noexcept = delete;
    FORCEINLINE bool operator!=(const Self& InOther) const noexcept = delete;

    FORCEINLINE bool operator <(const Self& InOther) const noexcept { return this->GetSize()  < InOther.GetSize(); }
    FORCEINLINE bool operator >(const Self& InOther) const noexcept { return this->GetSize()  > InOther.GetSize(); }
    FORCEINLINE bool operator<=(const Self& InOther) const noexcept { return this->GetSize() <= InOther.GetSize(); }
    FORCEINLINE bool operator>=(const Self& InOther) const noexcept { return this->GetSize() >= InOther.GetSize(); }
    FORCEINLINE bool EqualSize (const Self& InOther) const noexcept { return this->GetSize() == InOther.GetSize(); }

    //#
    //# Checks if both instances point to the same memory location.
    //#
    NODISCARD FORCEINLINE bool IsSameArray(const Self& InOther) const noexcept { return this->Impl.Data == InOther.Impl.Data; }

    //#
    //# Checks if both instances have the same meaningful data. The capacity is not checked.
    //#
    NODISCARD FORCEINLINE bool IsDataEqual(const Self& InOther) const noexcept;
    NODISCARD FORCEINLINE bool IsDataUnequal(const Self& InOther) const noexcept { return !this->IsDataEqual(InOther); }

    //#
    //# Add a new element to the array while potentially reallocating the whole array to fit.
    //# @return The index of the newly added element.
    //#
    SizeType Add(const T& InElement) noexcept requires (Self::IsDynamic());
    SizeType Add(T&& InElement) noexcept requires (Self::IsDynamic());
    void     AddAt(const SizeType InIndex, const T& InElement) noexcept requires (Self::IsDynamic());
    void     AddAt(const SizeType InIndex, T&& InElement) noexcept requires (Self::IsDynamic());
    SizeType AddDefault() noexcept requires (Self::IsDynamic());
    void     AddDefault(SizeType InCount) noexcept requires (Self::IsDynamic());
    SizeType AddZeroed() noexcept requires (Self::IsDynamic());
    void     AddZeroed(const SizeType InCount) noexcept requires (Self::IsDynamic());
    SizeType AddUninitialized() noexcept requires (Self::IsDynamic());
    void     AddUninitialized(const SizeType InCount) noexcept requires (Self::IsDynamic());

    //#
    //# Adds a new element to the array and constructs it in place while potentially
    //# reallocating the whole array to fit.
    //# @return The index of the newly added element.
    //#
    template <typename ... InTArgs>
    FORCEINLINE SizeType Emplace(InTArgs&&... InArgs) noexcept requires (Self::IsDynamic());
    template <typename ... InTArgs>
    FORCEINLINE void EmplaceAt(const SizeType InIndex, InTArgs&&... InArgs) noexcept requires (Self::IsDynamic());

    //#
    //# Appends new elements to the array while potentially reallocating the whole array to fit.
    //#
    void Append(const Self& InOther) noexcept requires (Self::IsDynamic());
    void Append(Self&& InOther) noexcept requires (Self::IsDynamic());
    void Append(const T* InElements, const SizeType InCount) noexcept requires (Self::IsDynamic());
    void Append(const T* InElements, const T* InEnd) noexcept requires (Self::IsDynamic());
    void Append(std::initializer_list<T> InElements) noexcept requires (Self::IsDynamic());
    void AppendAt(const SizeType InIndex, const Self& InOther) noexcept requires (Self::IsDynamic());
    void AppendAt(const SizeType InIndex, Self&& InOther) noexcept requires (Self::IsDynamic());
    void AppendAt(      SizeType InIndex, const T* InElements, const SizeType InCount) noexcept requires (Self::IsDynamic());
    void AppendAt(const SizeType InIndex, std::initializer_list<T> InElements) noexcept requires (Self::IsDynamic());

    FORCEINLINE void RemoveAt(const SizeType InIndex) noexcept requires (Self::IsDynamic());
    FORCEINLINE void RemoveAt(const SizeType InIndex, const SizeType InCount) noexcept requires (Self::IsDynamic());

    FORCEINLINE SizeType Remove(const T& InElement) noexcept requires (Self::IsDynamic());
    FORCEINLINE bool     RemoveOnce(const T& InElement) noexcept requires (Self::IsDynamic());
    FORCEINLINE bool     RemoveOnceChecked(const T& InElement) noexcept requires (Self::IsDynamic()) { const bool bOut = this->RemoveOnce(InElement); check(bOut); return bOut; }
    FORCEINLINE bool     RemoveOnceAsserted(const T& InElement) noexcept requires (Self::IsDynamic()) { const bool bOut = this->RemoveOnce(InElement); jassert(bOut); return bOut; }

    template <typename InOtherElement>
    FORCEINLINE SizeType Remove(const InOtherElement& InElement) noexcept requires (Self::IsDynamic());
    template <typename InOtherElement>
    FORCEINLINE bool     RemoveOnce(const InOtherElement& InElement) noexcept requires (Self::IsDynamic());
    template <typename InOtherElement>
    FORCEINLINE bool     RemoveOnceChecked(const InOtherElement& InElement) noexcept requires (Self::IsDynamic()) { const bool bOut = this->RemoveOnce(InElement); check(bOut); return bOut; }
    template <typename InOtherElement>
    FORCEINLINE bool     RemoveOnceAsserted(const InOtherElement& InElement) noexcept requires (Self::IsDynamic()) { const bool bOut = this->RemoveOnce(InElement); jassert(bOut); return bOut; }

    template <typename Predicate>
    FORCEINLINE SizeType RemoveByPredicate(const Predicate& InPredicate) noexcept requires (Self::IsDynamic());
    template <typename Predicate>
    FORCEINLINE bool     RemoveOnceByPredicate(const Predicate& InPredicate) noexcept requires (Self::IsDynamic());
    template <typename Predicate>
    FORCEINLINE bool     RemoveOnceByPredicateChecked(const Predicate& InPredicate) noexcept requires (Self::IsDynamic()) { const bool bOut = this->RemoveOnceByPredicate(InPredicate); check(bOut); return bOut; }
    template <typename Predicate>
    FORCEINLINE bool     RemoveOnceByPredicateAsserted(const Predicate& InPredicate) noexcept requires (Self::IsDynamic()) { const bool bOut = this->RemoveOnceByPredicate(InPredicate); jassert(bOut); return bOut; }

    FORCEINLINE SizeType Find(const T& InElement, T** OutElement) noexcept requires (Self::IsContentMutable());
    FORCEINLINE SizeType Find(const T& InElement, const T* const* OutElement) const noexcept;
    FORCEINLINE SizeType Find(const T& InElement) const noexcept;

    template <typename InOtherElement>
    FORCEINLINE SizeType Find(const InOtherElement& InElement, T** OutElement) noexcept requires (Self::IsContentMutable());
    template <typename InOtherElement>
    FORCEINLINE SizeType Find(const InOtherElement& InElement, const T* const* OutElement) const noexcept;
    template <typename InOtherElement>
    FORCEINLINE SizeType Find(const InOtherElement& InElement) const noexcept;

    template <typename Predicate>
    FORCEINLINE SizeType FindByPredicate(const Predicate& InPredicate, T** OutElement) noexcept requires (Self::IsContentMutable());
    template <typename Predicate>
    FORCEINLINE SizeType FindByPredicate(const Predicate& InPredicate, const T* const* OutElement) const noexcept;
    template <typename Predicate>
    FORCEINLINE SizeType FindByPredicate(const Predicate& InPredicate) const noexcept;

    FORCEINLINE       T* FindRef(const T& InElement, SizeType* OutIndex) noexcept requires (Self::IsContentMutable());
    FORCEINLINE const T* FindRef(const T& InElement, SizeType* OutIndex) const noexcept;
    FORCEINLINE       T* FindRef(const T& InElement) noexcept requires (Self::IsContentMutable());
    FORCEINLINE const T* FindRef(const T& InElement) const noexcept;

    template <typename InOtherElement>
    FORCEINLINE      T* FindRef(const InOtherElement& InElement, SizeType* OutIndex) noexcept requires (Self::IsContentMutable());
    template <typename InOtherElement>
    FORCEINLINE const T* FindRef(const InOtherElement& InElement, SizeType* OutIndex) const noexcept;
    template <typename InOtherElement>
    FORCEINLINE       T* FindRef(const InOtherElement& InElement) noexcept requires (Self::IsContentMutable());
    template <typename InOtherElement>
    FORCEINLINE const T* FindRef(const InOtherElement& InElement) const noexcept;

    template <typename Predicate>
    FORCEINLINE       T* FindRefByPredicate(const Predicate& InPredicate, SizeType* OutIndex) noexcept requires (Self::IsContentMutable());
    template <typename Predicate>
    FORCEINLINE const T* FindRefByPredicate(const Predicate& InPredicate, SizeType* OutIndex) const noexcept;
    template <typename Predicate>
    FORCEINLINE       T* FindRefByPredicate(const Predicate& InPredicate) noexcept requires (Self::IsContentMutable());
    template <typename Predicate>
    FORCEINLINE const T* FindRefByPredicate(const Predicate& InPredicate) const noexcept;

    FORCEINLINE bool Contains(const T& InElement) const noexcept;
    FORCEINLINE bool ContainsChecked(const T& InElement) const noexcept { const bool bOut = this->Contains(InElement); check(bOut); return bOut; }
    FORCEINLINE bool ContainsAsserted(const T& InElement) const noexcept { const bool bOut = this->Contains(InElement); jassert(bOut); return bOut; }
    template <typename InOtherElement>
    FORCEINLINE bool Contains(const InOtherElement& InElement) const noexcept;
    template <typename InOtherElement>
    FORCEINLINE bool ContainsChecked(const InOtherElement& InElement) const noexcept { const bool bOut = this->Contains(InElement); check(bOut); return bOut; }
    template <typename InOtherElement>
    FORCEINLINE bool ContainsAsserted(const InOtherElement& InElement) const noexcept { const bool bOut = this->Contains(InElement); jassert(bOut); return bOut; }
    template <typename Predicate>
    FORCEINLINE bool ContainsByPredicate(const Predicate& InPredicate) const noexcept;
    template <typename Predicate>
    FORCEINLINE bool ContainsByPredicateChecked(const Predicate& InPredicate) const noexcept { const bool bOut = this->ContainsByPredicate(InPredicate); check(bOut); return bOut; }
    template <typename Predicate>
    FORCEINLINE bool ContainsByPredicateAsserted(const Predicate& InPredicate) const noexcept { const bool bOut = this->ContainsByPredicate(InPredicate); jassert(bOut); return bOut; }

    template <typename Predicate>
    FORCEINLINE void ForEach(const Predicate& InPredicate) noexcept requires (Self::IsContentMutable());
    template <typename Predicate>
    FORCEINLINE void ForEach(const Predicate& InPredicate) const noexcept;

    //# The amount of elements that were replaced.
    FORCEINLINE SizeType Replace(const T& InElement, const T& InReplacement) noexcept requires (Self::IsContentMutable());

    //# Peeks at the last element in the array. Returns nullptr if the array is empty.
    FORCEINLINE       T* Peek()       noexcept requires (Self::IsContentMutable()) { return this->GetLast(); }
    FORCEINLINE const T* Peek() const noexcept { return this->GetLast(); }
    //# Removes the last element in the array.
    FORCEINLINE bool Pop() noexcept requires (Self::IsDynamic());
    FORCEINLINE void Pop(SizeType InCount) noexcept requires (Self::IsDynamic());

    FORCEINLINE Iterator<T>       begin()       noexcept requires (Self::IsContentMutable()) { return Iterator<T> (this->Impl.Data);  }
    FORCEINLINE Iterator<const T> begin() const noexcept { return Iterator<const T>(this->Impl.Data);  }
    FORCEINLINE Iterator<T>       end()         noexcept requires (Self::IsContentMutable()) { return Iterator<T> (this->Impl.Slack); }
    FORCEINLINE Iterator<const T> end()   const noexcept { return Iterator<const T>(this->Impl.Slack); }

    FORCEINLINE       Alloc& GetUnderlyingDataStructure()       noexcept { return this->Impl; }
    FORCEINLINE const Alloc& GetUnderlyingDataStructure() const noexcept { return this->Impl; }

private:

    FORCEINLINE void Grow() noexcept requires (Self::IsDynamic()) { this->Impl.Grow(); }
    FORCEINLINE void Grow(const SizeType InAmount) noexcept requires (Self::IsDynamic()) { this->Impl.Grow(InAmount); }

    FORCEINLINE static void Copy(Self& Dst, const Self& Src) noexcept { Dst.Impl = Src.Impl; }
    FORCEINLINE static void Move(Self& Dst, Self&& Src) noexcept { Dst.Impl = std::move(Src.Impl); }

    FORCEINLINE void DestroyAt(const SizeType InIndex) noexcept requires (Self::IsDynamic());
    FORCEINLINE void DestroyAt(T* InAddress) noexcept requires (Self::IsDynamic());

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
template<typename TOtherAlloc>
FORCEINLINE TArrayAllocator<InT, InSizeType, InTraits>::TArrayAllocator(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, InT> == false) : Data(), Slack(), End()
{
    JAFG_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )
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
    if (this == &Other)
    {
        return *this;
    }

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
template<typename TOtherAlloc>
FORCEINLINE typename TArrayAllocator<InT, InSizeType, InTraits>::Self& TArrayAllocator<InT, InSizeType, InTraits>::operator=(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, InT> == false)
{
    JAFG_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

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
void TArrayAllocator<InT, InSizeType, InTraits>::Grow() noexcept
{
    const SizeType NewCapacity = Traits::GetGrowSize(this->End - this->Data);
    JAFG_CHECK_ARRAY( NewCapacity > 0 && NewCapacity >= this->End - this->Data )

    if (this->Data == nullptr)
    {
#if LAL_WITH_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
        #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        this->Data  = static_cast<T*>(::malloc(NewCapacity * sizeof(T)));
#if LAL_WITH_GCC
        #pragma GCC diagnostic pop
#endif /* WITH_GCC */

        this->Slack = this->Data;
        this->End   = this->Data + NewCapacity;

        JAFG_CHECK_ARRAY( this->Data )

        return;
    }

    const SizeType UsedSize = this->Slack - this->Data;

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    this->Data  = static_cast<T*>(::realloc(this->Data, NewCapacity * sizeof(T)));
#if LAL_WITH_GCC
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
#if LAL_WITH_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
        #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        this->Data  = static_cast<T*>(::malloc(InAmount * sizeof(T)));
#if LAL_WITH_GCC
        #pragma GCC diagnostic pop
#endif /* WITH_GCC */

        this->Slack = this->Data;
        this->End   = this->Data + InAmount;

        JAFG_CHECK_ARRAY(this->Data)

        return;
    }

    const SizeType UsedSize = this->Slack - this->Data;

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    this->Data  = static_cast<T*>(::realloc(this->Data, InAmount * sizeof(T)));
#if LAL_WITH_GCC
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

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    this->Data  = static_cast<T*>(::realloc(this->Data, UsedSize * sizeof(T)));
#if LAL_WITH_GCC
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

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE TArrayViewAllocator<InT, InSizeType, InTraits>::TArrayViewAllocator(const Self& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    checkSlow( this->Data <= this->Slack )

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE TArrayViewAllocator<InT, InSizeType, InTraits>::TArrayViewAllocator(Self&& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    Other.Data  = nullptr;
    Other.Slack = nullptr;

    checkSlow( this->Data <= this->Slack )

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
template<typename TOtherAlloc>
FORCEINLINE TArrayViewAllocator<InT, InSizeType, InTraits>::TArrayViewAllocator(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, InT> == false)
{
    JAFG_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    checkSlow( this->Data <= this->Slack )

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE typename TArrayViewAllocator<InT, InSizeType, InTraits>::Self& TArrayViewAllocator<InT, InSizeType, InTraits>::operator=(const Self& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    checkSlow( this->Data <= this->Slack )

    return *this;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE typename TArrayViewAllocator<InT, InSizeType, InTraits>::Self& TArrayViewAllocator<InT, InSizeType, InTraits>::operator=(Self&& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    Other.Data  = nullptr;
    Other.Slack = nullptr;

    checkSlow( this->Data <= this->Slack )

    return *this;
}

template<typename InT, typename InSizeType, typename InTraits>
template<typename TOtherAlloc>
FORCEINLINE typename TArrayViewAllocator<InT, InSizeType, InTraits>::Self& TArrayViewAllocator<InT, InSizeType, InTraits>::operator=(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, InT> == false)
{
    JAFG_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    return *this;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE void TArrayViewAllocator<InT, InSizeType, InTraits>::Invalidate() noexcept
{
    this->Data  = nullptr;
    this->Slack = nullptr;

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE TMutableArrayViewAllocator<InT, InSizeType, InTraits>::TMutableArrayViewAllocator(const Self& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    checkSlow( this->Data <= this->Slack )

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE TMutableArrayViewAllocator<InT, InSizeType, InTraits>::TMutableArrayViewAllocator(Self&& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    Other.Data  = nullptr;
    Other.Slack = nullptr;

    checkSlow( this->Data <= this->Slack )

    return;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE typename TMutableArrayViewAllocator<InT, InSizeType, InTraits>::Self& TMutableArrayViewAllocator<InT, InSizeType, InTraits>::operator=(const Self& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    checkSlow( this->Data <= this->Slack )

    return *this;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE typename TMutableArrayViewAllocator<InT, InSizeType, InTraits>::Self& TMutableArrayViewAllocator<InT, InSizeType, InTraits>::operator=(Self&& Other) noexcept
{
    JAFG_CHECK_ARRAY( this != &Other )

    this->Data  = Other.Data;
    this->Slack = Other.Slack;

    Other.Data  = nullptr;
    Other.Slack = nullptr;

    checkSlow( this->Data <= this->Slack )

    return *this;
}

template<typename InT, typename InSizeType, typename InTraits>
template<typename TOtherAlloc>
FORCEINLINE typename TMutableArrayViewAllocator<InT, InSizeType, InTraits>::Self& TMutableArrayViewAllocator<InT, InSizeType, InTraits>::operator=(const TOtherAlloc& Other) noexcept requires (std::is_same_v<TOtherAlloc, InT> == false)
{
    JAFG_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->Data = Other.Data;
    this->Slack = Other.Slack;

    return *this;
}

template<typename InT, typename InSizeType, typename InTraits>
FORCEINLINE void TMutableArrayViewAllocator<InT, InSizeType, InTraits>::Invalidate() noexcept
{
    this->Data  = nullptr;
    this->Slack = nullptr;

    return;
}

template<typename InAlloc>
FORCEINLINE TArrayBase<InAlloc>::TArrayBase(std::initializer_list<T> InList) noexcept requires (Self::IsStrongAlloc() && (std::is_copy_assignable_v<T> || std::is_copy_constructible_v<T>)) : Impl()
{
    this->Reserve(InList.size());

    T* Me = this->Impl.Data;
    for (const T& Element : InList)
    {
        std::construct_at(Me, Element);
        ++Me;
    }
    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE TArrayBase<InAlloc>::TArrayBase(std::initializer_list<InOtherElement> InList) noexcept requires (Self::IsStrongAlloc() && (std::is_same_v<T, InOtherElement> == false) && std::is_convertible_v<InOtherElement, T>)
{
    this->Reserve(InList.size());

    T* Me = this->Impl.Data;
    for (const InOtherElement& Element : InList)
    {
        std::construct_at(Me, Element);
        ++Me;
    }
    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
template<typename ... InTArgs>
FORCEINLINE TArrayBase<InAlloc>::TArrayBase(InTArgs&&... InArgs) noexcept requires (Self::IsStrongAlloc() && std::is_move_constructible_v<T>)
{
    this->Reserve(sizeof...(InTArgs));

    T* Me = this->Impl.Data;
    ([&](void) -> void { new(Me++) T (std::move(InArgs)); return; } (), ...);
    this->Impl.Slack = Me;
    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
FORCEINLINE TArrayBase<InAlloc>::~TArrayBase() noexcept
{
    if constexpr (Self::IsStrongAlloc())
    {
        this->Empty();
    }

    return;
}

template<typename InAlloc>
template<typename TOtherAlloc>
FORCEINLINE TArrayBase<InAlloc>::TArrayBase(const TArrayBase<TOtherAlloc>& Other) noexcept requires (Self::IsWeakAlloc() && std::is_same_v<TOtherAlloc, InAlloc> == false)
{
    JAFG_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->Impl = Other.Impl;

    checkSlow( this->Impl.Data <= this->Impl.Slack && this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
template<typename TOtherAlloc>
FORCEINLINE TArrayBase<InAlloc>::TArrayBase(const TArrayBase<TOtherAlloc>& Other) noexcept requires (Self::IsStrongAlloc() && std::is_same_v<TOtherAlloc, InAlloc> == false) : Impl()
{
    JAFG_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->Reserve(Other.GetSize());

    T* Me = this->Impl.Data;
    for (const T& Element : Other)
    {
        std::construct_at(Me, Element);
        ++Me;
    }
    this->Impl.Slack = Me;

    checkSlow( this->Impl.Data <= this->Impl.Slack && this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::Self& TArrayBase<InAlloc>::operator=(std::initializer_list<T> InList) noexcept requires (Self::IsStrongAlloc())
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

template<typename InAlloc>
template<typename TOtherAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::Self& TArrayBase<InAlloc>::operator=(const TArrayBase<TOtherAlloc>& Other) noexcept requires (Self::IsWeakAlloc() && std::is_same_v<TOtherAlloc, InAlloc> == false)
{
    JAFG_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->Impl = Other.Impl;

    checkSlow( this->Impl.Data <= this->Impl.Slack && this->Impl.Slack <= this->Impl.End )

    return *this;
}

template<typename InAlloc>
template<typename TOtherAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::Self& TArrayBase<InAlloc>::operator=(const TArrayBase<TOtherAlloc>& Other) noexcept requires (Self::IsStrongAlloc() && std::is_same_v<TOtherAlloc, InAlloc> == false)
{
    JAFG_CHECK_ARRAY( static_cast<const void*>(this) != static_cast<const void*>(&Other) )

    this->Reset(Other.GetSize());

    T* Me = this->Impl.Data;
    for (const T& Element : Other)
    {
        std::construct_at(Me, Element);
        ++Me;
    }
    this->Impl.Slack = Me;

    checkSlow( this->Impl.Data <= this->Impl.Slack && this->Impl.Slack <= this->Impl.End )

    return *this;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::Invalidate() requires (Self::IsWeakAlloc())
{
    this->Impl.Invalidate();
    return;
}

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::MoveDataPointerUp(T* NewDataPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable())
{
    this->Impl.Data = Maths::Clamp(NewDataPointer, this->Impl.Data, this->Impl.Slack);
    return this->Impl.Data;
}

template<typename InAlloc>
FORCEINLINE const typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::MoveDataPointerUp(const T* NewDataPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst())
{
    this->Impl.Data = Maths::Clamp(NewDataPointer, this->Impl.Data, this->Impl.Slack);
    return this->Impl.Data;
}

template<typename InAlloc>
FORCEINLINE bool TArrayBase<InAlloc>::MoveDataPointerUp() noexcept requires (Self::IsWeakAlloc())
{
    if (this->Impl.Data == this->Impl.Slack)
    {
        return false;
    }

    ++this->Impl.Data;
    return true;
}

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::MoveDataPointerUp(const SizeType InOffset) noexcept requires(Self::IsWeakAlloc())
{
    if (this->Impl.Data == this->Impl.Slack)
    {
        return 0;
    }

    const SizeType Moved = Maths::Min(InOffset, this->GetSize());
    this->Impl.Data += Moved;

    return Moved;
}

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::MoveSlackPointerDown(T* NewSlackPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentMutable())
{
    this->Impl.Slack = Maths::Clamp(NewSlackPointer, this->Impl.Data, this->Impl.Slack);
    return this->Impl.Data;
}

template<typename InAlloc>
FORCEINLINE const typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::MoveSlackPointerDown(const T* NewSlackPointer) noexcept requires (Self::IsWeakAlloc() && Self::IsContentConst())
{
    this->Impl.Slack = Maths::Clamp(NewSlackPointer, this->Impl.Data, this->Impl.Slack);
    return this->Impl.Data;
}

template<typename InAlloc>
FORCEINLINE bool TArrayBase<InAlloc>::MoveSlackPointerDown() noexcept requires (Self::IsWeakAlloc())
{
    if (this->Impl.Slack == this->Impl.Data)
    {
        return false;
    }

    --this->Impl.Slack;
    return true;
}

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::MoveSlackPointerDown(const SizeType InOffset) noexcept requires (Self::IsWeakAlloc())
{
    if (this->Impl.Slack == this->Impl.Data)
    {
        return 0;
    }

    const SizeType Moved = Maths::Min(InOffset, this->GetSize());
    this->Impl.Slack -= Moved;

    return Moved;
}

template<typename InAlloc>
typename TArrayBase<InAlloc>::Self& TArrayBase<InAlloc>::CopyFrom(const Self& InOther, const SizeType InCount) noexcept requires (Self::IsStrongAlloc())
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

template<typename InAlloc>
typename TArrayBase<InAlloc>::Self& TArrayBase<InAlloc>::CopyFrom(const Self& InOther, const SizeType InOffset, const SizeType InCount) noexcept requires (Self::IsStrongAlloc())
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

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::T& TArrayBase<InAlloc>::operator[](const SizeType InIndex) noexcept requires (Self::IsContentMutable())
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )
    return this->Impl.Data[InIndex];
}

template<typename InAlloc>
FORCEINLINE const typename TArrayBase<InAlloc>::T& TArrayBase<InAlloc>::operator[](const SizeType InIndex) const noexcept
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )
    return this->Impl.Data[InIndex];
}

template<typename InAlloc>
FORCEINLINE void TArrayBase<InAlloc>::Reset(const SizeType InAmount) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
FORCEINLINE void TArrayBase<InAlloc>::Resize(const SizeType InSize, const bool bInShrinkToFit) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
FORCEINLINE void TArrayBase<InAlloc>::SwapBuffers(Self& InOther) noexcept
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

template<typename InAlloc>
FORCEINLINE void TArrayBase<InAlloc>::SwapIndices(const SizeType InIndexA, const SizeType InIndexB) noexcept requires (Self::IsContentMutable())
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndexA) && this->IsValidIndex(InIndexB) )

    if (InIndexA == InIndexB)
    {
        return;
    }

    alignas(T)
    u8 Temp[sizeof(T)];

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memcpy(Temp,                       this->Impl.Data + InIndexA, sizeof(T));
    ::memcpy(this->Impl.Data + InIndexA, this->Impl.Data + InIndexB, sizeof(T));
    ::memcpy(this->Impl.Data + InIndexB, Temp,                       sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    return;
}

template<typename InAlloc>
FORCEINLINE bool TArrayBase<InAlloc>::IsDataEqual(const Self& InOther) const noexcept
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

template<typename InAlloc>
typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Add(const T& InElement) noexcept requires (Self::IsDynamic())
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    std::construct_at(this->Impl.Slack++, InElement);

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InAlloc>
typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Add(T&& InElement) noexcept requires (Self::IsDynamic())
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    std::construct_at(this->Impl.Slack++, std::forward<T>(InElement));

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::AddAt(const SizeType InIndex, const T& InElement) noexcept requires (Self::IsDynamic())
{
    if (this->GetSize() == InIndex)
    {
        this->Add(InElement);
        return;
    }

    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )

    this->AddUninitialized();

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + 1, this->Impl.Data + InIndex, (this->GetSize() - InIndex - 1) * sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    std::construct_at(this->Impl.Data + InIndex, InElement);

    return;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::AddAt(const SizeType InIndex, T&& InElement) noexcept requires (Self::IsDynamic())
{
    if (this->GetSize() == InIndex)
    {
        this->Add(std::forward<T>(InElement));
        return;
    }

    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )

    this->AddUninitialized();

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + 1, this->Impl.Data + InIndex, (this->GetSize() - InIndex - 1) * sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    std::construct_at(this->Impl.Data + InIndex, std::forward<T>(InElement));

    return;
}

template<typename InAlloc>
typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::AddDefault() noexcept requires (Self::IsDynamic())
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    std::construct_at(this->Impl.Slack++);

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::AddDefault(SizeType InCount) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::AddZeroed() noexcept requires (Self::IsDynamic())
{
    if (this->IsCapped())
    {
        this->Grow();
    }

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memset(this->Impl.Slack++, 0, sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::AddZeroed(const SizeType InCount) noexcept requires (Self::IsDynamic())
{
    this->Reserve(InCount);

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memset(this->Impl.Slack, 0, InCount * sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    this->Impl.Slack += InCount;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::AddUninitialized() noexcept requires (Self::IsDynamic())
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    ++this->Impl.Slack;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return this->GetSize() - 1;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::AddUninitialized(const SizeType InCount) noexcept requires (Self::IsDynamic())
{
    this->Reserve(InCount);
    this->Impl.Slack += InCount;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
template<typename ... InTArgs>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Emplace(InTArgs&&... InArgs) noexcept requires (Self::IsDynamic())
{
    if (this->IsCapped())
    {
        this->Grow();
    }

    std::construct_at(this->Impl.Slack++, std::forward<InTArgs>(InArgs)...);

    return this->GetSize() - 1;
}

template<typename InAlloc>
template<typename ... InTArgs>
FORCEINLINE void TArrayBase<InAlloc>::EmplaceAt(const SizeType InIndex, InTArgs&&... InArgs) noexcept requires (Self::IsDynamic())
{
    if (this->GetSize() == InIndex)
    {
        this->Emplace(std::forward<InTArgs>(InArgs)...);
        return;
    }

    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )

    this->AddUninitialized();

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + 1, this->Impl.Data + InIndex, (this->GetSize() - InIndex - 1) * sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    std::construct_at(this->Impl.Data + InIndex, std::forward<InTArgs>(InArgs)...);

    return;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::Append(const Self& InOther) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
void TArrayBase<InAlloc>::Append(Self&& InOther) noexcept requires (Self::IsDynamic())
{
    if (InOther.GetSize() > 0)
    {
        this->Reserve(this->GetSize() + InOther.GetSize());

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        ::memcpy(this->Impl.Slack, InOther.Impl.Data, InOther.GetSize() * sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

        this->Impl.Slack += InOther.GetSize();

        InOther.Impl.Slack = InOther.Impl.Data;
        InOther.Impl.Orphan();
    }

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::Append(const T* InElements, const SizeType InCount) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
void TArrayBase<InAlloc>::Append(const T* InElements, const T* InEnd) noexcept requires (Self::IsDynamic())
{
    check( InElements <= InEnd )

    this->Reserve(this->GetSize() + (InEnd - InElements));

    T* Me = this->Impl.Slack;
    for (const T* RESTRICT Bulk = InElements; Bulk != InEnd; ++Bulk)
    {
        std::construct_at(Me, *Bulk);
        ++Me;
    }
    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::Append(std::initializer_list<T> InElements) noexcept requires (Self::IsDynamic())
{
    this->Reserve(this->GetSize() + InElements.size());

    T* Me = this->Impl.Slack;
    for (const T& Element : InElements)
    {
        std::construct_at(Me, Element);
        ++Me;
    }
    this->Impl.Slack = Me;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::AppendAt(const SizeType InIndex, const Self& InOther) noexcept requires (Self::IsDynamic())
{
    if (InOther.IsEmpty())
    {
        return;
    }

    this->Reserve(this->GetSize() + InOther.GetSize());

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + InOther.GetSize(), this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
#if LAL_WITH_GCC
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

template<typename InAlloc>
void TArrayBase<InAlloc>::AppendAt(const SizeType InIndex, Self&& InOther) noexcept requires (Self::IsDynamic())
{
    if (InOther.IsEmpty())
    {
        return;
    }

    this->Reserve(this->GetSize() + InOther.GetSize());

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + InOther.GetSize(), this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
    ::memcpy(this->Impl.Data + InIndex, InOther.Impl.Data, InOther.GetSize() * sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    this->Impl.Slack = this->Impl.Data + this->GetSize() + InOther.GetSize();

    InOther.Impl.Slack = InOther.Impl.Data;
    InOther.Impl.Orphan();

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
void TArrayBase<InAlloc>::AppendAt(SizeType InIndex, const T* InElements, const SizeType InCount) noexcept requires (Self::IsDynamic())
{
    if (InCount == 0)
    {
        return;
    }

    this->Reserve(this->GetSize() + InCount);

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + InCount, this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
#if LAL_WITH_GCC
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

template<typename InAlloc>
void TArrayBase<InAlloc>::AppendAt(const SizeType InIndex, std::initializer_list<T> InElements) noexcept requires (Self::IsDynamic())
{
    this->Reserve(this->GetSize() + InElements.size());

#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
    ::memmove(this->Impl.Data + InIndex + InElements.size(), this->Impl.Data + InIndex, (this->GetSize() - InIndex) * sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */

    T* Me = this->Impl.Data + InIndex;
    for (const T* RESTRICT Bulk = InElements.begin(); Bulk != InElements.end(); ++Bulk)
    {
        std::construct_at(Me, *Bulk);
        ++Me;
    }
    this->Impl.Slack = this->Impl.Data + this->GetSize() + InElements.size();

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
FORCEINLINE void TArrayBase<InAlloc>::RemoveAt(const SizeType InIndex) noexcept requires (Self::IsDynamic())
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )

    this->DestroyAt(InIndex);

    if (InIndex < this->GetSize() - 1)
    {
#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        ::memmove(this->Impl.Data + InIndex, this->Impl.Data + InIndex + 1, (this->GetSize() - InIndex - 1) * sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */
    }

    --this->Impl.Slack;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
FORCEINLINE void TArrayBase<InAlloc>::RemoveAt(const SizeType InIndex, const SizeType InCount) noexcept requires (Self::IsDynamic())
{
    JAFG_CHECK_ARRAY( InCount > 0 && InIndex >= 0 && this->IsValidIndex(InIndex + InCount) )

    for (SizeType Index = InIndex; Index < InIndex + InCount; ++Index)
    {
        this->DestroyAt(Index);
    }

    if (InIndex + InCount < this->GetSize())
    {
#if LAL_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnontrivial-memcall"
    #pragma GCC diagnostic ignored "-Wdynamic-class-memaccess"
#endif /* WITH_GCC */
        ::memmove(this->Impl.Data + InIndex, this->Impl.Data + InIndex + InCount, (this->GetSize() - InIndex - InCount) * sizeof(T));
#if LAL_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */
    }

    this->Impl.Slack -= InCount;

    checkSlow( this->Impl.Slack <= this->Impl.End )

    return;
}

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Remove(const T& InElement) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
FORCEINLINE bool TArrayBase<InAlloc>::RemoveOnce(const T& InElement) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Remove(const InOtherElement& InElement) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE bool TArrayBase<InAlloc>::RemoveOnce(const InOtherElement& InElement) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::RemoveByPredicate(const Predicate& InPredicate) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE bool TArrayBase<InAlloc>::RemoveOnceByPredicate(const Predicate& InPredicate) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Find(const T& InElement, T** OutElement) noexcept requires (Self::IsContentMutable())
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

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Find(const T& InElement, const T* const* OutElement) const noexcept
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

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Find(const T& InElement) const noexcept
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Find(const InOtherElement& InElement, T** OutElement) noexcept requires (Self::IsContentMutable())
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Find(const InOtherElement& InElement, const T* const* OutElement) const noexcept
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Find(const InOtherElement& InElement) const noexcept
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::FindByPredicate(const Predicate& InPredicate, T** OutElement) noexcept requires (Self::IsContentMutable())
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::FindByPredicate(const Predicate& InPredicate, const T* const* OutElement) const noexcept
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::FindByPredicate(const Predicate& InPredicate) const noexcept
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

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRef(const T& InElement, SizeType* OutIndex) noexcept requires (Self::IsContentMutable())
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

template<typename InAlloc>
FORCEINLINE const typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRef(const T& InElement, SizeType* OutIndex) const noexcept
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

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRef(const T& InElement) noexcept requires (Self::IsContentMutable())
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

template<typename InAlloc>
FORCEINLINE const typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRef(const T& InElement) const noexcept
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRef(const InOtherElement& InElement, SizeType* OutIndex) noexcept requires (Self::IsContentMutable())
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE const typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRef(const InOtherElement& InElement, SizeType* OutIndex) const noexcept
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRef(const InOtherElement& InElement) noexcept requires (Self::IsContentMutable())
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE const typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRef(const InOtherElement& InElement) const noexcept
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRefByPredicate(const Predicate& InPredicate, SizeType* OutIndex) noexcept requires (Self::IsContentMutable())
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE const typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRefByPredicate(const Predicate& InPredicate, SizeType* OutIndex) const noexcept
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRefByPredicate(const Predicate& InPredicate) noexcept requires (Self::IsContentMutable())
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE const typename TArrayBase<InAlloc>::T* TArrayBase<InAlloc>::FindRefByPredicate(const Predicate& InPredicate) const noexcept
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

template<typename InAlloc>
FORCEINLINE bool TArrayBase<InAlloc>::Contains(const T& InElement) const noexcept
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

template<typename InAlloc>
template<typename InOtherElement>
FORCEINLINE bool TArrayBase<InAlloc>::Contains(const InOtherElement& InElement) const noexcept
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE bool TArrayBase<InAlloc>::ContainsByPredicate(const Predicate& InPredicate) const noexcept
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

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE void TArrayBase<InAlloc>::ForEach(const Predicate& InPredicate) noexcept requires (Self::IsContentMutable())
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        InPredicate(this->Impl.Data[Index]);
    }

    return;
}

template<typename InAlloc>
template<typename Predicate>
FORCEINLINE void TArrayBase<InAlloc>::ForEach(const Predicate& InPredicate) const noexcept
{
    for (SizeType Index = 0; Index < this->GetSize(); ++Index)
    {
        InPredicate(this->Impl.Data[Index]);
    }

    return;
}

template<typename InAlloc>
FORCEINLINE typename TArrayBase<InAlloc>::SizeType TArrayBase<InAlloc>::Replace(const T& InElement, const T& InReplacement) noexcept requires (Self::IsContentMutable())
{
    SizeType Replaced = 0;
    this->ForEach([&Replaced, &InElement, &InReplacement](T& InValue)
    {
        if (InValue == InElement)
        {
            InValue = InReplacement;
            ++Replaced;
        }

        return;
    });

    return Replaced;
}

template<typename InAlloc>
FORCEINLINE bool TArrayBase<InAlloc>::Pop() noexcept requires (Self::IsDynamic())
{
    if (this->GetSize() > 0)
    {
        this->DestroyAt(this->Impl.Slack - 1);
        --this->Impl.Slack;
        return true;
    }

    return false;
}

template<typename InAlloc>
FORCEINLINE void TArrayBase<InAlloc>::Pop(SizeType InCount) noexcept requires (Self::IsDynamic())
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

template<typename InAlloc>
FORCEINLINE void TArrayBase<InAlloc>::DestroyAt(const SizeType InIndex) noexcept requires (Self::IsDynamic())
{
    JAFG_CHECK_ARRAY( this->IsValidIndex(InIndex) )
    this->Impl.Data[InIndex].~T();
    return;
}

template<typename InAlloc>
FORCEINLINE void TArrayBase<InAlloc>::DestroyAt(T* InAddress) noexcept requires (Self::IsDynamic())
{
    JAFG_CHECK_ARRAY( InAddress >= this->Impl.Data && InAddress < this->Impl.Slack )
    InAddress->~T();
    return;
}

} /* ~Namespace Jafg */

#undef JAFG_CHECK_ARRAY
