// Copyright mzoesch. All rights reserved.

#pragma once
#include <cstdint>

enum : int8 { INDEX_NONE = -1 };

enum ELazyInit  : int8 { LazyInit  };
enum EForceInit : int8 { ForceInit };
enum EZeroInit  : int8 { ZeroInit  };
enum ESkipInit  : int8 { SkipInit  }; // Dangerous.

namespace EPlatformExit
{

enum Type : int32
{
    Success     = 0b0000'0000,
    Error       = 0b0000'0010,
    Fatal       = 0b0000'0100,
    Restart     = 0b0000'1000,
};

} /* ~Namespace EPlatformExit */

#if PLATFORM_WINDOWS_WITH_MSVC
    typedef ::std::intptr_t         LPtrSize;
    typedef ::std::uintptr_t        LuPtrSize;
#elif WITH_GCC
    typedef intptr_t                LPtrSize;
    typedef uintptr_t               LuPtrSize;
#elif WITH_CLANG
    typedef intptr_t                LPtrSize;
    typedef uintptr_t               LuPtrSize;
#else /* WITH_CLANG */
    #error "Missing implementation for this platform."
#endif /* WITH_CLANG */
enum : int8 { POINTER_BYTE_SIZE = sizeof(LPtrSize) };
#if PLATFORM_USES_32_BIT
    static_assert(sizeof(LPtrSize) == 4, "LPtrSize is not 4 bytes.");
    static_assert(sizeof(LuPtrSize) == 4, "LuPtrSize is not 4 bytes.");
    static_assert(sizeof(LPtrSize) == sizeof(void*), "LPtrSize is not the same size as a pointer.");
    static_assert(sizeof(LuPtrSize) == sizeof(void*), "LuPtrSize is not the same size as a pointer.");
    static_assert(POINTER_BYTE_SIZE == 4, "POINTER_BYTE_SIZE is not 4 bytes.");
#endif /* PLATFORM_USES_32_BIT */
#if PLATFORM_USES_64_BIT
    static_assert(sizeof(LPtrSize) == 8, "LPtrSize is not 8 bytes.");
    static_assert(sizeof(LuPtrSize) == 8, "LuPtrSize is not 8 bytes.");
    static_assert(sizeof(LPtrSize) == sizeof(void*), "LPtrSize is not the same size as a pointer.");
    static_assert(sizeof(LuPtrSize) == sizeof(void*), "LuPtrSize is not the same size as a pointer.");
    static_assert(POINTER_BYTE_SIZE == 8, "POINTER_BYTE_SIZE is not 8 bytes.");
#endif /* PLATFORM_USES_64_BIT */

typedef int64  LBigSizeTy;
typedef uint64 LuBigSizeTy;
typedef int32  LSizeTy;

#define JTXTW(x)        LITERAL_WIDE(x)
#define JTXT8(x)        LITERAL_UTF8(x)
#define JTXTu(x)        LITERAL_UTF16(x)
#define JTXTU(x)        LITERAL_UTF32(x)
#if PLATFORM_USES_UTF8
    #define JTXT(x) JTXT8(x)
#elif PLATFORM_USES_UTF16
    #define JTXT(x) JTXTu(x)
#elif PLATFORM_USES_UTF32
    #define JTXT(x) JTXTU(x)
#else /* PLATFORM_USES_UTF32 */
    #error "Could not resolve platform encoding."
#endif /* !PLATFORM_USES_UTF8 */

//# We define this because some methods / functions may be noexcept.
//# But may contain development checks that are not present in release builds.
// TODO Move this to platform specific code, maybe? Some platforms may behave differently when encountering exceptions. Looking at you Wasm :()
#if DO_SLOW_CHECKS
    #define noexceptslow        // May throw.
#else /* DO_SLOW_CHECKS */
    #define noexceptslow        noexcept // Slow check will not compile so just mark as noexcept.
#endif /* !DO_SLOW_CHECKS */
#if DO_CHECKS
    #define noexceptcheck       // May throw.
#else /* DO_CHECKS */
    #define noexceptcheck       noexcept // Check will not compile so just mark as noexcept.
#endif  /* !DO_CHECKS */

#ifndef FALSE
    #define FALSE       0
#endif /* !FALSE */
#ifndef TRUE
    #define TRUE        1
#endif /* !TRUE */
