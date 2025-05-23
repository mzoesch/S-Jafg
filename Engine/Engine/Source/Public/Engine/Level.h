// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/InputMode.h"

namespace Jafg
{


//#
//# Represents a level in the engine.
//# A level is a blueprint for a world.
//#
//# You can create your own levels by registering them with the engine.
//#
//# The core levels that are part of the engine's core are:
//#   - LFrontEnd (Client only. This is the front-end of the client window)
//#   - LWorld (The world.)
//#
struct LLevel final
{
    LLevel() = default;

    explicit LLevel
    (
        const LString& InIdentifier,
        const EInputMode::Type InInputMode = EInputMode::None,
        const bool bShowMouseCursor = true,
        const LLinearColor& BackgroundColor = LLinearColor::Gray
    )
        : Identifier(InIdentifier)
        , InputMode(InInputMode)
        , bShowMouseCursor(bShowMouseCursor)
        , BackgroundColor(BackgroundColor)
    {
        return;
    }

    DEFAULT_REALLOC_OF_ANY_FORM(LLevel)

    FORCEINLINE bool operator==(const LLevel&  Other) const noexcept { return this->Identifier == Other.Identifier; }
    FORCEINLINE bool operator!=(const LLevel&  Other) const noexcept { return !(*this == Other); }
    FORCEINLINE bool operator==(const LString& Other) const noexcept { return this->Identifier == Other; }
    FORCEINLINE bool operator!=(const LString& Other) const noexcept { return !(*this == Other); }

    LString Identifier;

    EInputMode::Type InputMode;
    bool bShowMouseCursor;

    LLinearColor BackgroundColor;
};

} /* Namespace Jafg */
