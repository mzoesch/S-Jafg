// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Build/EngineBuildInfo.h"

const Jafg::LSimpleString& Jafg::BuildInfo::GetBuildTime()
{
    static LSimpleString BuildTime = __TIME__;
    return BuildTime;
}

const Jafg::LSimpleString& Jafg::BuildInfo::GetBuildDate()
{
    static LSimpleString BuildDate = __DATE__;
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
