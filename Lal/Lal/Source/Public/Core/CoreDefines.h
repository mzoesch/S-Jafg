// Copyright mzoesch. All rights reserved.

#pragma once

enum : int8 { INDEX_NONE = -1 };

namespace EPlatformExit
{

enum Type : int32
{
    Success     = 0b0000'0000,
    Error       = 0b0000'0010,
    Fatal       = 0b0000'0100,
    Restart     = 0b0000'1000,
};

} /* ~Namespace EPlatformExit. */

#if PLATFORM_WINDOWS_WITH_MSVC
    typedef ::std::intptr_t  LPtrSize;
    typedef ::std::uintptr_t LuPtrSize;
#elif WITH_GNU
    typedef intptr_t  LPtrSize;
    typedef uintptr_t LuPtrSize;
#endif /* WITH_GNU */
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
