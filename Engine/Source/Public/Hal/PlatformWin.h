// Copyright mzoesch. All rights reserved.

#pragma once

/*-----------------------------------------------------------------------------
    Validate compiler and forward declare JAFG Windows logic.
-----------------------------------------------------------------------------*/

#if !PLATFORM_WINDOWS
    #error "Wanted to override generic platform types with Windows specific types, but platform is not Windows."
#endif /* !PLATFORM_WINDOWS */

#if JAFG_WITH_GCC || JAFG_WITH_CLANG
    #ifdef _MSC_VER
        #if !WITH_IDEA_INTELLISENSE /* We might still wanna use msvc intellisense because it's cool. */
            #error "Compiling with gcc/clang but _MSC_VER is defined."
        #endif /* !WITH_IDEA_INTELLISENSE */
    #endif /* _MSC_VER */
#endif /* JAFG_WITH_GCC || JAFG_WITH_CLANG*/

//# We currently no longer support MSVC on Windows, but in the future we will maybe switch again.
//# Let's have a look at C++26 support from the Microsoft side. Currently, it's not looking good >:(
#if JAFG_WITH_MSVC
    #if _MSC_VER < 1930
        #error "Program requires at least verion \"Visual Studio 2022 RTW 17.0\" of the MSVC compiler."
    #endif /* _MSC_VER < 1930 */
#endif /* JAFG_WITH_MSVC */

#ifndef __cplusplus
    #error "No cpp standard specified."
#else /* !__cplusplus */
    #if __cplusplus == 199711L
        #if JAFG_WITH_MSVC
            #error "Mvsc command line build flag was not set. Missing \"/Zc:__cplusplus\" to handle cpp verion ctrl correctly."
        #endif /* JAFG_WITH_MSVC */
    #endif /* __cplusplus == 199711L */
    #if __cplusplus < DETAIL_JAFG_CPLUSPLUS
        #error "Program requires at least Config/.__cplusplus."
    #endif /* __cplusplus < DETAIL_JAFG_CPLUSPLUS */
#endif /* __cplusplus */

#if JAFG_WITH_MSVC 
    #ifndef _MSVC_LANG
        #error "_MSVC_LANG is not defined."
    #endif /* !_MSVC_LANG */
#endif /* JAFG_WITH_MSVC */

namespace Jafg
{

struct LPrimitivePlatformTypesGeneric;

//# The platform types specification for Windows.
struct LPrimitivePlatformTypesWindows final : public LPrimitivePlatformTypesGeneric
{
    #define JAFG_PLATFORM_U64_SIZET_EQ 1
    typedef wchar_t LChar;
};

//# Make it public.
#ifndef JAFG_PLATFORM_TYPES_STRUCT
    #define JAFG_PLATFORM_TYPES_STRUCT                                  ::Jafg::LPrimitivePlatformTypesWindows
#endif /* !JAFG_PLATFORM_TYPES_STRUCT */

//# The platform break implementation details for break behavior on Windows.
struct LOnPlatformBreakWindows;

//# Make it public.
typedef LOnPlatformBreakWindows                                         LOnPlatformBreak;

} /* ~Namespace Jafg */

#if JAFG_DO_COMPILER_DIAGNOSTIC_SETUP
///////////////////////////////////////////////////////////////////////////////
// Compiler config

//#
//# Do not warn about misuses of pragmas, such as incorrect parameters, invalid syntax, or conflicts between pragmas.
//# See also -Wunknown-pragmas.
//#
//# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wpragmas
//#
#if JAFG_WITH_GCC
    #pragma GCC diagnostic error "-Wpragmas"
#endif /* JAFG_WITH_GCC */

//#
//# Warn when a #pragma directive is encountered that is not understood by GCC.
//# If this command-line option is used, warnings are even issued for unknown pragmas in system header files.
//# This is not the case if the warnings are only enabled by the -Wall command-line option.
//#
//# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#index-Wunknown-pragmas
//#
#if JAFG_WITH_GCC
    #pragma GCC diagnostic error "-Wunknown-pragmas"
#endif /* JAFG_WITH_GCC */

#if JAFG_WITH_MSVC
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
#endif /* JAFG_WITH_MSVC */

#if JAFG_WITH_GCC
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
#endif /* JAFG_WITH_GCC */

#if JAFG_WITH_CLANG
    #include "Definitions/PushCommonClangDiagnostics.h"
#endif /* JAFG_WITH_CLANG */

// ~Compiler config
///////////////////////////////////////////////////////////////////////////////
#endif /* JAFG_DO_COMPILER_DIAGNOSTIC_SETUP */

///////////////////////////////////////////////////////////////////////////////
// Compiler dependent features

#if AS_CLIENT
    #ifndef JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
        #define JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER              1
    #endif /* JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
#endif /* AS_CLIENT */

#ifndef JAFG_WITH_REST_CLS
    #define JAFG_WITH_REST_CLS                                          1
#endif /* !JAFG_WITH_REST_CLS */

#ifndef JAFG_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS
    #define JAFG_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS       1
#endif /* !JAFG_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS */

#ifndef JAFG_PLATFORM_USES_64_BIT
    #define JAFG_PLATFORM_USES_64_BIT                                   1
#endif /* !JAFG_PLATFORM_USES_64_BIT */

//# https://learn.microsoft.com/en-us/cpp/mfc/windows-sockets-byte-ordering?view=msvc-170
#ifndef JAFG_PLATFORM_USES_LITTLE_ENDIAN
    #define JAFG_PLATFORM_USES_LITTLE_ENDIAN                            1
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */

#ifndef JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #define JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES                     1
#endif /* !JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES */

#ifndef JAFG_PLATFORM_SUPPORTS_STD_FLUSH
    #define JAFG_PLATFORM_SUPPORTS_STD_FLUSH                            1
#endif /* !JAFG_PLATFORM_SUPPORTS_STD_FLUSH */

#ifndef JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES
    #define JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES                         1
#endif /* !JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES */

#ifndef JAFG_PLATFORM_SUPPORTS_SIMD
    #define JAFG_PLATFORM_SUPPORTS_SIMD                                 1
#endif /* !JAFG_PLATFORM_SUPPORTS_SIMD */

#ifndef JAFG_PLATFORM_SUPPORTS_MEMORY_SHRINK
    #define JAFG_PLATFORM_SUPPORTS_MEMORY_SHRINK                        1
#endif /* !JAFG_PLATFORM_SUPPORTS_MEMORY_SHRINK */

#ifndef JAFG_PLATFORM_USES_UTF16
    #define JAFG_PLATFORM_USES_UTF16                                    1
#endif /* !JAFG_PLATFORM_USES_UTF16 */

#ifndef JAFG_PLATFORM_WCHAR_SIZE
    #define JAFG_PLATFORM_WCHAR_SIZE                                    2
#endif /* !JAFG_PLATFORM_WCHAR_SIZE */

#ifndef LITERAL_TEXT
    #define LITERAL_TEXT(x)                                             LITERAL_WIDE(x)
#endif /* !LITERAL_TEXT */

#ifndef JAFG_PLATFORM_CALLSPEC_IN
    #define JAFG_PLATFORM_CALLSPEC_IN                                    __declspec ( dllimport )
#endif /* !JAFG_PLATFORM_CALLSPEC_IN */

#ifndef JAFG_PLATFORM_CALLSPEC_OUT
    #define JAFG_PLATFORM_CALLSPEC_OUT                                   __declspec ( dllexport )
#endif /* !JAFG_PLATFORM_CALLSPEC_OUT */

#ifndef JAFG_PLATFORM_EXTERNSPEC_IN
    #define JAFG_PLATFORM_EXTERNSPEC_IN                                  extern
#endif /* !JAFG_PLATFORM_EXTERNSPEC_IN */

#ifndef JAFG_PLATFORM_EXTERNSPEC_OUT
    #define JAFG_PLATFORM_EXTERNSPEC_OUT
#endif /* !JAFG_PLATFORM_EXTERNSPEC_OUT */

#ifndef JAFG_PLATFORM_MAX_PATH
    #define JAFG_PLATFORM_MAX_PATH                                       MAX_PATH
#endif /* JAFG_PLATFORM_MAX_PATH */

#ifndef JAFG_PLATFORM_USES_STD_FINDER
    #define JAFG_PLATFORM_USES_STD_FINDER                                1
#endif /* JAFG_PLATFORM_USES_STD_FINDER */

#ifndef JAFG_PLATFORM_NO_DISCARD_CTRL_PATH
    #if JAFG_WITH_MSVC
        #include <intrin.h>
        #define JAFG_PLATFORM_NO_DISCARD_CTRL_PATH                       { __nop(); }
    #elif JAFG_WITH_GCC || JAFG_WITH_CLANG
        #define JAFG_PLATFORM_NO_DISCARD_CTRL_PATH                       { __asm__ __volatile__ ("nop"); }
    #endif /* JAFG_WITH_GCC || JAFG_WITH_CLANG */
#endif /* !JAFG_PLATFORM_NO_DISCARD_CTRL_PATH */

#ifndef JAFG_PLATFORM_BREAK
    #if JAFG_WITH_MSVC
        #define JAFG_PLATFORM_BREAK()                                    __debugbreak();
    #else /* JAFG_WITH_MSVC */
        #if defined(__aarch64__) || defined(__arm64ec__)
            #define JAFG_PLATFORM_BREAK()                               { __asm__ __volatile__("brk #0xf000"); }
        #elif defined(__i386__) || defined(__x86_64__)
            #define JAFG_PLATFORM_BREAK()                               { __asm__ __volatile__("int {$}3":); }
        #elif defined(__arm__)
            #define JAFG_PLATFORM_BREAK()                               { __asm__ __volatile__("udf #0xfe"); }
        #else /* defined(__arm__) */
            #define JAFG_PLATFORM_BREAK()                               { __asm__ __volatile__("unimplemented"); }
        #endif /* !defined(__arm__) */
    #endif /* !JAFG_WITH_MSVC */
#endif  /* !JAFG_PLATFORM_BREAK */

//# Do we want to use __ud2 or __fastfail. This has to be evaluated.
#ifndef JAFG_PLATFORM_TRAP
    #define JAFG_PLATFORM_TRAP()                                         (__ud2());
#endif /* !JAFG_PLATFORM_TRAP */

#ifndef JAFG_PLATFORM_UNREACHABLE
    #define JAFG_PLATFORM_UNREACHABLE()                                  __assume ( 0 )
#endif /* JAFG_PLATFORM_UNREACHABLE */

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
        #if JAFG_WITH_MSVC
            #define FORCEINLINE                                         _forceinline
        #else JAFG_WITH_GCC || JAFG_WITH_CLANG
            #define FORCEINLINE                                         __attribute__ ((always_inline))
        #endif /* JAFG_WITH_GCC || JAFG_WITH_CLANG */
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

//# https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
#if JAFG_WITH_GCC || JAFG_WITH_CLANG
    #ifdef WINVER
        #undef WINVER
    #endif /* WINVER */
    #ifdef _WIN32_WINNT
        #undef _WIN32_WINNT /* @see _mingw.h <--- Wants to compile with Windows Vista??? Bro. Shame. */
    #endif /* _WIN32_WINNT */
#endif /* JAFG_WITH_GCC || JAFG_WITH_CLANG */
#if !JAFG_WITH_MSVC
    //# Defines it ourselves - we do not need backwards compatibility for systems prior to Windows 10.
    #define WINVER          0x0A00  // 0x0400 <=> Windows NT 4.0 ;;; 0x0500 <=> Windows 2000 ;;; 0x0501 <=> Windows XP
    #define _WIN32_WINNT    0x0A00  // 0x0502 <=> Windows Server 2003 ;;; 0x0600 <=> Windows Vista
                                    // 0x0600 <=> Windows Server 2008 ;;; 0x0601 <=> Windows 7 ;;; 0x0602 <=> Windows 8
                                    // 0x0603 <=> Windows 8.1 ;; 0x0A00 <=> Windows 10
#endif /* !JAFG_WITH_MSVC */

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#ifdef TEXT
    #undef TEXT
#endif /* TEXT */

//# Windows Platform
///////////////////////////////////////////////////////////////////////////////

#if JAFG_WITH_GCC
    #include <locale>
#endif /* JAFG_WITH_GCC */

namespace Jafg
{

struct LOnPlatformBreakWindows final
{
    [[noreturn]] NOINLINE
    ENGINE_API static void ExitQuietly();

    [[noreturn]] NOINLINE
    ENGINE_API static void OnProgramPanicImpl
    (
        LPrimitivePlatformTypesGeneric::LJafgChar const* InMessage
    );

    [[noreturn]] NOINLINE
    ENGINE_API static void OnProgramPanic
    (
        LPrimitivePlatformTypesGeneric::LJafgChar const* InBaseMessage,
        LPrimitivePlatformTypesGeneric::LJafgChar const* InFile,
        LPrimitivePlatformTypesGeneric::u64       const  InLine
    );
};

} /* ~Namespace Jafg */
