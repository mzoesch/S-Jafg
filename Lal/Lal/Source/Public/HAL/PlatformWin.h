// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_WINDOWS
    #error "Wanted to override generic platform types with windows specific types, but platform is not windows."
#endif /* !PLATFORM_WINDOWS */

/* Why the duck does this even exists. This destroys so many normal functions - wtf?? */
#define NOMINMAX
#include <Windows.h>

#include <intrin.h>

/*
 * TODO Hide this in a private translation file.
 *      We want to get rid of all these includes. Including the ones in PartyPCH.h.
 */
#include <string>

/*
 * We forward declare this shit, as this file should only be included implicitly by including Platform.h
 */
struct LGenericPlatformTypes;

struct  LWinPlatformTypes;
typedef LWinPlatformTypes LPlatformTypes;

struct LWinPlatformTypes final : public LGenericPlatformTypes
{
};

struct LWinPlatformBreakDefines final
{
    NOINLINE static void OnProgramPanic(const std::string& InMessage, const std::string& InFile, const int32_t Line)
    {
        const std::string  InCaption     = "Jafg panicked";
        const std::wstring InCaptionWide = std::wstring(InCaption.begin(), InCaption.end());

        std::string InMessageWithAdditionalInfo;
        InMessageWithAdditionalInfo += "Jafg entered an one-way enclosing block inside a critical control path and panicked.\n\n";
        InMessageWithAdditionalInfo += "File: ";
        InMessageWithAdditionalInfo += InFile;
        InMessageWithAdditionalInfo += "\nLine: ";
        InMessageWithAdditionalInfo += std::to_string(Line);
        InMessageWithAdditionalInfo += "\n\nExpression: ";
        InMessageWithAdditionalInfo += InMessage;

        const std::wstring InMessageWide = std::wstring(InMessageWithAdditionalInfo.begin(), InMessageWithAdditionalInfo.end());

        MessageBox(nullptr, InMessageWide.c_str(), InCaptionWide.c_str(), MB_ICONERROR | MB_OK);

        return;
    }
};

#ifdef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is already defined."
#endif /* PLATFORM_MAX_PATH */
#define PLATFORM_MAX_PATH \
    MAX_PATH

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
            (void)(                                                                                   \
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
