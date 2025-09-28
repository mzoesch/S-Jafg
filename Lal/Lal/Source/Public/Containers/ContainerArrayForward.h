// Copyright mzoesch. All rights reserved.

#pragma once

///////////////////////////////////////////////////////////////////////////////
// Array.
///////////////////////////////////////////////////////////////////////////////
namespace Lal
{

/*----------------------------------------------------------------------------
    Forward implementations.
----------------------------------------------------------------------------*/

template <std::integral TSizeType, template <typename, bool> typename TIterator, TSizeType TSizeCapacity>
struct TArrayBaseCappedAllocatorDefaultTraitsWeakImpl;

template <std::integral TSizeType, template <typename, bool> typename TIterator, TSizeType TSizeCapacity>
struct TArrayBaseCappedAllocatorDefaultTraitStrongImpl;

#if PRIVATE_LAL_WITH_LEGACY_ALLOCATORS
    template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
    struct TArrayBaseMutableDefaultAllocatorStrongImpl;

    template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
    struct TArrayBaseConstDefaultAllocatorWeakImpl;

    template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
    struct TArrayBaseMutableDefaultAllocatorWeakImpl;

    template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
    struct TArrayBaseMutableDefaultFixedAllocatorWeakImpl;

    template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptWeak TTraits>
    struct TArrayBaseMutableDefaultStackAllocatorWeakImpl;

    template <typename TIn, TArrayBaseCappedAllocatorTraitsConceptStrong TTraits>
    struct TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl;
#endif /* PRIVATE_LAL_WITH_LEGACY_ALLOCATORS */

template <typename TIn, TArrayBaseAllocatorTraitsConceptStrong TTraits>
struct TArrayBaseDefaultAllocatorHeapImpl;

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseDefaultAllocatorViewImpl;

template <typename TIn, TArrayBaseAllocatorTraitsConceptBase TTraits>
struct TArrayBaseDefaultAllocatorMutableViewImpl;

//#
//# An array container that may use any form of (un-)owned stack or heap allocated memory.
//#
//# This container checks OOB accesses and all inputs for correctness during debug and development builds
//# but will not check these in release builds.
//#
//# This array may not handle complex types that require move semantics as it was designed to be
//# a fast and simple container for extreme fast-paced memory read and write operations.
//#
//# What copy and move assignment semantics are called?
//# - Copy ctors / assignments from any variable: Always.
//# - Copy ctors / assignments from any other array: Always.
//# - Move assignments from any statement: Always
//# - Move ctors from another x- and pr-valued statement: Always.
//# - Move ctors from another x- and pr-valued array: Sometimes, depending on the allocator and their state.
//# - Move ctors during buffer reallocation: Never.
//# - Move ctors during reallocation inside the parent array: Mostly never.
//#
//# Of course, this only applies to the default allocators that Lal provides. You may define your own allocators
//# that follow rules differently.
//#
//# @note dtors will always be correctly called.
//#
template <TArrayBaseAllocatorConceptBase TAllocator>
class TArrayBase;


/*----------------------------------------------------------------------------
    Core implementations.
----------------------------------------------------------------------------*/

template <std::integral TSizeType, template <typename, bool> typename TIterator>
struct TArrayBaseAllocatorDefaultTraitsWeak
{
    typedef TSizeType SizeType;

    template <typename T, bool bMove = false>
    using Iterator = TIterator<T, bMove>;
};


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

typedef TArrayBaseAllocatorDefaultTraitsStrong<LSize, Lal::TDefaultIterator> LArrayBaseAllocatorDefaultTraitsStrong;
typedef TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator> LArrayBaseAllocatorDefaultTraitsWeak;

template <LSize TSizeCapacity>
using TArrayBaseCappedAllocatorDefaultTraitsWeak
    = TArrayBaseCappedAllocatorDefaultTraitsWeakImpl<LSize, Lal::TDefaultIterator, TSizeCapacity>;
template <LSize TSizeCapacity>
using TArrayBaseCappedAllocatorDefaultTraitsStrong
    = TArrayBaseCappedAllocatorDefaultTraitStrongImpl<LSize, Lal::TDefaultIterator, TSizeCapacity>;

#if PRIVATE_LAL_WITH_LEGACY_ALLOCATORS
    template <typename T>
    using TArrayBaseMutableDefaultAllocatorStrong
        = TArrayBaseMutableDefaultAllocatorStrongImpl<T, LArrayBaseAllocatorDefaultTraitsStrong>;
    template <typename T>
    using TArrayBaseConstDefaultAllocatorWeak
        = TArrayBaseConstDefaultAllocatorWeakImpl<T, LArrayBaseAllocatorDefaultTraitsWeak>;
    template <typename T>
    using TArrayBaseMutableDefaultAllocatorWeak
        = TArrayBaseMutableDefaultAllocatorWeakImpl<T, LArrayBaseAllocatorDefaultTraitsWeak>;
    template <typename T, TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator>::SizeType TSizeCapacity>
    using TArrayBaseMutableDefaultFixedAllocatorWeak
        = TArrayBaseMutableDefaultFixedAllocatorWeakImpl<T, TArrayBaseCappedAllocatorDefaultTraitsWeak<TSizeCapacity>>;
    template <typename T, TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator>::SizeType TSizeCapacity>
    using TArrayBaseMutableDefaultStackAllocatorWeak
        = TArrayBaseMutableDefaultStackAllocatorWeakImpl<T, TArrayBaseCappedAllocatorDefaultTraitsWeak<TSizeCapacity>>;
    template <typename T, TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator>::SizeType TStackSizeCapacity>
    using TArrayBaseMutableDefaultStackOptimizedAllocatorStrong
        = TArrayBaseMutableDefaultStackOptimizedAllocatorStrongImpl<T, TArrayBaseCappedAllocatorDefaultTraitsStrong<TStackSizeCapacity>>;
#endif /* PRIVATE_LAL_WITH_LEGACY_ALLOCATORS */

template <typename T>
using TArrayBaseDefaultHeapAllocator
    = TArrayBaseDefaultAllocatorHeapImpl<T, LArrayBaseAllocatorDefaultTraitsStrong>;
template <typename T>
using TArrayBaseDefaultViewAllocator
    = TArrayBaseDefaultAllocatorViewImpl<T, LArrayBaseAllocatorDefaultTraitsWeak>;
template <typename T>
using TArrayBaseDefaultMutableViewAllocator
    = TArrayBaseDefaultAllocatorMutableViewImpl<T, LArrayBaseAllocatorDefaultTraitsWeak>;

} /* ~Namespace Lal */

//#
//# An array that lives on the heap and own the memory it uses.
//#
//# @tparam T The type of the elements in this array.
//#
template <typename T>
using TArray = Lal::TArrayBase<Lal::TArrayBaseDefaultHeapAllocator<T>>;

//#
//# An array that has a view on some memory of any other array.
//# It may resize itself, but this will not affect the original array.
//# It does not own the memory it uses and cannot modify it.
//#
//# @tparam T The type of the elements in this array.
//#
template <typename T>
using TArrayView = Lal::TArrayBase<Lal::TArrayBaseDefaultViewAllocator<T>>;

//#
//# An array that has a view on some memory of any other array.
//# It may resize itself, but this will not affect the original array.
//# It does not own the memory it uses but can modify it.
//#
//# @tparam T The type of the elements in this array.
//#
template <typename T>
using TMutableArrayView = Lal::TArrayBase<Lal::TArrayBaseDefaultMutableViewAllocator<T>>;

#if PRIVATE_LAL_WITH_LEGACY_ALLOCATORS
    //#
    //# An array that pre-allocates a specific number of elements and lives on the heap.
    //# This array does not grow or shrink, it is fixed in its maximal size.
    //#
    //# @tparam T             The type of the elements in this array.
    //# @tparam TSizeCapacity The size of the heap buffer in number of elements. This is the maximal size this array
    //#                       can ever hold.
    //#
    template <typename T, LSize TSizeCapacity>
    using TFixedArray = Lal::TArrayBase<Lal::TArrayBaseMutableDefaultFixedAllocatorWeak<T, TSizeCapacity>>;

    //#
    //# An array that pre-allocates a specific number of elements and lives on the stack.
    //# It does therefore not allocate any memory in the heap.
    //# This array does not grow or shrink, it is fixed in its maximal size.
    //#
    //# @tparam T             The type of the elements in this array.
    //# @tparam TSizeCapacity The size of the stack buffer in number of elements. This is the maximal size this array
    //#                       can ever hold.
    //#
    template <typename T, LSize TSizeCapacity>
    using TStackArray = Lal::TArrayBase<Lal::TArrayBaseMutableDefaultStackAllocatorWeak<T, TSizeCapacity>>;

    //#
    //# An array that has an internal buffer on the stack and uses the heap for larger arrays.
    //# If a small array is used it will use the stack buffer, if it gets larger it will automatically switch to the heap;
    //# and if it gets smaller again, it will switch back to the stack buffer and orphan its heap memory.
    //#
    //# @tparam T                  The type of the elements in this array.
    //# @tparam TStackSizeCapacity The size of the stack buffer in number of elements.
    //#
    template <typename T, Lal::TArrayBaseAllocatorDefaultTraitsWeak<LSize, Lal::TDefaultIterator>::SizeType TStackSizeCapacity>
    using TStackOptimizedArray = Lal::TArrayBase<Lal::TArrayBaseMutableDefaultStackOptimizedAllocatorStrong<T, TStackSizeCapacity>>;
#endif /* PRIVATE_LAL_WITH_LEGACY_ALLOCATORS */
