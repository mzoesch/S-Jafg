// Copyright mzoesch. All rights reserved.

#pragma once

/*-----------------------------------------------------------------------------
    Predefined macros.
-----------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////
// Build configurations.

//#
//# If this is true, then the build is in debug mode.
//# Debug mode is the slowest version of Jafg but with the most security.
//# Generally speaking, it is not recommended to use debug builds during development.
//# @see AssertionMacros.h for more information about what macros are enabled in this build configuration.
//#
#ifndef IN_DEBUG
    #define IN_DEBUG                    0
#endif /* !IN_DEBUG */

//#
//# If this is true, then the build is in development mode.
//# Performance heave security checks are stripped but important fast passed assertions are still present.
//# This is the recommended build configuration for development.
//# @see AssertionMacros.h for more information about what macros are enabled in this build configuration.
//#
#ifndef IN_DEVELOPMENT
    #define IN_DEVELOPMENT              0
#endif /* !IN_DEVELOPMENT */

//#
//# If this is true, then the build is in shipping mode.
//# Shipping modes is the fastest version of Jafg but with the least security. If offers slim to none
//# security checks as it assumes that the underlying code is correct and tested.
//# This build has by default no debug symbols and is therefore not debuggable. You may enable debug symbols
//# by defining LAL_DO_DEBUG_SYMBOLS_IN_SHIPPING in your CMake configuration. Generally, you should only debug errors
//# in shipping builds if these errors are not reproducible in debug or development builds.
//# @see AssertionMacros.h for more information about what macros are enabled in this build configuration.
//#
#ifndef IN_SHIPPING
    #define IN_SHIPPING                 0
#endif /* !IN_SHIPPING */

#if !(IN_DEBUG || IN_DEVELOPMENT || IN_SHIPPING)
    #error "No build configuration specified."
#endif /* !(IN_DEBUG || IN_DEVELOPMENT || IN_SHIPPING) */
#if IN_DEBUG
    #if IN_DEVELOPMENT || IN_SHIPPING
        #error "Multiple build configurations specified."
    #endif /* IN_DEVELOPMENT || IN_SHIPPING */
#endif /* IN_DEBUG */
#if IN_DEVELOPMENT
    #if IN_DEBUG || IN_SHIPPING
        #error "Multiple build configurations specified."
    #endif /* IN_SHIPPING */
#endif /* IN_DEVELOPMENT */
#if IN_SHIPPING
    #if IN_DEBUG || IN_DEVELOPMENT
        #error "Multiple build configurations specified."
    #endif /* IN_DEVELOPMENT */
#endif /* IN_SHIPPING */

///////////////////////////////////////////////////////////////////////////////
// Build targets.
#ifndef AS_CLIENT
    #define AS_CLIENT                   0
    #define WITH_LOCAL_LAYER            0
#else /* !AS_CLIENT */
    /* This includes everything that cannot be daemonized (rendering, local access layer, etc.). */
    #define WITH_LOCAL_LAYER            1
#endif /* AS_CLIENT */
#ifndef AS_DAEMON
    #define AS_DAEMON                   0
#endif /* !AS_DAEMON */

#if !(AS_CLIENT || AS_DAEMON)
    #error "No build platform specified."
#endif /* !(AS_CLIENT || AS_DAEMON) */
#if AS_CLIENT
    #if AS_DAEMON
        #error "Multiple build targets specified."
    #endif /* AS_DAEMON */
#endif /* AS_CLIENT */
#if AS_DAEMON
    #if AS_CLIENT
        #error "Multiple build targets specified."
    #endif /* AS_CLIENT */
#endif /* AS_DAEMON */

/* Default to no tests. */
#ifndef WITH_TESTS
    #define WITH_TESTS                  0
#endif /* !WITH_TESTS */

//
// This is of course just hardcoded here for now.
// Later when we build an actual testing framework or use one, we would use
// the IDEA to determine what test should run.
//
#ifndef DO_TEST_UNITS
    #if WITH_TESTS
        #define DO_TEST_UNITS           1
    #else /* WITH_TESTS */
        #define DO_TEST_UNITS           0
    #endif /* !WITH_TESTS */
#endif /* !DO_TEST_UNITS */


/*-----------------------------------------------------------------------------
    Development macros.
-----------------------------------------------------------------------------*/

//# Only true if viewed by the intellisense. Might not be supported on all IDEs or compiler servers.
#ifndef WITH_IDEA_INTELLISENSE
    #ifdef __INTELLISENSE__
        #define WITH_IDEA_INTELLISENSE          1
    #else /* __INTELLISENSE__ */
        #define WITH_IDEA_INTELLISENSE          0
    #endif /* !__INTELLISENSE__ */
#endif /* !WITH_IDEA_INTELLISENSE */

#ifndef LAL_WITH_SANITIZERS
    #define LAL_WITH_SANITIZERS                 0
#endif /* !LAL_WITH_SANITIZERS */


/*-----------------------------------------------------------------------------
    Low level assertion macro forwards.
-----------------------------------------------------------------------------*/

//# Whether to ever do checks. This affects all build configurations.
#ifndef LAL_DO_EVER_CHECKS
    #define LAL_DO_EVER_CHECKS                                                  1
#endif /* !LAL_DO_EVER_CHECKS */

//# Whether to override the default behavior to strip checks in shipping builds.
#ifndef LAL_DO_CHECKS_IN_SHIPPING
    #define LAL_DO_CHECKS_IN_SHIPPING                                           0
#endif /* !LAL_DO_CHECKS_IN_SHIPPING */

//# Whether to allow to compile slow-checks in debug builds.
#ifndef LAL_ALLOW_SLOW_CHECKS
    #define LAL_ALLOW_SLOW_CHECKS                                               1
#endif /* !LAL_ALLOW_SLOW_CHECKS */

//# Whether slow checks should share the same lifetime as normal checks among build configurations.
#ifndef LAL_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME
    #define LAL_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME                            0
#endif /* !LAL_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME */

//#
//# Whether to only do panics instead of static asserts when the compiler / runtime encounters
//# an unimplemented code path in shipping. Useful when testing in shipping configuration.
//# But it should always be turned off in production shipping.
//#
#ifndef LAL_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING
    #define LAL_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING         0
#endif /* !LAL_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING */

#if LAL_DO_EVER_CHECKS
    #if IN_SHIPPING
        #deinfe LAL_DO_CHECKS                                                   LAL_DO_CHECKS_IN_SHIPPING
        #if LAL_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME
            #define LAL_DO_SLOW_CHECKS                                          LAL_DO_CHECKS_IN_SHIPPING
        #else /* LAL_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME */
            #define LAL_DO_SLOW_CHECKS                                          0
        #endif /* !LAL_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME */
    #else /* IN_SHIPPING */
        #define LAL_DO_CHECKS                                                   1
        #if IN_DEVELOPMENT
            #define LAL_DO_SLOW_CHECKS                                          LAL_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME
        #else /* IN_DEVELOPMENT */
            #define LAL_DO_SLOW_CHECKS                                          LAL_ALLOW_SLOW_CHECKS
        #endif /* !IN_DEVELOPMENT */
    #endif /* !IN_SHIPPING */
#else /* LAL_DO_EVER_CHECKS */
    #define LAL_DO_CHECKS                                                       0
    #define LAL_DO_SLOW_CHECKS                                                  0
#endif /* !LAL_DO_EVER_CHECKS */

#ifndef LAL_DO_CHECKS
    #error "LAL_DO_CHECKS is not defined."
#endif /* !LAL_DO_CHECKS */
#ifndef LAL_DO_SLOW_CHECKS
    #error "LAL_DO_SLOW_CHECKS is not defined."
#endif /* !LAL_DO_SLOW_CHECKS */

//# Default to only do ensure if checks are enabled.
#ifndef LAL_DO_ENSURES
    #define LAL_DO_ENSURES                                                      LAL_DO_CHECKS
#endif /* !LAL_DO_ENSURES */

#define PRIVATE_LAL_BUILD_H_INCLUDED                                            1
