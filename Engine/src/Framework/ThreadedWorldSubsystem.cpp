// Copyright mzoesch. All rights reserved.

#include "Framework/ThreadedWorldSubsystem.h"
#include "Engine/World.h"

void Jafg::JThreadedWorldSubsystem::Initialize(LSubsystemCollection& Collection)
{
    STAT_FUNCTION()

    Super::Initialize(Collection);

    if (auto Rc{Tasks::LaunchRunnable(*this)}; Rc != Tasks::EExit::Success)
    {
        LOG_FATAL(LogTaskSystem
            , "[{}]: Failed to associated thread."
            , this->GetHumanReadableName()
            )
    }
}

void Jafg::JThreadedWorldSubsystem::TearDown()
{
    Super::TearDown();

    STAT_CUSTOM_ZONE_FMT("TearDown::{}", this->GetHumanReadableName())

    this->Stop(ERunnableStopReason::RequestedStop);

    {
        LOG_SCOPED_DURATION(LogTasks, Verbose, "Joining associated thread")
        this->Join();
    }
}
