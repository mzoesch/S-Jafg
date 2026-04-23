// Copyright mzoesch. All rights reserved.

#pragma once

/*-----------------------------------------------------------------------------
    Validate compiler and forward declare JAFG Linux logic.
-----------------------------------------------------------------------------*/

#if !PLATFORM_LINUX
    #error "Wanted to override generic platform types with Linux specific types, but platform is not Linux."
#endif /* !PLATFORM_LINUX */

#ifndef __cplusplus
    #error "No cpp standard specified."
#else /* !__cplusplus */
    #if __cplusplus < PRIVATE_JAFG_CPLUSPLUS
        #error "Program requires at least Config/.__cplusplus."
    #endif /* __cplusplus < PRIVATE_JAFG_CPLUSPLUS */
#endif /* __cplusplus */

namespace Jafg
{

struct LPrimitivePlatformTypesGeneric;

//# The platform types specification for Linux.
struct LPrimitivePlatformTypesLinux final : public LPrimitivePlatformTypesGeneric
{
};

//# Make it public.
#ifndef JAFG_PLATFORM_TYPES_STRUCT
    #define JAFG_PLATFORM_TYPES_STRUCT                                  ::Jafg::LPrimitivePlatformTypesLinux
#endif /* !JAFG_PLATFORM_TYPES_STRUCT */

//# The platform break implementation details for break behavior on Linux.
struct LOnPlatformBreakLinux;

//# Make it public.
typedef LOnPlatformBreakLinux                                           LOnPlatformBreak;

} /* ~Namespace Jafg */

extern "C"
{

extern void __assert_fail
(
    const char *__assertion,
    const char *__file,
    unsigned int __line,
    const char *__function
)
noexcept __attribute__ ((__noreturn__)) /* __attribute__ ((__cold)) */;

} /* extern "C" */

#if !JAFG_WITH_CLANG
    #error "Linux only supports clang as a valid compiler for the moment."
#endif /* !JAFG_WITH_CLANG */

#if !(__clang__)
    #error "We think we are on a clang compiler, but the compiler does not think so. And she / he ("it" would be disrespectful) must know right?"
#endif /* !(__clang__) */

#if !defined(__GLIBCXX__)
    #error "Wrong std library. We need libstdc++."
#endif /* !__GLIBCXX__ */

#if defined(_LIBCPP_VERSION)
    #error "Wrong std library. No LLVM por favor."
#endif /* _LIBCPP_VERSION */

#if !(__linux__)
    #error "This is not linux; lol."
#endif /* !(__linux__) */


/*-----------------------------------------------------------------------------
    Change compiler behavior.
-----------------------------------------------------------------------------*/

#if JAFG_DO_COMPILER_DIAGNOSTIC_SETUP
    #include "Definitions/PushCommonClangDiagnostics.h"
#endif /* JAFG_DO_COMPILER_DIAGNOSTIC_SETUP */


/*-----------------------------------------------------------------------------
    Define platform specific macros.
-----------------------------------------------------------------------------*/

#ifndef JAFG_UNLIKELY
    #define JAFG_UNLIKELY(Expr)                                         (__builtin_expect(!!(Expr), 0))
#endif /* !JAFG_UNLIKELY */

#ifndef JAFG_LIKELY
    #define JAFG_LIKELY(Expr)                                           (__builtin_expect(!!(Expr), 1))
#endif /* !JAFG_LIKELY */

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

#ifndef JAFG_PLATFORM_WCHAR_SIZE
    #define JAFG_PLATFORM_WCHAR_SIZE                                    4
#endif /* !JAFG_PLATFORM_WCHAR_SIZE */

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

#ifndef JAFG_PLATFORM_USES_STD_FINDER
    #define JAFG_PLATFORM_USES_STD_FINDER                               1
#endif /* JAFG_PLATFORM_USES_STD_FINDER */

#ifndef JAFG_PLATFORM_NO_DISCARD_CTRL_PATH
    #define JAFG_PLATFORM_NO_DISCARD_CTRL_PATH                          { __asm__ __volatile__ ("nop"); }
#endif /* !JAFG_PLATFORM_NO_DISCARD_CTRL_PATH */

#ifndef JAFG_PLATFORM_BREAK
    #if __has_builtin(__builtin_debugtrap)
        #define JAFG_PLATFORM_BREAK()                                   (__builtin_debugtrap());
    #else /* __has_builtin(__builtin_debugtrap) */
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
    #if IN_DEBUG
        //#
        //# Inlining is disabled in debug builds as following the debugger through inlined code is a pain
        //# in the ass.
        //#
        #define FORCEINLINE                                             inline
    #else /* IN_DEBUG */
        #define FORCEINLINE                                             __attribute__ ((always_inline))
    #endif /* !IN_DEBUG */
#endif /* !FORCEINLINE */

namespace Jafg
{

struct LOnPlatformBreakLinux final
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
