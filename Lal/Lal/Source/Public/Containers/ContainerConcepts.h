// Copyright mzoesch. All rights reserved.

#pragma once

///////////////////////////////////////////////////////////////////////////////
// Array.
///////////////////////////////////////////////////////////////////////////////
namespace Lal
{

//#
//# The traits a trait of a weak allocator must implement to qualify as a weak array base allocator trait.
//#
template <typename T>
concept TArrayBaseAllocatorTraitsConceptBase = requires
{
    //#
    //# The size type that the allocator is going to use for its internal memory management.
    //#
    typename T::SizeType;
    requires std::integral<typename T::SizeType>;

    //#
    //# The iterator type that the allocator is going to use.
    //# The iterator must be instantiable with const T. But may also be constructed with T
    //# or any other type U if applicable.
    //#
    //# We here just check if there is even an iterator that can be constructed with const LSize.
    //# It is not the responsibility for traits to provide concrete data types.
    //#
    typename T::template Iterator<const LSize>;
};

//#
//# The traits a trait of a strong allocator must implement to qualify as a strong array base allocator trait.
//#
template <typename T>
concept TArrayBaseAllocatorTraitsConceptStrong = TArrayBaseAllocatorTraitsConceptBase<T> && requires
{
    //#
    //# The growth function that returns the new size of the array given its current size.
    //#
    { T::GetGrowSize(std::declval<typename T::SizeType>()) } -> std::convertible_to<typename T::SizeType>;
};

//#
//# The traits a static allocator must implement to qualify as a static array base allocator trait.
//#
template <typename T>
concept TArrayBaseAllocatorTraitsConceptWeak = TArrayBaseAllocatorTraitsConceptBase<T>;

template <typename T>
concept TArrayBaseCappedAllocatorTraitsConceptWeak = TArrayBaseAllocatorTraitsConceptBase<T> && requires
{
    { T::SizeCapacity } -> std::convertible_to<typename T::SizeType>;
};

template <typename T>
concept TArrayBaseCappedAllocatorTraitsConceptStrong = TArrayBaseAllocatorTraitsConceptStrong<T> && requires
{
    { T::SizeCapacity } -> std::convertible_to<typename T::SizeType>;
};

//#
//# The traits an allocator must implement to qualify as an array base allocator.
//#
template <typename T>
concept TArrayBaseAllocatorConceptBase = requires(T Allocator)
{
    //#
    //# The actual underlying allocator type.
    //#
    typename T::T;

    //#
    //# The allocator traits that the allocator uses for its internal memory management.
    //#
    typename T::Traits;
    requires TArrayBaseAllocatorTraitsConceptBase<typename T::Traits>;

    //# @see #TArrayBaseAllocatorTraitsConceptBase.
    typename T::SizeType;
    requires std::integral<typename T::SizeType>;

    //#
    //# The iterator type that the allocator is going to use.
    //#
    typename T::Iterator;
    typename T::ConstIterator;

    //#
    //# A fully qualified pointer type.
    //#
    typename T::Pointer;
    typename T::ConstPointer;

    //#
    //# A fully qualified reference type.
    //#
    typename T::Reference;
    typename T::ConstReference;

    //#
    //# For internal use only. Do not call.
    //#
    //# Resets the allocator to its default state. It must assume that relevant data was exported and heap
    //# allocated memory was successfully freed or re-owned.
    //# Therefore it has to correctly implement a function #IsCurrentDataOnHeap if special handling is required.
    //#
    { Allocator._ResetToDefaultState() } -> std::same_as<void>;

    //#
    //# Whether this array owns the memory it uses for its data.
    //#
    { Allocator.IsOwningMemoryAllocator() } -> std::convertible_to<bool>;
};

//#
//# The traits an allocator must implement to qualify as an array base allocator with mutable data.
//#
template <typename T>
concept TArrayBaseMutableAllocatorConceptBase = TArrayBaseAllocatorConceptBase<T> && requires(T Allocator)
{
    //#
    //# A pointer to the beginning of the allocated memory or nullptr if no memory is allocated.
    //#
    { +Allocator.Data } -> std::same_as<typename T::T*>;

    //#
    //# A pointer to the first element of the slack or nullptr if no slack is allocated.
    //#
    { +Allocator.Slack } -> std::same_as<typename T::T*>;

    //#
    //# A pointer to the first element after the slack or nullptr if no memory is allocated.
    //#
    { +Allocator.End } -> std::same_as<typename T::T*>;
};

//#
//# The traits an allocator must implement to qualify as an array base allocator with const data.
//#
template <typename T>
concept TArrayBaseConstAllocatorConceptBase = TArrayBaseAllocatorConceptBase<T> && requires(T Allocator)
{
    //#
    //# A pointer to the beginning of the allocated memory or nullptr if no memory is allocated.
    //#
    { +Allocator.Data } -> std::same_as<const typename T::T*>;

    //#
    //# A pointer to the first element of the slack or nullptr if no slack is allocated.
    //#
    { +Allocator.Slack } -> std::same_as<const typename T::T*>;

    //#
    //# A pointer to the first element after the slack or nullptr if no memory is allocated.
    //#
    { +Allocator.End } -> std::same_as<const typename T::T*>;
};

template <typename T>
concept TArrayBaseMutableAllocatorConceptStrong = TArrayBaseMutableAllocatorConceptBase<T> && requires(T Allocator)
{
    { Allocator.Grow() };
    { Allocator.GrowTo(std::declval<typename T::SizeType>()) };

    { Allocator.Resize(std::declval<typename T::SizeType>()) } -> std::same_as<typename T::SizeType>;

    { Allocator.ShrinkToFit() };
    { Allocator.ShrinkTo(std::declval<typename T::SizeType>()) };

    { Allocator.Orphan() };
};

template <typename T>
concept TArrayBaseConstAllocatorConceptWeak = TArrayBaseConstAllocatorConceptBase<T> &&
(
    requires(T Allocator)
    {
        { Allocator.Orphan() };
    }
    ||
    requires(T Allocator)
    {
        { Allocator.Empty() };
    }
);

template <typename T>
concept TArrayBaseMutableAllocatorConceptWeak = TArrayBaseMutableAllocatorConceptBase<T> &&
(
    requires(T Allocator)
    {
        { Allocator.Orphan() };
    }
    ||
    requires(T Allocator)
    {
        { Allocator.Empty() };
    }
);

template <typename T>
concept TArrayBaseMutableCappedAllocatorConceptWeak = TArrayBaseMutableAllocatorConceptWeak<T> && requires
{
    { T::SizeCapacity } -> std::convertible_to<typename T::SizeType>;
};

template <typename T>
concept TArrayBaseMutableCapacityAllocatorConceptStrong = TArrayBaseMutableAllocatorConceptStrong<T> && requires
{
    { T::SizeCapacity } -> std::convertible_to<typename T::SizeType>;
};

} /* ~Namespace Lal */
