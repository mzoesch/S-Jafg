// Copyright mzoesch. All rights reserved.

#include "Core/Stats.h"
#include "Core/App.h"

#if JAFG_WITH_STATS

#ifndef DETAIL_JAFG_STATS_PROFILER
    #error "DETAIL_JAFG_STATS_PROFILER is not defined."
#endif /* DETAIL_JAFG_STATS_PROFILER */

#if JAFG_WITH_LOCAL_LAYER
LPath const& Jafg::App::Detail::GetProfilerExecutable() noexcept
{
    static LPath Profiler{LITERAL_TEXT("" JAFG_MAKE_STRING_OUTER(DETAIL_JAFG_STATS_PROFILER) "")};
    return Profiler;
}
#endif /* JAFG_WITH_LOCAL_LAYER */

#include "Definitions/PushNoWarnings.h"
#include "TracyClient.cpp"
#include "Definitions/PopDiagnostics.h"

#endif /* JAFG_WITH_STATS */
