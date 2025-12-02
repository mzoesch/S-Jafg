// Copyright mzoesch. All rights reserved.

#pragma once

#include <cstdint>

enum : i8 { INDEX_NONE = -1 };

enum ELazyInit    : i8 { LazyInit    };
enum EForceInit   : i8 { ForceInit   };
enum EZeroInit    : i8 { ZeroInit    };
enum ESkipInit    : i8 { SkipInit    }; // Dangerous.
enum EDefaultInit : i8 { DefaultInit };

namespace EPlatformExit
{

enum Type : i32
{
    Success     = 0b0000'0000,
    Error       = 0b0000'0010,
    Fatal       = 0b0000'0100,
    Restart     = 0b0000'1000,
};

} /* ~Namespace EPlatformExit */

enum : i8 { POINTER_BYTE_SIZE = sizeof(LSize) };

#define JTXTW(x)        LITERAL_WIDE(x)
#define JTXT8(x)        LITERAL_UTF8(x)
#define JTXTu(x)        LITERAL_UTF16(x)
#define JTXTU(x)        LITERAL_UTF32(x)
#if LAL_PLATFORM_USES_UTF8
    #define JTXT(x) JTXT8(x)
#elif LAL_PLATFORM_USES_UTF16
    #define JTXT(x) JTXTu(x)
#elif LAL_PLATFORM_USES_UTF32
    #define JTXT(x) JTXTU(x)
#else /* LAL_PLATFORM_USES_UTF32 */
    #error "Could not resolve platform encoding."
#endif /* !LAL_PLATFORM_USES_UTF8 */

//# We define this because some methods / functions may be noexcept.
//# But may contain development checks that are not present in release builds.
// TODO Move this to platform specific code, maybe? Some platforms may behave differently when encountering exceptions. Looking at you Wasm :()
#if LAL_DO_SLOW_CHECKS
    #define noexceptslow        // May throw.
#else /* LAL_DO_SLOW_CHECKS */
    #define noexceptslow        noexcept // Slow check will not compile so just mark as noexcept.
#endif /* !LAL_DO_SLOW_CHECKS */
#if LAL_DO_CHECKS
    #define noexceptcheck       // May throw.
#else /* LAL_DO_CHECKS */
    #define noexceptcheck       noexcept // Check will not compile so just mark as noexcept.
#endif  /* !LAL_DO_CHECKS */

#ifndef FALSE
    #define FALSE       0
#endif /* !FALSE */
#ifndef TRUE
    #define TRUE        1
#endif /* !TRUE */
