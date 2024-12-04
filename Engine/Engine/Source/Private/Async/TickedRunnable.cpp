// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Async/TickedRunnable.h"
#include "Core/Application.h"

Jafg::ETaskExit::Type Jafg::LTickedRunnable::Run()
{
    double LastTickTime = Application::GetDeltaSinceStaticStorageInitialization();

    while (this->bShouldTick)
    {
        const double Now = Application::GetDeltaSinceStaticStorageInitialization();
        const double DeltaTime = Now - LastTickTime;

        if (DeltaTime > this->TickInterval)
        {
            this->FixedTick(static_cast<float>(DeltaTime));
            LastTickTime = Now;
        }
#if !PLATFORM_WASM
        else
        {
            const double TimeRemaining = this->TickInterval - DeltaTime;
            if (TimeRemaining > 0.001)
            {
                /* Spare cpu time for other tasks. */
                PlatformHal::Sleep(TimeRemaining * 0.997);
            }
        }
#endif /* !PLATFORM_WASM */

        continue;
    }

    return ETaskExit::Success;
}

void Jafg::LTickedRunnable::Stop(const ERunnableStopReason::Type InType)
{
    LOG_TRACE(LogTasks, "Stopping ticked runnable.")

    if (this->bShouldTick == false)
    {
        LOG_WARNING(LogTasks, "Ticked runnable already stopped.")
        return;
    }

    LRunnable::Stop(InType);
    this->bShouldTick = false;
    this->StopReason  = InType;

    return;
}
