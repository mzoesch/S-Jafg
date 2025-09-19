// Copyright mzoesch. All rights reserved.

#pragma once


///////////////////////////////////////////////////////////////////////////////
// Iterator.
///////////////////////////////////////////////////////////////////////////////
namespace Lal
{

//#
//# The factories used by Lal containers to create iterators.
//#
template <typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultIteratorFactory;
template <typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultReversedIteratorFactory;
template <typename TPredicate, typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultFilteredIteratorFactory;
template <typename TPredicate, typename TIterator, typename TReversedIterator, template <typename> typename TFilteredIterator, template <typename> typename TReversedFilteredIterator>
struct TDefaultReversedFilteredIteratorFactory;

//#
//# The default iterators used by Lal containers.
//# You may define your own iterators if you want to. You may also define your own factories.
//#
template <typename TIn, bool bMove>
struct TDefaultIterator;
template <typename TIn, bool bMove>
struct TDefaultReversedIterator;
template <typename TPredicate, typename TIn, bool bMove>
struct TDefaultFilteredIterator;
template <typename TPredicate, typename TIn, bool bMove>
struct TDefaultReversedFilteredIterator;

} /* ~Namespace Lal */


///////////////////////////////////////////////////////////////////////////////
// Array.
///////////////////////////////////////////////////////////////////////////////
namespace Lal
{

template <std::integral TSizeType, template <typename, bool> typename TIterator>
struct TArrayBaseAllocatorDefaultTraitsStrong;

template <std::integral TSizeType, template <typename, bool> typename TIterator>
struct TArrayBaseAllocatorDefaultTraitsWeak;

//#
//# Whether to allow trivial memory buffer moves even on non-trivial move types.
//#
template <typename T>
struct TArrayBaseAllowTrivialMemoryBufferMove : BoolConstant<std::is_trivially_move_constructible_v<T>>
{
};

template <typename T>
inline constexpr bool TArrayBaseAllowTrivialMemoryBufferMove_v = TArrayBaseAllowTrivialMemoryBufferMove<T>::Value;

template <std::integral TSizeType, template <typename, bool> typename TIterator>
struct TArrayBaseAllocatorDefaultTraitsStrong : public TArrayBaseAllocatorDefaultTraitsWeak<TSizeType, TIterator>
{
    typedef TArrayBaseAllocatorDefaultTraitsWeak<TSizeType, TIterator> _Super;

    typedef typename _Super::SizeType SizeType;

    template <SizeType Current>
    NODISCARD
    FORCEINLINE consteval static SizeType GetGrowSizeStatic() noexcept
    {
        return Current < 5 ? 5 : Current < 20 ? 20 : Current * 2;
    }

    NODISCARD
    FORCEINLINE constexpr static SizeType GetGrowSize(const SizeType Current) noexcept
    {
        return Current < 5 ? 5 : Current < 20 ? 20 : Current * 2;
    }
};

template <std::integral TSizeType, template <typename, bool> typename TIterator, TSizeType TSizeCapacity>
struct TArrayBaseCappedAllocatorDefaultTraitsWeakImpl : public TArrayBaseAllocatorDefaultTraitsWeak<TSizeType, TIterator>
{
    typedef TArrayBaseAllocatorDefaultTraitsWeak<TSizeType, TIterator> _Super;

    typedef typename _Super::SizeType SizeType;

    static constexpr SizeType SizeCapacity { static_cast<SizeType>(TSizeCapacity) };
};

template <std::integral TSizeType, template <typename, bool> typename TIterator, TSizeType TSizeCapacity>
struct TArrayBaseCappedAllocatorDefaultTraitStrongImpl : public TArrayBaseAllocatorDefaultTraitsStrong<TSizeType, TIterator>
{
    typedef TArrayBaseAllocatorDefaultTraitsStrong<TSizeType, TIterator> _Super;

    typedef typename _Super::SizeType SizeType;

    static constexpr SizeType SizeCapacity { static_cast<SizeType>(TSizeCapacity) };
};

} /* ~Namespace Lal */
