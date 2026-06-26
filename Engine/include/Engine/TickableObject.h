// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//#
//# An object that can be registered with some sort of context that is ticked by the engine or a sub-tickable system
//# from the engine-tick.
//#
class LTickableObject
{
public:

    LTickableObject() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LTickableObject)
    virtual ~LTickableObject() = default;

    virtual void Tick(f32 Dt) = 0;

    FORCEINLINE virtual bool ShouldTickableObjectTick() const { return true; }
};

} /* ~Namespace Jafg */
