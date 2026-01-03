// Copyright mzoesch. All rights reserved.

#include "Build/EngineBuildInfo.h"

const LString& Jafg::BuildInfo::GetBuildTime()
{
    static LString BuildTime{ __TIME__ };
    return BuildTime;
}

const LString& Jafg::BuildInfo::GetBuildDate()
{
    static LString BuildDate{ __DATE__ };
    return BuildDate;
}

const LString& Jafg::BuildInfo::GetVcsBranch()
{
    static LString Branch{ PRIVATE_ENGINE_VCS_BRANCH };
    return Branch;
}

const LString& Jafg::BuildInfo::GetVcsRevision()
{
    static LString Revision{ PRIVATE_ENGINE_VCS_REVISION };
    return Revision;
}

const LString& Jafg::BuildInfo::GetEngineVersionStr()
{
    static LString Version{ PRIVATE_ENGINE_VERSION };
    return Version;
}

Jafg::BuildInfo::LEngineVersion Jafg::BuildInfo::GetEngineVersion()
{
    return
    {
        .Major = PRIVATE_ENGINE_VERSION_MAJOR,
        .Minor = PRIVATE_ENGINE_VERSION_MINOR,
        .Patch = PRIVATE_ENGINE_VERSION_PATCH
    };
}

LString const& Jafg::BuildInfo::GetCompilerVersion()
{
    static LString Version{ __VERSION__ };
    return Version;
}

LString const& Jafg::BuildInfo::GetCxxStandard()
{
    static LString Standard{ Lal::SprintF("C++{}", __cplusplus) };
    return Standard;
}

LString const& Jafg::BuildInfo::GetJafgTargetPlatform()
{
#if PLATFORM_WINDOWS
    static LString Platform{ "Windows" };
#elif PLATFORM_LINUX
    static LString Platform{ "Linux" };
#elif PLATFORM_WASM
    static LString Platform{ "Wasm" };
#else /* PLATFORM_WASM */
    #error "Platform missing implementation."
#endif /* !PLATFORM_WASM */

    return Platform;
}

LString const& Jafg::BuildInfo::GetJafgTargetArchitecture()
{
    static LString Architecture{ PRIVATE_ENGINE_TARGET_ARCHITECTURE };
    return Architecture;
}

LString const& Jafg::BuildInfo::GetJafgTargetType()
{
#if AS_CLIENT && !DO_TEST_UNITS
    static LString Type{ "Client" };
#elif AS_CLIENT && DO_TEST_UNITS
    static LString Type{ "TestUnit" };
#elif AS_DAEMON
    static LString Type{ "Daemon" };
#else /* AS_DAEMON */
    #error "Target type missing implementation."
#endif /* !AS_DAEMON */

    return Type;
}

LString const& Jafg::BuildInfo::GetJafgTargetConfig()
{
#if IN_DEBUG
    static LString Config{ "Debug" };
#elif IN_DEVELOPMENT
    static LString Config{ "Development" };
#elif IN_SHIPPING
    static LString Config{ "Shipping" };
#else /* IN_SHIPPING */
    #error "Target config missing implementation."
#endif /* !IN_SHIPPING */

    return Config;
}

