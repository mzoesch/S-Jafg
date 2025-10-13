// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

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
    PROHIBIT_ANY_REALLOC_OTHER_THAN_CDR_DEFAULTED(LTickableObject)
    virtual ~LTickableObject() = default;

    virtual void Tick(const f32 DeltaTime) = 0;

    FORCEINLINE virtual bool ShouldTickableObjectTick() const { return true; }
};

} /* ~Namespace Jafg */
