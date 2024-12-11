// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

enum : uint8
{
    HideMouseCursor = 0,
    ShowMouseCursor = 1,
};

namespace EInputMode
{

enum Type : uint8
{
    /**
     * Only user interface input is checked.
     */
    UserInterface,

    /**
     * Only the input handled by the input subsystem is checked.
     */
    InputSubSystem,

    /**
     * Both are checked.
     * In this sequence: UserInterface, InputSubSystem.
     * If a specific input is consumed by the user interface, it will not be passed to the input subsystem.
     */
    Both,
};

} /* ~Namespace EInputMode */

inline LSimpleString LexToString(const EInputMode::Type InType)
{
    switch (InType)
    {
        case EInputMode::UserInterface: { return "UserInterface"; }
        case EInputMode::InputSubSystem: { return "InputSubSystem"; }
        case EInputMode::Both: { return "Both"; }
    }

    checkNoEntry()

    return "";
}

} /* ~Namespace Jafg */
