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
    Triggered = 0x1 << 0,
    Ongoing   = 0x1 << 1,
    Completed = 0x1 << 2,
};

} /* ~Namespace EUserInputActionTrigger */

ENUM_CLASS_FLAGS(EInputActionTrigger::Type)

} /* ~Namespace Jafg */
