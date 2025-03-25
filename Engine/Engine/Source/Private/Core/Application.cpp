// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Core/Application.h"

namespace Jafg::Application::Private
{

ENGINE_API LString CommandLine        { };
ENGINE_API bool    bDebuggerPresent   { false };
ENGINE_API bool    bAlwaysReportCrash { false };

ENGINE_API double DeltaTime     { 0.0 };
ENGINE_API double RealDeltaTime { 0.0 };
ENGINE_API double LostDeltaTime { 0.0 };
ENGINE_API double IdleDeltaTime { 0.0 };
ENGINE_API u64 FrameCount    { 0   };

ENGINE_API double CurrentFrameTime  { 0.0 };
ENGINE_API double PreviousFrameTime { 0.0 };

ENGINE_API LHrcTimePoint StaticContainerInitializationTime { Hrc::now() };
ENGINE_API double        LowestDeltaTime                   { std::numeric_limits<double>::max() };
ENGINE_API double        HighestDeltaTime                  { -1.0 };
ENGINE_API double        HighestLostDeltaTime              {  0.0 };
ENGINE_API LHrcTimePoint PreviousStatisticsStartTime       { StaticContainerInitializationTime };
ENGINE_API u64        PreviousStatisticsFrameCount      { FrameCount       };
ENGINE_API double        PreviousLowestDeltaTime           { LowestDeltaTime  };
ENGINE_API double        PreviousHighestDeltaTime          { HighestDeltaTime };
ENGINE_API LHrcTimePoint LastStatisticsTime                { StaticContainerInitializationTime };
ENGINE_API u64        StatisticsFrameCount              { 0 };
ENGINE_API float         StatisticsPeriod                  { 1.0f };

ENGINE_API LHrcTimePoint LastStdOutFlushTime;

} /* ~Namespace Jafg::Application::Private */

namespace Jafg::Application
{

void Private::UpdateApplicationCommandLineVariables()
{
    bAlwaysReportCrash = HasCmdLineParameter("AlwaysReportCrash");

    return;
}

bool HasCmdLineParameter(const LString& Parameter)
{
    const i32 Index = Private::CommandLine.FindFirstSub(Parameter.ToPtr());
    if (Index == INDEX_NONE)
    {
        return false;
    }

    if (Index == 0) // Has to start with -
    {
        return false;
    }

    if (Private::CommandLine.GetCharacterAtIndex(Index - 1) != '-')
    {
        return false;
    }


    if (Private::CommandLine.GetSize() + 1 == Parameter.GetSize())
    {
        return true;
    }

    if (Private::CommandLine.GetCharacterAtIndex(Index + Parameter.GetSize()) == ' ')
    {
        return true;
    }

    return Private::CommandLine.GetCharacterAtIndex(Index + Parameter.GetSize()) == '=';
}

} /* ~Namespace Jafg::Application */
