// Copyright mzoesch. All rights reserved.

#include "Framework/ThreadedWorldSubsystem.h"
#include "Engine/World.h"
#include "Stats/Stats.h"

void Jafg::JThreadedWorldSubsystem::Initialize(LSubsystemCollection& Collection)
{
    STAT_CYCLE_FUNCTION()

    Super::Initialize(Collection);
    this->SetPriorityTearDown(true);
    this->OnInitialize(Collection);

    this->ThisSubsystemsThreadName = Tasks::Private::MakeNewCustomNamedThreadId();
    const ETaskExit::Type ErrorLevel = Tasks::Private::LaunchNamedThread(this->ThisSubsystemsThreadName, this, false);
    if (ErrorLevel != ETaskExit::Success)
    {
        LOG_ERROR(
            LogTaskSystem,
            "Failed to launch the thread for subsystem {}[{}].",
            this->GetHumanReadableName(), LexToString(this->ThisSubsystemsThreadName)
        )
    }

    return;
}

void Jafg::JThreadedWorldSubsystem::TearDown()
{
    STAT_CYCLE_FUNCTION()

    Super::TearDown();

    if (this->ShouldTickRunnable())
    {
        this->Stop(ERunnableStopReason::RequestedStop);
        SCOPED_TIME_TAKEN_MEASURER(LogTasks, Verbose, "Joining thread")
        Tasks::JoinThread(this->ThisSubsystemsThreadName);
    }
    else
    {
        LOG_WARNING(LogTasks, "Teared down a threaded subsystem that is not ticking.")
    }

    this->OnTearDown();

    return;
}
