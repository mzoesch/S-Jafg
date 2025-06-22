// Copyright mzoesch. All rights reserved.

#include "Core/Application.h"
#include "Core/Uuid.h"
#include "Stats/Stats.h"

void Jafg::PlatformHal::Sleep(const f64 InSeconds)
{
    STAT_CYCLE_FUNCTION()
    PlatformHal::SleepNoStats(InSeconds);
    return;
}

namespace Jafg::Private
{

ENGINE_API LUnderlyingUuidType CurrentUuidValue { 0 };

} /* ~Namespace Jafg::Private */

namespace Jafg::Application::Private
{

ENGINE_API LString CommandLine        { };
ENGINE_API bool    bPauseBeforeExit   { false };
ENGINE_API bool    bDebuggerPresent   { false };
ENGINE_API bool    bAlwaysReportCrash { false };
ENGINE_API bool    bAllowProfiling    { false };

ENGINE_API f64 DeltaTime     { 0.0 };
ENGINE_API f64 RealDeltaTime { 0.0 };
ENGINE_API f64 LostDeltaTime { 0.0 };
ENGINE_API f64 IdleDeltaTime { 0.0 };
ENGINE_API u64 FrameCount    { 0   };

ENGINE_API f64 CurrentFrameTime  { 0.0 };
ENGINE_API f64 PreviousFrameTime { 0.0 };

ENGINE_API LHrcTimePoint StaticContainerInitializationTime { Hrc::now() };
ENGINE_API f64           LowestDeltaTime                   { std::numeric_limits<double>::max() };
ENGINE_API f64           HighestDeltaTime                  { -1.0 };
ENGINE_API f64           HighestLostDeltaTime              {  0.0 };
ENGINE_API LHrcTimePoint PreviousStatisticsStartTime       { StaticContainerInitializationTime };
ENGINE_API u64           PreviousStatisticsFrameCount      { FrameCount       };
ENGINE_API f64           PreviousLowestDeltaTime           { LowestDeltaTime  };
ENGINE_API f64           PreviousHighestDeltaTime          { HighestDeltaTime };
ENGINE_API LHrcTimePoint LastStatisticsTime                { StaticContainerInitializationTime };
ENGINE_API u64           StatisticsFrameCount              { 0 };
ENGINE_API f32           StatisticsPeriod                  { 1.0f };

ENGINE_API LHrcTimePoint LastStdOutFlushTime;

} /* ~Namespace Jafg::Application::Private */

namespace Jafg::Application
{

void Private::UpdateApplicationCommandLineVariables()
{
    bPauseBeforeExit   = HasCmdLineParameter("PauseBeforeExit");
    bAlwaysReportCrash = HasCmdLineParameter("AlwaysReportCrash");
    bAllowProfiling    = HasCmdLineParameter("AllowProfiling");

    LOG_VERBOSE(LogCli, "Command line: [{}].", Private::CommandLine)
    LOG_VERBOSE(LogCli, "bPauseBeforeExit: [{}].", bPauseBeforeExit)
    LOG_VERBOSE(LogCli, "bDebuggerPresent: [{}].", bDebuggerPresent)
    LOG_VERBOSE(LogCli, "bAlwaysReportCrash: [{}].", bAlwaysReportCrash)
    LOG_VERBOSE(LogCli, "bAllowProfiling: [{}].", bAllowProfiling)

    return;
}

bool HasCmdLineParameter(const LString& Parameter)
{
    const i32 Index = Private::CommandLine.FindFirst(Parameter.ToPtr());
    if (Index == INDEX_NONE)
    {
        return false;
    }

    if (Index == 0) // Has to start with -
    {
        return false;
    }

    // Strings are broken so just ignore this for now

    return true;
    // if (Private::CommandLine.GetCharacterAtIndex(Index - 1) != '-')
    // {
    //     return false;
    // }
    //
    // if (Private::CommandLine.GetSize() + 1 == Parameter.GetSize())
    // {
    //     return true;
    // }
    //
    // if (Private::CommandLine.GetCharacterAtIndex(Index + Parameter.GetSize()) == ' ')
    // {
    //     return true;
    // }
    //
    // return Private::CommandLine.GetCharacterAtIndex(Index + Parameter.GetSize()) == '=';
}

} /* ~Namespace Jafg::Application */
