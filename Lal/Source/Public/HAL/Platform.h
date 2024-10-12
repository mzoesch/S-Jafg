// Copyright mzoesch. All rights reserved.

#pragma once

#include "HAL/GenericPlatformTypes.h"


/*-----------------------------------------------------------------------------
    Define supported platforms.
-----------------------------------------------------------------------------*/

#ifndef PLATFORM_WINDOWS
    #define PLATFORM_WINDOWS        0
#endif


/*-----------------------------------------------------------------------------
    Generic platform macros.
-----------------------------------------------------------------------------*/

#define UNLIKELY(expr)              (!!(expr))


/*-----------------------------------------------------------------------------
    Let platform override platform based macros and types.
-----------------------------------------------------------------------------*/

#if PLATFORM_WINDOWS
    #include "HAL/PlatformWin.h"
#else /* PLATFORM_WINDOWS */
    /* Add more platform types here in the future... */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_WINDOWS */


/*-----------------------------------------------------------------------------
    Implicitly generated macros.
-----------------------------------------------------------------------------*/

#if PLATFORM_WINDOWS
    #define PLATFORM_DESKTOP        1
#else /* !PLATFORM_WINDOWS */
    #define PLATFORM_DESKTOP        0
#endif /* PLATFORM_WINDOWS */


/*-----------------------------------------------------------------------------
    Generalized types based on a specific platform.
-----------------------------------------------------------------------------*/

typedef LPlatformTypes::uint8        uint8;
typedef LPlatformTypes::uint16       uint16;
typedef LPlatformTypes::uint32       uint32;
typedef LPlatformTypes::uint64       uint64;

typedef LPlatformTypes::int8         int8;
typedef LPlatformTypes::int16        int16;
typedef LPlatformTypes::int32        int32;
typedef LPlatformTypes::int64        int64;

typedef LPlatformTypes::LChar        LChar;

typedef LPlatformTypes::LString      LString;
typedef LPlatformTypes::LWideString  LWideString;


/*-----------------------------------------------------------------------------
    Statically assert that the necessary platform-specific macros are defined.
-----------------------------------------------------------------------------*/

#ifndef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is not defined."
#endif /* !PLATFORM_MAX_PATH */

#ifndef PLATFORM_BREAK
    #error "PLATFORM_BREAK is not defined."
#endif /* !PLATFORM_BREAK */

#ifndef PLATFORM_CALLSPEC_IN
    #error "PLATFORM_CALLSPEC_IN is not defined."
#endif /* !PLATFORM_CALLSPEC_IN */
#ifndef PLATFORM_CALLSPEC_OUT
    #error "PLATFORM_CALLSPEC_OUT is not defined."
#endif /* !PLATFORM_CALLSPEC_OUT */
