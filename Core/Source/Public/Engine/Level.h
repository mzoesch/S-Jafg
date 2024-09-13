// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

struct LLevel
{
    LLevel() = default;
    explicit LLevel(String InIdentifier) : Identifier(std::move(InIdentifier))
    {
    }

    String Identifier;
};
