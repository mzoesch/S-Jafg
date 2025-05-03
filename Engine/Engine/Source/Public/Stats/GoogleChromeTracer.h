// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

#if !(WITH_STATS && JAFG_STATS_USE_GOOGLE_CHROME_TRACER)
    #error "This file should only be included if the application compiles with stats and the Google Chrome Tracer is enabled."
#endif /* !(WITH_STATS && JAFG_STATS_USE_GOOGLE_CHROME_TRACER) */

#include "Stats/StatsForward.h"
#include "System/Finder.h"

namespace Jafg::Stats::Vendor
{

struct LGoogleChromeTracer final
{

    LGoogleChromeTracer() noexcept = default;
    ENGINE_API ~LGoogleChromeTracer();

    ENGINE_API void BeginSession(LString&& InName);
    ENGINE_API void AddNamedThread(Private::LThread&& InThread);
    ENGINE_API void AddBookmark(Private::LBookmark&& InBookmark);
    ENGINE_API void AddEvent(Private::LSessionEvent&& InEvent);
    ENGINE_API void TryEndSession();
    ENGINE_API void EndSession();

private:

    LString GetPath() const;

    Private::LSession Session;
    std::ofstream     Stream;
    bool bFirstEvent = true;
    std::mutex Mutex;
};

} /* ~Namespace Jafg::Stats::Vendor */
