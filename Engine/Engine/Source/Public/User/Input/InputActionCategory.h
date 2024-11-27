// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

namespace EInputActionCategory
{

enum Type : uint8
{
    None    = 0x0 << 0,
    Boolean = 0x1 << 0,
    Axis1D  = 0x1 << 1,
    Axis2D  = 0x1 << 2,
    Axis3D  = 0x1 << 3
};

} /* ~Namespace EInputActionCategory */

ENUM_CLASS_FLAGS(EInputActionCategory::Type)

} /* ~Namespace Jafg */
