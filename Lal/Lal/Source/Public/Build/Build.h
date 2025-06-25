// Copyright mzoesch. All rights reserved.

#pragma once

/*-----------------------------------------------------------------------------
    Predefined macros.
-----------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////
// Build configurations.
#ifndef IN_DEBUG
    #define IN_DEBUG                0
#endif /* !IN_DEBUG */
#ifndef IN_DEVELOPMENT
    #define IN_DEVELOPMENT          0
#endif /* !IN_DEVELOPMENT */
#ifndef IN_SHIPPING
    #define IN_SHIPPING             0
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
    #define AS_CLIENT               0
    #define WITH_FRONTEND           0
    #define WITH_LOCAL_LAYER        0
#else /* !AS_CLIENT */
    #define WITH_FRONTEND           1
    #define WITH_LOCAL_LAYER        1
#endif /* AS_CLIENT */
#ifndef AS_DAEMON
    #define AS_DAEMON               0
#endif /* !AS_DAEMON */

#if !(AS_CLIENT || AS_DAEMON)
    #error "No build platform specified."
#endif /* !(AS_CLIENT || AS_DAEMON) */
#if AS_CLIENT
    #if AS_DAEMON
        #error "Multiple build platforms specified."
    #endif /* AS_DAEMON */
#endif /* AS_CLIENT */
#if AS_DAEMON
    #if AS_CLIENT
        #error "Multiple build platforms specified."
    #endif /* AS_CLIENT */
#endif /* AS_DAEMON */
#ifndef WITH_FRONTEND
    #error "WITH_FRONTEND not defined."
#endif /* !WITH_FRONTEND */

#ifndef WITH_TESTS
    #define WITH_TESTS              0
#endif /* !WITH_TESTS */

/*
 * This is of course just hardcoded here for now.
 * Later when we build an actual testing framework or use one, we would use
 * the IDEA to determine what test should run.
 */
#ifndef DO_TEST_UNITS
    #if WITH_TESTS
        #define DO_TEST_UNITS           1
    #else /* WITH_TESTS */
        #define DO_TEST_UNITS           0
    #endif /* !WITH_TESTS */
#endif /* !DO_TEST_UNITS */


/*-----------------------------------------------------------------------------
    Manual build override settings.
-----------------------------------------------------------------------------*/

#define PRIVATE_INCLUDED_AUTOMATIC_BUILD

#ifndef REFLECT_MANUAL_BUILD
    /** Always (EVEN IN SHIPPING) reflect manual build if not specified otherwise. */
    #define REFLECT_MANUAL_BUILD_OVERRIDES 1
#endif /* !REFLECT_MANUAL_BUILD */

#if REFLECT_MANUAL_BUILD_OVERRIDES
    /* Override with manual build settings. */
    #include "Build/ManualBuildOverride.h"
#endif /* REFLECT_MANUAL_BUILD */


/*-----------------------------------------------------------------------------
    Development macros.
-----------------------------------------------------------------------------*/

#ifdef __INTELLISENSE__
    #ifndef WITH_IDEA_INTELLISENSE
        #define WITH_IDEA_INTELLISENSE          1
    #endif /* !WITH_IDEA_INTELLISENSE */
#else /* __INTELLISENSE__ */
    #ifndef WITH_IDEA_INTELLISENSE
        #define WITH_IDEA_INTELLISENSE          0
    #endif /* !WITH_IDEA_INTELLISENSE */
#endif /* !__INTELLISENSE__ */

/**
 * Checks are only executed in development configurations unless overridden in manual build.
 */
#if DO_EVER_CHECKS
    #if IN_SHIPPING
        #define DO_CHECKS               DO_CHECKS_IN_SHIPPING
        #if LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME
            #define DO_SLOW_CHECKS      DO_CHECKS_IN_SHIPPING
        #else /* LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME */
            #define DO_SLOW_CHECKS      0
        #endif /* !LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME */
    #else /* IN_SHIPPING */
        #define DO_CHECKS               1
        #if IN_DEVELOPMENT
            #define DO_SLOW_CHECKS      LET_SLOW_CHECKS_SHARE_CHECK_LIFETIME
        #else /* IN_DEVELOPMENT */
            #define DO_SLOW_CHECKS      ALLOW_SLOW_CHECKS
        #endif /* !IN_DEVELOPMENT */
    #endif /* !IN_SHIPPING */
#else /* DO_EVER_CHECKS */
    #define DO_CHECKS                   0
    #define DO_SLOW_CHECKS              0
#endif /* !DO_EVER_CHECKS */

/**
 * Always do assertions (even in SHIPPING) if not overridden in manual build.
 */
#if DO_EVER_ASSERTS
    #if IN_SHIPPING
        #define DO_ASSERTS              !DO_STRIP_ASSERTS_IN_SHIPPING
    #else /* IN_SHIPPING */
        #define DO_ASSERTS              1
    #endif /* !IN_SHIPPING */
#else /* DO_EVER_ASSERTS */
    #define DO_ASSERTS                  0
#endif /* !DO_EVER_ASSERTS */

/**
 * Only do ensure if checks are enabled.
 */
#define DO_ENSURES                      DO_CHECKS

#define WITH_DEBUG_ZERO_UNBOUND         IN_DEBUG

#ifndef WITH_STATS
    #define WITH_STATS                  !IN_SHIPPING
#endif /* !WITH_STATS */
#ifndef JAFG_STATS_USE_GOOGLE_CHROME_TRACER
    #define JAFG_STATS_USE_GOOGLE_CHROME_TRACER      1
#endif /* !JAFG_STATS_USE_GOOGLE_CHROME_TRACER */


/*-----------------------------------------------------------------------------
    Jafg build tool.
-----------------------------------------------------------------------------*/

/** Pragmas for the Jafg Build Tool. */
#define PRAGMA_FOR_JAFG_BUILD_TOOL(Pragma)


/*-----------------------------------------------------------------------------
    Static errors not caught by the compiler.
-----------------------------------------------------------------------------*/

//
// Error C1189 : #error:  The C++ Standard Library forbids macroizing the keyword "dynamic_cast".
//
// Shit. We cannot define dynamic_cast?
// But how can we check this mistake at runtime?
//
// /**
//  * Do not use dynamic_cast in Jafg.
//  * Rtti is disabled, and using dynamic_cast will always return nullptr or cause an abnormal program termination.
//  */
// #define dynamic_cast UNAVAILABLE_DYNAMIC_CAST
//
