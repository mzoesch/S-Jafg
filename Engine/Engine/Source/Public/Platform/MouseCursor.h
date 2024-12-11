// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

namespace EMouseCursor
{

enum Type : uint8
{

    None,
    Default,
};

} /* ~Namespace EMouseCursor */

inline LSimpleString LexToString(const EMouseCursor::Type InCursor)
{
    switch (InCursor)
    {
        case EMouseCursor::None:    return "None";
        case EMouseCursor::Default: return "Default";
        default:                    return "Unknown";
    }
}

} /* ~Namespace Jafg */
