// Copyright mzoesch. All rights reserved.

#include "Build/EngineBuildInfo.h"

const Jafg::LString& Jafg::BuildInfo::GetBuildTime()
{
    static LString BuildTime = __TIME__;
    return BuildTime;
}

const Jafg::LString& Jafg::BuildInfo::GetBuildDate()
{
    static LString BuildDate = __DATE__;
    return BuildDate;
}

const Jafg::LString& Jafg::BuildInfo::GetVcsBranch()
{
    static  LString Revision { PRIVATE_ENGINE_VCS_BRANCH };
    return Revision;}

const Jafg::LString& Jafg::BuildInfo::GetVcsRevision()
{
    static  LString Revision { PRIVATE_ENGINE_VCS_REVISION };
    return Revision;
}

Jafg::LString Jafg::BuildInfo::GetEngineVersionStr()
{
    static LString Version { PRIVATE_ENGINE_VERSION };
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
