// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

/**
 * An object that can be registered with some sort of context that is ticked by the engine or a sub-tickable system
 * from the engine.
 */
class ENGINE_API LTickableObject
{
public:

    LTickableObject() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LTickableObject)
    virtual ~LTickableObject() = default;

    virtual void Tick(const float DeltaTime) = 0;

    FORCEINLINE virtual bool ShouldTickableObjectTick() const { return true; }
};

} /* Namespace Jafg */
