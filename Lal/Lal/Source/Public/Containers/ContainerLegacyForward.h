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

template <typename T, typename TSizeType = DefaultContainerSizeType>
class TMpmcQueue;
template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc, typename TSizeType = DefaultContainerSizeType>
class TSimpleQueue;


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

template <typename T, EQueueKind::Type TKind = EQueueKind::Spsc> using TQueue = TMpmcQueue<T, DefaultContainerSizeType>;

} /* ~Namespace Jafg */
