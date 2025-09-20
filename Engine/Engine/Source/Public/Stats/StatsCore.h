// Copyright mzoesch. All rights reserved.

#pragma once

#include "Stats/StatsForward.h"

#if !WITH_STATS
    #error "This file should only be included if the application compiles with stats."
#endif /* WITH_STATS */

#include "Core/Application.h"
#include "Async/TaskUtility.h"

namespace Jafg::Stats::Private
{

ENGINE_API extern LTracer* GTracer;

struct LStat final
{
    FORCEINLINE LStat(const std::string_view& InName) noexcept;
    FORCEINLINE ~LStat() { if (this->bStopped == false) { this->Stop(); } }

    FORCEINLINE void Stop();
    FORCEINLINE void Discard() { this->bStopped = true; return; }

private:

    std::string_view Name;
    bool bStopped = false;
    Application::LHrcTimePoint BeginTime;
};

FORCEINLINE LStat::LStat(const std::string_view& InName) noexcept
{
    if (GTracer)
    {
        this->Name = InName;
        this->BeginTime = Application::GetHighestNow();
    }
    else
    {
        this->bStopped = true;
    }

    return;
}

FORCEINLINE void LStat::Stop()
{
    if (this->bStopped)
    {
        return;
    }

    GTracer->AddEvent(
    {
        this->Name,
        static_cast<i64>(Application::GetTimeDifferenceFromStaticStorageInitialization(this->BeginTime) * LAL_S2MUS_D),
        static_cast<i64>(Application::GetDeltaSinceStaticStorageInitialization() * LAL_S2MUS_D),
        Tasks::GetCurrentThreadId()
    });

    this->bStopped = true;

    return;
}

} /* ~Namespace Jafg::Stats::Private */
