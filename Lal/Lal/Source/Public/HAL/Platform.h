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
    Branch prediction hints.
-----------------------------------------------------------------------------*/

#define UNLIKELY(Expr)              (!!(Expr))
#define LIKELY(Expr)                (!!(Expr))


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

typedef LPlatformTypes::uint8         uint8;
typedef LPlatformTypes::uint16        uint16;
typedef LPlatformTypes::uint32        uint32;
typedef LPlatformTypes::uint64        uint64;

typedef LPlatformTypes::int8          int8;
typedef LPlatformTypes::int16         int16;
typedef LPlatformTypes::int32         int32;
typedef LPlatformTypes::int64         int64;

typedef LPlatformTypes::LChar         LChar;

typedef LPlatformTypes::LStringLegacy LStringLegacy;
typedef LPlatformTypes::LWideString   LWideString;

typedef LPlatformTypes::LMaxAlign     LMaxAlign;
typedef LPlatformTypes::LNullptrTy    LNullptrTy;


/*-----------------------------------------------------------------------------
    Statically assert that the necessary platform-specific macros are defined.
-----------------------------------------------------------------------------*/

static_assert(sizeof(uint8)   == 1, "uint8  is not 1 byte.");
static_assert(sizeof(uint16)  == 2, "uint16 is not 2 bytes.");
static_assert(sizeof(uint32)  == 4, "uint32 is not 4 bytes.");
static_assert(sizeof(uint64)  == 8, "uint64 is not 8 bytes.");
static_assert(sizeof(int8)   == 1,  "int8   is not 1 byte.");
static_assert(sizeof(int16)  == 2,  "int16  is not 2 bytes.");
static_assert(sizeof(int32)  == 4,  "int32  is not 4 bytes.");
static_assert(sizeof(int64)  == 8,  "int64  is not 8 bytes.");
static_assert(sizeof(LChar)  == 1,  "LChar  is not 1 byte.");

#ifndef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is not defined."
#endif /* !PLATFORM_MAX_PATH */

#ifndef PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH
    #error "PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH is not defined."
#endif /* !PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH */

/**
 * Platform break. Should just break the debugger if attached and pause the program. It must allow for continuing.
 * If no debugger is attached, the behavior is undefined.
 */
#ifndef PLATFORM_BREAK
    #error "PLATFORM_BREAK is not defined."
#endif /* !PLATFORM_BREAK */

/**
 * Platform error break. This signals the user via a platform-specific pop-up window that an error occurred and the
 * program panicked. Resuming the program will not be possible except when an attached debugger is present.
 */
#ifndef PLATFORM_ERROR_BREAK
    #error "PLATFORM_ERROR_BREAK is not defined."
#endif /* !PLATFORM_ERROR_BREAK */
#ifndef PLATFORM_ERROR_BREAK_WITH_BODY
    #error "PLATFORM_ERROR_BREAK_WITH_BODY is not defined."
#endif /* !PLATFORM_ERROR_BREAK_WITH_BODY */

/**
 * Platform panic break. This signals the user via a platform-specific pop-up window that a fatal error occurred
 * and the program panicked. Resuming the program will not be possible.
 * This will only be called in shipping builds. This macro will be replaced in development build with
 * PLATFORM_ERROR_BREAK.
 */
#ifndef PLATFORM_PANIC_BREAK
    #error "PLATFORM_PANIC_BREAK is not defined."
#endif /* !PLATFORM_PANIC_BREAK */
#ifndef PLATFORM_PANIC_BREAK_WITH_BODY
    #error "PLATFORM_PANIC_BREAK_WITH_BODY is not defined."
#endif /* !PLATFORM_PANIC_BREAK_WITH_BODY */

#ifndef PLATFORM_CALLSPEC_IN
    #error "PLATFORM_CALLSPEC_IN is not defined."
#endif /* !PLATFORM_CALLSPEC_IN */
#ifndef PLATFORM_CALLSPEC_OUT
    #error "PLATFORM_CALLSPEC_OUT is not defined."
#endif /* !PLATFORM_CALLSPEC_OUT */

#ifndef PLATFORM_EXTERNSPEC_IN
    #error "PLATFORM_EXTERNSPEC_IN is not defined."
#endif /* !PLATFORM_EXTERNSPEC_IN */
#ifndef PLATFORM_EXTERNSPEC_OUT
    #error "PLATFORM_EXTERNSPEC_OUT is not defined."
#endif /* !PLATFORM_EXTERNSPEC_OUT */

/*
 * There is a way to confirm this at compile time. We should to
 * a test for this.
 */
#ifndef PLATFORM_USES_LITTLE_ENDIAN
    #error "PLATFORM_USES_LITTLE_ENDIAN is not defined."
#endif /* !PLATFORM_USES_LITTLE_ENDIAN */
/* Implicitly define opposite. */
#define PLATFORM_USES_BIG_ENDIAN        ( !PLATFORM_USES_LITTLE_ENDIAN )
