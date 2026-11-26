// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "PatrolComponent.generated.h"

namespace Jgc
{

//#
//# A minimal bot: Moves its character (#Jafg::ACharacterMovementComponent) from waypoint to waypoint, waits at each one
//# and starts over after the last one. Through the same intent API that humans use. For tests and showcases.
//#
DECLARE_JAFG_CLASS()
class JGC_API APatrolComponent : public Jafg::AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(APatrolComponent)
    {
        this->bTick = true;
    }

public:

    virtual void ParentTick(f32 Dt) override;

    //# In world space. Only the directions along the ground count.
    TArray<LWorldVec3> Waypoints;

    //# How close a waypoint has to be reached. In meters.
    CLASS_FIELD(EditorEditable)
    f32 AcceptanceRadius{ 0.75f };
    //# How long to wait at every waypoint. In seconds.
    CLASS_FIELD(EditorEditable)
    f32 WaitTime{ 1.5f };
    //# Scales the speed (the length of the move direction). In [0, 1].
    CLASS_FIELD(EditorEditable)
    f32 SpeedScale{ 1.0f };
    //# Whether to sprint on every second way, so that all speeds are seen.
    CLASS_FIELD(EditorEditable)
    bool bSprintEverySecondWay{ true };

private:

    std::size_t Target{};
    f32 Waited{};
};

} /* ~Namespace Jgc */
