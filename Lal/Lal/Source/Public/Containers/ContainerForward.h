// Copyright mzoesch. All rights reserved.

#pragma once

#include "Hal/Platform.h"

namespace Jafg
{


/*----------------------------------------------------------------------------
    Type Forwards.
----------------------------------------------------------------------------*/

typedef int32 DefaultContainerSizeType;

namespace ResizePolicy
{

enum Type : uint8
{
    Static,
    Dynamic
};

} /* ~Namespace ResizePolicy */

namespace AllocationPolicy
{

enum Type : uint8
{
    Stack,
    Heap
};

} /* ~Namespace AllocationPolicy */

namespace EQueueKind
{

enum Type : uint8
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

template
<
    typename                T                   ,
    ResizePolicy::Type      ResizePolicy        ,
    AllocationPolicy::Type  AllocationPolicy    ,
    typename                SizeType            = DefaultContainerSizeType
>
class TArray;

class LAsciiString;
class LUniEightString;

template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc, typename TSizeType = DefaultContainerSizeType>
class TComplexQueue;
template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc, typename TSizeType = DefaultContainerSizeType>
class TSimpleQueue;


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

template <typename T> using TStaticHeapArray        = TArray<T, ResizePolicy::Static , AllocationPolicy::Heap >;
template <typename T> using TDynamicHeapArray       = TArray<T, ResizePolicy::Dynamic, AllocationPolicy::Heap >;
template <typename T> using TStaticStackArray       = TArray<T, ResizePolicy::Static , AllocationPolicy::Stack>;
template <typename T> using TDynamicStackArray      = TArray<T, ResizePolicy::Dynamic, AllocationPolicy::Stack>;

template <typename T> using TshArray                = TStaticHeapArray<T>;
template <typename T> using TdhArray                = TDynamicHeapArray<T>;
template <typename T> using TssArray                = TStaticStackArray<T>;
template <typename T> using TdsArray                = TDynamicStackArray<T>;

typedef LAsciiString        LSimpleString;
typedef LUniEightString     LString;

template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc> using TQueue = TSimpleQueue<T, TKind>;


/*----------------------------------------------------------------------------
    Functional Forwards.
----------------------------------------------------------------------------*/

template <typename ... ArgTy>
LSimpleString Format(const LChar* Format, const ArgTy& ... Args);

template <typename ... ArgTy>
LStringLegacy FormatLegacy(const LChar* Format, const ArgTy& ... Args);

} /* ~Namespace Jafg */
