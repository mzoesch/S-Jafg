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
// Build "platforms".
#ifndef AS_CLIENT
    #define AS_CLIENT               0
#endif /* !AS_CLIENT */
#ifndef AS_SERVER
    #define AS_SERVER               0
#endif /* !AS_SERVER */

#if !(AS_CLIENT || AS_SERVER)
    #error "No build platform specified."
#endif /* !(AS_CLIENT || AS_SERVER) */
#if AS_CLIENT
    #if AS_SERVER
        #error "Multiple build platforms specified."
    #endif /* AS_SERVER */
#endif /* AS_CLIENT */
#if AS_SERVER
    #if AS_CLIENT
        #error "Multiple build platforms specified."
    #endif /* AS_CLIENT */
#endif /* AS_SERVER */


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

/**
 * Checks are only executed in development configurations unless overridden in manual build.
 */
#if DO_EVER_CHECKS
    #if IN_SHIPPING
        #define DO_CHECKS               DO_CHECKS_IN_SHIPPING
    #else /* IN_SHIPPING */
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
