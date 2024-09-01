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


/*-----------------------------------------------------------------------------
    Development macros.
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
    Implicitly generated macros.
-----------------------------------------------------------------------------*/

#if PLATFORM_WINDOWS
    #define PLATFORM_DESKTOP       1                                        // NOLINT(modernize-macro-to-enum)
#else /* !PLATFORM_WINDOWS */
    #define PLATFORM_DESKTOP       0
#endif /* PLATFORM_WINDOWS */


/*-----------------------------------------------------------------------------
    Manual build override settings.
-----------------------------------------------------------------------------*/

#define INCLUDED_AUTOMATIC_BUILD_PRIVATE

#ifndef REFLECT_MANUAL_BUILD
    #define REFLECT_MANUAL_BUILD_OVERRIDES 1
#endif /* !REFLECT_MANUAL_BUILD */

#if REFLECT_MANUAL_BUILD_OVERRIDES
    /* Override with manual build settings. */
    #include "Build/ManualBuildOverride.h"
#endif /* REFLECT_MANUAL_BUILD */
