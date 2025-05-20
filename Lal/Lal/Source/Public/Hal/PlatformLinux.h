// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_LINUX
    #error "Wanted to override generic platform types with linux specific types, but platform is not linux."
#endif /* !PLATFORM_LINUX */

#if __cplusplus < 202002L
    #error "Program requires at least C++20."
#endif /* __cplusplus < 202002L */

struct  LGenericPlatformTypes;
struct  LLinuxPlatformBreakDefines;
struct  LLinuxPlatformTypes;
typedef LLinuxPlatformTypes LPlatformTypes;

struct LLinuxPlatformTypes final : public LGenericPlatformTypes
{
};

#ifndef WITH_CLANG
    #define WITH_CLANG                          1
#endif /* !WITH_CLANG */

#pragma clang diagnostic error "-Wpragmas"
#pragma clang diagnostic error "-Wunknown-pragmas"
#pragma clang diagnostic error "-Wbuiltin-macro-redefined"
#pragma clang diagnostic error "-Wunknown-warning-option"
#pragma clang diagnostic error "-Winconsistent-missing-override"
#pragma clang diagnostic error "-Wunused-lambda-capture"
#pragma clang diagnostic error "-Wreturn-type"
#pragma clang diagnostic error "-Wmacro-redefined"
#pragma clang diagnostic error "-Wundefined-inline"
#pragma clang diagnostic error "-Wmismatched-new-delete"
#pragma clang diagnostic error "-Wswitch"
#pragma clang diagnostic ignored "-Wundefined-var-template" // ??? Why
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wcomment"
#pragma clang diagnostic ignored "-Wcomments"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wlogical-op-parentheses"

#ifdef NOINLINE
    #error "NOINLINE is already defined."
#endif /* NOINLINE */
#define NOINLINE                                __attribute__ ((noinline))

#ifdef FORCEINLINE
    #error "FORCEINLINE is already defined."
#endif /* FORCEINLINE */
#if IN_DEBUG
    /*
     * Inlining is disabled in debug builds as following the debugger through inlined code is a pain
     * in the ass.
     */
    #define FORCEINLINE                         inline
#else /* IN_DEBUG */
    #define FORCEINLINE                         __attribute__ ((always_inline))
#endif /* !IN_DEBUG */

#ifdef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is already defined."
#endif /* PLATFORM_MAX_PATH */
#define PLATFORM_MAX_PATH                       300 // https://man7.org/linux/man-pages/man3/realpath.3.html???

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

#ifdef PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH
    #error "PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH is already defined."
#endif /* PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH */
#define PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH() \
    __asm__ __volatile__ ("nop")

#ifdef PLATFORM_BREAK
    #error "PLATFORM_BREAK is already defined."
#endif /* PLATFORM_BREAK */
// When using gdb?:
//      __asm__ __volatile__ ("int3");
// How tf would we just break the debugger? Now the process is just trapped. We do not want that.
#define PLATFORM_BREAK()                                      \
    {{                                                        \
        {                                                     \
            LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()        \
        }                                                     \
        [](void) -> void                                      \
        {                                                     \
            PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH(); \
            __builtin_trap();                                 \
            PLATFORM_DO_NOT_DISCARD_RESULTING_CONTROL_PATH(); \
        }();                                                  \
    }}

#ifdef PLATFORM_ERROR_BREAK
#error "PLATFORM_ERROR_BREAK is already definded."
#endif /* PLATFORM_ERROR_BREAK */
#define PLATFORM_ERROR_BREAK(InMessage)                           \
    PLATFORM_ERROR_BREAK_WITH_BODY(InMessage, __FILE__, __LINE__)

extern "C"
{

extern void __assert_fail
(
    const char *__assertion,
    const char *__file,
    unsigned int __line,
    const char *__function
)
noexcept (true) __attribute__ ((__noreturn__)); // __attribute__ ((__cold));

} /* extern "C" */

#ifdef PLATFORM_ERROR_BREAK_WITH_BODY
    #error "PLATFORM_ERROR_BREAK_WITH_BODY is already definded."
#endif /* PLATFORM_ERROR_BREAK_WITH_BODY */
#define PLATFORM_ERROR_BREAK_WITH_BODY(InMessage, InFile, InLine)                       \
    {{                                                                                  \
        {                                                                               \
            LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()                                  \
        }                                                                               \
        {                                                                               \
            {                                                                           \
                __assert_fail(InMessage.c_str(), InFile.c_str(), InLine, __FUNCTION__); \
            }                                                                           \
        }                                                                               \
    }}

#ifdef PLATFORM_PANIC_BREAK
    #error "PLATFORM_PANIC_BREAK is already definded."
#endif /* PLATFORM_PANIC_BREAK */
#define PLATFORM_PANIC_BREAK(InMessage)                           \
    PLATFORM_PANIC_BREAK_WITH_BODY(InMessage, __FILE__, __LINE__)

#ifdef PLATFORM_PANIC_BREAK_WITH_BODY
    #error "PLATFORM_PANIC_BREAK_WITH_BODY is already definded."
#endif /* PLATFORM_PANIC_BREAK_WITH_BODY */
#define PLATFORM_PANIC_BREAK_WITH_BODY(InMessage, InFile, InLine)          \
    LLinuxPlatformBreakDefines::OnProgramPanic(InMessage, InFile, InLine);

#define PLATFORM_CALLSPEC_OUT           __attribute__ ((visibility ("default")))
#define PLATFORM_CALLSPEC_IN            __attribute__ ((visibility ("default")))
#define PLATFORM_EXTERNSPEC_OUT
#define PLATFORM_EXTERNSPEC_IN          extern

/*
 * Default to little endian.
 * But we should assert this 100% at runtime with something like htons() or nthohl().
 */
#ifdef PLATFORM_USES_LITTLE_ENDIAN
    #error "PLATFORM_USES_LITTLE_ENDIAN is already defined."
#endif /* PLATFORM_USES_LITTLE_ENDIAN */
#define PLATFORM_USES_LITTLE_ENDIAN                 1

#ifdef PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    #error "PLATFORM_USES_GLFW3_ABSTRACTION_LAYER is already defined."
#endif /* PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
#define PLATFORM_USES_GLFW3_ABSTRACTION_LAYER       1

#ifdef PLATFORM_WCHAR_SIZE
    #error "PLATFORM_WCHAR_SIZE is already defined."
#endif /* PLATFORM_WCHAR_SIZE */
#define PLATFORM_WCHAR_SIZE                         4

#ifdef PLATFORM_USES_32_BIT
    #error "PLATFORM_USES_32_BIT is already defined."
#endif /* PLATFORM_USES_32_BIT */
#ifdef PLATFORM_USES_64_BIT
    #error "PLATFORM_USES_64_BIT is already defined."
#endif /* PLATFORM_USES_64_BIT */
#define PLATFORM_USES_64_BIT                        1

#ifdef PLATFORM_USES_UTF8
    #error "PLATFORM_USES_UTF8 is already defined."
#endif /* PLATFORM_USES_UTF8 */
#define PLATFORM_USES_UTF8                          1

struct LLinuxPlatformBreakDefines final
{
    NORETURN NOINLINE
    static void OnProgramPanic(const std::string& InMessage, const std::string& InFile, const LLinuxPlatformTypes::i32 InLine);
};

#include <cmath>
#include <cstring>
