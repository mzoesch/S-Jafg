# Copyright mzoesch. All rights reserved.

include(CMake/ModuleFlagsForward.cmake)


###############################################################################
# Important misc CMake shit.
###############################################################################
message(STATUS "Flags for: Important misc CMake shit.")

option(
    LAL_DO_DEBUG_SYMBOLS_IN_SHIPPING
    "Whether to include debug symbols in shipping builds. This is not recommended. See Build.h for more information"
    OFF
    )
message(STATUS "LAL_DO_DEBUG_SYMBOLS_IN_SHIPPING: ${LAL_DO_DEBUG_SYMBOLS_IN_SHIPPING}")

option(
    LAL_DO_SANITIZED_BUILD
    "Whether to enable all sorts of sanitizers for the currently used compiler to assert at least some memory safety."
    OFF
    )
message(STATUS "LAL_DO_SANITIZED_BUILD: ${LAL_DO_SANITIZED_BUILD}")

option(
    LAL_DO_HARDEN_BUILD
    "Whether to enable some compiler flags to harder the final binary and make it more secure."
    OFF
    )
message(STATUS "LAL_DO_HARDEN_BUILD: ${LAL_DO_HARDEN_BUILD}")

unspecified_option(
    TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS
    "Whether to include legacy unit tests for the tester."
    )


###############################################################################
# Important misc flags.
###############################################################################
message(STATUS "Flags for: Important misc flags.")

option(
    LAL_DO_COMPILER_DIAGNOSTIC_SETUP
    "Whether to do diagnostic setup for warnings and errors (such as raise and ignore) to best fit the Jafg project."
    ON
    )
message(STATUS "LAL_DO_COMPILER_DIAGNOSTIC_SETUP: ${LAL_DO_COMPILER_DIAGNOSTIC_SETUP}")

unspecified_option(
    LAL_DO_ENABLE_SHIPPING_WARNINGS
    "Many warnings are disabled by default in development builds to ease development. This flag enables all
    warnings that are not enabled by default in development builds. This is useful for shipping builds to ensure
    correct and safe code."
    )


###############################################################################
# Log flags.
###############################################################################
message(STATUS "Flags for: Log flags.")

if(NOT DEFINED CACHE{LAL_LOG_DEFAULT_VERBOSITY})
    set(
        LAL_LOG_DEFAULT_VERBOSITY
        "Unspecified"
        CACHE STRING
        "The default log verbosity to use for log categories that respect this. Unspecified means to let C++ decide."
        )
endif()
set_property(
    CACHE LAL_LOG_DEFAULT_VERBOSITY PROPERTY STRINGS
    "Unspecified" "Trace" "Verbose" "Info" "Warning" "Error" "Fatal" "KillLog"
    )
message(STATUS "LAL_LOG_DEFAULT_VERBOSITY: ${LAL_LOG_DEFAULT_VERBOSITY}")

unspecified_option(
    LAL_LOG_ENABLE_TRACE
    "Whether to enable the trace log category. If this evaluates to false, then no trace logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )

unspecified_option(
    LAL_LOG_ENABLE_VERBOSE
    "Whether to enable the verbose log category. If this evaluates to false, then no verbose logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )

unspecified_option(
    LAL_LOG_ENABLE_INFO
    "Whether to enable the info log category. If this evaluates to false, then no info logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )

unspecified_option(
    LAL_LOG_ENABLE_WARNING
    "Whether to enable the warning log category. If this evaluates to false, then no warning logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )

unspecified_option(
    LAL_LOG_ENABLE_ERROR
    "Whether to enable the error log category. If this evaluates to false, then no error logs will be emitted by
    any category even if it is enabled for said category. Unspecified means to let C++ decide.
    @see Logging/LogMacros.h"
    )

unspecified_option(
    LAL_LOG_DO_SCOPED_TIME_TASK_MEASURER
    "Whether to enable scoped time task measurers. @see Logging/LogUtility.h"
    )

if(NOT DEFINED CACHE{JAFG_FORCE_LOG_FLUSH_INTERVAL})
    set(
        JAFG_FORCE_LOG_FLUSH_INTERVAL
        "Unspecified"
        CACHE STRING
        "The interval in seconds to force a log flush. This is useful for debugging purposes, but can cause performance
        issues. Not all platforms will respect this. Unspecified means to let C++ decide."
        )
endif()
message(STATUS "JAFG_FORCE_LOG_FLUSH_INTERVAL: ${JAFG_FORCE_LOG_FLUSH_INTERVAL}")

if(NOT DEFINED CACHE{JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES})
    set(
        JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES
        "Unspecified"
        CACHE STRING
        "The time in seconds for which a frame is considered very long. If a frame takes longer than this, it will be
        logged as a warning. Unspecified means to let C++ decide."
        )
endif()
message(STATUS "JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES: ${JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES}")


###############################################################################
# Container flags.
###############################################################################
message(STATUS "Flags for: Container flags.")

unspecified_option(
    LAL_CHECK_CONTAINER_BOUNDS
    "Whether to check container bounds. @see Containers/ContainerBuild.h"
    )

unspecified_option(
    LAL_CHECK_STRING_VALIDITY
    "Whether to validate string integrity. @see Containers/ContainerBuild.h"
    )

unspecified_flag(
    LAL_CHECK_ARRAY
    "The implementation of the lal array check. @see Containers/LalArray.h"
    )
