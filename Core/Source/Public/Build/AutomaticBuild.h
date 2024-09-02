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
#ifndef AS_DEDICATED_SERVER
    #define AS_DEDICATED_SERVER     0
#endif /* !AS_DEDICATED_SERVER */

#if !(IN_DEBUG || IN_DEVELOPMENT || IN_SHIPPING)
    #error "No build configuration specified."
#endif /* !(IN_DEBUG || IN_DEVELOPMENT || IN_SHIPPING) */


/*-----------------------------------------------------------------------------
    Implicitly generated macros.
-----------------------------------------------------------------------------*/

#if PLATFORM_WINDOWS
    #define PLATFORM_DESKTOP        1
#else /* !PLATFORM_WINDOWS */
    #define PLATFORM_DESKTOP        0
#endif /* PLATFORM_WINDOWS */


/*-----------------------------------------------------------------------------
    Manual build override settings.
-----------------------------------------------------------------------------*/

#define INCLUDED_AUTOMATIC_BUILD_PRIVATE

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

/**
 * Checks are only executed in development configurations unless overridden in manual build.
 */
#if DO_EVER_CHECKS
    #if IN_SHIPPING
        #define DO_CHECKS               DO_CHECKS_IN_SHIPPING
    #else /* !IN_SHIPPING */
        #define DO_CHECKS               1
    #endif /* !IN_SHIPPING */
#else /* DO_EVER_CHECKS */
    #define DO_CHECKS                   0
#endif /* !DO_EVER_CHECKS */

/**
 * Always do assertions (even in SHIPPING) if not overridden in manual build.
 */
#if DO_EVER_ASSERTS
    #if IN_SHIPPING
        #define DO_ASSERTS              !DO_STRIP_ASSERTS_IN_SHIPPING
    #else /* !IN_SHIPPING */
        #define DO_ASSERTS              1
    #endif /* !IN_SHIPPING */
#else /* DO_EVER_ASSERTS */
    #define DO_ASSERTS                  0
#endif /* !DO_EVER_ASSERTS */

/**
 * Only do ensure if checks are enabled.
 */
#define DO_ENSURES                      DO_CHECKS
