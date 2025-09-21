// Copyright mzoesch. All rights reserved.

#pragma once

/*-----------------------------------------------------------------------------
    Validate compiler and forward declare Lal Windows logic.
-----------------------------------------------------------------------------*/

#if !PLATFORM_WINDOWS
    #error "Wanted to override generic platform types with Windows specific types, but platform is not Windows."
#endif /* !PLATFORM_WINDOWS */

#ifdef LAL_PLATFORM_WINDOWS_WITH_GCC
    #ifdef LAL_PLATFORM_WINDOWS_WITH_MSVC
        #error "Both LAL_PLATFORM_WINDOWS_WITH_GCC and LAL_PLATFORM_WINDOWS_WITH_MSVC are defined."
    #endif /* LAL_PLATFORM_WINDOWS_WITH_MSVC */
    #ifdef _MSC_VER
        #if !WITH_IDEA_INTELLISENSE /* We might still wanna use msvc intellisense because it's cool. */
            #error "Compiling with gcc but _MSC_VER is defined."
        #endif /* !WITH_IDEA_INTELLISENSE */
    #endif /* _MSC_VER */
    #warning "Compiling windows target with the GCC toolchain. This is allowed but not recommended."
#endif /* LAL_PLATFORM_WINDOWS_WITH_GCC */
#ifdef LAL_PLATFORM_WINDOWS_WITH_MSVC
    #ifdef LAL_PLATFORM_WINDOWS_WITH_GCC
        #error "Both LAL_PLATFORM_WINDOWS_WITH_GCC and LAL_PLATFORM_WINDOWS_WITH_MSVC are defined."
    #endif /* LAL_PLATFORM_WINDOWS_WITH_GCC */
    #if _MSC_VER < 1930
        #error "Program requires at least verion \"Visual Studio 2022 RTW 17.0\" of the MSVC compiler."
    #endif /* _MSC_VER < 1930 */
#endif /* LAL_PLATFORM_WINDOWS_WITH_MSVC */

#if LAL_PLATFORM_WINDOWS_WITH_GCC
    #define LAL_PLATFORM_WINDOWS_WITH_MSVC                              0
    #define LAL_WITH_GCC                                                1
    #define LAL_WITH_MSVC                                               0
#endif /* LAL_PLATFORM_WINDOWS_WITH_GCC */
#if LAL_PLATFORM_WINDOWS_WITH_MSVC
    #define LAL_PLATFORM_WINDOWS_WITH_GCC                               0
    #define LAL_WITH_GCC                                                0
    #define LAL_WITH_MSVC                                               1
#endif /* LAL_PLATFORM_WINDOWS_WITH_MSVC */

#ifndef __cplusplus
    #error "No cpp standard specified."
#else /* !__cplusplus */
    #if __cplusplus == 199711L
        #if LAL_PLATFORM_WINDOWS_WITH_MSVC
            #error "Mvsc command line build flag was not set. Missing \"/Zc:__cplusplus\" to handle cpp verion ctrl correctly."
        #else /* LAL_PLATFORM_WINDOWS_WITH_MSVC */
            #error "Program requires at least C++23."
        #endif /* !LAL_PLATFORM_WINDOWS_WITH_MSVC */
    #else /* __cplusplus == 199711L */
        #if __cplusplus < PRIVATE_LAL_CPLUSPLUS
            #error "Program requires at least Config/.__cplusplus."
        #endif /* __cplusplus < 202002L */
    #endif /* __cplusplus != 199711L */
#endif /* __cplusplus */

#if LAL_WITH_MSVC 
    #ifndef _MSVC_LANG
        #error "_MSVC_LANG is not defined."
    #endif /* !_MSVC_LANG */
#endif /* LAL_WITH_MSVC */

#if !(LAL_WITH_GCC != LAL_WITH_MSVC)
    #error "No compiler specified or both."
#endif /* LAL_WITH_GCC != LAL_WITH_MSVC */

namespace Lal
{

struct LPrimitivePlatformTypesGeneric;

//# The platform types specification for Windows.
struct LPrimitivePlatformTypesWindows;

//# Make it public.
#ifndef LAL_PLATFORM_TYPES_STRUCT
    #define LAL_PLATFORM_TYPES_STRUCT                                   ::Lal::LPrimitivePlatformTypesWindows
#endif /* !LAL_PLATFORM_TYPES_STRUCT */

//# The platform break implementation details for break behavior on Windows.
struct LOnPlatformBreakWindows;

//# Make it public.
typedef LOnPlatformBreakWindows                                         LOnPlatformBreak;

} /* ~Namespace Lal */

///////////////////////////////////////////////////////////////////////////////
// Compiler config

//#
//# Do not warn about misuses of pragmas, such as incorrect parameters, invalid syntax, or conflicts between pragmas.
//# See also -Wunknown-pragmas.
//#
//# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wpragmas
//#
#if LAL_PLATFORM_WINDOWS_WITH_GCC
    #pragma GCC diagnostic error "-Wpragmas"
#endif /* LAL_PLATFORM_WINDOWS_WITH_GCC */

//#
//# Warn when a #pragma directive is encountered that is not understood by GCC.
//# If this command-line option is used, warnings are even issued for unknown pragmas in system header files.
//# This is not the case if the warnings are only enabled by the -Wall command-line option.
//#
//# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunknown-pragmas
//#
#if LAL_PLATFORM_WINDOWS_WITH_GCC
    #pragma GCC diagnostic error "-Wunknown-pragmas"
#endif /* LAL_PLATFORM_WINDOWS_WITH_GCC */

#if LAL_PLATFORM_WINDOWS_WITH_MSVC
    //#
    //# Warning C4002 (compiler warning level 1)   ==>   Raise to error:
    //# too many arguments for function-like macro invocation 'identifier'
    //#
    //# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4002?view=msvc-170
    //#
    #pragma warning(error: 4002)

    //#
    //# Warning C4003 (compiler warning level 1)   ===>   Raise to error:
    //# not enough arguments for function-like macro invocation 'identifier'
    //#
    //# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4003?view=msvc-170
    //#
    #pragma warning(error: 4003)

    //#
    //# Warning C4005 (compiler warning level 1)   ===>   Raise to error:
    //# 'identifier' : macro redefinition
    //#
    //# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4005?view=msvc-170
    //#
    #pragma warning(error: 4005)

    //#
    //# Warning C4172 (compiler warning level 1)   ===>   Raise to error:
    //# returning address of local variable or temporary: function.
    //#
    //# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4172?view=msvc-170
    //#
    #pragma warning(error : 4172)

    //#
    //# Warning C4251 (compiler warning level 2):
    //# 'type' : class 'type1' needs to have dll-interface to be used by clients of class 'type2'.
    //#
    //# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4251?view=msvc-170
    //#
    #pragma warning(disable : 4251)

    //#
    //# Warning C4275 (compiler warning level 1)   ===>   Raise to error:
    //# 'modifier': used more than once
    //#
    //# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4141?view=msvc-170
    //#
    #pragma warning(error : 4141)

    //#
    //# Warning C4553 (compiler warning level 1)   ===>   Raise to error:
    //# 'operator' : operator has no effect; did you intend 'operator'?
    //#
    //# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4553?view=msvc-170
    //#
    #pragma warning(error : 4553)

    //#
    //# Warning C4700 (compiler warning level 1)   ===>   Raise to error:
    //# uninitialized local variable 'name' used
    //#
    //# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-and-level-4-c4700?view=msvc-170
    //#
    #pragma warning(error : 4700)

    //#
    //# Warning C4717 (compiler warning level 1)   ===>   Raise to error:
    //# 'function' : recursive on all control paths, function will cause runtime stack overflow
    //#
    //# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4717?view=msvc-170
    //#
    #pragma warning(error : 4717)
#endif /* PLATFORM_WINDOWS_WITH_MSVC */

#if LAL_PLATFORM_WINDOWS_WITH_GCC
    // #pragma GCC diagnostic ignored "-Wno-gnu-anonymous-struct" /* <--- Currently not using pedantic. Because that's just to pedantic. */

    #pragma GCC diagnostic error "-Wbuiltin-macro-redefined"

    //#
    //# C++20 std change: A simple-template-id is no longer valid as the declarator-id of a constructor or destructor.
    //# http://eel.is/c++draft/diff.cpp17.class#2
    //#
    #pragma GCC diagnostic error "-Wtemplate-id-cdtor"

    //#
    //# Warn whenever a local variable is assigned to, but otherwise unused (aside from its declaration).
    //# This warning is enabled by -Wall.
    //# To suppress this warning use the unused attribute (see Specifying Attributes of Variables).
    //# This warning is also enabled by -Wunused, which is enabled by -Wall.
    //#
    //# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-but-set-variable
    //#
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"

    //#
    //# Warn whenever a static function is declared but not defined or a non-inline static function is unused.
    //# This warning is enabled by -Wall.
    //#
    //# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-function
    //#
    #pragma GCC diagnostic ignored "-Wunused-function"

    //#
    //# Warn whenever a function parameter is unused aside from its declaration.
    //# This option is not enabled by -Wunused unless -Wextra is also specified.
    //#
    //# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-parameter
    //#
    #pragma GCC diagnostic ignored "-Wunused-parameter"

    //#
    //# Warn whenever a local or static variable is unused aside from its declaration. This option implies
    //# -Wunused-const-variable=1 for C, but not for C++. This warning is enabled by -Wall.
    //#
    //# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunused-variable
    //#
    #pragma GCC diagnostic ignored "-Wunused-variable"

    //#
    //# Warn whenever a comment-start sequence '/ *' appears in a '/ *' comment, or whenever a backslash-newline
    //# appears in a '//' comment. This warning is enabled by -Wall.
    //# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wcomment
    //#
    #pragma GCC diagnostic ignored "-Wcomment"
    #pragma GCC diagnostic ignored "-Wcomments"

    //#
    //# Warn if a structure’s initializer has some fields missing. In C this option does not warn about designated
    //# initializers. In C++ this option does not warn about the empty { } initializer.
    //# This warning is included in -Wextra. To get other -Wextra warnings without this one, use -Wextra
    //# -Wno-missing-field-initializers.
    //#
    //# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wmissing-field-initializers
    //#
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif /* LAL_PLATFORM_WINDOWS_WITH_GCC */

// ~Compiler config
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Compiler dependent features


#if AS_CLIENT
    #ifndef JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
        #define JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER              1
    #endif /* JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
#endif /* AS_CLIENT */

#ifndef LAL_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS
    #define LAL_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS        1
#endif /* !LAL_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS */

#ifndef LAL_PLATFORM_USES_64_BIT
    #define LAL_PLATFORM_USES_64_BIT                                    1
#endif /* !LAL_PLATFORM_USES_64_BIT */

//# https://learn.microsoft.com/en-us/cpp/mfc/windows-sockets-byte-ordering?view=msvc-170
#ifndef LAL_PLATFORM_USES_LITTLE_ENDIAN
    #define LAL_PLATFORM_USES_LITTLE_ENDIAN                             1
#endif /* !LAL_PLATFORM_USES_LITTLE_ENDIAN */

#ifndef LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #define LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES                      1
#endif /* !LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES */

#ifndef LAL_PLATFORM_SUPPORTS_STD_FLUSH
    #define LAL_PLATFORM_SUPPORTS_STD_FLUSH                             1
#endif /* !LAL_PLATFORM_SUPPORTS_STD_FLUSH */

#ifndef LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES
    #define LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES                          1
#endif /* !LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES */

#ifndef LAL_PLATFORM_SUPPORTS_SIMD
    #define LAL_PLATFORM_SUPPORTS_SIMD                                  1
#endif /* !LAL_PLATFORM_SUPPORTS_SIMD */

#ifndef LAL_PLATFORM_SUPPORTS_MEMORY_SHRINK
    #define LAL_PLATFORM_SUPPORTS_MEMORY_SHRINK                         1
#endif /* !LAL_PLATFORM_SUPPORTS_MEMORY_SHRINK */

#if (defined(LAL_WITH_MSVC) && LAL_WITH_MSVC != 0) && !(_MSVC_LANG >= 201703L)
    #if !defined(PLATFORM_USES_UTF8) || PLATFORM_USES_UTF8 == 0
        #error "PLATFORM_USES_UTF8 is not defined or set to 0. Missing /utf-8 compiler flag."
    #endif /* !PLATFORM_USES_UTF8 */
#else /* (defined(LAL_WITH_MSVC) && LAL_WITH_MSVC != 0) && !(_MSVC_LANG >= 201703L) */
    #ifndef LAL_PLATFORM_USES_UTF8
        #define LAL_PLATFORM_USES_UTF8                                  1
    #endif /* !LAL_PLATFORM_USES_UTF8 */
#endif /* !defined(LAL_WITH_MSVC) || LAL_WITH_MSVC == 0 || _MSVC_LANG >= 201703L */

#ifndef LAL_PLATFORM_WCHAR_SIZE
    #define LAL_PLATFORM_WCHAR_SIZE                                     2
#endif /* !LAL_PLATFORM_WCHAR_SIZE */

#ifndef LAL_PLATFORM_CALLSPEC_IN
    #define LAL_PLATFORM_CALLSPEC_IN                                    __declspec ( dllimport )
#endif /* !LAL_PLATFORM_CALLSPEC_IN */

#ifndef LAL_PLATFORM_CALLSPEC_OUT
    #define LAL_PLATFORM_CALLSPEC_OUT                                   __declspec ( dllexport )
#endif /* !LAL_PLATFORM_CALLSPEC_OUT */

#ifndef LAL_PLATFORM_EXTERNSPEC_IN
    #define LAL_PLATFORM_EXTERNSPEC_IN                                  extern
#endif /* !LAL_PLATFORM_EXTERNSPEC_IN */

#ifndef LAL_PLATFORM_EXTERNSPEC_OUT
    #define LAL_PLATFORM_EXTERNSPEC_OUT
#endif /* !LAL_PLATFORM_EXTERNSPEC_OUT */

#ifndef LAL_PLATFORM_MAX_PATH
    #define LAL_PLATFORM_MAX_PATH                                       MAX_PATH
#endif /* LAL_PLATFORM_MAX_PATH */

#ifndef LAL_PLATFORM_USES_STD_FINDER
    #define LAL_PLATFORM_USES_STD_FINDER                                1
#endif /* LAL_PLATFORM_USES_STD_FINDER */

#ifndef LAL_PLATFORM_NO_DISCARD_CTRL_PATH
    #if LAL_WITH_MSVC
        #include <intrin.h>
        #define LAL_PLATFORM_NO_DISCARD_CTRL_PATH                       { __nop(); }
    #elif LAL_WITH_GCC
        #define LAL_PLATFORM_NO_DISCARD_CTRL_PATH                       { __asm__ __volatile__ ("nop"); }
    #endif /* LAL_WITH_GCC */
#endif /* !LAL_PLATFORM_NO_DISCARD_CTRL_PATH */

#ifndef LAL_PLATFORM_BREAK
    #define LAL_PLATFORM_BREAK()                                        __debugbreak();
#endif  /* !LAL_PLATFORM_BREAK */

//# Do we want to use __ud2 or __fastfail. This has to be evaluated.
#ifndef LAL_PLATFORM_TRAP
    #define LAL_PLATFORM_TRAP()                                         (__ud2());
#endif /* !LAL_PLATFORM_TRAP */

#ifndef NOINLINE
    #define NOINLINE                                                    __declspec ( noinline )
#endif /* !NOINLINE */

#ifndef FORCEINLINE
    #if IN_DEBUG
        //#
        //# Inlining is disabled in debug builds as following the debugger through inlined code is a pain
        //# in the ass.
        //#
        #define FORCEINLINE                                             inline
    #else /* IN_DEBUG */
        #if LAL_WITH_MSVC
            #define FORCEINLINE                                         _forceinline
        #else LAL_WITH_GCC
            #define FORCEINLINE                                         __attribute__ ((always_inline))
        #endif /* LAL_WITH_GCC */
    #endif /* !IN_DEBUG */
#endif /* !FORCEINLINE */

// ~Compiler dependent features
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//# Windows Platform

/* Why the duck does this even exists. This destroys so many normal functions - wtf?? */
#ifdef NOMINMAX
    #undef NOMINMAX
#endif /* NOMINMAX */
#define NOMINMAX        1 // <--- Always block. Jafg will not compile otherwise.

#ifndef UNICODE /* Always use wides on Windows. */
    #define UNICODE     1
#endif /* UNICODE */
#ifndef _UNICODE
    #define _UNICODE    1
#endif /* _UNICODE */

/**
 * https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
 */
#if LAL_PLATFORM_WINDOWS_WITH_GCC
    #ifdef WINVER
        #undef WINVER
    #endif /* WINVER */
    #ifdef _WIN32_WINNT
        #undef _WIN32_WINNT /* @see _mingw.h <--- Wants to compile with Windows Vista??? Bro. Shame. */
    #endif /* _WIN32_WINNT */
#endif /* LAL_PLATFORM_WINDOWS_WITH_GCC */
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

#ifdef TEXT
    #undef TEXT
#endif /* TEXT */

//# Windows Platform
///////////////////////////////////////////////////////////////////////////////

#if LAL_WITH_GCC
    #include <locale>
#endif /* LAL_WITH_GCC */

#include <codecvt>

namespace Lal
{

struct LPrimitivePlatformTypesWindows final : public LPrimitivePlatformTypesGeneric
{
    typedef size_t LSize;

    static LStringLegacy Ws2S(const LWideString& Ws)
    {
#if LAL_WITH_MSVC
        __pragma( warning(push) )
        __pragma( warning(disable: 4996) )
#endif /* LAL_WITH_MSVC */
        typedef std::codecvt_utf8<wchar_t> TypeX;
        std::wstring_convert<TypeX, wchar_t> Converter;
        return Converter.to_bytes(Ws);
#if LAL_WITH_MSVC
        __pragma( warning(pop) )
#endif /* LAL_WITH_MSVC */
    }

    static const char* Ws2CStr(const LWideString& Ws, LStringLegacy* OutContainer)
    {
        *OutContainer = LPrimitivePlatformTypesWindows::Ws2S(Ws);
        return OutContainer->c_str();
    }

    static std::wstring CStr2Ws(const char* Cs)
    {
#if LAL_WITH_MSVC
        __pragma( warning(push) )
        __pragma( warning(disable: 4996) )
#endif /* LAL_WITH_MSVC */
        typedef std::codecvt_utf8<wchar_t> TypeX;
        std::wstring_convert<TypeX, wchar_t> Converter;
        return Converter.from_bytes(Cs);
#if LAL_WITH_MSVC
        __pragma( warning(pop) )
#endif /* LAL_WITH_MSVC */
    }
};

struct LOnPlatformBreakWindows final
{
    [[noreturn]] NOINLINE
    static void ExitQuietly();

    [[noreturn]] NOINLINE
    static void OnProgramPanicImpl
    (
        const LPrimitivePlatformTypesGeneric::LChar* InMessage
    );

    [[noreturn]] NOINLINE
    static void OnProgramPanic
    (
        const LPrimitivePlatformTypesGeneric::LChar* InBaseMessage,
        const LPrimitivePlatformTypesGeneric::LChar* InFile,
        const LPrimitivePlatformTypesGeneric::u64    InLine
    );
};

} /* ~Namespace Lal */
