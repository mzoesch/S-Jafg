// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_WINDOWS
    #error "Wanted to override generic platform types with windows specific types, but platform is not windows."
#endif /* !PLATFORM_WINDOWS */

#ifdef PLATFORM_WINDOWS_WITH_GNU
    #ifdef PLATFORM_WINDOWS_WITH_MSVC
        #error "Both PLATFORM_WINDOWS_WITH_GNU and PLATFORM_WINDOWS_WITH_MSVC are defined."
    #endif /* PLATFORM_WINDOWS_WITH_MSVC */
    #ifdef _MSC_VER
        #error "Compiling with gnu but _MSC_VER is defined."
    #endif /* _MSC_VER */
    // #warning "Compiling windows target with the GNU toolchain. This is allowed but not recommended."
#endif /* PLATFORM_WINDOWS_WITH_GNU */
#ifdef PLATFORM_WINDOWS_WITH_MSVC
    #ifdef PLATFORM_WINDOWS_WITH_GNU
        #error "Both PLATFORM_WINDOWS_WITH_GNU and PLATFORM_WINDOWS_WITH_MSVC are defined."
    #endif /* PLATFORM_WINDOWS_WITH_GNU */
    #if _MSC_VER < 1930
        #error "Program requires at least verion \"Visual Studio 2022 RTW 17.0\" of the MSVC compiler."
    #endif /* _MSC_VER < 1930 */
#endif /* PLATFORM_WINDOWS_WITH_MSVC */

#if PLATFORM_WINDOWS_WITH_GNU
    #define PLATFORM_WINDOWS_WITH_MSVC      0
    #define WITH_GNU                        1
    #define WITH_MSVC                       0
#endif /* PLATFORM_WINDOWS_WITH_GNU */
#if PLATFORM_WINDOWS_WITH_MSVC
    #define PLATFORM_WINDOWS_WITH_GNU       0
    #define WITH_GNU                        0
    #define WITH_MSVC                       1
#endif /* PLATFORM_WINDOWS_WITH_MSVC */

#ifndef __cplusplus
    #error "No cpp standard specified."
#else /* !__cplusplus */
    #if __cplusplus == 199711L
        #if PLATFORM_WINDOWS_WITH_MSVC
            #error "Mvsc command line build flag was not set. Missing \"/Zc:__cplusplus\" to handle cpp verion ctrl correctly."
        #else /* PLATFORM_WINDOWS_WITH_MSVC */
            #error "Program requires at least C++20."
        #endif /* !PLATFORM_WINDOWS_WITH_MSVC */
    #else /* __cplusplus == 199711L */
        #if __cplusplus < 202002L
            #error "Program requires at least C++20."
        #endif /* __cplusplus < 202002L */
    #endif /* __cplusplus != 199711L */
#endif /* __cplusplus */

#ifndef _MSVC_LANG
    #error "_MSVC_LANG is not defined."
#endif /* !_MSVC_LANG */

///////////////////////////////////////////////////////////////////////////////
// Compiler config

/**
 * Do not warn about misuses of pragmas, such as incorrect parameters, invalid syntax, or conflicts between pragmas.
 * See also -Wunknown-pragmas.
 *
 * https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wpragmas
 */
#if PLATFORM_WINDOWS_WITH_GNU
    #pragma GCC diagnostic error "-Wpragmas"
#endif /* PLATFORM_WINDOWS_WITH_GNU */

/**
 * Warn when a #pragma directive is encountered that is not understood by GCC.
 * If this command-line option is used, warnings are even issued for unknown pragmas in system header files.
 * This is not the case if the warnings are only enabled by the -Wall command-line option.
 *
 * https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunknown-pragmas
 */
#if PLATFORM_WINDOWS_WITH_GNU
    #pragma GCC diagnostic error "-Wunknown-pragmas"
#endif /* PLATFORM_WINDOWS_WITH_GNU */

#if PLATFORM_WINDOWS_WITH_MSVC
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
     * Warning C4005 (compiler warning level 1)   ===>   Raise to error:
     * 'identifier' : macro redefinition
     *
     * https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4005?view=msvc-170
     */
    #pragma warning(error: 4005)

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
#endif /* PLATFORM_WINDOWS_WITH_MSVC */

#if PLATFORM_WINDOWS_WITH_GNU
    // #pragma GCC diagnostic ignored "-Wno-gnu-anonymous-struct" // <--- Currently not using pedantic.

    #pragma GCC diagnostic error "-Wbuiltin-macro-redefined"

    /**
     * C++20 std change: A simple-template-id is no longer valid as the declarator-id of a constructor or destructor.
     * http://eel.is/c++draft/diff.cpp17.class#2
     */
    #pragma GCC diagnostic error "-Wtemplate-id-cdtor"

    #pragma GCC diagnostic error "-Winconsistent-missing-override"

    /**
     * Warn whenever a local variable is assigned to, but otherwise unused (aside from its declaration).
     * This warning is enabled by -Wall.
     * To suppress this warning use the unused attribute (see Specifying Attributes of Variables).
     * This warning is also enabled by -Wunused, which is enabled by -Wall.
     *
     * https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-but-set-variable
     */
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"

    /**
     * Warn whenever a static function is declared but not defined or a non-inline static function is unused.
     * This warning is enabled by -Wall.
     *
     * https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-function
     */
    #pragma GCC diagnostic ignored "-Wunused-function"

    /**
     * Warn whenever a function parameter is unused aside from its declaration.
     * This option is not enabled by -Wunused unless -Wextra is also specified.
     *
     * https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-parameter
     */
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    /**
     * Warn whenever a local or static variable is unused aside from its declaration. This option implies
     * -Wunused-const-variable=1 for C, but not for C++. This warning is enabled by -Wall.
     *
     * https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-variable
     */
    #pragma GCC diagnostic ignored "-Wunused-variable"

    /**
     * Warn whenever a comment-start sequence '/ *' appears in a '/ *' comment, or whenever a backslash-newline
     * appears in a '//' comment. This warning is enabled by -Wall.
     * https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wcomment
     */
    #pragma GCC diagnostic ignored "-Wcomment"
    #pragma GCC diagnostic ignored "-Wcomments"

    /**
     * Warn if a structure’s initializer has some fields missing. In C this option does not warn about designated
     * initializers. In C++ this option does not warn about the empty { } initializer.
     * This warning is included in -Wextra. To get other -Wextra warnings without this one, use -Wextra
     * -Wno-missing-field-initializers.
     *
     * https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wmissing-field-initializers
     */
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif /* PLATFORM_WINDOWS_WITH_GNU */

// ~Compiler config
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Compiler dependent features

#ifndef NOINLINE
    #define NOINLINE            __declspec(noinline)
#endif /* !NOINLINE */

#ifdef FORCEINLINE
    #undef FORCEINLINE
#endif /* FORCEINLINE */
#if IN_DEBUG
    /*
     * Inlining is disabled in debug builds as following the debugger through inlined code is a pain
     * in the ass.
     */
    #define FORCEINLINE inline /* Inlining will most likely be ignored in debug builds by the current compiler. */
#else /* IN_DEBUG */
    #if PLATFORM_WINDOWS_WITH_MSVC
        #define FORCEINLINE _forceinline
    #elif PLATFORM_WINDOWS_WITH_GNU
        #define FORCEINLINE __attribute__((always_inline))
    #endif /* PLATFORM_WINDOWS_WITH_GNU */
#endif /* !IN_DEBUG */

// ~Compiler dependent features
///////////////////////////////////////////////////////////////////////////////

/* Why the duck does this even exists. This destroys so many normal functions - wtf?? */
#ifdef NOMINMAX
    #undef NOMINMAX
#endif /* NOMINMAX */
#define NOMINMAX        1 // <--- Always block. Jafg will not compile otherwise.

/**
 * https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
 */
#if PLATFORM_WINDOWS_WITH_GNU
    #ifdef WINVER
        #undef WINVER
    #endif /* WINVER */
    #ifdef _WIN32_WINNT
        #undef _WIN32_WINNT /* @see _mingw.h <--- Wants to compile with Windows Vista??? Bro. Shame. */
    #endif /* _WIN32_WINNT */
#endif /* PLATFORM_WINDOWS_WITH_GNU */
#if !PLATFORM_WINDOWS_WITH_MSVC
    /*
     * Defines it ourselves - we do not need backwards compatibility for systems prior to Windows 10.
     */
    #define WINVER          0x0A00  // 0x0400 <=> Windows NT 4.0 ;;; 0x0500 <=> Windows 2000 ;;; 0x0501 <=> Windows XP
    #define _WIN32_WINNT    0x0A00  // 0x0502 <=> Windows Server 2003 ;;; 0x0600 <=> Windows Vista
                                    // 0x0600 <=> Windows Server 2008 ;;; 0x0601 <=> Windows 7 ;;; 0x0602 <=> Windows 8
                                    // 0x0603 <=> Windows 8.1 ;; 0x0A00 <=> Windows 10
#endif /* !PLATFORM_WINDOWS_WITH_MSVC */

#include <Windows.h>

#if PLATFORM_WINDOWS_WITH_GNU
    #include <locale>
#endif /* PLATFORM_WINDOWS_WITH_GNU */

/*
 * We forward declare this shit, as this file should only be included implicitly by including Platform.h
 */
struct LGenericPlatformTypes;

struct  LWinPlatformTypes;
struct  LWinPlatformBreakDefines;
typedef LWinPlatformTypes LPlatformTypes;

struct LWinPlatformTypes final : public LGenericPlatformTypes
{
    static LStringLegacy Ws2S(const LWideString& Ws)
    {
#if PLATFORM_WINDOWS_WITH_MSVC
        __pragma( warning(push) )
        __pragma( warning(disable: 4996) )
#endif /* PLATFORM_WINDOWS_WITH_MSVC */
        typedef std::codecvt_utf8<wchar_t> TypeX;
        std::wstring_convert<TypeX, wchar_t> Converter;
        return Converter.to_bytes(Ws);
#if PLATFORM_WINDOWS_WITH_MSVC
        __pragma( warning(pop) )
#endif /* PLATFORM_WINDOWS_WITH_MSVC */
    }

    static const LChar* Ws2CStr(const LWideString& Ws, LStringLegacy& OutContainer)
    {
        OutContainer = Ws2S(Ws);
        return OutContainer.c_str();
    }

    static std::wstring CStr2Ws(const char* Cs)
    {
#if PLATFORM_WINDOWS_WITH_MSVC
        __pragma( warning(push) )
        __pragma( warning(disable: 4996) )
#endif /* PLATFORM_WINDOWS_WITH_MSVC */
        typedef std::codecvt_utf8<wchar_t> TypeX;
        std::wstring_convert<TypeX, wchar_t> Converter;
        return Converter.from_bytes(Cs);
#if PLATFORM_WINDOWS_WITH_MSVC
        __pragma( warning(pop) )
#endif /* PLATFORM_WINDOWS_WITH_MSVC */
    }
};

#ifdef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is already defined."
#endif /* PLATFORM_MAX_PATH */
#define PLATFORM_MAX_PATH                       MAX_PATH

#ifdef PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #error "PLATFORM_SUPPORTS_SHARED_LIBRARIES is already defined."
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
#define PLATFORM_SUPPORTS_SHARED_LIBRARIES      1

#ifdef PLATFORM_SUPPORTS_STD_FLUSH
    #error "PLATFORM_SUPPORTS_STD_FLUSH is already defined."
#endif /* PLATFORM_SUPPORTS_STD_FLUSH */
#define PLATFORM_SUPPORTS_STD_FLUSH             1

#ifdef PLATFORM_SUPPORTS_ANSI_ESCAPES
    #error "PLATFORM_SUPPORTS_ANSI_ESCAPES is already defined."
#endif /* PLATFORM_SUPPORTS_ANSI_ESCAPES */
#define PLATFORM_SUPPORTS_ANSI_ESCAPES          1

#ifdef PLATFORM_SUPPORTS_SIMD
    #error "PLATFORM_SUPPORTS_SIMD is already defined."
#endif /* PLATFORM_SUPPORTS_SIMD */
#define PLATFORM_SUPPORTS_SIMD                  1

#ifdef PLATFORM_SUPPORTS_MEMORY_SHRINK
    #error "PLATFORM_SUPPORTS_MEMORY_SHRINK is already defined."
#endif /* PLATFORM_SUPPORTS_MEMORY_SHRINK */
#define PLATFORM_SUPPORTS_MEMORY_SHRINK         1

#if PLATFORM_WINDOWS_WITH_MSVC
    #include <intrin.h>
    #define PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH() \
        __nop()
#elif PLATFORM_WINDOWS_WITH_GNU
    #define PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH() \
        __asm__ __volatile__ ("nop")
#endif /* WITH_MSVC */

/**
 * Platform break. Should just break the debugger if attached and pause the program. It must allow for continuing.
 * If no debugger is attached, the behavior is undefined.
 */
#ifdef PLATFORM_BREAK
    #error "PLATFORM_BREAK is already defined."
#endif /* PLATFORM_BREAK */
#if PLATFORM_WINDOWS_WITH_MSVC
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
#elif PLATFORM_WINDOWS_WITH_GNU
    #define PLATFORM_BREAK()                                      \
        {{                                                        \
            {                                                     \
                LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()        \
            }                                                     \
            [] (void) -> void                                     \
            {                                                     \
                PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH(); \
                __debugbreak();                                   \
                PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH(); \
            }();                                                  \
        }}
#endif /* PLATFORM_WINDOWS_WITH_GNU */

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
        const uint32       ___InLine        = InLine;                                                 \
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
#define PLATFORM_USES_64_BIT            1

#if !(_MSVC_LANG >= 201703L)
    #if !defined(PLATFORM_USES_UTF8) || PLATFORM_USES_UTF8 == 0
        #error "PLATFORM_USES_UTF8 is not defined or set to 0. Missing /utf-8 compiler flag."
    #endif /* !PLATFORM_USES_UTF8 */
#else /* !(_MSVC_LANG >= 201703L) */
    #define PLATFORM_USES_UTF8          1
#endif /* _MSVC_LANG >= 201703L */

struct LWinPlatformBreakDefines final
{
    NORETURN NOINLINE
    static void OnProgramPanic(const std::string& InMessage, const std::string& InFile, const LWinPlatformTypes::int32 InLine);
};
