// Copyright mzoesch. All rights reserved.

#include "EngineRunnable.h"
#include "Core/App.h"
#include "Core/TaskUtility.h"

Jafg::ETaskExit::Type Jafg::LEngineRunnable::Run()
{
    STAT_FUNCTION()

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
