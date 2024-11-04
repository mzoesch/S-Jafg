// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

class ENGINE_API LTickableObject
{
public:

    LTickableObject() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LTickableObject)
    virtual ~LTickableObject() = default;

    virtual void Tick(const float DeltaTime) = 0;
};

} /* Namespace Jafg */
