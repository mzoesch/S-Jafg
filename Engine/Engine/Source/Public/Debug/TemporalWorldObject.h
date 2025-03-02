// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

#if !AS_CLIENT
    #error "This file is for local client only."
#endif /* !AS_CLIENT */

namespace Jafg
{

class LWorld;
class LViewport;
class LEye;
class LTemporalWorldObject;


//#
//# Skip the AActor creation hassle and just use this temporal object inside a world.
//# Local client only, no replication, dynamic creation, etc.
//# This object has to be trivially movable.
//# If you need access to the tick event, inherit from LTickableObject - but this goes against
//# the temporal nature of this object and usually should be avoided due to bad design.
//#
class LTemporalWorldObject
{
public:

    enum OneDrawCall { DrawOnce };

    LTemporalWorldObject() = delete;
    LTemporalWorldObject(const float InTimeToLive) : TimeToLive(InTimeToLive) { check( this->TimeToLive > 0.0f ) }
    /** Will only be drawn once. */
    LTemporalWorldObject(const OneDrawCall InTimeToLive) { check( this->TimeToLive == 0.f ) }
    PROHIBIT_COPY(LTemporalWorldObject)
    DEFAULT_MOVE(LTemporalWorldObject)
    virtual ~LTemporalWorldObject() = default;

    FORCEINLINE void ReduceLifeTime(const float DeltaTime) { this->TimeToLive -= DeltaTime; }
    FORCEINLINE bool IsAlive() const { return this->TimeToLive > 0.0f; }

    virtual void Draw(const LWorld& InContext, const LViewport& InViewport, const LEye& InEye) const = 0;

private:

    float TimeToLive = 0.0f;
};

} /* ~Namespace Jafg */
