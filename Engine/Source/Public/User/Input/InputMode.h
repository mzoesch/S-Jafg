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

    ShowMouseCursor = 0x01 << 2,

    //#
    //# Both are checked.
    //# In this sequence: UserInterface, InputSubSystem.
    //# If a specific input is consumed by the user interface, it will not be passed to the input subsystem.
    //#
    Both            = UserInterface | InputSubSystem,
};

} /* ~Namespace EInputMode */
ENUM_CLASS_FLAGS(EInputMode::Type)

inline LString LexToString(const EInputMode::Type InType)
{
    switch (InType)
    {
        case EInputMode::None:              { return "None"; }
        case EInputMode::UserInterface:     { return "UserInterface"; }
        case EInputMode::InputSubSystem:    { return "InputSubSystem"; }
        case EInputMode::ShowMouseCursor:   { return "ShowMouseCursor"; }
        case EInputMode::Both:              { return "Both"; }
        default:
        {
            std::stringstream SS;
            if (InType & EInputMode::UserInterface)    { SS << "UserInterface"; }
            if (InType & EInputMode::InputSubSystem)   { if (!SS.str().empty()){SS<<"|";} SS << "InputSubSystem"; }
            if (InType & EInputMode::ShowMouseCursor)  { if (!SS.str().empty()){SS<<"|";} SS << "ShowMouseCursor"; }
            return "";
        }
    }

    checkNoEntry()

    return "";
}

} /* ~Namespace Jafg */
