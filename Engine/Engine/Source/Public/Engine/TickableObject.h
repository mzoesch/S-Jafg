// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

//#
//# An object that can be registered with some sort of context that is ticked by the engine or a sub-tickable system
//# from the engine-tick.
//#
class ENGINE_API LTickableObject
{
public:

    LTickableObject() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LTickableObject)
    virtual ~LTickableObject() = default;

    virtual void Tick(const f32 DeltaTime) = 0;

    FORCEINLINE virtual bool ShouldTickableObjectTick() const { return true; }
};

} /* ~Namespace Jafg */
