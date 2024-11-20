// Copyright mzoesch. All rights reserved.

#pragma once

#include "Async/Runnable.h"

namespace Jafg
{

/**
 * A runnable that has a fixed tick interval and defines common boilerplate logic for ticking and stopping.
 */
class ENGINE_API LTickedRunnable : public LRunnable
{
public:

    virtual auto Initialize() -> ETaskExit::Type override { return LRunnable::Initialize(); }
    virtual auto Run() -> ETaskExit::Type override;
    virtual auto Stop(const ERunnableStopReason::Type InType) -> void override final;
    virtual auto Exit() -> void override { LRunnable::Exit(); }

    FORCEINLINE auto GetTickInterval() const -> double { return this->TickInterval; }
    FORCEINLINE auto SetTickInterval(const double Interval) -> void { this->TickInterval = Interval; }

protected:

    /**
     * Called on the tick thread to perform the tick logic.
     * This tick is detached in any way from the tick of the engine. If you want to asynchronously tick with the
     * engine, use the #LEngineTickedRunnable class instead.
     *
     * @param RunnableDeltaTime The delta time since the last call of #FixedTick. NOT the delta time of the current
     *                          frame from the engine.
     */
    virtual void FixedTick(const float RunnableDeltaTime) = 0;

private:

    /**
     * The tick interval in seconds. Zero means no interval -> tick always (not recommended).
     */
    double TickInterval = 0.1f;

    bool bShouldTick = true;
    ERunnableStopReason::Type StopReason = ERunnableStopReason::Custom;
};

} /* ~Namespace Jafg */
