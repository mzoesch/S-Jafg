// Copyright mzoesch. All rights reserved.

#include "Engine/EngineRunnable.h"
#include "Stats/Stats.h"

Jafg::ETaskExit::Type Jafg::LEngineRunnable::Run()
{
    STAT_CYCLE_FUNCTION()

    while (!this->IsStopped())
    {
        if (Tasks::TryRunTasks(ENamedThreads::WorkerThread, ETaskTime::Whenever, 1) == 0)
        {
            App::SleepNoStats(0.2);
        }

        continue;
    }

    return ETaskExit::Success;
}
