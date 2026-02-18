// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# Include this file to get access to the stats-system.
//# This file will include all necessary headers for you.
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

#include "Stats/StatsForward.h"

#if WITH_STATS

#if JAFG_STATS_USE_GOOGLE_CHROME_TRACER
    #include "Stats/GoogleChromeTracer.h"
#endif /* JAFG_STATS_USE_GOOGLE_CHROME_TRACER */

#include "Stats/StatsCore.h"

#endif /* WITH_STATS */

#if WITH_STATS

#define PRIVATE_JAFG_STAT_TRACER ::Jafg::Stats::Private::GTracer
#define PRIVATE_JAFG_STAT ::Jafg::Stats::Private::LStat
#define PRIVATE_JAFG_STAT_UNIQUE_SYMBOL JAFG_JOIN_OUTER_TWO(_Stat, __LINE__)

//#
//# Start a new stat for cycles that has a unique symbol so it may be ended before leaving the scope.
//# If it is not ended manually, it will be ended automatically when going out of scope.
//# Also provide manually a display name for the stat.
//#
#define STAT_CYCLE_START(Symbol, Name) \
    PRIVATE_JAFG_STAT Symbol { Name };
//# Counterpart for #STAT_CYCLE_START to end the stat.
#define STAT_CYCLE_END(Symbol) \
    Symbol.Stop();

//# Provide a display name for the stat cycle. When this stat is going out of scope, it will be ended automatically.
#define STAT_QUICK_CYCLE_START(Name) \
    STAT_CYCLE_START(PRIVATE_JAFG_STAT_UNIQUE_SYMBOL, Name)

//#
//# Stat cycles with a unique symbol to end it manually before going out of scope.
//# The display name will be automatically generated from the function name.
//#
#define STAT_CYCLE_FUNCTION_START(Symbol) \
    STAT_CYCLE_START(Symbol, JAFG_PRETTY_FUNCTION_NAME)
//# Counterpart for #STAT_CYCLE_FUNCTION_START to end the stat.
#define STAT_CYCLE_FUNCTION_END(Symbol) \
    STAT_CYCLE_END(Symbol)

//#
//# Quick stat cycles within a function. The stat will be ended automatically when going out of scope.
//#
#define STAT_CYCLE_FUNCTION() \
    STAT_CYCLE_FUNCTION_START(PRIVATE_JAFG_STAT_UNIQUE_SYMBOL)

//#
//# Discard an already ongoing stat cycle.
//#
#define STAT_DISCARD(Symbol) \
    Symbol.Discard();

#define STAT_BOOKMARK(Name)                                                                                     \
    if (PRIVATE_JAFG_STAT_TRACER) {                                                                             \
        PRIVATE_JAFG_STAT_TRACER->AddBookmark({                                                                 \
            Name,                                                                                               \
            static_cast<i64>(::Jafg::Application::GetDeltaSinceStaticStorageInitialization() * maths::s2mus_d), \
            ::Jafg::Tasks::GetCurrentThreadId()                                                                 \
        });                                                                                                     \
    }

#else /* WITH_STATS */
    #define STAT_CYCLE_START(Symbol, Name)
    #define STAT_CYCLE_END(Symbol)
    #define STAT_QUICK_CYCLE_START(Name)
    #define STAT_CYCLE_FUNCTION_START(Symbol)
    #define STAT_CYCLE_FUNCTION_END(Symbol)
    #define STAT_CYCLE_FUNCTION()
    #define STAT_DISCARD(Name)
#endif /* !WITH_STATS */
