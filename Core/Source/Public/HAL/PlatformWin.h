// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_WINDOWS
    #error "Wanted to override generic platform types with windows specific types, but platform is not windows."
#endif /* !PLATFORM_WINDOWS */

struct GenericPlatformTypes;
struct WinPlatformTypes;

typedef WinPlatformTypes PlatformTypes;

struct WinPlatformTypes final : public GenericPlatformTypes
{
};
