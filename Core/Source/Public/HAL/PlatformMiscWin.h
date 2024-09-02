// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_WINDOWS
    #error "Wanted to override generic platform types with windows specific types, but platform is not windows."
#endif /* !PLATFORM_WINDOWS */

#include "HAL/GenericPlatformMisc.h"

struct GenericPlatformMisc;
struct WinPlatformMisc;

typedef WinPlatformMisc PlatformMisc;

struct WinPlatformMisc final : public GenericPlatformMisc
{

    static String GetEngineRootDir();

};
