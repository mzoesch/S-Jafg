// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Runnable.h"

namespace Jafg
{

//#
//# A runnable that has a fixed tick interval and defines common boilerplate logic for ticking and stopping.
//#
class LTickedRunnable : public LRunnable
{
public:

    virtual auto Initialize() -> ETaskExit::Type override { return LRunnable::Initialize(); }
    virtual auto Run() -> ETaskExit::Type override final;
    virtual void OnStop(const ERunnableStopReason::Type InType) override final;
    virtual auto Exit() -> void override { LRunnable::Exit(); }

    FORCEINLINE f64  GetTickInterval() const { return this->TickInterval; }
    FORCEINLINE void SetTickInterval(const f64 Interval) { this->TickInterval = Interval; }

protected:

    //#
    //# Called on the tick thread to perform the tick logic.
    //# This tick is detached in any way from the tick of the engine. If you want to asynchronously tick with the
    //# engine, use the #LEngineTickedRunnable class instead.
    //#
    //# @param RunnableDeltaTime The delta time since the last call of #FixedTick. NOT the delta time of the current
    //#                          frame from the engine.
    //#
    virtual void FixedTick(const float RunnableDeltaTime) = 0;

    FORCEINLINE bool ShouldTickRunnable() const { return this->bShouldTick; }

private:

    //#
    //# The tick interval in seconds. Zero means no interval -> tick always (not recommended).
    //#
    f64 TickInterval{ 0.1 };

    bool bShouldTick{ true };
    ERunnableStopReason::Type StopReason{ ERunnableStopReason::Custom };
};

} /* ~Namespace Jafg */
