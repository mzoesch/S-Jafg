# Copyright mzoesch. All rights reserved.

###############################################################################
# Important misc CMake shit.
###############################################################################
option(JAFG_DO_DEBUG_SYMBOLS_IN_SHIPPING
    "Whether to include debug symbols in shipping builds. This is not recommended. See Build.h for more information"
    OFF
    )
option(JAFG_DO_SANITIZED_BUILD
    "Whether to enable all sorts of sanitizers for the currently used compiler to assert at least some memory safety."
    OFF
    )
option(JAFG_DO_HARDEN_BUILD
    "Whether to enable some compiler flags to harder the final binary and make it more secure."
    OFF
    )

###############################################################################
# Important misc flags.
###############################################################################
unspecified_flag(JAFG_DO_COMPILER_DIAGNOSTIC_SETUP
    "Whether to do diagnostic setup for warnings and errors (such as raise and ignore) to best fit the Jafg project."
    )
unspecified_flag(JAFG_DO_ENABLE_SHIPPING_WARNINGS
    "Many warnings are disabled by default in development builds to ease development. This flag enables all
    warnings that are not enabled by default in development builds. This is useful for shipping builds to ensure
    correct and safe code."
    )

###############################################################################
# Log flags.
###############################################################################
if(NOT DEFINED CACHE{JAFG_LOG_DEFAULT_VERBOSITY})
    set(JAFG_LOG_DEFAULT_VERBOSITY
        "Unspecified"
        CACHE STRING
        "The default log verbosity to use for log categories that respect this. Unspecified means to let C++ decide."
        )
endif()
set_property(
    CACHE JAFG_LOG_DEFAULT_VERBOSITY PROPERTY STRINGS
    "Unspecified" "Trace" "Verbose" "Info" "Warning" "Error" "Fatal" "KillLog"
    )
unspecified_flag(JAFG_LOG_ENABLE_TRACE
    "Whether to enable the trace log category. If this evaluates to false, then no trace logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
unspecified_flag(JAFG_LOG_ENABLE_VERBOSE
    "Whether to enable the verbose log category. If this evaluates to false, then no verbose logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
unspecified_flag(JAFG_LOG_ENABLE_INFO
    "Whether to enable the info log category. If this evaluates to false, then no info logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
unspecified_flag(JAFG_LOG_ENABLE_WARNING
    "Whether to enable the warning log category. If this evaluates to false, then no warning logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
unspecified_flag(JAFG_LOG_ENABLE_ERROR
    "Whether to enable the error log category. If this evaluates to false, then no error logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
unspecified_flag(JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER
    "Whether to enable scoped time task measurers. @see Logging/LogUtility.h"
    )
if(NOT DEFINED CACHE{JAFG_FORCE_LOG_FLUSH_INTERVAL})
    set(JAFG_FORCE_LOG_FLUSH_INTERVAL
        "Unspecified"
        CACHE STRING
        "The interval in seconds to force a log flush. This is useful for debugging purposes, but can cause performance
        issues. Not all platforms will respect this. Unspecified means to let C++ decide."
        )
endif()

if(NOT DEFINED CACHE{JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES})
    set(JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES
        "Unspecified"
        CACHE STRING
        "The time in seconds for which a frame is considered very long. If a frame takes longer than this, it will be
        logged as a warning. Unspecified means to let C++ decide."
        )
endif()

###############################################################################
# Jafg flags.
###############################################################################

unspecified_flag(JAFG_WITH_STATS "Whether to enable stats. Disabled in shipping. @see Core/Stats.h")
unspecified_flag(JAFG_STATS_ON_DEMAND "Whether to enable stats on demand. Usually always enabled. @see Core/Stats.h")
unspecified_flag(JAFG_STATS_NO_EXIT
    "Whether to enable that jafg does not exit until all profile data is sent to a server.
    If JAFG_STATS_ON_DEMAND and JAFG_STATS_NO_EXIT is enabled then the application will be in a deadlock at the end
    its lifetime. This is a current limitation of the vendor stats system.
    @see Core/Stats.h"
    )

if(NOT DEFINED CACHE{JAFG_MAX_FRAMES_IN_FLIGHT})
    set(
        JAFG_MAX_FRAMES_IN_FLIGHT
        "Unspecified"
        CACHE STRING
        "How many frames at a maximum can be in flight. This is the theoretical maximum; the actual maximum may be
        lower depending on the device capabilities."
        )
endif()
set_property(CACHE ${flag_name} PROPERTY STRINGS "Unspecified" "1" "2" "3")
