// Copyright mzoesch. All rights reserved.

#include "Async/TickedRunnable.h"
#include "Core/Application.h"
#include "Stats/Stats.h"

Jafg::ETaskExit::Type Jafg::LTickedRunnable::Run()
{
    STAT_CYCLE_FUNCTION()

    f64 LastTickTime { Application::GetDeltaSinceStaticStorageInitialization() };

    while (this->bShouldTick)
    {
        const f64 Now { Application::GetDeltaSinceStaticStorageInitialization() };
        const f64 DeltaTime { Now - LastTickTime };

        if (DeltaTime > this->TickInterval)
        {
            this->FixedTick(static_cast<f32>(DeltaTime));
            LastTickTime = Now;
        }
#if !PLATFORM_WASM
        else
        {
            if (const f64 TimeRemaining { this->TickInterval - DeltaTime }; TimeRemaining > 0.001)
            {
                /* Spare cpu time for other tasks. */
                Lal::Hal::SleepNoStats(TimeRemaining * 0.997);
            }
        }
#endif /* !PLATFORM_WASM */

        continue;
    }

    return ETaskExit::Success;
}

void Jafg::LTickedRunnable::OnStop(const ERunnableStopReason::Type InType)
{
    STAT_CYCLE_FUNCTION()

    LOG_TRACE(LogTasks, "Stopping ticked runnable.")

    if (this->bShouldTick == false)
    {
        LOG_WARNING(LogTasks, "Ticked runnable already stopped.")
        return;
    }

    LRunnable::OnStop(InType);
    this->bShouldTick = false;
    this->StopReason  = InType;

    return;
}
