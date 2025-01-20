// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

namespace EInputActionTrigger
{

enum Type : uint8
{
    None      = 0x0 << 0,

    //#
    //# Marks an action that was just triggered.
    //#
    Triggered = 0x1 << 0,

    //#
    //# Marks an action that is ongoing.
    //# Will be called the same tick where the action was triggered.
    //#
    Ongoing   = 0x1 << 1,

    //#
    //# Marks an action that was just completed.
    //#
    Completed = 0x1 << 2,
};

} /* ~Namespace EUserInputActionTrigger */

ENUM_CLASS_FLAGS(EInputActionTrigger::Type)

} /* ~Namespace Jafg */
