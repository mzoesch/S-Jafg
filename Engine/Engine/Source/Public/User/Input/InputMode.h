// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

enum : u8
{
    HideMouseCursor = 0,
    ShowMouseCursor = 1,
};

namespace EInputMode
{

enum Type : u8
{
    None            = 0x00,

    //#
    //# Only user interface input is checked.
    //#
    UserInterface   = 0x01 << 0,

    //#
    //# Only the input handled by the input subsystem is checked.
    //#
    InputSubSystem  = 0x01 << 1,

    //#
    //# Both are checked.
    //# In this sequence: UserInterface, InputSubSystem.
    //# If a specific input is consumed by the user interface, it will not be passed to the input subsystem.
    //#
    Both            = UserInterface | InputSubSystem,
};

} /* ~Namespace EInputMode */

inline LString LexToString(const EInputMode::Type InType)
{
    switch (InType)
    {
        case EInputMode::None:              { return "None"; }
        case EInputMode::UserInterface:     { return "UserInterface"; }
        case EInputMode::InputSubSystem:    { return "InputSubSystem"; }
        case EInputMode::Both:              { return "Both"; }
    }

    checkNoEntry()

    return "";
}

} /* ~Namespace Jafg */
