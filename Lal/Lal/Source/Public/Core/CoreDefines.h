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
