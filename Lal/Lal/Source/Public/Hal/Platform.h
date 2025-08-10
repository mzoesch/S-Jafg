// Copyright mzoesch. All rights reserved.

#pragma once

#include <string>
#include <bit>

/*-----------------------------------------------------------------------------
    Common types for almost all compilers and platforms.
-----------------------------------------------------------------------------*/

namespace Lal
{

struct LPrimitivePlatformTypesGeneric
{
    //# 8-bit unsigned integer
    typedef unsigned char               u8;
    //# 16-bit unsigned integer
    typedef unsigned short int          u16;
    //# 32-bit unsigned integer
    typedef unsigned int                u32;
    //# 64-bit unsigned integer
    typedef unsigned long long          u64;

    //# 8-bit signed integer
    typedef signed char                 i8;
    //# 16-bit signed integer
    typedef signed short int            i16;
    //# 32-bit signed integer
    typedef signed int                  i32;
    //# 64-bit signed integer
    typedef signed long long            i64;

    typedef float                       f32;
    typedef double                      f64;

    //# The size type for the platform pointer.
    typedef u32                         LSize;

    //# An ASCII character. 8-bit fixed-width representation of 7-bit characters.
    typedef char                        LAsciiChar;

    //# Char used by jafg.
    typedef char                        LJafgChar;

    //# Native char.
    typedef char                        LChar;

    //#
    //# A wide character. ?-bit fixed-width representation of the platform's natural wide character set. It could
    //# be different sizes on different platforms.
    //#
    typedef wchar_t                     LWideChar;

    typedef std::wstring                LWideString;
    typedef std::string                 LStringLegacy;

    typedef decltype(nullptr)           LNullptrTy;
};

} /* ~Namespace Lal */


/*-----------------------------------------------------------------------------
    Define supported platforms.
-----------------------------------------------------------------------------*/

#ifndef PLATFORM_LINUX
    #define PLATFORM_LINUX              0
#endif /* !PLATFORM_LINUX */
#ifndef PLATFORM_WASM
    #define PLATFORM_WASM               0
#endif /* !PLATFORM_WASM */
#ifndef PLATFORM_WINDOWS
    #define PLATFORM_WINDOWS            0
#endif /* !PLATFORM_WINDOWS */

#if !(PLATFORM_LINUX || PLATFORM_WASM || PLATFORM_WINDOWS)
    #error "No platforms specified."
#endif /* !(PLATFORM_LINUX || PLATFORM_WASM || PLATFORM_WINDOWS) */
#if PLATFORM_LINUX
    #if PLATFORM_WASM || PLATFORM_WINDOWS
        #error "Multiple platforms specified."
    #endif /* PLATFORM_WASM || PLATFORM_WINDOWS */
#endif /* PLATFORM_LINUX */
#if PLATFORM_WASM
    #if PLATFORM_LINUX || PLATFORM_WINDOWS
        #error "Multiple platforms specified."
    #endif /* PLATFORM_LINUX || PLATFORM_WINDOWS */
#endif /* PLATFORM_WASM */
#if PLATFORM_WINDOWS
    #if PLATFORM_LINUX || PLATFORM_WASM
        #error "Multiple platforms specified."
    #endif /* PLATFORM_LINUX || PLATFORM_WASM */
#endif /* PLATFORM_WINDOWS */


/*-----------------------------------------------------------------------------
    Let platform override platform based macros and types.
-----------------------------------------------------------------------------*/

#if PLATFORM_LINUX
    #include "Hal/PlatformLinux.h"
#elif PLATFORM_WASM
    #include "Hal/PlatformWasm.h"
#elif PLATFORM_WINDOWS
    #include "Hal/PlatformWin.h"
#else /* PLATFORM_WINDOWS */
    #error "Failed to resolve platform."
#endif /* !PLATFORM_WINDOWS */


/*-----------------------------------------------------------------------------
    Implicitly generated macros.
-----------------------------------------------------------------------------*/

#ifndef PLATFORM_DESKTOP
    #if (PLATFORM_WINDOWS || PLATFORM_LINUX)
        #define PLATFORM_DESKTOP                                        1
    #else /* (PLATFORM_WINDOWS || PLATFORM_LINUX) */
        #define PLATFORM_DESKTOP                                        0
    #endif /* !(PLATFORM_WINDOWS || PLATFORM_LINUX) */
#endif /* PLATFORM_DESKTOP */

#ifndef LAL_PLATFORM_WINDOWS_WITH_GCC
    #define LAL_PLATFORM_WINDOWS_WITH_GCC                               0
#endif /* !LAL_PLATFORM_WINDOWS_WITH_GCC */
#ifndef LAL_PLATFORM_WINDOWS_WITH_MSVC
    #define LAL_PLATFORM_WINDOWS_WITH_MSVC                              0
#endif /* !LAL_PLATFORM_WINDOWS_WITH_MSVC */

#ifndef LAL_WITH_GCC
    #define LAL_WITH_GCC                                                0
#endif /* !LAL_WITH_GCC */
#ifndef LAL_WITH_MSVC
    #define LAL_WITH_MSVC                                               0
#endif /* !LAL_WITH_MSVC */
#ifndef LAL_WITH_CLANG
    #define LAL_WITH_CLANG                                              0
#endif /* !LAL_WITH_CLANG */

#if !LAL_WITH_GCC && !LAL_WITH_MSVC && !LAL_WITH_CLANG
    #error "No compiler spcified."
#endif /* LAL_WITH_GCC || LAL_WITH_MSVC || LAL_WITH_CLANG */

//# Branch prediction hint: Is this boolean expression likely to be true?
#ifndef LAL_UNLIKELY
    #define LAL_UNLIKELY(Expr)                                          (!!(Expr))
#endif /* !LAL_UNLIKELY */

//# Branch prediction hint: Is this boolean expression likely to be false?
#ifndef LAL_LIKELY
    #define LAL_LIKELY(Expr)                                            (!!(Expr))
#endif /* !LAL_LIKELY */

//# Whether to include and link glad. Some compilers may precede this.
#ifndef JAFG_NO_GLAD /* Part of Jafg and not Lal. Therefore, this is in the Jafg namespace. */
    #define JAFG_NO_GLAD                                                0
#endif /* !JAFG_NO_GLAD */

//# Whether to include and link glfw3. Some compilers may precede this.
#ifndef JAFG_NO_GLFW3 /* Part of Jafg and not Lal. Therefore, this is in the Jafg namespace. */
    #define JAFG_NO_GLFW3                                               0
#endif /* !JAFG_NO_GLFW3 */

//# Whether to include and link freetype. Some compilers may precede this.
#ifndef JAFG_NO_FREETYPE /* Part of Jafg and not Lal. Therefore, this is in the Jafg namespace. */
    #define JAFG_NO_FREETYPE                                            0
#endif /* !JAFG_NO_FREETYPE */

//# Whether to use the WebGL2 API.
#ifndef JAFG_PLATFORM_USES_WEBGL_TWO /* Part of Jafg and not Lal. Therefore, this is in the Jafg namespace. */
    #define JAFG_PLATFORM_USES_WEBGL_TWO                                0
#endif /* JAFG_PLATFORM_USES_WEBGL_TWO */

//# Whether this platform uses the GLFW3 abstraction layer under the hood.
#ifndef JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER /* Part of Jafg and not Lal. Therefore, this is in the Jafg namespace. */
    #define JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER                  0
#endif /* !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */

//# Whether this platform uses JavaScript as its frontend.
#ifndef JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND /* Part of Jafg and not Lal. Therefore, this is in the Jafg namespace. */
    #define JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND                     0
#endif /* !JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND */

#if AS_CLIENT
    #if !(JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER || JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND)
        #error "No frontend layer specified."
    #endif /* !(JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER || JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND) */
    #if (JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER && JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND)
        #error "Multiple frontend layers specified."
    #endif /* (JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER && JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND) */
#else /* AS_CLIENT */
    #if (JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER || JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND)
        #error "Frontend layer specified but not in client build."
    #endif /* (JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER || JAFG_PLATFORM_USES_JAVA_SCRIPT_FRONTEND) */
#endif /* !AS_CLIENT */

//# Whether the used compiler supports extern template specifications.
#ifndef LAL_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS
    #define LAL_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS        0
#endif /* !LAL_PLATFORM_SUPPORTS_EXTERN_TEMPLATE_SPECIFICATIONS */

//# Whether this platform uses 64-bit integers as pointers.
#ifndef LAL_PLATFORM_USES_64_BIT
    #define LAL_PLATFORM_USES_64_BIT                                    0
#endif /* LAL_PLATFORM_USES_64_BIT */

//# Whether this platform uses 32-bit integers as pointers.
#ifndef LAL_PLATFORM_USES_32_BIT
    #define LAL_PLATFORM_USES_32_BIT                                    0
#endif /* LAL_PLATFORM_USES_32_BIT */

#if !(LAL_PLATFORM_USES_64_BIT || LAL_PLATFORM_USES_32_BIT)
    #error "No pointer size specified."
#endif /* !(LAL_PLATFORM_USES_64_BIT || LAL_PLATFORM_USES_32_BIT) */
#if (LAL_PLATFORM_USES_64_BIT && LAL_PLATFORM_USES_32_BIT)
    #error "Multiple pointer sizes specified."
#endif /* (LAL_PLATFORM_USES_64_BIT && LAL_PLATFORM_USES_32_BIT) */

//# Whether this platform uses the little endian byte order.
#ifndef LAL_PLATFORM_USES_LITTLE_ENDIAN
    #ifdef LAL_PLATFORM_USES_BIG_ENDIAN
        #define LAL_PLATFORM_USES_LITTLE_ENDIAN                         (!LAL_PLATFORM_USES_BIG_ENDIAN)
    #else /* LAL_PLATFORM_USES_BIG_ENDIAN */
        #error "Platform is missing LAL_PLATFORM_USES_LITTLE_ENDIAN definition."
    #endif /* !LAL_PLATFORM_USES_BIG_ENDIAN */
#endif /* LAL_PLATFORM_USES_LITTLE_ENDIAN */

//# Whether this platform uses the big endian byte order.
#ifndef LAL_PLATFORM_USES_BIG_ENDIAN
    #ifdef LAL_PLATFORM_USES_LITTLE_ENDIAN
        #define  LAL_PLATFORM_USES_BIG_ENDIAN                           (!LAL_PLATFORM_USES_LITTLE_ENDIAN)
    #else /* LAL_PLATFORM_USES_LITTLE_ENDIAN */
        #error "Platform is missing  LAL_PLATFORM_USES_BIG_ENDIAN definition."
    #endif /* !LAL_PLATFORM_USES_LITTLE_ENDIAN */
#endif /*  LAL_PLATFORM_USES_BIG_ENDIAN */

#if !LAL_PLATFORM_USES_LITTLE_ENDIAN && !LAL_PLATFORM_USES_BIG_ENDIAN
    #error "No endianness specified."
#endif /* !LAL_PLATFORM_USES_LITTLE_ENDIAN && !LAL_PLATFORM_USES_BIG_ENDIAN */
#if (LAL_PLATFORM_USES_LITTLE_ENDIAN && LAL_PLATFORM_USES_BIG_ENDIAN)
    #error "Multiple endianness specified."
#endif /* (LAL_PLATFORM_USES_LITTLE_ENDIAN && LAL_PLATFORM_USES_BIG_ENDIAN) */

//#
//# If no, the platform tries to compile all libraries as static libraries or make a monolithic unity build.
//# This will not allow for dynamic linking at runtime, and therefore all plugins must be present at compile time.
//# Generated translation units for _all_ modules may be deferred and then compiled and linked directly into
//# the main executable.
//# Exact behavior over one module in monolithic unity builds has to be discussed and defined in the build
//# script for said module.
//#
#ifndef LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #define LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES                      0
#endif /* !LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES */

//# Whether this platform's standard output buffer stream supports flushing.
#ifndef LAL_PLATFORM_SUPPORTS_STD_FLUSH
    #define LAL_PLATFORM_SUPPORTS_STD_FLUSH                             0
#endif /* !LAL_PLATFORM_SUPPORTS_STD_FLUSH */

//# Whether the platform's standard output buffer stream supports ANSI escape codes.
#ifndef LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES
    #define LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES                          0
#endif /* !LAL_PLATFORM_SUPPORTS_ANSI_ESCAPES */

//# Whether the platform supports Single-Instruction-Multiple-Data intrinsics.
#ifndef LAL_PLATFORM_SUPPORTS_SIMD
    #define LAL_PLATFORM_SUPPORTS_SIMD                                  0
#endif /* !LAL_PLATFORM_SUPPORTS_SIMD */

//#
//# Whether allocated memory can be shrunk by platform intrinsics or memory shrinking is performed by allocating
//# a new smaller bulk of memory, copying the data and freeing the old memory.
//# Not that, generally speaking, on platforms that support memory shrinking, the memory might still be
//# reallocated if the underlying platform thinks it is necessary. Lal does not provide any guarantees for that.
//#
#ifndef LAL_PLATFORM_SUPPORTS_MEMORY_SHRINK
    #define LAL_PLATFORM_SUPPORTS_MEMORY_SHRINK                         0
#endif /* !LAL_PLATFORM_SUPPORTS_MEMORY_SHRINK */

//# Whether this platform uses the generic platform-agnostic loop provided by the runtime library of Jafg.
#ifndef LAL_PLATFORM_USES_NON_GENERIC_LOOP
    #define LAL_PLATFORM_USES_NON_GENERIC_LOOP                          0
#endif /* !LAL_PLATFORM_USES_NON_GENERIC_LOOP */
#if LAL_PLATFORM_USES_NON_GENERIC_LOOP
    #ifndef LAL_PLATFORM_GUARDED_LOOP
        #error "LAL_PLATFORM_GUARDED_LOOP is not defined."
    #endif /* !LAL_PLATFORM_GUARDED_LOOP */
#else /* LAL_PLATFORM_USES_NON_GENERIC_LOOP */
    #ifdef LAL_PLATFORM_GUARDED_LOOP
        #error "LAL_PLATFORM_GUARDED_LOOP is defined but platform does use the generic platform-agnostic loop."
    #endif /* LAL_PLATFORM_GUARDED_LOOP */
#endif /* !LAL_PLATFORM_USES_NON_GENERIC_LOOP */

//#
//# Whether this platform uses the generic platform-agnostic exit procedure provided by the runtime library of Jafg.
//# @note The default runtime library uses a Resource-Acquisition-Is-Initialization-based approach to exit the program.
//#       If your platform does not support clean defined destructor behavior, another non-trivial exit procedure
//#       must be used.
#ifndef LAL_PLATFORM_USES_NON_GENERIC_EXIT
    #define LAL_PLATFORM_USES_NON_GENERIC_EXIT                          0
#endif /* !LAL_PLATFORM_USES_NON_GENERIC_EXIT */

//# Whether this platform uses UTF-8, UTF-16 or UTF-32 as its native character encoding.
#ifndef LAL_PLATFORM_USES_UTF8
    #define LAL_PLATFORM_USES_UTF8                                      0
#endif /* !LAL_PLATFORM_USES_UTF8 */
#ifndef LAL_PLATFORM_USES_UTF16
    #define LAL_PLATFORM_USES_UTF16                                     0
#endif /* !LAL_PLATFORM_USES_UTF16 */
#ifndef LAL_PLATFORM_USES_UTF32
    #define LAL_PLATFORM_USES_UTF32                                     0
#endif /* !LAL_PLATFORM_USES_UTF32 */

#if !(LAL_PLATFORM_USES_UTF8 || LAL_PLATFORM_USES_UTF16 || LAL_PLATFORM_USES_UTF32)
    #error "No native characther endocing specified."
#endif /* !(LAL_PLATFORM_USES_UTF8 || LAL_PLATFORM_USES_UTF16 || LAL_PLATFORM_USES_UTF32) */
#if IN_DEBUG
    #if LAL_PLATFORM_USES_UTF16 || LAL_PLATFORM_USES_UTF32
        #error "Multiple native characther endocing specified."
    #endif /* LAL_PLATFORM_USES_UTF16 || LAL_PLATFORM_USES_UTF32 */
#endif /* LAL_PLATFORM_USES_UTF8 */
#if PLATFORM_WASM
    #if LAL_PLATFORM_USES_UTF8 || LAL_PLATFORM_USES_UTF32
        #error "Multiple native characther endocing specified."
    #endif /* LAL_PLATFORM_USES_UTF32 */
#endif /* LAL_PLATFORM_USES_UTF16 */
#if LAL_PLATFORM_USES_UTF32
    #if LAL_PLATFORM_USES_UTF8 || LAL_PLATFORM_USES_UTF16
        #error "Multiple native characther endocing specified."
    #endif /* LAL_PLATFORM_USES_UTF16 */
#endif /* LAL_PLATFORM_USES_UTF32 */

#if LAL_PLATFORM_USES_UTF8
    #if LAL_PLATFORM_USES_UTF16
        #error "LAL_PLATFORM_USES_UTF8 and LAL_PLATFORM_USES_UTF16 are both defined."
    #endif /* LAL_PLATFORM_USES_UTF16 */
    #if PLATFORM_USES_UTF32
        #error "LAL_PLATFORM_USES_UTF8 and LAL_PLATFORM_USES_UTF32 are both defined."
    #endif /* LAL_PLATFORM_USES_UTF32 */
#endif /* LAL_PLATFORM_USES_UTF8 */
#if LAL_PLATFORM_USES_UTF16
    #if LAL_PLATFORM_USES_UTF8
        #error "LAL_PLATFORM_USES_UTF16 and LAL_PLATFORM_USES_UTF8 are both defined."
    #endif /* LAL_PLATFORM_USES_UTF8 */
    #if LAL_PLATFORM_USES_UTF32
        #error "LAL_PLATFORM_USES_UTF16 and LAL_PLATFORM_USES_UTF32 are both defined."
    #endif /* LAL_PLATFORM_USES_UTF32 */
#endif /* LAL_PLATFORM_USES_UTF16 */
#if LAL_PLATFORM_USES_UTF32
    #if LAL_PLATFORM_USES_UTF8
        #error "LAL_PLATFORM_USES_UTF32 and LAL_PLATFORM_USES_UTF8 are both defined."
    #endif /* LAL_PLATFORM_USES_UTF8 */
    #if LAL_PLATFORM_USES_UTF16
        #error "LAL_PLATFORM_USES_UTF32 and LAL_PLATFORM_USES_UTF16 are both defined."
    #endif /* LAL_PLATFORM_USES_UTF16 */
#endif /* LAL_PLATFORM_USES_UTF32 */
#if !LAL_PLATFORM_USES_UTF8 && !LAL_PLATFORM_USES_UTF16 && !LAL_PLATFORM_USES_UTF32
    #error "No platform encoding is defined."
#endif /* !LAL_PLATFORM_USES_UTF8 && !LAL_PLATFORM_USES_UTF16 && !LAL_PLATFORM_USES_UTF32 */

//# The size of the wchar_t type on the current platform.
#ifndef LAL_PLATFORM_WCHAR_SIZE
    #error "Platform is missing LAL_PLATFORM_WCHAR_SIZE definition."
#endif /* !LAL_PLATFORM_WCHAR_SIZE */

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

//# The call spec for imported functions.
#ifndef LAL_PLATFORM_CALLSPEC_IN
    #error "Platform is missing LAL_PLATFORM_CALLSPEC_IN definition."
#endif /* !LAL_PLATFORM_CALLSPEC_IN */

//# The call spec for exported functions.
#ifndef LAL_PLATFORM_CALLSPEC_OUT
    #error "Platform is missing LAL_PLATFORM_CALLSPEC_OUT definition."
#endif /* !LAL_PLATFORM_CALLSPEC_OUT */

//# The extern call spec for imported functions.
#ifndef LAL_PLATFORM_EXTERNSPEC_IN
    #error "Platform is missing LAL_PLATFORM_EXTERNSPEC_IN definition."
#endif /* !LAL_PLATFORM_EXTERNSPEC_IN */

//# The extern call spec for exported functions.
#ifndef LAL_PLATFORM_EXTERNSPEC_OUT
    #error "Platform is missing LAL_PLATFORM_EXTERNSPEC_OUT definition."
#endif /* !LAL_PLATFORM_EXTERNSPEC_OUT */

//# The maximal path length for the current platform.
#ifndef LAL_PLATFORM_MAX_PATH
    #error "Platform is missing LAL_PLATFORM_MAX_PATH definition."
#endif /* !LAL_PLATFORM_MAX_PATH */

//# The exit type for the platform.
#ifndef LAL_PLATFORM_EXIT_TYPE
    #define LAL_PLATFORM_EXIT_TYPE                                      Lal::LPlatformTypes::i32
#endif /* LAL_PLATFORM_EXIT_TYPE */

//# The maximal number of frames that can be stored when tracing.
#ifndef LAL_PLATFORM_MAX_FRAMES
    #define LAL_PLATFORM_MAX_FRAMES                                     128
#endif /* LAL_PLATFORM_MAX_FRAMES */

//# This tells the compiler to never discard the resulting control path.
#ifndef LAL_PLATFORM_NO_DISCARD_CTRL_PATH
    #error "Platform is missing LAL_PLATFORM_NO_DISCARD_CTRL_PATH definition."
#endif /* !LAL_PLATFORM_NO_DISCARD_CTRL_PATH */

//#
//# Only gets called if a debugger is watching us. If no debugger is attached, the behavior of this macro is
//# undefined.
//# Usually if no debugger is attached, the program (when in a defined and good state) will call the LAL_PLATFORM_TRAP
//# macro instead.
//# This macro has to allow for continuing either within the debugger or in the program itself.
//#
#ifndef LAL_PLATFORM_BREAK
    #error "Platform is missing LAL_PLATFORM_BREAK definition."
#endif /* !LAL_PLATFORM_BREAK */

//# Trap this process.
#ifndef LAL_PLATFORM_TRAP
    #error "Platform is missing LAL_PLATFORM_TRAP definition."
#endif /* !LAL_PLATFORM_TRAP */

//# Tells the compiler to never inline a function or method.
#ifndef NOINLINE
    #error "Platform is missing NOINLINE definition."
#endif /* !NOINLINE */

//# Tells the compiler to always inline a function or method.
#ifndef FORCEINLINE
    #error "Platform is missing FORCEINLINE definition."
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

//# Set this macro to a struct that defines different types for the platform if necessary.
#ifndef LAL_PLATFORM_TYPES_STRUCT
    #define LAL_PLATFORM_TYPES_STRUCT                                   ::Lal::LPrimitivePlatformTypesGeneric
#endif /* LAL_PLATFORM_TYPES_STRUCT */

//# The maximal aligned type for the current compiled platform.
#ifndef LAL_MAX_ALIGN_TYPE /* No other type is more aligned than double on a common platform. */
    #define LAL_MAX_ALIGN_TYPE                                          LAL_PLATFORM_TYPES_STRUCT::f64
#endif /* !LAL_MAX_ALIGN_TYPE */

namespace Lal
{

typedef LAL_MAX_ALIGN_TYPE                                              LMaxAlign;

//# The actual used primitive platform types. This is always valid.
typedef LAL_PLATFORM_TYPES_STRUCT                                       LPlatformTypes;

namespace Private
{

//# Is true if RTTI is enabled for the current compiler.
constexpr bool IsRttiEnabled()
{
#if LAL_WITH_CLANG
    return __has_feature(cxx_rtti);
#elif WITH_GCC
    return __GXX_RTTI;
#elif LAL_WITH_MSVC
    #ifdef _CPPRTTI
        return true;
    #else /* _CPPRTTI */
        return false;
    #endif /* !_CPPRTTI */
#else /* LAL_WITH_MSVC */
    /* Assume it is enabled by default. */
    return true;
#endif /* !LAL_WITH_MSVC */
}

static_assert(IsRttiEnabled() == false, "No, stupid feature.");

constexpr bool IsLittleEndian { std::endian::native == std::endian::little };
constexpr bool IsBigEndian    { std::endian::native == std::endian::big };

#if LAL_PLATFORM_USES_LITTLE_ENDIAN
    static_assert(IsLittleEndian);
    static_assert(IsBigEndian == false);
#endif /* LAL_PLATFORM_USES_LITTLE_ENDIAN */

#if LAL_PLATFORM_USES_BIG_ENDIAN
    static_assert(IsBigEndian);
    static_assert(IsLittleEndian == false);
#endif /* LAL_PLATFORM_USES_BIG_ENDIAN */

//# @return True if the platform is little endian at runtime.
LAL_API NOINLINE bool IsLittleEndianRuntime(); /* NOINLINE to prevent LTO. */
FORCEINLINE      bool IsBigEndianRuntime() { return IsLittleEndianRuntime() == false; }

} /* ~Namespace Private */

template <typename T>
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

    typename T::LSize;

    typename T::LAsciiChar;
    typename T::LJafgChar;
    typename T::LChar;
    typename T::LWideChar;

    typename T::LWideString;
    typename T::LStringLegacy;

    typename T::LNullptrTy;
};

static_assert(IsPrimitivePlatformTypesValid<LPlatformTypes>, "Lal::LPlatformTypes is not valid.");

static_assert(sizeof(LPlatformTypes::u8)                == 1);
static_assert(sizeof(LPlatformTypes::u16)               == 2);
static_assert(sizeof(LPlatformTypes::u32)               == 4);
static_assert(sizeof(LPlatformTypes::u64)               == 8);
static_assert(sizeof(LPlatformTypes::i8)                == 1);
static_assert(sizeof(LPlatformTypes::i16)               == 2);
static_assert(sizeof(LPlatformTypes::i32)               == 4);
static_assert(sizeof(LPlatformTypes::i64)               == 8);

static_assert(sizeof(LPlatformTypes::f32)               == 4);
static_assert(sizeof(LPlatformTypes::f64)               == 8);

static_assert(sizeof(LPlatformTypes::LSize)             == sizeof(void*));

static_assert(sizeof(wchar_t)     == LAL_PLATFORM_WCHAR_SIZE );
static_assert(sizeof(char)                              == 1 );
static_assert(sizeof(char8_t)                           == 1 );
static_assert(sizeof(char16_t)                          == 2 );
static_assert(sizeof(char32_t)                          == 4 );

#if LAL_PLATFORM_USES_64_BIT
    static_assert(sizeof(void*) == sizeof(LPlatformTypes::u64));
    static_assert(sizeof(void*) == sizeof(LPlatformTypes::i64));
#endif /* LAL_PLATFORM_USES_64_BIT */
#if LAL_PLATFORM_USES_32_BIT
    static_assert(sizeof(void*) == sizeof(LPlatformTypes::u32));
    static_assert(sizeof(void*) == sizeof(LPlatformTypes::i32));
#endif /* LAL_PLATFORM_USES_32_BIT */

template <typename T>
concept IsOnProgramPanicValid = requires
    (
        T t,
        LPlatformTypes::LChar* InBaseMessage,
        LPlatformTypes::LChar* InMessage,
        LPlatformTypes::LChar* InFile,
        LPlatformTypes::u64    InLine
    )
{
    { t.ExitQuietly() } -> std::same_as<void>;
    { t.OnProgramPanicImpl(InBaseMessage) } -> std::same_as<void>;
    { t.OnProgramPanic(InMessage, InFile, InLine) } -> std::same_as<void>;
};

static_assert(IsOnProgramPanicValid<LOnPlatformBreak>);

} /* ~Namespace Lal */


/*-----------------------------------------------------------------------------
    Make platform primitives public.
-----------------------------------------------------------------------------*/

typedef Lal::LPlatformTypes::u8                                         u8;
typedef Lal::LPlatformTypes::u16                                        u16;
typedef Lal::LPlatformTypes::u32                                        u32;
typedef Lal::LPlatformTypes::u64                                        u64;

typedef Lal::LPlatformTypes::i8                                         i8;
typedef Lal::LPlatformTypes::i16                                        i16;
typedef Lal::LPlatformTypes::i32                                        i32;
typedef Lal::LPlatformTypes::i64                                        i64;

typedef Lal::LPlatformTypes::f32                                        f32;
typedef Lal::LPlatformTypes::f64                                        f64;

typedef Lal::LPlatformTypes::LSize                                      LSize;

typedef Lal::LPlatformTypes::LAsciiChar                                 LAsciiChar;
typedef Lal::LPlatformTypes::LJafgChar                                  LJafgChar;
typedef Lal::LPlatformTypes::LChar                                      LChar;
typedef Lal::LPlatformTypes::LWideChar                                  LWideChar;
typedef Lal::LPlatformTypes::LStringLegacy                              LStringLegacy;

typedef Lal::LPlatformTypes::LWideString                                LWideString;

typedef Lal::LPlatformTypes::LNullptrTy                                 LNullptrTy;


/*-----------------------------------------------------------------------------
    HAL free function that must be valid and implemented on all platforms.
-----------------------------------------------------------------------------*/

namespace Lal::Hal
{

static constexpr LAL_PLATFORM_EXIT_TYPE LalPanicExitCode { 0b100000000 };

//#
//# Very dangerous function. Use with care and never in critical code paths.
//# Currently not supported for all platforms.
//#
LAL_API void SleepNoStats(const f64 InSeconds);

//#
//# May not be supported on all platforms. Use with caution.
//#
LAL_API void YieldThread();

//#
//# Checks the process address of the tracer PID. If the process is not valid, it returns false.
//# In case of any error or forbidden access rights, it returns false.
//#
LAL_API bool IsTracerPidValidVerySlow();

} /* ~Namespace Lal::Hal */
