// Copyright mzoesch. All rights reserved.

#pragma once

/*-----------------------------------------------------------------------------
    Validate compiler and forward declare JAFG Linux logic.
-----------------------------------------------------------------------------*/

#if !JAFG_PLATFORM_LINUX
    #error "Wanted to override generic platform types with Linux specific types, but platform is not Linux."
#endif /* !JAFG_PLATFORM_LINUX */

#if !(__linux__)
    #error "This is not linux; lol."
#endif /* !(__linux__) */

#ifndef __cplusplus
    #error "No cpp standard specified."
#else /* !__cplusplus */
    #if __cplusplus < DETAIL_JAFG_CPLUSPLUS
        #error "Program requires at least Config/.__cplusplus."
    #endif /* __cplusplus < DETAIL_JAFG_CPLUSPLUS */
#endif /* __cplusplus */

namespace Jafg::Detail
{

typedef LPrimitivePlatformTypesGeneric LPrimitivePlatformTypes;

} /* ~Namespace Jafg::Detail */

extern "C"
{
extern void __assert_fail(
    char const* assertion,
    char const* file, unsigned int line, char const* function
    ) noexcept __attribute__ ((__noreturn__)) /* __attribute__ ((__cold)) */;
} /* extern "C" */

#if (!JAFG_WITH_CLANG) && (!JAFG_WITH_GCC)
    #error "Linux only supports clang and gcc as a valid compiler for the moment."
#endif /* !JAFG_WITH_CLANG */

#if !defined(__GLIBCXX__)
    #error "Wrong std library. We need libstdc++."
#endif /* !__GLIBCXX__ */
#if defined(_LIBCPP_VERSION)
    #error "Wrong std library. No LLVM."
#endif /* _LIBCPP_VERSION */

#if JAFG_DO_COMPILER_DIAGNOSTIC_SETUP
    #if JAFG_WITH_CLANG
        #include "Definitions/PushCommonClangDiagnostics.h"
    #endif /* JAFG_WITH_CLANG */
    #if JAFG_WITH_GCC
        // #pragma clang diagnostic ignored "-Wformat-security"
        #include "Definitions/PushCommonGccDiagnostics.h"
    #endif /* JAFG_WITH_GCC */
#endif /* JAFG_DO_COMPILER_DIAGNOSTIC_SETUP */

#ifndef JAFG_PLATFORM_DESKTOP
    #define JAFG_PLATFORM_DESKTOP                                       1
#endif /* !JAFG_PLATFORM_DESKTOP */

#ifndef JAFG_UNLIKELY
    #define JAFG_UNLIKELY(Expr)                                         (__builtin_expect(!!(Expr), 0))
#endif /* !JAFG_UNLIKELY */

#ifndef JAFG_LIKELY
    #define JAFG_LIKELY(Expr)                                           (__builtin_expect(!!(Expr), 1))
#endif /* !JAFG_LIKELY */

#if JAFG_WITH_LOCAL_LAYER
    #ifndef JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
        #define JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER              1
    #endif /* JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
#endif /* JAFG_WITH_LOCAL_LAYER */

#ifndef JAFG_WITH_REST_CLS
    #define JAFG_WITH_REST_CLS                                          1
#endif /* !JAFG_WITH_REST_CLS */

#ifndef JAFG_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS
    #define JAFG_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS       1
#endif /* !JAFG_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS */

#ifndef JAFG_PLATFORM_USES_64_BIT
    #define JAFG_PLATFORM_USES_64_BIT                                   1
#endif /* !JAFG_PLATFORM_USES_64_BIT */

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

#ifndef JAFG_PLATFORM_USES_UTF8
    #define JAFG_PLATFORM_USES_UTF8                                     1
#endif /* !JAFG_PLATFORM_USES_UTF8 */

#ifndef LITERAL_TEXT
    #define LITERAL_TEXT(x)                                             x
#endif /* !LITERAL_TEXT */

#ifndef JAFG_PLATFORM_CALLSPEC_IN
    #define JAFG_PLATFORM_CALLSPEC_IN                                   __attribute__ ((visibility ("default")))
#endif /* !JAFG_PLATFORM_CALLSPEC_IN */

#ifndef JAFG_PLATFORM_CALLSPEC_OUT
    #define JAFG_PLATFORM_CALLSPEC_OUT                                  __attribute__ ((visibility ("default")))
#endif /* !JAFG_PLATFORM_CALLSPEC_OUT */

#ifndef JAFG_PLATFORM_EXTERNSPEC_IN
    #define JAFG_PLATFORM_EXTERNSPEC_IN                                 extern
#endif /* !JAFG_PLATFORM_EXTERNSPEC_IN */

#ifndef JAFG_PLATFORM_EXTERNSPEC_OUT
    #define JAFG_PLATFORM_EXTERNSPEC_OUT
#endif /* !JAFG_PLATFORM_EXTERNSPEC_OUT */

//# https://man7.org/linux/man-pages/man3/realpath.3.html ???
#ifndef JAFG_PLATFORM_MAX_PATH
    #define JAFG_PLATFORM_MAX_PATH                                      300
#endif /* JAFG_PLATFORM_MAX_PATH */

#ifndef JAFG_PLATFORM_NO_DISCARD_CTRL_PATH
    #define JAFG_PLATFORM_NO_DISCARD_CTRL_PATH                          { __asm__ __volatile__ ("nop"); }
#endif /* !JAFG_PLATFORM_NO_DISCARD_CTRL_PATH */

#ifndef JAFG_PLATFORM_BREAK
    #if __has_builtin(__builtin_debugtrap)
        #define JAFG_PLATFORM_BREAK()                                   (__builtin_debugtrap());
    #else /* __has_builtin(__builtin_debugtrap) */
        #include <signal.h>
        #define JAFG_PLATFORM_BREAK()                                   (raise(SIGTRAP));
    #endif /* !__has_builtin(__builtin_debugtrap) */
#endif  /* !JAFG_PLATFORM_BREAK */

#ifndef JAFG_PLATFORM_TRAP
    #if __has_builtin(__builtin_trap)
        #define JAFG_PLATFORM_TRAP()                                    (__builtin_trap());
    #else /* __has_builtin(__builtin_trap) */
        #error "Encountered unimplemented code path." /* ? How. */
    #endif /* !__has_builtin(__builtin_trap) */
#endif /* !JAFG_PLATFORM_TRAP */

#ifndef JAFG_PLATFORM_UNREACHABLE
    #define JAFG_PLATFORM_UNREACHABLE()                                 (__builtin_unreachable());
#endif /* JAFG_PLATFORM_UNREACHABLE */

#ifndef NOINLINE
    #define NOINLINE                                                    __attribute__ ((noinline))
#endif /* !NOINLINE */

#ifndef FORCEINLINE
    #if !JAFG_IN_DEBUG
        #define FORCEINLINE                                             __attribute__ ((always_inline))
    #endif /* !JAFG_IN_DEBUG */
#endif /* !FORCEINLINE */
