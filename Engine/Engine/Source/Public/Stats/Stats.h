// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# Include this file to get access to the stats-system.
//# This file will include all necessary headers for you.
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

#include "CoreAfx.h"

#if WITH_STATS

#include "Stats/StatsForward.h"

#if JAFG_STATS_USE_GOOGLE_CHROME_TRACER
    #include "Stats/GoogleChromeTracer.h"
#endif /* JAFG_STATS_USE_GOOGLE_CHROME_TRACER */

#include "Stats/StatsCore.h"

#endif /* WITH_STATS */

#if WITH_STATS

#define PRIVATE_JAFG_STAT ::Jafg::Stats::Private::LStat
#define PRIVATE_JAFG_STAT_UNIQUE_SYMBOL PRIVATE_JAFG_CORE_JOIN_OUTER_TWO(_Stat, __LINE__)

#define STAT_CYCLE_START(Symbol, Name) \
    PRIVATE_JAFG_STAT Symbol { Name };
#define STAT_CYCLE_END(Symbol) \
    Symbol.Stop();

#define STAT_QUICK_CYCLE_START(Name) \
    STAT_CYCLE_START(PRIVATE_JAFG_STAT_UNIQUE_SYMBOL, Name)

#define STAT_CYCLE_FUNCTION_START(Symbol) \
    STAT_CYCLE_START(Symbol, JAFG_PRETTY_FUNCTION)
#define STAT_CYCLE_FUNCTION_END(Symbol) \
    STAT_CYCLE_END(Symbol)

#define STAT_CYCLE_UNIQUE(Name) \
    STAT_CYCLE_START(PRIVATE_JAFG_STAT_UNIQUE_SYMBOL, Name)

#define STAT_CYCLE_FUNCTION() \
    STAT_CYCLE_FUNCTION_START(PRIVATE_JAFG_STAT_UNIQUE_SYMBOL)

#define STAT_DISCARD(Name) \
    Name.Discard();

#else /* WITH_STATS */
// #define
#endif /* !WITH_STATS */
