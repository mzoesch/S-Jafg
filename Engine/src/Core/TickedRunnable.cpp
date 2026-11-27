// Copyright mzoesch. All rights reserved.

#include "Framework/TickedRunnable.h"
#include "Core/App.h"

Tasks::EExit::Type Jafg::LTickedRunnable::Run(std::stop_token const& Stoken)
{
    STAT_FUNCTION()

    f64 LastTickTime{App::GetElapsedTime()};
    while (!Stoken.stop_requested())
    {
        const f64 Now{App::GetElapsedTime()};
        const f64 DeltaTime{Now - LastTickTime};

        if (DeltaTime > this->TickInterval)
        {
            this->FixedTick(static_cast<f32>(DeltaTime));
            LastTickTime = Now;
        }
#if !JAFG_PLATFORM_WASM
        else
        {
            if (f64 TimeRemaining{this->TickInterval - DeltaTime}; TimeRemaining > 0.001)
            {
                /* Spare cpu time for other tasks. */
                App::SleepNoStats(TimeRemaining * 0.997);
            }
        }
#endif /* !JAFG_PLATFORM_WASM */
    }

    return Tasks::EExit::Success;
}
