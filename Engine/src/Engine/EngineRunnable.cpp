// Copyright mzoesch. All rights reserved.

#include "EngineRunnable.h"
#include "Core/App.h"
#include "Core/TaskUtility.h"

Tasks::EExit::Type Jafg::LEngineRunnable::Run(std::stop_token const& Stoken)
{
    STAT_CUSTOM_ZONE_FMT("Run::{}", this->GetHumanReadableName())

    check(!Tasks::IsOnWorkerThread())

    Tasks::Detail::SetIsWorkerThread(true);
    Tasks::Worker.ConsumeAndWaitUntil(Stoken);
    Tasks::Detail::SetIsWorkerThread(false);

    return Tasks::EExit::Success;
}
