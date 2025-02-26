// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_WASM
    #error "Wanted to override generic platform types with wasm specific types, but platform is not wasm."
#endif /* !PLATFORM_WASM */

#ifndef __EMSCRIPTEN__
    #error "__EMSCRIPTEN__ is not defined."
#endif /* !__EMSCRIPTEN__ */

#ifndef __EMSCRIPTEN_PTHREADS__
    #error "__EMSCRIPTEN_PTHREADS__ is not defined."
#endif /* !__EMSCRIPTEN_PTHREADS__ */

//# For now. But we should make this an option in cmake.
#define PLATFORM_USES_WEBGL_TWO 1

///////////////////////////////////////////////////////////////////////////////
// Compiler dependent features

#ifndef NOINLINE
    #define NOINLINE            __attribute__((noinline))
#endif /* !NOINLINE */

#ifdef FORCEINLINE
    #undef FORCEINLINE
#endif /* FORCEINLINE */
#define FORCEINLINE             inline

// ~Compiler dependent features
///////////////////////////////////////////////////////////////////////////////

#ifndef WITH_GCC
    #define WITH_GCC            1
#endif /* !WITH_GCC */

#pragma GCC diagnostic error "-Wpragmas"
#pragma GCC diagnostic error "-Wunknown-pragmas"
#pragma GCC diagnostic error "-Wbuiltin-macro-redefined"
#pragma GCC diagnostic error "-Wunknown-warning-option"
#pragma GCC diagnostic error "-Winconsistent-missing-override"
#pragma GCC diagnostic error "-Wdynamic-class-memaccess"
#pragma GCC diagnostic error "-Wnonportable-include-path"
#pragma GCC diagnostic error "-Wunused-lambda-capture"
#pragma GCC diagnostic error "-Wreturn-type"
#pragma GCC diagnostic error "-Wmacro-redefined"
#pragma GCC diagnostic error "-Wundefined-inline"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wcomment"
#pragma GCC diagnostic ignored "-Wcomments"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wlogical-op-parentheses"

#include <emscripten/emscripten.h>
#include <emscripten/console.h>
#include <emscripten/html5.h>

struct LGenericPlatformTypes;

struct LWasmPlatformTypes;
typedef LWasmPlatformTypes LPlatformTypes;

struct LWasmPlatformTypes final : public LGenericPlatformTypes
{
};

#ifdef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is already defined."
#endif /* PLATFORM_MAX_PATH */
#define PLATFORM_MAX_PATH                   _MAX_PATH

#define WITH_VIRTUAL_FILESYSTEM             1
#define PLATFORM_USES_JAVA_SCRIPT_FRONTEND  1
#define PLATFORM_WCHAR_SIZE                 4
#define PLATFORM_USES_UTF8                  1

#define JAFG_NO_GLAD                        1 /* Let the compiler handle that - so emscripten. */
#define JAFG_NO_GLFW3                       1

#define PLATFORM_USES_NON_GENERIC_LOOP      1
#define PLATFORM_GUARDED_LOOP                              \
    ::emscripten_set_main_loop(::WasmGuardedLoop, 0, true)
#define PLATFORM_USES_NON_GENERIC_EXIT      1

/**
 * Intrinsic stuff is not really supported in wasm - and for the things that are supported, are,
 * for me, just too much a pain in the ass to implement it.
 */
#define PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH()

/**
 * Platform break. Should just break the debugger if attached and pause the program. It must allow for continuing.
 * If no debugger is attached, the behavior is undefined.
 */
#ifdef PLATFORM_BREAK
    #error "PLATFORM_BREAK is already defined."
#endif /* PLATFORM_BREAK */
#define PLATFORM_BREAK() \
    { emscripten_debugger(); }

/**
 * Platform error break. This signals the user via a platform-specific pop-up window that an error occurred and the
 * program panicked. Resuming the program will not be possible except when an attached debugger is present.
 */
#ifdef PLATFORM_ERROR_BREAK
    #error "PLATFORM_ERROR_BREAK is already definded."
#endif /* PLATFORM_ERROR_BREAK */
#define PLATFORM_ERROR_BREAK(InMessage)                           \
    PLATFORM_ERROR_BREAK_WITH_BODY(InMessage, __FILE__, __LINE__)

#ifdef PLATFORM_ERROR_BREAK_WITH_BODY
    #error "PLATFORM_ERROR_BREAK_WITH_BODY is already definded."
#endif /* PLATFORM_ERROR_BREAK_WITH_BODY */
#define PLATFORM_ERROR_BREAK_WITH_BODY(InMessage, InFile, InLine) \
    { ::emscripten_cancel_main_loop(); assert(false); }

/**
 * Platform panic break. This signals the user via a platform-specific pop-up window that a fatal error occurred
 * and the program panicked. Resuming the program will not be possible.
 * This will only be called in shipping builds. This macro will be replaced in development build with
 * PLATFORM_ERROR_BREAK.
 */
#ifdef PLATFORM_PANIC_BREAK
    #error "PLATFORM_PANIC_BREAK is already definded."
#endif /* PLATFORM_PANIC_BREAK */
#define PLATFORM_PANIC_BREAK(InMessage)                           \
    PLATFORM_PANIC_BREAK_WITH_BODY(InMessage, __FILE__, __LINE__)

#ifdef PLATFORM_PANIC_BREAK_WITH_BODY
    #error "PLATFORM_PANIC_BREAK_WITH_BODY is already definded."
#endif /* PLATFORM_PANIC_BREAK_WITH_BODY */
#define PLATFORM_PANIC_BREAK_WITH_BODY(InMessage, InFile, InLine)         \
    LWasmPlatformBreakDefines::OnProgramPanic(InMessage, InFile, InLine);

/**
 * Disallowed as emscripten does not support shared libraries.
 */
#define PLATFORM_CALLSPEC_OUT
#define PLATFORM_CALLSPEC_IN
#define PLATFORM_EXTERNSPEC_OUT
#define PLATFORM_EXTERNSPEC_IN

#define PLATFORM_USES_LITTLE_ENDIAN     1
#define PLATFORM_USES_32_BIT            1

struct LWasmPlatformBreakDefines final
{
    NORETURN NOINLINE /* Ambiguous symbol error can be ignored. Visual Studio is just too stupid to understand. */
    static void OnProgramPanic(const char* InMessage, const char* InFile, LWasmPlatformTypes::int32 InLine);
};

extern void WasmGuardedLoop();
