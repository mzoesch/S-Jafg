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
#ifndef JAFG_IN_DEBUG
    #define JAFG_IN_DEBUG                    0
#endif /* !JAFG_IN_DEBUG */

//#
//# If this is true, then the build is in development mode.
//# Performance heave security checks are stripped but important fast passed assertions are still present.
//# This is the recommended build configuration for development.
//# @see AssertionMacros.h for more information about what macros are enabled in this build configuration.
//#
#ifndef JAFG_IN_DEVELOPMENT
    #define JAFG_IN_DEVELOPMENT              0
#endif /* !JAFG_IN_DEVELOPMENT */

//#
//# If this is true, then the build is in shipping mode.
//# Shipping modes is the fastest version of Jafg but with the least security. If offers slim to none
//# security checks as it assumes that the underlying code is correct and tested.
//# This build has by default no debug symbols and is therefore not debuggable. You may enable debug symbols
//# by defining JAFG_DO_DEBUG_SYMBOLS_IN_SHIPPING in your CMake configuration. Generally, you should only debug errors
//# in shipping builds if these errors are not reproducible in debug or development builds.
//# @see AssertionMacros.h for more information about what macros are enabled in this build configuration.
//#
#ifndef JAFG_IN_SHIPPING
    #define JAFG_IN_SHIPPING                 0
#endif /* !JAFG_IN_SHIPPING */

#if !(JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT || JAFG_IN_SHIPPING)
    #error "No build configuration specified."
#endif /* !(JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT || JAFG_IN_SHIPPING) */
#if JAFG_IN_DEBUG
    #if JAFG_IN_DEVELOPMENT || JAFG_IN_SHIPPING
        #error "Multiple build configurations specified."
    #endif /* JAFG_IN_DEVELOPMENT || JAFG_IN_SHIPPING */
#endif /* JAFG_IN_DEBUG */
#if JAFG_IN_DEVELOPMENT
    #if JAFG_IN_DEBUG || JAFG_IN_SHIPPING
        #error "Multiple build configurations specified."
    #endif /* JAFG_IN_SHIPPING */
#endif /* JAFG_IN_DEVELOPMENT */
#if JAFG_IN_SHIPPING
    #if JAFG_IN_DEBUG || JAFG_IN_DEVELOPMENT
        #error "Multiple build configurations specified."
    #endif /* JAFG_IN_DEVELOPMENT */
#endif /* JAFG_IN_SHIPPING */

///////////////////////////////////////////////////////////////////////////////
// Whether this is a reflected build.
#ifndef JAFG_IS_REFLECTED
    #define JAFG_IS_REFLECTED            0
#endif /* !JAFG_IS_REFLECTED */

///////////////////////////////////////////////////////////////////////////////
// Build targets.
#ifndef JAFG_AS_CLIENT
    #define JAFG_AS_CLIENT                   0
    #define WITH_LOCAL_LAYER            0
#else /* !JAFG_AS_CLIENT */
    /* This includes everything that cannot be daemonized (rendering, local access layer, etc.). */
    #define WITH_LOCAL_LAYER            1
#endif /* JAFG_AS_CLIENT */
#ifndef JAFG_AS_DAEMON
    #define JAFG_AS_DAEMON                   0
#endif /* !JAFG_AS_DAEMON */

#if !(JAFG_AS_CLIENT || JAFG_AS_DAEMON)
    #error "No build platform specified."
#endif /* !(JAFG_AS_CLIENT || JAFG_AS_DAEMON) */
#if JAFG_AS_CLIENT
    #if JAFG_AS_DAEMON
        #error "Multiple build targets specified."
    #endif /* JAFG_AS_DAEMON */
#endif /* JAFG_AS_CLIENT */
#if JAFG_AS_DAEMON
    #if JAFG_AS_CLIENT
        #error "Multiple build targets specified."
    #endif /* JAFG_AS_CLIENT */
#endif /* JAFG_AS_DAEMON */

/* Default to no tests. */
#ifndef JAFG_WITH_TESTS
    #define JAFG_WITH_TESTS                  0
#endif /* !JAFG_WITH_TESTS */

//
// This is of course just hardcoded here for now.
// Later when we build an actual testing framework or use one, we would use
// the IDEA to determine what test should run.
//
#ifndef DO_TEST_UNITS
    #if JAFG_WITH_TESTS
        #define DO_TEST_UNITS           1
    #else /* JAFG_WITH_TESTS */
        #define DO_TEST_UNITS           0
    #endif /* !JAFG_WITH_TESTS */
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

#ifndef JAFG_WITH_SANITIZERS
    #define JAFG_WITH_SANITIZERS                 0
#endif /* !JAFG_WITH_SANITIZERS */


/*-----------------------------------------------------------------------------
    Low level assertion macro forwards.
-----------------------------------------------------------------------------*/

//# Whether to ever do checks. This affects all build configurations.
#ifndef JAFG_DO_EVER_CHECKS
    #define JAFG_DO_EVER_CHECKS                                                  1
#endif /* !JAFG_DO_EVER_CHECKS */

//# Whether to override the default behavior to strip checks in shipping builds.
#ifndef JAFG_DO_CHECKS_IN_SHIPPING
    #define JAFG_DO_CHECKS_IN_SHIPPING                                           0
#endif /* !JAFG_DO_CHECKS_IN_SHIPPING */

//# Whether to allow to compile slow-checks in debug builds.
#ifndef JAFG_ALLOW_SLOW_CHECKS
    #define JAFG_ALLOW_SLOW_CHECKS                                               1
#endif /* !JAFG_ALLOW_SLOW_CHECKS */

//# Whether slow checks should share the same lifetime as normal checks among build configurations.
#ifndef JAFG_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME
    #define JAFG_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME                            0
#endif /* !JAFG_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME */

//#
//# Whether to only do panics instead of static asserts when the compiler / runtime encounters
//# an unimplemented code path in shipping. Useful when testing in shipping configuration.
//# But it should always be turned off in production shipping.
//#
#ifndef JAFG_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING
    #define JAFG_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING         0
#endif /* !JAFG_DO_COMPILER_IGNORE_UNIMPLEMENTED_CTRL_PATHS_IN_SHIPPING */

#if JAFG_DO_EVER_CHECKS
    #if JAFG_IN_SHIPPING
        #define JAFG_DO_CHECKS                                                   JAFG_DO_CHECKS_IN_SHIPPING
        #if JAFG_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME
            #define JAFG_DO_SLOW_CHECKS                                          JAFG_DO_CHECKS_IN_SHIPPING
        #else /* JAFG_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME */
            #define JAFG_DO_SLOW_CHECKS                                          0
        #endif /* !JAFG_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME */
    #else /* JAFG_IN_SHIPPING */
        #define JAFG_DO_CHECKS                                                   1
        #if JAFG_IN_DEVELOPMENT
            #define JAFG_DO_SLOW_CHECKS                                          JAFG_LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME
        #else /* JAFG_IN_DEVELOPMENT */
            #define JAFG_DO_SLOW_CHECKS                                          JAFG_ALLOW_SLOW_CHECKS
        #endif /* !JAFG_IN_DEVELOPMENT */
    #endif /* !JAFG_IN_SHIPPING */
#else /* JAFG_DO_EVER_CHECKS */
    #define JAFG_DO_CHECKS                                                       0
    #define JAFG_DO_SLOW_CHECKS                                                  0
#endif /* !JAFG_DO_EVER_CHECKS */

#ifndef JAFG_DO_CHECKS
    #error "JAFG_DO_CHECKS is not defined."
#endif /* !JAFG_DO_CHECKS */
#ifndef JAFG_DO_SLOW_CHECKS
    #error "JAFG_DO_SLOW_CHECKS is not defined."
#endif /* !JAFG_DO_SLOW_CHECKS */

//# Default to only do ensure if checks are enabled.
#ifndef JAFG_DO_ENSURES
    #define JAFG_DO_ENSURES                                                      JAFG_DO_CHECKS
#endif /* !JAFG_DO_ENSURES */

#define PRIVATE_JAFG_BUILD_H_INCLUDED                                            1

#if JAFG_WITH_MSVC
    #if JAFG_IN_DEBUG
        #ifndef _DEBUG
            #define _DEBUG
        #endif /* _DEBUG */
    #endif /* JAFG_IN_DEBUG */
#endif /* JAFG_WITH_MSVC */
