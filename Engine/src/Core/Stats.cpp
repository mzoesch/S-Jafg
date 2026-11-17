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

//# Some Windows SDK macros that tracy uses, are undefined by Jafg.
#if JAFG_PLATFORM_WINDOWS && !defined(TEXT)
    #define TEXT(quote)                         __TEXT(quote)
    #define DETAIL_JAFG_STATS_RESTORED_TEXT     1
#endif /* JAFG_PLATFORM_WINDOWS && !defined(TEXT) */

#include "Definitions/PushNoWarnings.h"
#include "TracyClient.cpp"
#include "Definitions/PopDiagnostics.h"

#ifdef DETAIL_JAFG_STATS_RESTORED_TEXT
    #undef TEXT
    #undef DETAIL_JAFG_STATS_RESTORED_TEXT
#endif /* DETAIL_JAFG_STATS_RESTORED_TEXT */

#endif /* JAFG_WITH_STATS */
