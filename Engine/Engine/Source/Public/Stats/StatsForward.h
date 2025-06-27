// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

//#
//# If not defined, always do stats except in shipping builds.
//#
#ifndef WITH_STATS
    #define WITH_STATS                                                  (!IN_SHIPPING)
#endif /* !WITH_STATS */

//#
//# Default to google chrome tracer export style.
//# https://www.chromium.org/developers/how-tos/trace-event-profiling-tool/
//#
#ifndef JAFG_STATS_USE_GOOGLE_CHROME_TRACER
    #define JAFG_STATS_USE_GOOGLE_CHROME_TRACER                          1
#endif /* !JAFG_STATS_USE_GOOGLE_CHROME_TRACER */

#if WITH_STATS

#include "Async/TaskUtility.h"

namespace Jafg::Stats::Vendor
{

struct LGoogleChromeTracer;

} /* ~Namespace Jafg::Stats::Vendor */

namespace Jafg::Stats::Private
{

struct LSession;
struct LThread;
struct LBookmark;
struct LSessionEvent;
struct LStat;

} /* ~Namespace Jafg::Stats::Private */

namespace Jafg::Stats
{

#if JAFG_STATS_USE_GOOGLE_CHROME_TRACER
    typedef Vendor::LGoogleChromeTracer LTracer;
#endif /* JAFG_STATS_USE_GOOGLE_CHROME_TRACER */

} /* ~Namespace Jafg::Stats */

namespace Jafg::Stats::Private
{

struct LSession final
{
    LString Name;
    FORCEINLINE bool IsValid() const { return this->Name.IsEmpty() == false; }
};

struct LThread final
{
    LString Name;
    LThreadId ThreadId;
};

struct LBookmark final
{
    std::string_view Name;
    i64 BeginTime;
    LThreadId ThreadId;
};

struct LSessionEvent final
{
    std::string_view Name;
    i64 BeginTime;
    i64 EndTime;
    LThreadId ThreadId;
};

} /* ~Namespace Jafg::Stats::Private */

#endif /* WITH_STATS */
