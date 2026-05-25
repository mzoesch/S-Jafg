// Copyright mzoesch. All rights reserved.

#pragma once


/*-----------------------------------------------------------------------------
    Common types for almost all compilers and platforms.
-----------------------------------------------------------------------------*/

namespace Jafg::Detail
{

struct LPrimitivePlatformTypesGeneric
{
    typedef std::uint8_t  u8;
    typedef std::uint16_t u16;
    typedef std::uint32_t u32;
    typedef std::uint64_t u64;
    typedef std::int8_t   i8;
    typedef std::int16_t  i16;
    typedef std::int32_t  i32;
    typedef std::int64_t  i64;

    typedef float  f32;
    typedef double f64;

    //# Native char.
    typedef char LChar;
};

} /* ~Namespace Jafg::Detail */

#ifndef JAFG_WITH_GCC
    #if (!!__GNUC__) && !(__clang__)
        #define JAFG_WITH_GCC               1
    #endif /* !!__GNUC__ && !(__clang__)) */
#endif /* !JAFG_WITH_GCC */
#ifndef JAFG_WITH_MSVC
    #if _MSC_VER
        #define JAFG_WITH_MSVC              1
    #endif /* _MSC_VER */
#endif /* !JAFG_WITH_MSVC */
#ifndef JAFG_WITH_CLANG
    #if __clang__
        #define JAFG_WITH_CLANG             1
    #endif /* __clang__ */
#endif /* !JAFG_WITH_CLANG */


/*-----------------------------------------------------------------------------
    Define supported platforms.
-----------------------------------------------------------------------------*/

#ifndef JAFG_PLATFORM_LINUX
    #define JAFG_PLATFORM_LINUX              0
#endif /* !JAFG_PLATFORM_LINUX */
#ifndef JAFG_PLATFORM_WASM
    #define JAFG_PLATFORM_WASM               0
#endif /* !JAFG_PLATFORM_WASM */
#ifndef JAFG_PLATFORM_WINDOWS
    #define JAFG_PLATFORM_WINDOWS            0
#endif /* !JAFG_PLATFORM_WINDOWS */

#if !(JAFG_PLATFORM_LINUX || JAFG_PLATFORM_WASM || JAFG_PLATFORM_WINDOWS)
    #error "No platforms specified."
#endif /* !(JAFG_PLATFORM_LINUX || JAFG_PLATFORM_WASM || JAFG_PLATFORM_WINDOWS) */
#if JAFG_PLATFORM_LINUX
    #if JAFG_PLATFORM_WASM || JAFG_PLATFORM_WINDOWS
        #error "Multiple platforms specified."
    #endif /* JAFG_PLATFORM_WASM || JAFG_PLATFORM_WINDOWS */
#endif /* JAFG_PLATFORM_LINUX */
#if JAFG_PLATFORM_WASM
    #if JAFG_PLATFORM_LINUX || JAFG_PLATFORM_WINDOWS
        #error "Multiple platforms specified."
    #endif /* JAFG_PLATFORM_LINUX || JAFG_PLATFORM_WINDOWS */
#endif /* JAFG_PLATFORM_WASM */
#if JAFG_PLATFORM_WINDOWS
    #if JAFG_PLATFORM_LINUX || JAFG_PLATFORM_WASM
        #error "Multiple platforms specified."
    #endif /* JAFG_PLATFORM_LINUX || JAFG_PLATFORM_WASM */
#endif /* JAFG_PLATFORM_WINDOWS */


/*-----------------------------------------------------------------------------
    Let platform override platform based macros and types.
-----------------------------------------------------------------------------*/

#ifndef DETAIL_JAFG_CPLUSPLUS
    #error "DETAIL_JAFG_CPLUSPLUS is not defined."
#endif /* DETAIL_JAFG_CPLUSPLUS */

#if JAFG_PLATFORM_LINUX
    #include "Hal/PlatformLinux.h"
#elif JAFG_PLATFORM_WASM
    #include "Hal/PlatformWasm.h"
#elif JAFG_PLATFORM_WINDOWS
    #include "Hal/PlatformWin.h"
#else /* JAFG_PLATFORM_WINDOWS */
    #error "Failed to resolve platform."
#endif /* !JAFG_PLATFORM_WINDOWS */


/*-----------------------------------------------------------------------------
    Implicitly generated macros.
-----------------------------------------------------------------------------*/

#ifndef JAFG_PLATFORM_DESKTOP
    #define JAFG_PLATFORM_DESKTOP                                        0
#endif /* JAFG_PLATFORM_DESKTOP */

#ifndef JAFG_WITH_GCC
    #define JAFG_WITH_GCC                                                0
#endif /* !JAFG_WITH_GCC */
#ifndef JAFG_WITH_MSVC
    #define JAFG_WITH_MSVC                                               0
#endif /* !JAFG_WITH_MSVC */
#ifndef JAFG_WITH_CLANG
    #define JAFG_WITH_CLANG                                              0
#endif /* !JAFG_WITH_CLANG */
#if !JAFG_WITH_GCC && !JAFG_WITH_MSVC && !JAFG_WITH_CLANG
    #error "No compiler spcified."
#endif /* JAFG_WITH_GCC || JAFG_WITH_MSVC || JAFG_WITH_CLANG */
#if (JAFG_WITH_GCC + JAFG_WITH_MSVC + JAFG_WITH_CLANG) > 1
    #error "Multiple compilers specified."
#endif /* (JAFG_WITH_GCC + JAFG_WITH_MSVC + JAFG_WITH_CLANG) > 1 */

#if JAFG_WITH_GCC
    #if !(__GNUC__)
        #error "JAFG_WITH_GCC is set but compiler is not GCC."
    #endif /* !(__GNUC__ && !(__clang__)) */
#endif /* JAFG_WITH_GCC */
#if JAFG_WITH_MSVC
    #if !(defined(_MSC_VER))
        #error "JAFG_WITH_MSVC is set but compiler is not MSVC."
    #endif /* !(defined(_MSC_VER)) */
#endif /* JAFG_WITH_MSVC */
#if JAFG_WITH_CLANG
    #if !(__clang__)
    #error "JAFG_WITH_CLANG is set but compiler is not Clang."
    #endif /* !(__clang__) */
#endif /* JAFG_WITH_CLANG */

//# Whether the platform uses the Posix Api.
#ifndef JAFG_PLATFORM_USES_POSIX
    #define JAFG_PLATFORM_USES_POSIX                                     (!JAFG_PLATFORM_WINDOWS)
#endif /* !JAFG_PLATFORM_USES_POSIX */

//# Branch prediction hint: Is this boolean expression likely to be true?
#ifndef JAFG_UNLIKELY
    #define JAFG_UNLIKELY(Expr)                                          (!!(Expr))
#endif /* !JAFG_UNLIKELY */

//# Branch prediction hint: Is this boolean expression likely to be false?
#ifndef JAFG_LIKELY
    #define JAFG_LIKELY(Expr)                                            (!!(Expr))
#endif /* !JAFG_LIKELY */

//# Whether to include and link glad. Some compilers may precede this.
#ifndef JAFG_NO_GLAD /* Part of Jafg and not JAFG. Therefore, this is in the Jafg namespace. */
    #define JAFG_NO_GLAD                                                1
#endif /* !JAFG_NO_GLAD */

//# Whether to include and link glfw3. Some compilers may precede this.
#ifndef JAFG_NO_GLFW3 /* Part of Jafg and not JAFG. Therefore, this is in the Jafg namespace. */
    #define JAFG_NO_GLFW3                                               0
#endif /* !JAFG_NO_GLFW3 */

//# Whether to include and link freetype. Some compilers may precede this.
#ifndef JAFG_NO_FREETYPE /* Part of Jafg and not JAFG. Therefore, this is in the Jafg namespace. */
    #define JAFG_NO_FREETYPE                                            0
#endif /* !JAFG_NO_FREETYPE */

//# Whether to use the WebGL2 API.
#ifndef JAFG_PLATFORM_USES_WEBGL_TWO /* Part of Jafg and not JAFG. Therefore, this is in the Jafg namespace. */
    #define JAFG_PLATFORM_USES_WEBGL_TWO                                0
#endif /* JAFG_PLATFORM_USES_WEBGL_TWO */

//# Whether this platform uses the GLFW3 abstraction layer under the hood.
#ifndef JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER /* Part of Jafg and not JAFG. Therefore, this is in the Jafg namespace. */
    #define JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER                  0
#endif /* !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */

//# Whether this platform uses JavaScript as its frontend.
#ifndef JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND /* Part of Jafg and not JAFG. Therefore, this is in the Jafg namespace. */
    #define JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND                     0
#endif /* !JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND */

#if JAFG_AS_CLIENT
    #if !(JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER || JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND)
        #error "No frontend layer specified."
    #endif /* !(JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER || JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND) */
    #if (JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER && JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND)
        #error "Multiple frontend layers specified."
    #endif /* (JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER && JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND) */
#else /* JAFG_AS_CLIENT */
    #if (JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER || JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND)
        #error "Frontend layer specified but not in client build."
    #endif /* (JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER || JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND) */
#endif /* !JAFG_AS_CLIENT */

//# Whether to compile with the ReST command line system.
#ifndef JAFG_WITH_REST_CLS
    #define JAFG_WITH_REST_CLS                                           0
#endif /* !JAFG_WITH_REST_CLS */

//# Whether the used compiler supports extern template specifications.
#ifndef JAFG_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS
    #define JAFG_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS        0
#endif /* !JAFG_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS */

//# Whether this platform uses 64-bit integers as pointers.
#ifndef JAFG_PLATFORM_USES_64_BIT
    #define JAFG_PLATFORM_USES_64_BIT                                    0
#endif /* JAFG_PLATFORM_USES_64_BIT */

//# Whether this platform uses 32-bit integers as pointers.
#ifndef JAFG_PLATFORM_USES_32_BIT
    #define JAFG_PLATFORM_USES_32_BIT                                    0
#endif /* JAFG_PLATFORM_USES_32_BIT */

#if !(JAFG_PLATFORM_USES_64_BIT || JAFG_PLATFORM_USES_32_BIT)
    #error "No pointer size specified."
#endif /* !(JAFG_PLATFORM_USES_64_BIT || JAFG_PLATFORM_USES_32_BIT) */
#if (JAFG_PLATFORM_USES_64_BIT && JAFG_PLATFORM_USES_32_BIT)
    #error "Multiple pointer sizes specified."
#endif /* (JAFG_PLATFORM_USES_64_BIT && JAFG_PLATFORM_USES_32_BIT) */

//# Whether this platform uses the little endian byte order.
#ifndef JAFG_PLATFORM_USES_LITTLE_ENDIAN
    #ifdef JAFG_PLATFORM_USES_BIG_ENDIAN
        #define JAFG_PLATFORM_USES_LITTLE_ENDIAN                         (!JAFG_PLATFORM_USES_BIG_ENDIAN)
    #else /* JAFG_PLATFORM_USES_BIG_ENDIAN */
        #error "Platform is missing JAFG_PLATFORM_USES_LITTLE_ENDIAN definition."
    #endif /* !JAFG_PLATFORM_USES_BIG_ENDIAN */
#endif /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */

//# Whether this platform uses the big endian byte order.
#ifndef JAFG_PLATFORM_USES_BIG_ENDIAN
    #ifdef JAFG_PLATFORM_USES_LITTLE_ENDIAN
        #define  JAFG_PLATFORM_USES_BIG_ENDIAN                           (!JAFG_PLATFORM_USES_LITTLE_ENDIAN)
    #else /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
        #error "Platform is missing  JAFG_PLATFORM_USES_BIG_ENDIAN definition."
    #endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN */
#endif /*  JAFG_PLATFORM_USES_BIG_ENDIAN */

#if !JAFG_PLATFORM_USES_LITTLE_ENDIAN && !JAFG_PLATFORM_USES_BIG_ENDIAN
    #error "No endianness specified."
#endif /* !JAFG_PLATFORM_USES_LITTLE_ENDIAN && !JAFG_PLATFORM_USES_BIG_ENDIAN */
#if (JAFG_PLATFORM_USES_LITTLE_ENDIAN && JAFG_PLATFORM_USES_BIG_ENDIAN)
    #error "Multiple endianness specified."
#endif /* (JAFG_PLATFORM_USES_LITTLE_ENDIAN && JAFG_PLATFORM_USES_BIG_ENDIAN) */

//#
//# If no, the platform tries to compile all libraries as static libraries or make a monolithic unity build.
//# This will not allow for dynamic linking at runtime, and therefore all plugins must be present at compile time.
//# Generated translation units for _all_ modules may be deferred and then compiled and linked directly into
//# the main executable.
//# Exact behavior over one module in monolithic unity builds has to be discussed and defined in the build
//# script for said module.
//#
#ifndef JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #define JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES                      0
#endif /* !JAFG_PLATFORM_SUPPORTS_SHARED_LIBRARIES */

//# Whether this platform's standard output buffer stream supports flushing.
#ifndef JAFG_PLATFORM_SUPPORTS_STD_FLUSH
    #define JAFG_PLATFORM_SUPPORTS_STD_FLUSH                             0
#endif /* !JAFG_PLATFORM_SUPPORTS_STD_FLUSH */

//# Whether the platform's standard output buffer stream supports ANSI escape codes.
#ifndef JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES
    #define JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES                          0
#endif /* !JAFG_PLATFORM_SUPPORTS_ANSI_ESCAPES */

//# Whether logs are saved in memory for later retrieval.
#ifndef JAFG_SAVE_LOGS_IN_MEMORY
    #define JAFG_SAVE_LOGS_IN_MEMORY                                     JAFG_WITH_REST_CLS
#endif /* !JAFG_SAVE_LOGS_IN_MEMORY */

//# Whether the platform supports Single-Instruction-Multiple-Data intrinsics.
#ifndef JAFG_PLATFORM_SUPPORTS_SIMD
    #define JAFG_PLATFORM_SUPPORTS_SIMD                                  0
#endif /* !JAFG_PLATFORM_SUPPORTS_SIMD */

//# Whether the platform supports C++23 stacktrace in the stl library.
#ifndef JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE
    #define JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE                        0
#endif /* !JAFG_PLATFORM_SUPPORTS_STD_STACKTRACE */

//#
//# Whether allocated memory can be shrunk by platform intrinsics or memory shrinking is performed by allocating
//# a new smaller bulk of memory, copying the data and freeing the old memory.
//# Not that, generally speaking, on platforms that support memory shrinking, the memory might still be
//# reallocated if the underlying platform thinks it is necessary. JAFG does not provide any guarantees for that.
//#
#ifndef JAFG_PLATFORM_SUPPORTS_MEMORY_SHRINK
    #define JAFG_PLATFORM_SUPPORTS_MEMORY_SHRINK                         0
#endif /* !JAFG_PLATFORM_SUPPORTS_MEMORY_SHRINK */

//# Whether this platform uses the generic platform-agnostic loop provided by the runtime library of Jafg.
#ifndef JAFG_PLATFORM_USES_NON_GENERIC_LOOP
    #define JAFG_PLATFORM_USES_NON_GENERIC_LOOP                          0
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_LOOP */
#if JAFG_PLATFORM_USES_NON_GENERIC_LOOP
    #ifndef JAFG_PLATFORM_GUARDED_LOOP
        #error "JAFG_PLATFORM_GUARDED_LOOP is not defined."
    #endif /* !JAFG_PLATFORM_GUARDED_LOOP */
#else /* JAFG_PLATFORM_USES_NON_GENERIC_LOOP */
    #ifdef JAFG_PLATFORM_GUARDED_LOOP
        #error "JAFG_PLATFORM_GUARDED_LOOP is defined but platform does use the generic platform-agnostic loop."
    #endif /* JAFG_PLATFORM_GUARDED_LOOP */
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_LOOP */

//#
//# Whether this platform uses the generic platform-agnostic exit procedure provided by the runtime library of Jafg.
//# @note The default runtime library uses a Resource-Acquisition-Is-Initialization-based approach to exit the program.
//#       If your platform does not support clean defined destructor behavior, another non-trivial exit procedure
//#       must be used.
#ifndef JAFG_PLATFORM_USES_NON_GENERIC_EXIT
    #define JAFG_PLATFORM_USES_NON_GENERIC_EXIT                          0
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_EXIT */

//# Whether this platform uses UTF-8, UTF-16 or UTF-32 as its native character encoding.
#ifndef JAFG_PLATFORM_USES_UTF8
    #define JAFG_PLATFORM_USES_UTF8                                      0
#endif /* !JAFG_PLATFORM_USES_UTF8 */
#ifndef JAFG_PLATFORM_USES_UTF16
    #define JAFG_PLATFORM_USES_UTF16                                     0
#endif /* !JAFG_PLATFORM_USES_UTF16 */
#ifndef JAFG_PLATFORM_USES_UTF32
    #define JAFG_PLATFORM_USES_UTF32                                     0
#endif /* !JAFG_PLATFORM_USES_UTF32 */

#if !(JAFG_PLATFORM_USES_UTF8 || JAFG_PLATFORM_USES_UTF16 || JAFG_PLATFORM_USES_UTF32)
    #error "No native characther endocing specified."
#endif /* !(JAFG_PLATFORM_USES_UTF8 || JAFG_PLATFORM_USES_UTF16 || JAFG_PLATFORM_USES_UTF32) */
#if JAFG_PLATFORM_USES_UTF8
    #if JAFG_PLATFORM_USES_UTF16 || JAFG_PLATFORM_USES_UTF32
        #error "Multiple native characther endocing specified."
    #endif /* JAFG_PLATFORM_USES_UTF16 || JAFG_PLATFORM_USES_UTF32 */
#endif /* JAFG_PLATFORM_USES_UTF8 */
#if JAFG_PLATFORM_USES_UTF16
    #if JAFG_PLATFORM_USES_UTF8 || JAFG_PLATFORM_USES_UTF32
        #error "Multiple native characther endocing specified."
    #endif /* JAFG_PLATFORM_USES_UTF32 */
#endif /* JAFG_PLATFORM_USES_UTF16 */
#if JAFG_PLATFORM_USES_UTF32
    #if JAFG_PLATFORM_USES_UTF8 || JAFG_PLATFORM_USES_UTF16
        #error "Multiple native characther endocing specified."
    #endif /* JAFG_PLATFORM_USES_UTF16 */
#endif /* JAFG_PLATFORM_USES_UTF32 */

#if JAFG_PLATFORM_USES_UTF8
    #if JAFG_PLATFORM_USES_UTF16
        #error "JAFG_PLATFORM_USES_UTF8 and JAFG_PLATFORM_USES_UTF16 are both defined."
    #endif /* JAFG_PLATFORM_USES_UTF16 */
    #if PLATFORM_USES_UTF32
        #error "JAFG_PLATFORM_USES_UTF8 and JAFG_PLATFORM_USES_UTF32 are both defined."
    #endif /* JAFG_PLATFORM_USES_UTF32 */
#endif /* JAFG_PLATFORM_USES_UTF8 */
#if JAFG_PLATFORM_USES_UTF16
    #if JAFG_PLATFORM_USES_UTF8
        #error "JAFG_PLATFORM_USES_UTF16 and JAFG_PLATFORM_USES_UTF8 are both defined."
    #endif /* JAFG_PLATFORM_USES_UTF8 */
    #if JAFG_PLATFORM_USES_UTF32
        #error "JAFG_PLATFORM_USES_UTF16 and JAFG_PLATFORM_USES_UTF32 are both defined."
    #endif /* JAFG_PLATFORM_USES_UTF32 */
#endif /* JAFG_PLATFORM_USES_UTF16 */
#if JAFG_PLATFORM_USES_UTF32
    #if JAFG_PLATFORM_USES_UTF8
        #error "JAFG_PLATFORM_USES_UTF32 and JAFG_PLATFORM_USES_UTF8 are both defined."
    #endif /* JAFG_PLATFORM_USES_UTF8 */
    #if JAFG_PLATFORM_USES_UTF16
        #error "JAFG_PLATFORM_USES_UTF32 and JAFG_PLATFORM_USES_UTF16 are both defined."
    #endif /* JAFG_PLATFORM_USES_UTF16 */
#endif /* JAFG_PLATFORM_USES_UTF32 */
#if !JAFG_PLATFORM_USES_UTF8 && !JAFG_PLATFORM_USES_UTF16 && !JAFG_PLATFORM_USES_UTF32
    #error "No platform encoding is defined."
#endif /* !JAFG_PLATFORM_USES_UTF8 && !JAFG_PLATFORM_USES_UTF16 && !JAFG_PLATFORM_USES_UTF32 */

//# The literal wide prefix for the current platform.
#ifndef LITERAL_WIDE /* Compiler-dependent feature. Evaluates to a correct macro for almost all compilers and platforms. */
    #define LITERAL_WIDE(x)                                             L##x
#endif /* !LITERAL_WIDE */

//# The literal UTF-8 prefix for the current platform.
#ifndef LITERAL_UTF8 /* Compiler-dependent feature. Evaluates to a correct macro for almost all compilers and platforms. */
    #define LITERAL_UTF8(x)                                             u8##x
#endif /* !LITERAL_UTF8 */

//# The literal UTF-16 prefix for the current platform.
#ifndef LITERAL_UTF16 /* Compiler-dependent feature. Evaluates to a correct macro for almost all compilers and platforms. */
    #define LITERAL_UTF16(x)                                            u##x
#endif /* !LITERAL_UTF16 */

//# The literal UTF-32 prefix for the current platform.
#ifndef LITERAL_UTF32 /* Compiler-dependent feature. Evaluates to a correct macro for almost all compilers and platforms. */
    #define LITERAL_UTF32(x)                                            U##x
#endif /* !LITERAL_UTF32 */

//# The literal for native platform exchange.
#ifndef LITERAL_TEXT
    #error "Platform is missing LITERAL_TEXT definition."
#endif /* !LITERAL_TEXT */

//# The call spec for imported functions.
#ifndef JAFG_PLATFORM_CALLSPEC_IN
    #error "Platform is missing JAFG_PLATFORM_CALLSPEC_IN definition."
#endif /* !JAFG_PLATFORM_CALLSPEC_IN */

//# The call spec for exported functions.
#ifndef JAFG_PLATFORM_CALLSPEC_OUT
    #error "Platform is missing JAFG_PLATFORM_CALLSPEC_OUT definition."
#endif /* !JAFG_PLATFORM_CALLSPEC_OUT */

//# The extern call spec for imported functions.
#ifndef JAFG_PLATFORM_EXTERNSPEC_IN
    #error "Platform is missing JAFG_PLATFORM_EXTERNSPEC_IN definition."
#endif /* !JAFG_PLATFORM_EXTERNSPEC_IN */

//# The extern call spec for exported functions.
#ifndef JAFG_PLATFORM_EXTERNSPEC_OUT
    #error "Platform is missing JAFG_PLATFORM_EXTERNSPEC_OUT definition."
#endif /* !JAFG_PLATFORM_EXTERNSPEC_OUT */

//# The maximal path length for the current platform.
#ifndef JAFG_PLATFORM_MAX_PATH
    #error "Platform is missing JAFG_PLATFORM_MAX_PATH definition."
#endif /* !JAFG_PLATFORM_MAX_PATH */

//# The finder implementation strategy.
#ifndef JAFG_PLATFORM_USES_STD_FINDER
    #error "Platform is missing JAFG_PLATFORM_USES_STD_FINDER definition."
#endif /* JAFG_PLATFORM_USES_STD_FINDER */

//# The maximal number of frames that can be stored when tracing.
#ifndef JAFG_PLATFORM_MAX_FRAMES
    #define JAFG_PLATFORM_MAX_FRAMES                                    128
#endif /* JAFG_PLATFORM_MAX_FRAMES */

//# This tells the compiler to never discard the resulting control path.
#ifndef JAFG_PLATFORM_NO_DISCARD_CTRL_PATH
    #error "Platform is missing JAFG_PLATFORM_NO_DISCARD_CTRL_PATH definition."
#endif /* !JAFG_PLATFORM_NO_DISCARD_CTRL_PATH */

//#
//# Only gets called if a debugger is watching us. If no debugger is attached, the behavior of this macro is
//# undefined.
//# Usually if no debugger is attached, the program (when in a defined and good state) will call the JAFG_PLATFORM_TRAP
//# macro instead.
//# This macro has to allow for continuing either within the debugger or in the program itself.
//#
#ifndef JAFG_PLATFORM_BREAK
    #error "Platform is missing JAFG_PLATFORM_BREAK definition."
#endif /* !JAFG_PLATFORM_BREAK */

//# Trap this process.
#ifndef JAFG_PLATFORM_TRAP
    #error "Platform is missing JAFG_PLATFORM_TRAP definition."
#endif /* !JAFG_PLATFORM_TRAP */

//# Tells the compiler to never inline a function or method.
#ifndef NOINLINE
    #error "Platform is missing NOINLINE definition."
#endif /* !NOINLINE */

//# Tells the compiler to always inline a function or method.
#ifndef FORCEINLINE
    #if JAFG_IN_DEBUG
        //#
        //# Inlining is disabled in debug builds as following the debugger through inlined code is a pain
        //# in the ass.
        //#
        #define FORCEINLINE                                             inline
    #else /* JAFG_IN_DEBUG */
        #error "Platform is missing FORCEINLINE definition."
    #endif /* !JAFG_IN_DEBUG */
#endif /* !FORCEINLINE */

#ifndef NODISCARD /* Compiler-dependent feature. Evaluates to a correct macro for almost all compilers and platforms. */
    #define NODISCARD                                                   [[nodiscard]]
#endif /* !NODISCARD */

#ifndef UNUSED /* Compiler-dependent feature. Evaluates to a correct macro for almost all compilers and platforms. */
    #define UNUSED                                                      [[maybe_unused]]
#endif /* !UNUSED */

#ifndef NORETURN /* Compiler-dependent feature. Evaluates to a correct macro for almost all compilers and platforms. */
    #define NORETURN                                                    [[noreturn]]
#endif /* !NORETURN */

#ifndef RESTRICT /* Compiler-dependent feature. Evaluates to a correct macro for almost all compilers and platforms. */
    #define RESTRICT                                                    __restrict
#endif /* !RESTRICT */


/*-----------------------------------------------------------------------------
    Generalize types based on the specific platform used.
-----------------------------------------------------------------------------*/

namespace Jafg::Detail
{

//# Is true if RTTI is enabled for the current compiler.
inline constexpr bool IsRttiEnabled
{
#if JAFG_WITH_CLANG
    __has_feature(cxx_rtti)
#elif WITH_GCC
    __GXX_RTTI
#elif JAFG_WITH_MSVC
    #ifdef _CPPRTTI
        true
    #else /* _CPPRTTI */
        false
    #endif /* !_CPPRTTI */
#else /* JAFG_WITH_MSVC */
    /* Assume it is enabled by default. */
    true
#endif /* !JAFG_WITH_MSVC */
};
static_assert(!IsRttiEnabled, "No, stupid feature.");

#if JAFG_PLATFORM_USES_LITTLE_ENDIAN
    static_assert(std::endian::native == std::endian::little);
    static_assert(std::endian::native != std::endian::big);
#endif /* JAFG_PLATFORM_USES_LITTLE_ENDIAN */
#if JAFG_PLATFORM_USES_BIG_ENDIAN
static_assert(std::endian::native == std::endian::big);
static_assert(std::endian::native != std::endian::little);
#endif /* JAFG_PLATFORM_USES_BIG_ENDIAN */

template<typename T>
concept IsPrimitivePlatformTypesValid = requires
{
    typename T::u8;
    typename T::u16;
    typename T::u32;
    typename T::u64;
    typename T::i8;
    typename T::i16;
    typename T::i32;
    typename T::i64;

    typename T::f32;
    typename T::f64;

    typename T::LChar;
};
static_assert(IsPrimitivePlatformTypesValid<LPrimitivePlatformTypes>);

static_assert(sizeof(LPrimitivePlatformTypes::u8)  == 1);
static_assert(sizeof(LPrimitivePlatformTypes::u16) == 2);
static_assert(sizeof(LPrimitivePlatformTypes::u32) == 4);
static_assert(sizeof(LPrimitivePlatformTypes::u64) == 8);
static_assert(sizeof(LPrimitivePlatformTypes::i8)  == 1);
static_assert(sizeof(LPrimitivePlatformTypes::i16) == 2);
static_assert(sizeof(LPrimitivePlatformTypes::i32) == 4);
static_assert(sizeof(LPrimitivePlatformTypes::i64) == 8);

static_assert(sizeof(LPrimitivePlatformTypes::f32) == 4);
static_assert(sizeof(LPrimitivePlatformTypes::f64) == 8);

#if JAFG_PLATFORM_USES_64_BIT
    static_assert(sizeof(void*) == sizeof(LPrimitivePlatformTypes::u64));
    static_assert(sizeof(void*) == sizeof(LPrimitivePlatformTypes::i64));
#endif /* JAFG_PLATFORM_USES_64_BIT */
#if JAFG_PLATFORM_USES_32_BIT
    static_assert(sizeof(void*) == sizeof(LPrimitivePlatformTypes::u32));
    static_assert(sizeof(void*) == sizeof(LPrimitivePlatformTypes::i32));
#endif /* JAFG_PLATFORM_USES_32_BIT */

} /* ~Namespace Jafg::Detail */


/*-----------------------------------------------------------------------------
    Make platform primitives public.
-----------------------------------------------------------------------------*/

typedef Jafg::Detail::LPrimitivePlatformTypes::u8       u8;
typedef Jafg::Detail::LPrimitivePlatformTypes::u16      u16;
typedef Jafg::Detail::LPrimitivePlatformTypes::u32      u32;
typedef Jafg::Detail::LPrimitivePlatformTypes::u64      u64;
typedef Jafg::Detail::LPrimitivePlatformTypes::i8       i8;
typedef Jafg::Detail::LPrimitivePlatformTypes::i16      i16;
typedef Jafg::Detail::LPrimitivePlatformTypes::i32      i32;
typedef Jafg::Detail::LPrimitivePlatformTypes::i64      i64;

typedef Jafg::Detail::LPrimitivePlatformTypes::f32 f32;
typedef Jafg::Detail::LPrimitivePlatformTypes::f64 f64;

typedef Jafg::Detail::LPrimitivePlatformTypes::LChar LChar;

namespace Jafg::App::Detail
{

//#
//# Checks the process address of the tracer PID. If the process is not valid, it returns false.
//# In case of any error or forbidden access rights, it returns false.
//#
ENGINE_API bool IsTracerPidValidVerySlow();

struct TrapMeFn
{
    [[noreturn]]
    ENGINE_API void operator()() const noexcept;

    [[noreturn]]
    ENGINE_API void operator()(std::string_view Message) const noexcept;

    [[noreturn]]
    void operator()(this auto&& Self, std::string_view Message, std::string_view File, std::size_t Line) noexcept
    {
        std::ostringstream Stream;
        Stream << "Fuck. Jafg entered an one-way enclosing block inside a critical control path and lost the war of being a good boy." << "\n\n"
               << Message << "\n\n"
               << "~File: " << File << "\n"
               << "~Line: " << Line
               ;
        Self.operator()(Stream.str());
    }
};

inline constexpr TrapMeFn TrapMe{};

} /* ~Namespace Jafg::App::Detail */
