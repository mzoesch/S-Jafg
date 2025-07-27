// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{


/*----------------------------------------------------------------------------
    Type Forwards.
----------------------------------------------------------------------------*/

typedef i32 DefaultContainerSizeType;

namespace EQueueKind
{

enum Type : u8
{
    //#
    //# Single producer; single consumer.
    //#
    Spsc,

    //#
    //# Multiple producers; single consumer.
    //#
    Mpsc,

    //#
    //# Multiple producers; multiple consumers.
    //#
    Mpmc,
};

} /* ~Namespace EQueueKind */

//#
//# Basic traits an allocator must implement.
//#
template <typename InSizeType>
struct TArrayAllocatorTraits;
//#
//# An allocator for TArrayBaseOldv2.
//# @see JafgArray.h for more information about what traits to implement.
//#
template <typename InT, typename InSizeType, typename InTraits>
struct TArrayAllocator;

//#
//# Basic traits an allocator view must implement.
//#
template <typename InSizeType>
struct TArrayViewAllocatorTraits;
template <typename InSizeType>
struct TMutableArrayViewAllocatorTraits;
//#
//# A view on a TArrayBaseOldv2.
//#
template <typename InT, typename InSizeType, typename InTraits>
struct TArrayViewAllocator;
template <typename InT, typename InSizeType, typename InTraits>
struct TMutableArrayViewAllocator;

template <typename InAlloc>
class TArrayBaseOldv2;

template <typename InT>
struct TStringTraits;
template <typename Derived, typename InTraits, typename InAlloc>
class TStringBase;
template <typename InTraits, typename InAlloc>
class _TStringBase;

template <typename T, typename TSizeType = DefaultContainerSizeType>
class TMpmcQueue;
template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc, typename TSizeType = DefaultContainerSizeType>
class TSimpleQueue;


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

//#
//# The default array for jafg. This array is dynamic and allocates memory on the heap.
//#
template <typename T> using TArrayOldv2 = TArrayBaseOldv2<TArrayAllocator<T, DefaultContainerSizeType, TArrayAllocatorTraits<DefaultContainerSizeType>>>;

//#
//# The default array view for jafg. This array view is a view on any TArrayBase.
//#
template <typename T> using TArrayViewOldv2 = TArrayBaseOldv2<TArrayViewAllocator<T, DefaultContainerSizeType, TArrayViewAllocatorTraits<DefaultContainerSizeType>>>;
template <typename T> using TMutableArrayViewOldv2 = TArrayBaseOldv2<TMutableArrayViewAllocator<T, DefaultContainerSizeType, TArrayViewAllocatorTraits<DefaultContainerSizeType>>>;

template <typename InT>
using TStringBasic = _TStringBase<TStringTraits<InT>, TArrayOldv2<InT>>;
template <typename InT>
using TStringViewBasic = _TStringBase<TStringTraits<InT>, TArrayViewOldv2<InT>>;
template <typename InT>
using TMutableStringViewBasic = _TStringBase<TStringTraits<InT>, TMutableArrayViewOldv2<InT>>;

//#
//# The default string for jafg.
//# This string is utf8-encoded and dynamic.
//#
typedef TStringBasic<char> LString;
//#
//# The default string view for jafg.
//# This string view expects an utf8-encoded string.
//#
typedef TStringViewBasic<char>        LStringView;
typedef TMutableStringViewBasic<char> LMutableStringView;

template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc> using TQueue = TMpmcQueue<T, DefaultContainerSizeType>;

} /* ~Namespace Jafg */
