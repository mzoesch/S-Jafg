// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

namespace Hal
{

/**
 * Very dangerous function. Use with care and never in critical code paths.
 * Currently not supported for all platforms.
 */
ENGINE_API void Sleep(const f64 InSeconds);

} /* ~Namespace PlatformHal */

namespace Application
{

typedef std::chrono::high_resolution_clock             Hrc;
typedef std::chrono::high_resolution_clock::time_point LHrcTimePoint;

namespace Private
{

//#
//# The command line. A parameter is defined as the following:
//#   -parameter
//#   -parameter=value                  (no spaces allowed)
//#   -parameter="string value"         (escape " with \)
//#   -parameter item item item         (no - allowed on items)
//#   -parameter="item item item"
//#   -parameter="\"item\" \"item\" \"item\""
//#
//# Core application command line parameters:
//#   -PauseBeforeExit
//#   -WaitForDebugger
//#   -AlwaysReportCrash
//#   -AllowProfiling
//#
ENGINE_API extern LString CommandLine;
ENGINE_API extern bool    bPauseBeforeExit; // Not all platforms respect this.
ENGINE_API extern bool    bDebuggerPresent;
ENGINE_API extern bool    bAlwaysReportCrash;
ENGINE_API extern bool    bAllowProfiling;

ENGINE_API void UpdateApplicationCommandLineVariables();

} /* ~Namespace Private */

FORCEINLINE auto GetCmdLine() -> const LString& { return Private::CommandLine; }
ENGINE_API  bool HasCmdLineParameter(const LString& Parameter);
FORCEINLINE bool IsPauseBeforeExit() { return Private::bPauseBeforeExit; }
FORCEINLINE bool IsDebuggerPresent() { return Private::bDebuggerPresent; }
FORCEINLINE bool IsAlwaysReportCrash() { return Private::bAlwaysReportCrash; }
FORCEINLINE bool IsAllowProfiling() { return Private::bAllowProfiling; }

//#
//# Global application functions.
//# All time related functions are measured in seconds except stated otherwise.
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

FORCEINLINE auto GetHighestNow() -> LHrcTimePoint;
FORCEINLINE f64  GetTimeDifferenceFromStaticStorageInitialization(const LHrcTimePoint& Point);
FORCEINLINE f64  GetTimeDiff(const LHrcTimePoint& A, const LHrcTimePoint& B);
FORCEINLINE f64  GetTimeDiffFromNow(const LHrcTimePoint& Point);
FORCEINLINE f64  GetDeltaSinceStaticStorageInitialization();

FORCEINLINE f64  GetDeltaTime();
FORCEINLINE f32  GetDeltaTimeAsFloat();
FORCEINLINE f64  GetRealDeltaTime();
FORCEINLINE f32  GetRealDeltaTimeAsFloat();
FORCEINLINE bool HasLostDeltaTime();
FORCEINLINE f64  GetLostDeltaTime();
FORCEINLINE bool HasIdleDeltaTime();
FORCEINLINE f64  GetIdleDeltaTime();
FORCEINLINE u64  GetFrameCount();

FORCEINLINE f64 GetCurrentFrameTime();
FORCEINLINE f64 GetPreviousFrameTime();

FORCEINLINE f32  GetCurrentFps();
FORCEINLINE f64  GetLowestDeltaTime();
FORCEINLINE f64  GetHighestDeltaTime();
FORCEINLINE f64  GetHighestLostDeltaTime();
FORCEINLINE f64  GetRealTimeOfPreviousStatisticsDuration();
FORCEINLINE u64  GetPreviousFrameCount();
FORCEINLINE f64  GetPreviousLowestDeltaTime();
FORCEINLINE f64  GetPreviousHighestDeltaTime();
FORCEINLINE f32  CalculateLowestFps();
FORCEINLINE f32  CalculateHighestFps();
FORCEINLINE auto GetLastStatisticsTime() -> LHrcTimePoint;
FORCEINLINE void SetLastStatisticsTime(const LHrcTimePoint LastStatisticsTime);
FORCEINLINE u64  GetStatisticsFrameCount();
FORCEINLINE void ResetStatistics();
FORCEINLINE f32  GetStatisticsPeriod();

//#
//# The maximum delta time allowed between frames.
//# See the #Private::LostDeltaTime for the time that is lost when lag spikes occur.
//#
FORCEINLINE constexpr f64 MaxDeltaTime { 1.0 / 3.0 };

namespace Private
{

ENGINE_API extern f64 DeltaTime;
ENGINE_API extern f64 RealDeltaTime;
ENGINE_API extern f64 LostDeltaTime;
ENGINE_API extern f64 IdleDeltaTime;
ENGINE_API extern u64 FrameCount;

ENGINE_API extern f64 CurrentFrameTime;
ENGINE_API extern f64 PreviousFrameTime;

ENGINE_API extern LHrcTimePoint StaticContainerInitializationTime;
ENGINE_API extern f64           LowestDeltaTime;
ENGINE_API extern f64           HighestDeltaTime;
ENGINE_API extern f64           HighestLostDeltaTime;
ENGINE_API extern LHrcTimePoint PreviousStatisticsStartTime;
ENGINE_API extern u64           PreviousStatisticsFrameCount;
ENGINE_API extern f64           PreviousLowestDeltaTime;
ENGINE_API extern f64           PreviousHighestDeltaTime;
ENGINE_API extern LHrcTimePoint LastStatisticsTime;
ENGINE_API extern u64           StatisticsFrameCount;
ENGINE_API extern f32           StatisticsPeriod;

ENGINE_API extern LHrcTimePoint LastStdOutFlushTime;

} /* ~Namespace Private */

} /* ~Namespace Application */

} /* ~Namespace Jafg */

FORCEINLINE Jafg::Application::LHrcTimePoint Jafg::Application::GetHighestNow()
{
    return Hrc::now();
}

FORCEINLINE f64 Jafg::Application::GetTimeDifferenceFromStaticStorageInitialization(const LHrcTimePoint& Point)
{
    return std::chrono::duration<f64>(Point - Private::StaticContainerInitializationTime).count();
}

FORCEINLINE f64 Jafg::Application::GetTimeDiff(const LHrcTimePoint& A, const LHrcTimePoint& B)
{
    return std::chrono::duration<f64>(B - A).count();
}

FORCEINLINE f64 Jafg::Application::GetTimeDiffFromNow(const LHrcTimePoint& Point)
{
    return Application::GetTimeDiff(Point, Application::GetHighestNow());
}

FORCEINLINE f64 Jafg::Application::GetDeltaSinceStaticStorageInitialization()
{
    return GetTimeDifferenceFromStaticStorageInitialization(GetHighestNow());
}

FORCEINLINE f64 Jafg::Application::GetDeltaTime()
{
    return Private::DeltaTime;
}

FORCEINLINE f32 Jafg::Application::GetDeltaTimeAsFloat()
{
    return static_cast<f32>(Private::DeltaTime);
}

FORCEINLINE f64 Jafg::Application::GetRealDeltaTime()
{
    return Private::RealDeltaTime;
}

FORCEINLINE f32 Jafg::Application::GetRealDeltaTimeAsFloat()
{
    return static_cast<f32>(Private::RealDeltaTime);
}

FORCEINLINE bool Jafg::Application:: HasLostDeltaTime()
{
    return Private::LostDeltaTime > 0.0;
}

FORCEINLINE f64 Jafg::Application:: GetLostDeltaTime()
{
    return Private::LostDeltaTime;
}

FORCEINLINE bool Jafg::Application:: HasIdleDeltaTime()
{
    return Private::IdleDeltaTime > 0.0;
}

FORCEINLINE f64 Jafg::Application:: GetIdleDeltaTime()
{
    return Private::IdleDeltaTime;
}

FORCEINLINE u64 Jafg::Application::GetFrameCount()
{
    return Private::FrameCount;
}

FORCEINLINE f64 Jafg::Application::GetCurrentFrameTime()
{
    return Private::CurrentFrameTime;
}

FORCEINLINE f64 Jafg::Application::GetPreviousFrameTime()
{
    return Private::PreviousFrameTime;
}

FORCEINLINE f32 Jafg::Application::GetCurrentFps()
{
    return static_cast<f32>(1.0 / Private::RealDeltaTime);
}

FORCEINLINE f64 Jafg::Application::GetLowestDeltaTime()
{
    return Private::LowestDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetHighestDeltaTime()
{
    return Private::HighestDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetHighestLostDeltaTime()
{
    return Private::HighestLostDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetRealTimeOfPreviousStatisticsDuration()
{
    return std::chrono::duration<f64>(Private::LastStatisticsTime - Private::PreviousStatisticsStartTime).count();
}

FORCEINLINE u64 Jafg::Application::GetPreviousFrameCount()
{
    return Private::PreviousStatisticsFrameCount;
}

FORCEINLINE f64 Jafg::Application::GetPreviousLowestDeltaTime()
{
    return Private::PreviousLowestDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetPreviousHighestDeltaTime()
{
    return Private::PreviousHighestDeltaTime;
}

FORCEINLINE f32 Jafg::Application::CalculateLowestFps()
{
    return static_cast<f32>(1.0 / Private::PreviousLowestDeltaTime);
}

FORCEINLINE f32 Jafg::Application::CalculateHighestFps()
{
    return static_cast<f32>(1.0 / Private::PreviousHighestDeltaTime);
}

FORCEINLINE Jafg::Application::LHrcTimePoint Jafg::Application::GetLastStatisticsTime()
{
    return Private::LastStatisticsTime;
}

FORCEINLINE void Jafg::Application::SetLastStatisticsTime(const LHrcTimePoint LastStatisticsTime)
{
    Private::LastStatisticsTime = LastStatisticsTime;
}

FORCEINLINE u64 Jafg::Application::GetStatisticsFrameCount()
{
    return Private::StatisticsFrameCount;
}

FORCEINLINE void Jafg::Application::ResetStatistics()
{
    Private::PreviousStatisticsStartTime  = Private::LastStatisticsTime;
    Private::PreviousStatisticsFrameCount = Private::StatisticsFrameCount;
    Private::PreviousLowestDeltaTime      = Private::LowestDeltaTime;
    Private::PreviousHighestDeltaTime     = Private::HighestDeltaTime;

    Private::LastStatisticsTime   = Application::GetHighestNow();
    Private::StatisticsFrameCount = 0;
    Private::LowestDeltaTime      = std::numeric_limits<f64>::max();
    Private::HighestDeltaTime     = -1.0;
    Private::HighestLostDeltaTime =  0.0;

    return;
}

FORCEINLINE f32 Jafg::Application::GetStatisticsPeriod()
{
    return Private::StatisticsPeriod;
}
