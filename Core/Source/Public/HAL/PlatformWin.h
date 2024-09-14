// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_WINDOWS
    #error "Wanted to override generic platform types with windows specific types, but platform is not windows."
#endif /* !PLATFORM_WINDOWS */

// #include <Windows.h>

/*
 * We forward declare this shit, as this file should only be included implicitly by including Platform.h
 */
struct GenericPlatformTypes;

struct WinPlatformTypes;

typedef WinPlatformTypes PlatformTypes;

struct WinPlatformTypes final : public GenericPlatformTypes
{
};

#ifdef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is already defined."
#endif /* PLATFORM_MAX_PATH */
#define PLATFORM_MAX_PATH \
    260// MAX_PATH // 260

#ifdef PLATFORM_BREAK
    #error "PLATFORM_BREAK is already defined."
#endif /* PLATFORM_BREAK */
#define PLATFORM_BREAK() (__nop(), __debugbreak())
