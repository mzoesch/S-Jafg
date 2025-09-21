// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

namespace EInputActionCategory
{

enum Type : u8
{
    None,
    Boolean,
    Axis1D,
    Axis2D,
    Axis3D,
};

} /* ~Namespace EInputActionCategory */
ENGINE_API LString LexToString(const EInputActionCategory::Type InType);

} /* ~Namespace Jafg */
