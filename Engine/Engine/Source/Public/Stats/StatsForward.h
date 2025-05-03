// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

#if !WITH_STATS
    #error "This file should only be included if the application compiles with stats."
#endif /* WITH_STATS */

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
