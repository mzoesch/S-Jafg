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

    LString Identifier;
};

} /* Namespace Jafg */
