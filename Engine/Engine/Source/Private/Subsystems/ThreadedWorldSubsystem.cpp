// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Subsystems/ThreadedWorldSubsystem.h"

void Jafg::JThreadedWorldSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
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
    Super::TearDown();

    if (this->ShouldTickRunnable())
    {
        LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
        this->Stop(ERunnableStopReason::RequestedStop);
        LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
        Tasks::Private::JoinThread(this->ThisSubsystemsThreadName);
        LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
    }
    else
    {
        LOG_WARNING(LogTasks, "Teared down a threaded subsystem that is not ticking.")
        LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
    }

    this->OnTearDown();

    return;
}
