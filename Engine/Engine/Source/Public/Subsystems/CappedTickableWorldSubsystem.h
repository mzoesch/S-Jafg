// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/TickableWorldSubsystem.h"
#include "CappedTickableWorldSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(EClassFlags::Abstract,)
class ENGINE_API JCappedTickableWorldSubsystem : public JTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JCappedTickableWorldSubsystem)

    // JTickableWorldSubsystem implementation
    virtual void Tick(const float DeltaTime) override final;
    // JTickableWorldSubsystem implementation

    /**
     * Override this method to implement the capped tick logic.
     *
     * @param EngineDeltaTime    The delta time of the engine.
     * @param SubsystemDeltaTime The delta time since the lass call of #CappedTick.
     */
    virtual void FixedTick(const float EngineDeltaTime, const float SubsystemDeltaTime) { }

    FORCEINLINE auto GetTickInterval() const -> float { return this->TickInterval; }
    FORCEINLINE auto SetTickInterval(const float Interval) -> void { this->TickInterval = Interval; }

private:

    /**
     * The tick interval in seconds. Zero means no interval -> tick every frame.
     */
    float TickInterval = 0.0f;

    /**
     * The last time the subsystem was ticked.
     */
    float LastTickTime = 0.0f;
};

} /* ~Namespace Jafg */
