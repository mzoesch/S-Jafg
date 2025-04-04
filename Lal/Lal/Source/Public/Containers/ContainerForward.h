// Copyright mzoesch. All rights reserved.

#pragma once

#include "Hal/Platform.h"

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
    /**
     * Single producer; single consumer.
     */
    Spsc,

    /**
     * Multiple producers; single consumer.
     */
    Mpsc,

    /**
     * Multiple producers; multiple consumers.
     */
    Mpmc,
};

} /* ~Namespace EQueueKind */

/**
 * Basic traits an allocator must implement.
 */
template <typename InSizeType>
struct TArrayAllocatorTraits;
/**
 * An allocator for TArrayBase.
 * @see JafgArray.h for more information about what traits to implement.
 */
template <typename InT, typename InSizeType, typename InTraits>
struct TArrayAllocator;
template <typename InT, typename InAlloc>
class TArrayBase;

template <typename InT>
struct TStringTraits;
template <typename InTraits, typename InAlloc>
class TStringBase;

template <typename T, typename TSizeType = DefaultContainerSizeType>
class TMpscQueue;
template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc, typename TSizeType = DefaultContainerSizeType>
class TSimpleQueue;


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

/**
 * The default array for jafg. This array is dynamic and allocates memory on the heap.
 */
template <typename T> using TArray = TArrayBase<T, TArrayAllocator<T, DefaultContainerSizeType, TArrayAllocatorTraits<DefaultContainerSizeType>>>;

template <typename InT>
using TStringBasic = TStringBase<TStringTraits<InT>, TArray<InT>>;

/**
 * The default string for jafg.
 * This string is utf8-encoded and dynamic.
 */
typedef TStringBasic<char> LString;

template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc> using TQueue = TSimpleQueue<T, TKind>;

using LStringView = std::basic_string_view<char>;

} /* ~Namespace Jafg */
