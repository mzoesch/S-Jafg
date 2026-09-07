// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# If not defined, always do stats except in shipping builds.
//#
#ifndef JAFG_WITH_STATS
    #define JAFG_WITH_STATS                                             (!JAFG_IN_SHIPPING)
#endif /* !JAFG_WITH_STATS */
#ifdef TRACY_ENABLE
    #error "TRACY_ENABLE is enabled. But you should use JAFG_WITH_STATS instead."
#endif /* TRACY_ENABLE */
#if JAFG_WITH_STATS
    #define TRACY_ENABLE
#endif /* JAFG_WITH_STATS */

#if JAFG_WITH_STATS

    #ifdef TRACY_ON_DEMAND
        #error "TRACY_ON_DEMAND is defined. But you should use JAFG_STATS_ON_DEMAND instead."
    #endif /* TRACY_ON_DEMAND */
    #ifndef JAFG_STATS_ON_DEMAND
        #define JAFG_STATS_ON_DEMAND                                    1
    #endif /* !JAFG_STATS_ON_DEMAND */
    #if JAFG_STATS_ON_DEMAND
        #define TRACY_ON_DEMAND                                         1
    #endif /* JAFG_STATS_ON_DEMAND */

    #ifdef TRACY_NO_EXIT
        #error "TRACY_NO_EXIT is defined. But you should use JAFG_STATS_NO_EXIT instead."
    #endif /* TRACY_NO_EXIT */
    #ifndef JAFG_STATS_NO_EXIT
        #define JAFG_STATS_NO_EXIT                                      0
    #endif /* !JAFG_STATS_NO_EXIT */
    #if JAFG_STATS_NO_EXIT
        #define TRACY_NO_EXIT                                           1
    #endif /* JAFG_STATS_NO_EXIT */

    //# Force localhost.
    #define TRACY_ONLY_LOCALHOST                                        1

    #include "Definitions/PushNoWarnings.h"
    #include "tracy/Tracy.hpp"
    #include "Definitions/PopDiagnostics.h"

    #ifndef STAT_DEFAULT_VARNAME
        #define STAT_DEFAULT_VARNAME                                    ___tracy_scoped_zone
    #endif /* !STAT_DEFAULT_VARNAME */
    #ifndef STAT_DEFAULT_SOURCE_LOCATION
        #define STAT_DEFAULT_SOURCE_LOCATION                            __tracy_source_location
    #endif /* !STAT_DEFAULT_SOURCE_LOCATION */

    #define DETAIL_STAT_ZONE_NAMED(qual, varname, name, active) \
        static qual tracy::SourceLocationData TracyConcat(STAT_DEFAULT_SOURCE_LOCATION,TracyLine) \
        { \
            name, TracyFunction,  TracyFile, static_cast<uint32_t>(TracyLine), 0 \
        }; \
        tracy::ScopedZone varname(&TracyConcat(STAT_DEFAULT_SOURCE_LOCATION,TracyLine), TRACY_CALLSTACK, active)

    #define STAT_FRAME_MARK()               FrameMark;
    #define STAT_FRAME_MARK_NAMED(name)     FrameMarkNamed(name);
    #define STAT_FRAME_MARK_START(name)     FrameMarkStart(name);
    #define STAT_FRAME_MARK_END(name)       FrameMarkEnd(name);

    //# Stat a scope with the name being the pretty function repr.
    #define STAT_FUNCTION() \
        static LString JAFG_JOIN_OUTER_TWO(STAT_DEFAULT_SOURCE_LOCATION, _fn_name){JAFG_PRETTY_FUNCTION_NAME}; \
        DETAIL_STAT_ZONE_NAMED(,STAT_DEFAULT_VARNAME, JAFG_JOIN_OUTER_TWO(STAT_DEFAULT_SOURCE_LOCATION, _fn_name).c_str(), true);
    //# Stat a scope with a custom name.
    #define STAT_ZONE(name) STAT_NAMED_ZONE(STAT_DEFAULT_VARNAME, name)
    //# Same as STAT_ZONE but allows one to set a custom name for the stat.
    #define STAT_NAMED_ZONE(varname, name) SuppressVarShadowWarning(DETAIL_STAT_ZONE_NAMED(constexpr, varname, name, true))
    //# Transient zone which should always be renamed.
    #define STAT_TRANSIENT_ZONE() STAT_TRANSIENT_NAMED_ZONE(STAT_DEFAULT_VARNAME)
    #define STAT_TRANSIENT_NAMED_ZONE(varname) STAT_NAMED_ZONE(varname, "__Transient")
    //# Declare a stat that has always a different name. Based of its context.
    #define STAT_CUSTOM_ZONE_STR(str) STAT_CUSTOM_NAMED_ZONE_STR(STAT_DEFAULT_VARNAME, str)
    #define STAT_CUSTOM_NAMED_ZONE_STR(varname, str) STAT_TRANSIENT_NAMED_ZONE(varname) STAT_RENAME_NAMED_ZONE_STR(varname, str)
    #define STAT_CUSTOM_ZONE_FMT(fmt, ...) STAT_CUSTOM_NAMED_ZONE_FMT(STAT_DEFAULT_VARNAME, fmt __VA_OPT__(,) __VA_ARGS__)
    #define STAT_CUSTOM_NAMED_ZONE_FMT(varname, fmt, ...) STAT_TRANSIENT_NAMED_ZONE(varname) STAT_RENAME_NAMED_ZONE_FMT(varname, fmt __VA_OPT__(,) __VA_ARGS__)

    //# Rename a zone (warning: is slow. Do not use in hot ctrl paths).
    #define STAT_RENAME_ZONE(ptr, size) STAT_RENAME_NAMED_ZONE(STAT_DEFAULT_VARNAME, ptr, size);
    #define STAT_RENAME_NAMED_ZONE(varname, ptr, size) ZoneNameV(varname, ptr, size);
    #define STAT_RENAME_ZONE_STR(str) STAT_RENAME_NAMED_ZONE_STR(STAT_DEFAULT_VARNAME, str);
    #define STAT_RENAME_NAMED_ZONE_STR(varname, str) STAT_RENAME_NAMED_ZONE(varname, str.data(), str.size())
    #define STAT_RENAME_ZONE_FMT(fmt, ...) STAT_RENAME_NAMED_ZONE_FMT(STAT_DEFAULT_VARNAME, fmt __VA_OPT__(,) __VA_ARGS__);
    #define STAT_RENAME_NAMED_ZONE_FMT(varname, fmt, ...) \
        LString JAFG_JOIN_OUTER_TWO(varname, _format_string){::algo::sprintf(fmt __VA_OPT__(,) __VA_ARGS__)}; \
        STAT_RENAME_NAMED_ZONE(varname, JAFG_JOIN_OUTER_TWO(varname, _format_string).data(), JAFG_JOIN_OUTER_TWO(varname, _format_string).size())

    //# Execute code that is only compiled when stats are enabled.
    #define STAT_EXEC(...) do { __VA_ARGS__; } while (false);

#else /* JAFG_WITH_STATS */

    #define DETAIL_STAT_ZONE_NAMED(qual, varname, name, active)

    #define STAT_FRAME_MARK()
    #define STAT_FRAME_MARK_NAMED(name)
    #define STAT_FRAME_MARK_START(name)
    #define STAT_FRAME_MARK_END(name)

    #define STAT_FUNCTION()
    #define STAT_ZONE(name)
    #define STAT_NAMED_ZONE(varname, name)
    #define STAT_TRANSIENT_ZONE()
    #define STAT_TRANSIENT_NAMED_ZONE(varname)
    #define STAT_CUSTOM_ZONE_STR(str)
    #define STAT_CUSTOM_NAMED_ZONE_STR(varname, str)
    #define STAT_CUSTOM_ZONE_FMT(fmt, ...)
    #define STAT_CUSTOM_NAMED_ZONE_FMT(varname, fmt, ...)

    #define STAT_RENAME_ZONE(ptr, size)
    #define STAT_RENAME_NAMED_ZONE(varname, ptr, size)
    #define STAT_RENAME_ZONE_STR(str)
    #define STAT_RENAME_NAMED_ZONE_STR(varname, str)
    #define STAT_RENAME_ZONE_FMT(fmt, ...)
    #define STAT_RENAME_NAMED_ZONE_FMT(varname, fmt, ...)

    #define STAT_EXEC(...)

#endif /* !JAFG_WITH_STATS */
