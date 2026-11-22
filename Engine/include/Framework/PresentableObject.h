// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//#
//# An object that presents simulated state, e.g., interpolates it between the last two physics ticks so that it moves
//# smoothly although the physics only ticks at a fixed rate. Called by its world once per tick, after the physics
//# ticked and before the views of the controllers are updated (so that they follow what is presented).
//#
//# @see #LWorld::RegisterPresentableObject
//#
class LPresentableObject
{
public:

    LPresentableObject() = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LPresentableObject)
    virtual ~LPresentableObject() = default;

    //#
    //# @param PhysicsAlpha How far the time of the world is between the last physics tick (0) and the next one (1).
    //#                     One if the physics of the world do not tick.
    //#
    virtual void Present(f32 PhysicsAlpha) = 0;
};

} /* ~Namespace Jafg */
