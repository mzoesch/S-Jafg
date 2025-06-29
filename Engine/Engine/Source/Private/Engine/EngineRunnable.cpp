// Copyright mzoesch. All rights reserved.

#include "Engine/EngineRunnable.h"
#include "Stats/Stats.h"

//
// Moved out of a header file into this translation file, so that this translation unit can be the unique
// owner of the v-table pointer from the #LEngineRunnable class.
//
Jafg::LEngineRunnable::LEngineRunnable()
{
    return;
}

Jafg::LEngineRunnable::LEngineRunnable(const LString& InHumanReadableName)
    : LRunnable(InHumanReadableName)
{
    return;
}

Jafg::ETaskExit::Type Jafg::LEngineRunnable::Run()
{
    STAT_CYCLE_FUNCTION()

    while (this->IsStopped() == false)
    {
        if (Tasks::TryRunTasks(ENamedThreads::WorkerThread, ETaskTime::Whenever, 1) == 0)
        {
            Lal::Hal::SleepNoStats(0.2);
        }

        continue;
    }

    return ETaskExit::Success;
}
