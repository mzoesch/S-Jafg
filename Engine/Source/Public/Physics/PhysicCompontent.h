// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Physics/TraceUtility.h"

namespace Jafg
{

class [[deprecated]] LPhysicsComponent
{
public:

    virtual ~LPhysicsComponent() noexcept = default;

    //#
    //# Marches the point to the physical volume of the target object.
    //# @return True if the point is inside the physical volume of the target object, false otherwise.
    //#
    virtual bool Overlaps(LWorldVec3 const& Point) const = 0;

    //#
    //# Checks if the sphere with the given radius overlaps the physical volume of the target object.
    //# @return True if the sphere overlaps the physical volume of the target object, false otherwise.
    //#
    virtual bool Overlaps(LWorldVec3 const& Point, LWorldReal Radius) const = 0;

    //#
    //# Checks if the physical volume of the target object overlaps with the physical volume of the other object.
    //# @return True if the physical volumes overlap, false otherwise.
    //#
    virtual bool Overlaps(LPhysicsComponent const& Other) const = 0;

    //#
    //# Sweeps along the ray from #Start to #End and returns the first hit result.
    //# @return True if a blocking surface was hit, false otherwise.
    //#
    virtual bool Sweep(LWorldVec3 const& Start, LWorldVec3 const& End, LHitResult& OutHit) const = 0;
};

} /* ~Namespace Jafg */
