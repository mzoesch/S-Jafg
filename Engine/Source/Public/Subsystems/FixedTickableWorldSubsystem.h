// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/TickableWorldSubsystem.h"
#include "FixedTickableWorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class JFixedTickableWorldSubsystem : public JTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JFixedTickableWorldSubsystem)

    // JTickableWorldSubsystem implementation
    virtual void Tick(const f32 DeltaTime) override final;
    // JTickableWorldSubsystem implementation

    //#
    //# Override this method to implement the fixed tick logic.
    //#
    //# @param EngineDeltaTime    The delta time of the engine.
    //# @param SubsystemDeltaTime The delta time since the lass call of #CappedTick.
    //#
    virtual void FixedTick(const f32 EngineDeltaTime, const f32 FixedDeltaTime) { }

    FORCEINLINE f32  GetTickInterval() const { return this->TickInterval; }
    FORCEINLINE void SetTickInterval(const f32 Interval) { this->TickInterval = Interval; }

private:

    //#
    //# The tick interval in seconds. Zero means no interval -> tick every frame.
    //#
    f32 TickInterval{ 0.0f };

    //#
    //# The last time the subsystem was ticked.
    //#
    f32 LastTickTime{ 0.0f };
};

} /* ~Namespace Jafg */
