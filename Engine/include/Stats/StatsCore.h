// Copyright mzoesch. All rights reserved.

#pragma once

#include "Stats/StatsForward.h"

#if !WITH_STATS
    #error "This file should only be included if the application compiles with stats."
#endif /* WITH_STATS */

#include "Core/App.h"
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
    algo::clock::time_point BeginTime;
};

FORCEINLINE LStat::LStat(const std::string_view& InName) noexcept
{
    if (GTracer)
    {
        this->Name = InName;
        this->BeginTime = algo::now();
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

    GTracer->AddEvent({
        this->Name,
        static_cast<i64>(algo::time_diff(App::GetStaticStorageInitializationTime(), this->BeginTime) * maths::s2mus_d),
        static_cast<i64>(App::GetElapsedTime() * maths::s2mus_d),
        Tasks::GetCurrentThreadId()
        });
    this->bStopped = true;

    return;
}

} /* ~Namespace Jafg::Stats::Private */
