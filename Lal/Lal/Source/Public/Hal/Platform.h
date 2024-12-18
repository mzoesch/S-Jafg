// Copyright mzoesch. All rights reserved.

#pragma once

#include "Hal/GenericPlatformTypes.h"


/*-----------------------------------------------------------------------------
    Define supported platforms.
-----------------------------------------------------------------------------*/

#ifndef PLATFORM_WINDOWS
    #define PLATFORM_WINDOWS        0
#endif /* !PLATFORM_WINDOWS */
#ifndef PLATFORM_WASM
    #define PLATFORM_WASM           0
#endif /* !PLATFORM_WASM */


/*-----------------------------------------------------------------------------
    Branch prediction hints.
-----------------------------------------------------------------------------*/

#define UNLIKELY(Expr)              (!!(Expr))
#define LIKELY(Expr)                (!!(Expr))


/*-----------------------------------------------------------------------------
    Let platform override platform based macros and types.
-----------------------------------------------------------------------------*/

#if PLATFORM_WINDOWS
    #include "Hal/PlatformWin.h"
#elif PLATFORM_WASM
    #include "Hal/PlatformWasm.h"
#else /* PLATFORM_WASM */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_WASM */


/*-----------------------------------------------------------------------------
    Implicitly generated macros.
-----------------------------------------------------------------------------*/

#if PLATFORM_WINDOWS
    #define PLATFORM_DESKTOP        1
#else /* !PLATFORM_WINDOWS */
    #define PLATFORM_DESKTOP        0
#endif /* PLATFORM_WINDOWS */

#ifndef PLATFORM_WINDOWS_WITH_GNU
    #define PLATFORM_WINDOWS_WITH_GNU       0
#endif /* !PLATFORM_WINDOWS_WITH_GNU */
#ifndef PLATFORM_WINDOWS_WITH_MSVC
    #define PLATFORM_WINDOWS_WITH_MSVC      0
#endif /* !PLATFORM_WINDOWS_WITH_MSVC */
#ifndef WITH_GNU
    #define WITH_GNU        0
#endif /* !WITH_GNU */
#ifndef WITH_MSVC
    #define WITH_MSVC       0
#endif /* !WITH_MSVC */

/**
 * Whether to create a virtual filesystem for the platform at compile time.
 * @see System/VFilesystem.h
 */
#ifndef WITH_VIRTUAL_FILESYSTEM
    #define WITH_VIRTUAL_FILESYSTEM                 0
#endif /* !WITH_VIRTUAL_FILESYSTEM */

/**
 * Whether to use glad.
 */
#ifndef JAFG_NO_GLAD
    #define JAFG_NO_GLAD                            0
#endif /* !JAFG_NO_GLAD */

/**
 * Whether to use glfw3.
 */
#ifndef JAFG_NO_GLFW3
    #define JAFG_NO_GLFW3                           0
#endif /* !JAFG_NO_GLFW3 */

/**
 * If no, the platform will compile all libraries as static libraries.
 * This will not allow for dynamic linking at runtime, and therefore all plugins must be present at compile time.
 */
#ifndef PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #define PLATFORM_SUPPORTS_SHARED_LIBRARIES      0
#endif /* !PLATFORM_SUPPORTS_SHARED_LIBRARIES */

/**
 * Whether this platforms standard output buffer stream supports flushing.
 */
#ifndef PLATFORM_SUPPORTS_STD_FLUSH
    #define PLATFORM_SUPPORTS_STD_FLUSH             0
#endif /* !PLATFORM_SUPPORTS_STD_FLUSH */

/**
 * Whether the standard output buffer stream supports ANSI escape codes.
 */
#ifndef PLATFORM_SUPPORTS_ANSI_ESCAPES
    #define PLATFORM_SUPPORTS_ANSI_ESCAPES          0
#endif /* !PLATFORM_SUPPORTS_ANSI_ESCAPES */

#ifndef PLATFORM_SUPPORTS_SIMD
    #define PLATFORM_SUPPORTS_SIMD                  0
#endif /* !PLATFORM_SUPPORTS_SIMD */

/**
 * Whether allocated memory can be shrinked by platform intrinsics or memory shrinking is performed by allocating a
 * new smaller bulk of memory, copying the data and freeing the old memory.
 */
#ifndef PLATFORM_SUPPORTS_MEMORY_SHRINK
    #define PLATFORM_SUPPORTS_MEMORY_SHRINK         0
#endif /* !PLATFORM_SUPPORTS_MEMORY_SHRINK */

#ifndef PLATFORM_USES_NON_GENERIC_LOOP
    #define PLATFORM_USES_NON_GENERIC_LOOP          0
#endif /* !PLATFORM_USES_NON_GENERIC_LOOP */
#if PLATFORM_USES_NON_GENERIC_LOOP
    #ifndef PLATFORM_GUARDED_LOOP
        #error "PLATFORM_GUARDED_LOOP is not defined."
    #endif /* !PLATFORM_GUARDED_LOOP */
#else /* PLATFORM_USES_NON_GENERIC_LOOP */
    #ifdef PLATFORM_GUARDED_LOOP
        #error "PLATFORM_GUARDED_LOOP is defined but platform does use the generic platform-agnostic loop."
    #endif /* PLATFORM_GUARDED_LOOP */
    #define PLATFORM_GUARDED_LOOP
#endif /* !PLATFORM_USES_NON_GENERIC_LOOP */
#ifndef PLATFORM_USES_NON_GENERIC_EXIT
    #define PLATFORM_USES_NON_GENERIC_EXIT          0
#endif /* !PLATFORM_USES_NON_GENERIC_EXIT */

#ifndef PLATFORM_USES_UTF8
    #define PLATFORM_USES_UTF8          0
#endif /* !PLATFORM_USES_UTF8 */
#ifndef PLATFORM_USES_UTF16
    #define PLATFORM_USES_UTF16         0
#endif /* !PLATFORM_USES_UTF16 */
#ifndef PLATFORM_USES_UTF32
    #define PLATFORM_USES_UTF32         0
#endif /* !PLATFORM_USES_UTF32 */
#if PLATFORM_USES_UTF8
    #if PLATFORM_USES_UTF16
        #error "PLATFORM_USES_UTF8 and PLATFORM_USES_UTF16 are both defined."
    #endif /* PLATFORM_USES_UTF16 */
    #if PLATFORM_USES_UTF32
        #error "PLATFORM_USES_UTF8 and PLATFORM_USES_UTF32 are both defined."
    #endif /* PLATFORM_USES_UTF32 */
#endif /* PLATFORM_USES_UTF8 */
#if PLATFORM_USES_UTF16
    #if PLATFORM_USES_UTF8
        #error "PLATFORM_USES_UTF16 and PLATFORM_USES_UTF8 are both defined."
    #endif /* PLATFORM_USES_UTF8 */
    #if PLATFORM_USES_UTF32
        #error "PLATFORM_USES_UTF16 and PLATFORM_USES_UTF32 are both defined."
    #endif /* PLATFORM_USES_UTF32 */
#endif /* PLATFORM_USES_UTF16 */
#if PLATFORM_USES_UTF32
    #if PLATFORM_USES_UTF8
        #error "PLATFORM_USES_UTF32 and PLATFORM_USES_UTF8 are both defined."
    #endif /* PLATFORM_USES_UTF8 */
    #if PLATFORM_USES_UTF16
        #error "PLATFORM_USES_UTF32 and PLATFORM_USES_UTF16 are both defined."
    #endif /* PLATFORM_USES_UTF16 */
#endif /* PLATFORM_USES_UTF32 */
#if !PLATFORM_USES_UTF8 && !PLATFORM_USES_UTF16 && !PLATFORM_USES_UTF32
    #error "No platform encoding is defined."
#endif /* !PLATFORM_USES_UTF8 && !PLATFORM_USES_UTF16 && !PLATFORM_USES_UTF32 */


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

typedef LPlatformTypes::LAsciiChar    LAsciiChar;
typedef LPlatformTypes::LWideChar     LWideChar;
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
static_assert(sizeof(int8)    == 1, "int8   is not 1 byte.");
static_assert(sizeof(int16)   == 2, "int16  is not 2 bytes.");
static_assert(sizeof(int32)   == 4, "int32  is not 4 bytes.");
static_assert(sizeof(int64)   == 8, "int64  is not 8 bytes.");
static_assert(sizeof(LChar)   == 1, "LChar  is not 1 byte.");

#if PLATFORM_USES_64_BIT
    static_assert(sizeof(uint64) == sizeof(void*), "uint64 is not the same size as a pointer.");
    static_assert(sizeof(int64)  == sizeof(void*), "int64  is not the same size as a pointer.");
    #ifdef PLATFORM_USES_32_BIT
        #error "PLATFORM_USES_32_BIT and PLATFORM_USES_64_BIT are both defined."
    #endif /* PLATFORM_USES_32_BIT */
    #define PLATFORM_USES_32_BIT            0
#endif /* PLATFORM_USES_64_BIT */
#if PLATFORM_USES_32_BIT
    static_assert(sizeof(uint32) == sizeof(void*), "uint32 is not the same size as a pointer.");
    static_assert(sizeof(int32)  == sizeof(void*), "int32  is not the same size as a pointer.");
    #ifdef PLATFORM_USES_64_BIT
        #error "PLATFORM_USES_32_BIT and PLATFORM_USES_64_BIT are both defined."
    #endif /* PLATFORM_USES_64_BIT */
    #define PLATFORM_USES_64_BIT            0
#endif /* PLATFORM_USES_32_BIT */

#ifndef NOINLINE
    #error "NOINLINE is not defined for this platform."
#endif /* !NOINLINE */

#ifndef FORCEINLINE
    #error "FORCEINLINE is not defined for this platform."
#endif /* !FORCEINLINE */

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

#ifndef LITERAL_WIDE
    #error "LITERAL_WIDE is not defined."
#endif /* !LITERAL_WIDE */
#ifndef LITERAL_UTF8
    #error "LITERAL_UTF8 is not defined."
#endif /* !LITERAL_UTF8 */
#ifndef LITERAL_UTF16
    #error "LITERAL_UTF16 is not defined."
#endif /* !LITERAL_UTF16 */
#ifndef LITERAL_UTF32
    #error "LITERAL_UTF32 is not defined."
#endif /* !LITERAL_UTF32 */

namespace PlatformHal
{

/**
 * Very dangerous function. Use with care and never in critical code paths.
 * Currently not supported for all platforms.
 */
void Sleep(const double InSeconds);

} /* ~Namespace PlatformHal */
