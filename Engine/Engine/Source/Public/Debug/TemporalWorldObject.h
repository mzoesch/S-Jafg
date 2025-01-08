// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

#if !AS_CLIENT
    #error "This file is for local client only."
#endif /* !AS_CLIENT */

namespace Jafg
{

class LWorld;

/**
 * Skip the AActor creation hassle and just use this temporal object inside a world.
 * Local client only.
 * This object has to be trivially movable.
 */
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

    virtual void Draw(const LWorld& InContext) const = 0;

private:

    float TimeToLive = 0.0f;
};

} /* ~Namespace Jafg */
