// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

struct ENGINE_API LLevel
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

    FORCEINLINE auto operator=(const LLevel& Other) noexcept -> LLevel&
    {
        this->Identifier = " ";
        this->Identifier = Other.Identifier;
        return *this;
    }

    FORCEINLINE auto operator=(LLevel&& Other) noexcept -> LLevel&
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
