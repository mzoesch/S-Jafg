// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/TickableWorldSubsystem.h"
#include "FixedTickableWorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(EJxxClassBits::Abstract)
class JFixedTickableWorldSubsystem : public JTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JFixedTickableWorldSubsystem)

private:

    // JTickableWorldSubsystem implementation
    virtual void Tick(f32 Dt) override final
    {
        Super::Tick(Dt);
        this->LastTickTime += Dt;
        if (this->LastTickTime > this->TickInterval)
        {
            this->FixedTick(Dt, this->LastTickTime);
            this->LastTickTime = 0.0f;
        }
        return;
    }
    // JTickableWorldSubsystem implementation

public:

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

    //# The tick interval in seconds. Zero means no interval -> tick every frame.
    f32 TickInterval{};

    //# The last time the subsystem was ticked.
    f32 LastTickTime{};
};

} /* ~Namespace Jafg */
