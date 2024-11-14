// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_WINDOWS
    #error "Wanted to override generic platform types with windows specific types, but platform is not windows."
#endif /* !PLATFORM_WINDOWS */

#ifndef _MSC_VER
    #error "Compiling windows platform specific code with non-msvc compiler."
#else /* !_MSC_VER */
    #if _MSC_VER < 1930
        #error "Program requires at least verion \"Visual Studio 2022 RTW 17.0\" of the MSVC compiler."
    #endif /* _MSC_VER < 1930 */
#endif /* _MSC_VER */

#ifndef __cplusplus
    #error "No cpp standard specified."
#else /* !__cplusplus */
    #if __cplusplus == 199711L
        #error "Mvsc command line build flag was not set. Missing \"/Zc:__cplusplus\" to handle cpp verion ctrl correctly."
    #else /* __cplusplus == 199711L */
        #if __cplusplus < 202002L
            #error "Program requires at least C++20."
        #endif /* __cplusplus < 202002L */
    #endif /* __cplusplus != 199711L */
#endif /* __cplusplus */

/* Why the duck does this even exists. This destroys so many normal functions - wtf?? */
#define NOMINMAX
#include <Windows.h>

///////////////////////////////////////////////////////////////////////////////
// Compiler config

/**
 * Warning C4002 (compiler warning level 1)   ==>   Raise to error:
 * too many arguments for function-like macro invocation 'identifier'
 *
 * https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4002?view=msvc-170
 */
#pragma warning(error: 4002)

/**
 * Warning C4003 (compiler warning level 1)   ===>   Raise to error:
 * not enough arguments for function-like macro invocation 'identifier'
 *
 * https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4003?view=msvc-170
 */
#pragma warning(error: 4003)

/**
 * Warning C4172 (compiler warning level 1)   ===>   Raise to error:
 * returning address of local variable or temporary: function.
 *
 * https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4172?view=msvc-170
 */
#pragma warning(error : 4172)

/**
 * Warning C4251 (compiler warning level 2):
 * 'type' : class 'type1' needs to have dll-interface to be used by clients of class 'type2'.
 *
 * https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4251?view=msvc-170
 */
#pragma warning(disable : 4251)

/**
 * Warning C4553 (compiler warning level 1)   ===>   Raise to error:
 * 'operator' : operator has no effect; did you intend 'operator'?
 *
 * https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4553?view=msvc-170
 */
#pragma warning(error : 4553)

// ~Compiler config
///////////////////////////////////////////////////////////////////////////////

/*
 * We forward declare this shit, as this file should only be included implicitly by including Platform.h
 */
struct LGenericPlatformTypes;

struct  LWinPlatformTypes;
struct  LWinPlatformBreakDefines;
typedef LWinPlatformTypes LPlatformTypes;

struct LWinPlatformTypes final : public LGenericPlatformTypes
{
};

#ifdef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is already defined."
#endif /* PLATFORM_MAX_PATH */
#define PLATFORM_MAX_PATH \
    MAX_PATH

#include <intrin.h>
#define PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH() \
    __nop()

/**
 * Platform break. Should just break the debugger if attached and pause the program. It must allow for continuing.
 * If no debugger is attached, the behavior is undefined.
 */
#ifdef PLATFORM_BREAK
    #error "PLATFORM_BREAK is already defined."
#endif /* PLATFORM_BREAK */
#define PLATFORM_BREAK()                                      \
    {{                                                        \
        {                                                     \
            LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()        \
        }                                                     \
        (                                                     \
            PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH(), \
            __debugbreak(),                                   \
            PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH()  \
        )                                                     \
        ;                                                     \
    }}

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
#define PLATFORM_ERROR_BREAK_WITH_BODY(InMessage, InFile, InLine)                                     \
    {{                                                                                                \
        {                                                                                             \
            LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()                                                \
        }                                                                                             \
        const std::string  ___InMessage     = InMessage;                                              \
        const std::string  ___InFile        = InFile;                                                 \
        const std::wstring ___InMessageWide = std::wstring(___InMessage.begin(), ___InMessage.end()); \
        const std::wstring ___InFileWide    = std::wstring(___InFile.begin(),    ___InFile.end()   ); \
        const int32        ___InLine        = InLine;                                                 \
        {                                                                                             \
            (void)                                                                                    \
            (                                                                                         \
            (                                                                                         \
                _wassert(___InMessageWide.c_str(), ___InFileWide.c_str(), ___InLine), 0               \
            )                                                                                         \
            )                                                                                         \
            ;                                                                                         \
        }                                                                                             \
    }}

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
    #define PLATFORM_PANIC_BREAK_WITH_BODY(InMessage, InFile, InLine)        \
        LWinPlatformBreakDefines::OnProgramPanic(InMessage, InFile, InLine);

#define PLATFORM_CALLSPEC_OUT           __declspec ( dllexport )
#define PLATFORM_CALLSPEC_IN            __declspec ( dllimport )
#define PLATFORM_EXTERNSPEC_OUT
#define PLATFORM_EXTERNSPEC_IN          extern

/**
 * https://learn.microsoft.com/en-us/cpp/mfc/windows-sockets-byte-ordering?view=msvc-170
 */
#define PLATFORM_USES_LITTLE_ENDIAN     1

struct LWinPlatformBreakDefines final
{
    NORETURN NOINLINE
    static void OnProgramPanic(const std::string& InMessage, const std::string& InFile, const int32_t InLine);
};
