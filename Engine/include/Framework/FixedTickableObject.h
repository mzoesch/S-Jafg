// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//#
//# An object that can be registered to a system that ticks in a fixed interval.
//#
class LFixedTickableObject
{
public:

    LFixedTickableObject() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LFixedTickableObject)
    virtual ~LFixedTickableObject() = default;

    //#
    //# @param Dt        The time step for a tick.
    //# @param bFollowup Whether the FixedTick was previously already called this frame.
    //#
    virtual void FixedTick(f32 Dt, bool bFollowup) = 0;

    FORCEINLINE virtual bool ShouldFixedTickableObjectTick() const { return true; }
};

} /* ~Namespace Jafg */
