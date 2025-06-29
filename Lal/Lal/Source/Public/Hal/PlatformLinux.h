// Copyright mzoesch. All rights reserved.

#pragma once

/*-----------------------------------------------------------------------------
    Validate compiler and forward declare Lal linux logic.
-----------------------------------------------------------------------------*/

#if !PLATFORM_LINUX
    #error "Wanted to override generic platform types with linux specific types, but platform is not linux."
#endif /* !PLATFORM_LINUX */

#if __cplusplus < 202302L
    #error "Program requires at least C++23."
#endif /* __cplusplus < 202002L */

namespace Lal
{

struct LPrimitivePlatformTypesGeneric;

//# The platform break implementation details for break behavior on linux.
struct LOnPlatformBreakLinux;

//# Make it public.
typedef LOnPlatformBreakLinux                                           LOnPlatformBreak;

} /* ~Namespace Lal */

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

#ifndef LAL_WITH_CLANG
    #define LAL_WITH_CLANG                                              1
#endif /* !LAL_WITH_CLANG */

#if !LAL_WITH_CLANG
    #error "Linux only supports clang as a valid compiler for the moment."
#endif /* !LAL_WITH_CLANG */

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

#if LAL_DO_COMPILER_DIAGNOSTIC_SETUP
    //
    // Customize clang warnings.
    // @see https://clang.llvm.org/docs/DiagnosticsReference.html
    //

    /*-----------------------------------------------------------------------------
        Raise.
    -----------------------------------------------------------------------------*/
    #pragma clang diagnostic error "-Wbuiltin-macro-redefined"
    #pragma clang diagnostic error "-Wdangling-else"
    #pragma clang diagnostic error "-Wextra-semi"
    #pragma clang diagnostic error "-Wextra-tokens"
    #pragma clang diagnostic error "-Winconsistent-missing-override"
    #pragma clang diagnostic error "-Winvalid-noreturn"
    #pragma clang diagnostic error "-Wlogical-op-parentheses"
    #pragma clang diagnostic error "-Wmacro-redefined"
    #pragma clang diagnostic error "-Wmismatched-new-delete"
    #pragma clang diagnostic error "-Wmismatched-tags"
    #pragma clang diagnostic error "-Wnull-pointer-subtraction"
    #pragma clang diagnostic error "-Wparentheses"
    #pragma clang diagnostic error "-Wpessimizing-move"
    #pragma clang diagnostic error "-Wpragmas"
    #pragma clang diagnostic error "-Wreturn-type"
    #pragma clang diagnostic error "-Wsign-compare"
    #pragma clang diagnostic error "-Wswitch"
    #pragma clang diagnostic error "-Wundefined-inline"
    #pragma clang diagnostic error "-Wunknown-pragmas"
    #pragma clang diagnostic error "-Wunknown-warning-option"
    #pragma clang diagnostic error "-Wunused-lambda-capture"

    /*-----------------------------------------------------------------------------
        Ignore.
    -----------------------------------------------------------------------------*/
    #pragma clang diagnostic ignored "-Wcomment"
    #pragma clang diagnostic ignored "-Wcomments"
    #pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
    #pragma clang diagnostic ignored "-Wmissing-designated-field-initializers"
    #pragma clang diagnostic ignored "-Wmissing-field-initializers"
    #pragma clang diagnostic ignored "-Wnested-anon-types"
    #pragma clang diagnostic ignored "-Wunused-parameter"

    /*-----------------------------------------------------------------------------
        Shipping only.
    -----------------------------------------------------------------------------*/
    #if LAL_DO_ENABLE_SHIPPING_WARNINGS
        #pragma clang diagnostic warning "-Wundefined-var-template"
        #pragma clang diagnostic warning "-Wunused-but-set-variable"
        #pragma clang diagnostic warning "-Wunused-private-field"
        #pragma clang diagnostic warning "-Wunused-function"
        #pragma clang diagnostic warning "-Wunused-variable"
    #else /* LAL_DO_ENABLE_SHIPPING_WARNINGS */
        #pragma clang diagnostic ignored "-Wundefined-var-template"
        #pragma clang diagnostic ignored "-Wunused-but-set-variable"
        #pragma clang diagnostic ignored "-Wunused-private-field"
        #pragma clang diagnostic ignored "-Wunused-function"
        #pragma clang diagnostic ignored "-Wunused-variable"
    #endif /* !LAL_DO_ENABLE_SHIPPING_WARNINGS */
#endif /* LAL_DO_COMPILER_DIAGNOSTIC_SETUP */


/*-----------------------------------------------------------------------------
    Define platform specific macros.
-----------------------------------------------------------------------------*/

#ifndef LAL_UNLIKELY
    #define LAL_UNLIKELY(Expr)                                          (__builtin_expect(!!(Expr), 0))
#endif /* !LAL_UNLIKELY */

#ifndef LAL_LIKELY
    #define LAL_LIKELY(Expr)                                            (__builtin_expect(!!(Expr), 1))
#endif /* !LAL_LIKELY */

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

#ifndef LAL_PLATFORM_USES_UTF8
    #define LAL_PLATFORM_USES_UTF8                                      1
#endif /* !LAL_PLATFORM_USES_UTF8 */

#ifndef LAL_PLATFORM_WCHAR_SIZE
    #define LAL_PLATFORM_WCHAR_SIZE                                     4
#endif /* !LAL_PLATFORM_WCHAR_SIZE */

#ifndef LAL_PLATFORM_CALLSPEC_IN
    #define LAL_PLATFORM_CALLSPEC_IN                                    __attribute__ ((visibility ("default")))
#endif /* !LAL_PLATFORM_CALLSPEC_IN */

#ifndef LAL_PLATFORM_CALLSPEC_OUT
    #define LAL_PLATFORM_CALLSPEC_OUT                                   __attribute__ ((visibility ("default")))
#endif /* !LAL_PLATFORM_CALLSPEC_OUT */

#ifndef LAL_PLATFORM_EXTERNSPEC_IN
    #define LAL_PLATFORM_EXTERNSPEC_IN                                  extern
#endif /* !LAL_PLATFORM_EXTERNSPEC_IN */

#ifndef LAL_PLATFORM_EXTERNSPEC_OUT
    #define LAL_PLATFORM_EXTERNSPEC_OUT
#endif /* !LAL_PLATFORM_EXTERNSPEC_OUT */

//# https://man7.org/linux/man-pages/man3/realpath.3.html ???
#ifndef LAL_PLATFORM_MAX_PATH
    #define LAL_PLATFORM_MAX_PATH                                       300
#endif /* LAL_PLATFORM_MAX_PATH */

#ifndef LAL_PLATFORM_NO_DISCARD_CTRL_PATH
    #define LAL_PLATFORM_NO_DISCARD_CTRL_PATH                           (__asm__ __volatile__ ("nop"))
#endif /* !LAL_PLATFORM_NO_DISCARD_CTRL_PATH */

#ifndef LAL_PLATFORM_BREAK
    #if __has_builtin(__builtin_debugtrap)
        #define LAL_PLATFORM_BREAK()                                    (__builtin_debugtrap());
    #else /* __has_builtin(__builtin_debugtrap) */
        #define LAL_PLATFORM_BREAK()                                    (raise(SIGTRAP));
    #endif /* !__has_builtin(__builtin_debugtrap) */
#endif  /* !LAL_PLATFORM_BREAK */

#ifndef LAL_PLATFORM_TRAP
    #if __has_builtin(__builtin_trap)
        #define LAL_PLATFORM_TRAP()                                     (__builtin_trap());
    #else /* __has_builtin(__builtin_trap) */
        #error "Encountered unimplemented code path." /* ? How. */
    #endif /* !__has_builtin(__builtin_trap) */
#endif /* !LAL_PLATFORM_TRAP */

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

namespace Lal
{

struct LOnPlatformBreakLinux final
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
