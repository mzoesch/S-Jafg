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
    static LString Standard{ Jafg::SprintF("C++{}", __cplusplus) };
    return Standard;
}
