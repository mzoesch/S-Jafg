# Copyright mzoesch. All rights reserved.

###############################################################################
# Important misc flags.
###############################################################################
option(
    LAL_DO_DEBUG_SYMBOLS_IN_SHIPPING
    "Whether to include debug symbols in shipping builds. This is not recommended. See Build.h for more information"
    OFF
    )

option(
    LAL_FLAG_DO_COMPILER_DIAGNOSTIC_SETUP
    "Whether to do diagnostic setup for warnings and errors (such as raise and ignore) to best fit the Jafg project."
    ON
    )

###############################################################################
# Log flags.
###############################################################################
set(
    LAL_FLAG_DEFAULT_LOG_VERBOSITY
    "Unspecified"
    CACHE STRING
    "The default log verbosity to use for log categories that respect this. Unspecified means to let C++ decide."
    )
set_property(
    CACHE LAL_FLAG_DEFAULT_LOG_VERBOSITY PROPERTY STRINGS
    "Unspecified" "Trace" "Verbose" "Info" "Warning" "Error" "Fatal" "KillLog"
    )

set(
    LAL_FLAG_LOG_ENABLE_TRACE
    "Unspecified"
    CACHE STRING
    "Whether to enable the trace log category. If this evaluates to false, then no trace logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
set_property(
    CACHE LAL_FLAG_LOG_ENABLE_TRACE PROPERTY STRINGS
    "Unspecified" "1" "0"
    )

set(
    LAL_FLAG_LOG_ENABLE_VERBOSE
    "Unspecified"
    CACHE STRING
    "Whether to enable the verbose log category. If this evaluates to false, then no verbose logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
set_property(
    CACHE LAL_FLAG_LOG_ENABLE_VERBOSE PROPERTY STRINGS
    "Unspecified" "1" "0"
    )

set(
    LAL_FLAG_LOG_ENABLE_INFO
    "Unspecified"
    CACHE STRING
    "Whether to enable the info log category. If this evaluates to false, then no info logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
set_property(
    CACHE LAL_FLAG_LOG_ENABLE_INFO PROPERTY STRINGS
    "Unspecified" "1" "0"
    )

set(
    LAL_FLAG_LOG_ENABLE_WARNING
    "Unspecified"
    CACHE STRING
    "Whether to enable the warning log category. If this evaluates to false, then no warning logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
set_property(
    CACHE LAL_FLAG_LOG_ENABLE_WARNING PROPERTY STRINGS
    "Unspecified" "1" "0"
    )

set(
    LAL_FLAG_LOG_ENABLE_ERROR
    "Unspecified"
    CACHE STRING
    "Whether to enable the error log category. If this evaluates to false, then no error logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )
set_property(
    CACHE LAL_FLAG_LOG_ENABLE_ERROR PROPERTY STRINGS
    "Unspecified" "1" "0"
    )

set(
    LAL_FLAG_LOG_DO_SCOPED_TIME_TASK_MEASURER
    "Unspecified"
    CACHE STRING
    "Whether to enable scoped time task measurers. @see Logging/LogUtility.h"
    )
set_property(
    CACHE LAL_FLAG_LOG_DO_SCOPED_TIME_TASK_MEASURER PROPERTY STRINGS
    "Unspecified" "1" "0"
    )

set(
    JAFG_FLAG_FORCE_LOG_FLUSH_INTERVAL
    "Unspecified"
    CACHE STRING
    "The interval in seconds to force a log flush. This is useful for debugging purposes, but can cause performance
    issues. Not all platforms will respect this. Unspecified means to let C++ decide."
    )

set(
    JAFG_FLAG_LOG_TIME_FOR_VERY_LONG_FRAMES
    "Unspecified"
    CACHE STRING
    "The time in seconds for which a frame is considered very long. If a frame takes longer than this, it will be
    logged as a warning. Unspecified means to let C++ decide."
    )
