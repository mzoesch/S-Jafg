// Copyright mzoesch. All rights reserved.

#pragma once

#include "HAL/Platform.h"

namespace Jafg
{


/*----------------------------------------------------------------------------
    Type Forwards.
----------------------------------------------------------------------------*/

typedef int32                                   DefaultContainerSizeType;

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

template
<
    typename                T,
    ResizePolicy::Type      ResizePolicy        = ResizePolicy::Static,
    AllocationPolicy::Type  AllocationPolicy    = AllocationPolicy::Heap,
    typename                SizeType            = DefaultContainerSizeType
>
class TArray;
// class LString;


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

} /* ~Namespace Jafg */
