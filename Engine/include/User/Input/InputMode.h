// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

enum struct EInputModeBits : u8
{
    Identity = 0x00,

    //# Whether to show the mouse cursor.
    ShowMouseCursor = 0x01 << 1,
    //# Whether to show the mouse cursor.
    HideMouseCursor = Identity,
    //#
    //# Keeps a shown mouse cursor inside the surface. Only has an effect together with #ShowMouseCursor. Hidden
    //# cursors are always captured.
    //#
    ConfineMouseCursor = 0x01 << 2,
};
ENUM_STRUCT_FLAGS(EInputModeBits, EInputMode)
inline LString LexToString(EInputMode InFlags)
{
    std::stringstream SS;
    if (InFlags & EInputModeBits::ShowMouseCursor) { SS << "ShowMouseCursor"; }
    if (InFlags & EInputModeBits::ConfineMouseCursor) { SS << (SS.tellp() > 0 ? "|" : "") << "ConfineMouseCursor"; }
    return SS.str();
}

} /* ~Namespace Jafg */
