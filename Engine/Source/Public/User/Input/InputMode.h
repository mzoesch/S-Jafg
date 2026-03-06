// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

enum : u8
{
    HideMouseCursor = 0,
    ShowMouseCursor = 1,
};

//#
//# If #UserInterface and #InputSubsystem they will be parsed in this sequence:
//#     UserInterface, InputSubsystem.
//# If a specific input is consumed by the user interface, it will not be passed to the input subsystem.
//#
enum struct EInputModeBits
{
    None            = 0x00,

    //# Only user interface input is checked.
    UserInterface   = 0x01 << 0,

    //# Only the input handled by the input subsystem is checked.
    InputSubsystem  = 0x01 << 1,

    //# Whether to show the mouse cursor.
    ShowMouseCursor = 0x01 << 2,
};
ENUM_STRUCT_FLAGS(EInputModeBits, EInputMode)
inline LString LexToString(EInputMode InFlags)
{
    std::stringstream SS;
    if (InFlags & EInputModeBits::UserInterface)    { SS << "UserInterface|"; }
    if (InFlags & EInputModeBits::InputSubsystem)   { SS << "InputSubSystem|"; }
    if (InFlags & EInputModeBits::ShowMouseCursor)  { SS << "ShowMouseCursor|"; }
    return SS.str();
}

} /* ~Namespace Jafg */
