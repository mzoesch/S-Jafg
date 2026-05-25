// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

enum struct EInputModeBits
{
    Identity = 0x00,

    //# Whether to show the mouse cursor.
    ShowMouseCursor = 0x01 << 1,
    //# Whether to show the mouse cursor.
    HideMouseCursor = Identity,
};
ENUM_STRUCT_FLAGS(EInputModeBits, EInputMode)
inline LString LexToString(EInputMode InFlags)
{
    std::stringstream SS;
    if (InFlags & EInputModeBits::ShowMouseCursor) { SS << "ShowMouseCursor"; }
    return SS.str();
}

} /* ~Namespace Jafg */
