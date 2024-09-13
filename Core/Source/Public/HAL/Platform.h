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

typedef PlatformTypes::uint8        uint8;
typedef PlatformTypes::uint16       uint16;
typedef PlatformTypes::uint32       uint32;
typedef PlatformTypes::uint64       uint64;

typedef PlatformTypes::int8         int8;
typedef PlatformTypes::int16        int16;
typedef PlatformTypes::int32        int32;
typedef PlatformTypes::int64        int64;

typedef PlatformTypes::TChar        TChar;

typedef PlatformTypes::String       String;
typedef PlatformTypes::WideString   WideString;


/*-----------------------------------------------------------------------------
    Statically assert that necessary platform-specific macros are defined.
-----------------------------------------------------------------------------*/

#ifndef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is not defined."
#endif /* !PLATFORM_MAX_PATH */

#ifndef PLATFORM_BREAK
    #error "PLATFORM_BREAK is not defined."
#endif /* !PLATFORM_BREAK */
