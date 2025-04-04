// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
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

Jafg::BuildInfo::LEngineVersion Jafg::BuildInfo::GetEngineVersion()
{
    return
    {
        /*
         * We ofc will make this be feed in by our build system soon.
         */
        .Major = 0,
        .Minor = 0,
        .Patch = 1
    };
}
