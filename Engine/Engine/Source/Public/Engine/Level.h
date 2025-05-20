// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

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
    explicit LLevel(LString InIdentifier) : Identifier(std::move(InIdentifier))
    {
    }

    FORCEINLINE LLevel(const LLevel& Other)
    {
        this->Identifier = Other.Identifier;
    }

    FORCEINLINE LLevel(LLevel&& Other) noexcept
    {
        this->Identifier = std::move(Other.Identifier);
    }

    FORCEINLINE LLevel& operator=(const LLevel& Other) noexcept
    {
        this->Identifier = Other.Identifier;
        return *this;
    }

    FORCEINLINE LLevel& operator=(LLevel&& Other) noexcept
    {
        this->Identifier = std::move(Other.Identifier);
        return *this;
    }

    LString Identifier;

    FORCEINLINE auto operator==(const LLevel& Other) const -> bool  { return Identifier == Other.Identifier; }
    FORCEINLINE auto operator!=(const LLevel& Other) const -> bool  { return !(*this == Other);              }
    FORCEINLINE auto operator==(const LString& Other) const -> bool { return Identifier == Other;            }
    FORCEINLINE auto operator!=(const LString& Other) const -> bool { return !(*this == Other);              }
};

} /* Namespace Jafg */
