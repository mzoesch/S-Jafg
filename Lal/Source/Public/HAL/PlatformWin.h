// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_WINDOWS
    #error "Wanted to override generic platform types with windows specific types, but platform is not windows."
#endif /* !PLATFORM_WINDOWS */

/*
 * We forward declare this shit, as this file should only be included implicitly by including Platform.h
 */
struct LGenericPlatformTypes;

struct LWinPlatformTypes;

typedef LWinPlatformTypes LPlatformTypes;

struct LWinPlatformTypes final : public LGenericPlatformTypes
{
};

#ifdef PLATFORM_MAX_PATH
    #error "PLATFORM_MAX_PATH is already defined."
#endif /* PLATFORM_MAX_PATH */
#define PLATFORM_MAX_PATH \
    MAX_PATH

#ifdef PLATFORM_BREAK
    #error "PLATFORM_BREAK is already defined."
#endif /* PLATFORM_BREAK */
#define PLATFORM_BREAK() \
    ( __nop(), __debugbreak() )

#define PLATFORM_CALLSPEC_OUT       __declspec ( dllexport )
#define PLATFORM_CALLSPEC_IN        __declspec ( dllimport )
