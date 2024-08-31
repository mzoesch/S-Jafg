// Copyright mzoesch. All rights reserved.

#pragma once

#include "GenericPlatform.h"

#ifndef PLATFORM_WINDOWS
    #define PLATFORM_WINDOWS        0
#endif /* !PLATFORM_WINDOWS */

#if PLATFORM_WINDOWS
    #include "HAL/PlatformWin.h"
#else /* PLATFORM_WINDOWS */
    /* Add more platform types here in the future... */
    #error "Could not resolve PLATFORM."
#endif /* PLATFORM_WINDOWS */

// ReSharper disable CppInconsistentNaming
typedef PlatformTypes::uint8    uint8;
typedef PlatformTypes::uint16   uint16;
typedef PlatformTypes::uint32   uint32;
typedef PlatformTypes::uint64   uint64;

typedef PlatformTypes::int8     int8;
typedef PlatformTypes::int16    int16;
typedef PlatformTypes::int32    int32;
typedef PlatformTypes::int64    int64;
// ReSharper restore CppInconsistentNaming

typedef PlatformTypes::TChar    TChar;
