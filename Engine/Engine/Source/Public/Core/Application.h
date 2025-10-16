// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Core/Arguments.h"

namespace Jafg
{

namespace Hal
{

//#
//# Very dangerous function. Use with care and never in critical code paths.
//# Currently not supported for all platforms.
//#
ENGINE_API void Sleep(const f64 InSeconds);

} /* ~Namespace PlatformHal */

namespace Application
{

typedef std::chrono::high_resolution_clock             Hrc;
typedef std::chrono::high_resolution_clock::time_point LHrcTimePoint;

typedef LGenericArgument LProgramArgument;

namespace Private
{

ENGINE_API void WaitForDebuggerGracefully(const bool bAllowInstantBreak);

//#
//# The command line. A parameter is defined as the following:
//#   -parameter
//#   -parameter=value                  (no spaces allowed)
//#   -parameter="string value"         (escape " with \)
//#   -parameter="\"item\" \"item\" \"item\""
//#   -parameter item item item         (no - allowed on items)
//#
ENGINE_API extern TArray<LString> RawCommandLine;
ENGINE_API extern TArray<LProgramArgument> ProcessedCommandLine;
ENGINE_API void ProcessCommandLineVariables();

ENGINE_API auto ConditionallyShowHelpAndExit() -> std::tuple<bool, EPlatformExit::Type>;
ENGINE_API auto ConditionallyShowVersionAndExit() -> std::tuple<bool, EPlatformExit::Type>;

//# Not all platforms may respect this.
ENGINE_API extern bool bPauseBeforeExit;
ENGINE_API extern bool bHasTracerPid;
ENGINE_API extern bool bAlwaysReportCrash;

#if WITH_STATS
    ENGINE_API extern bool bAllowProfiling;
#endif /* WITH_STATS */

} /* ~Namespace Private */

FORCEINLINE auto GetRawCmdLine() -> const TArray<LString>& { return Private::RawCommandLine; }
FORCEINLINE auto GetProcessedCmdLine() -> const TArray<LProgramArgument>& { return Private::ProcessedCommandLine; }

//#
//# Whether the command line contains the given parameter.
//# This function will not work during static storage initialization or early program startup.
//#
ENGINE_API  bool HasCmdLineParameter(const LString& Parameter, LProgramArgument const** Out = nullptr) noexcept;

FORCEINLINE bool IsPauseBeforeExit() noexcept { return Private::bPauseBeforeExit; }

//# Whether the current process has a pid for any sort of tracer.
FORCEINLINE bool HasTracerPid() noexcept { return Private::bHasTracerPid; }
//# Whether the current process now has a pid for any sort of tracer. This overrides the result of #HasTracerPid.
ENGINE_API  bool HasTracerPidNow();

FORCEINLINE bool IsAlwaysReportCrash() noexcept { return Private::bAlwaysReportCrash; }

ENGINE_API  bool CanEverProfile() noexcept;
ENGINE_API  bool IsAllowProfiling() noexcept;

//#
//# Global application functions.
//# All time related functions are measured in seconds except stated otherwise.
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

FORCEINLINE auto GetHighestNow() noexcept -> LHrcTimePoint;
FORCEINLINE f64  GetTimeDifferenceFromStaticStorageInitialization(const LHrcTimePoint& Point) noexcept;
FORCEINLINE f64  GetTimeDiff(const LHrcTimePoint& A, const LHrcTimePoint& B) noexcept;
FORCEINLINE f64  GetTimeDiffFromNow(const LHrcTimePoint& Point) noexcept;
FORCEINLINE f64  GetDeltaSinceStaticStorageInitialization() noexcept;

FORCEINLINE f64  GetDeltaTime() noexcept;
FORCEINLINE f32  GetDeltaTimeAsFloat() noexcept;
FORCEINLINE f64  GetRealDeltaTime() noexcept;
FORCEINLINE f32  GetRealDeltaTimeAsFloat() noexcept;
FORCEINLINE bool HasLostDeltaTime() noexcept;
FORCEINLINE f64  GetLostDeltaTime() noexcept;
FORCEINLINE bool HasIdleDeltaTime() noexcept;
FORCEINLINE f64  GetIdleDeltaTime() noexcept;
FORCEINLINE u64  GetFrameCount() noexcept;

FORCEINLINE f64  GetCurrentFrameTime() noexcept;
FORCEINLINE f64  GetPreviousFrameTime() noexcept;

FORCEINLINE f32  GetCurrentFps() noexcept;
FORCEINLINE f64  GetLowestDeltaTime() noexcept;
FORCEINLINE f64  GetHighestDeltaTime() noexcept;
FORCEINLINE f64  GetHighestLostDeltaTime() noexcept;
FORCEINLINE f64  GetHighestIdleDeltaTime() noexcept;
FORCEINLINE f64  GetRealTimeOfPreviousStatisticsDuration() noexcept;
FORCEINLINE u64  GetPreviousFrameCount() noexcept;
FORCEINLINE f64  GetPreviousLowestDeltaTime() noexcept;
FORCEINLINE f64  GetPreviousHighestDeltaTime() noexcept;
FORCEINLINE f64  GetPreviousHighestLostDeltaTime() noexcept;
FORCEINLINE f64  GetPreviousHighestIdleDeltaTime() noexcept;
FORCEINLINE f32  CalculateLowestFps() noexcept;
FORCEINLINE f32  CalculateHighestFps() noexcept;
FORCEINLINE auto GetLastStatisticsTime() noexcept -> LHrcTimePoint;
FORCEINLINE void SetLastStatisticsTime(const LHrcTimePoint LastStatisticsTime) noexcept;
FORCEINLINE u64  GetStatisticsFrameCount() noexcept;
FORCEINLINE void ResetStatistics() noexcept;
FORCEINLINE f32  GetStatisticsPeriod() noexcept;

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
ENGINE_API extern f64           HighestIdleDeltaTime;
ENGINE_API extern LHrcTimePoint PreviousStatisticsStartTime;
ENGINE_API extern u64           PreviousStatisticsFrameCount;
ENGINE_API extern f64           PreviousLowestDeltaTime;
ENGINE_API extern f64           PreviousHighestDeltaTime;
ENGINE_API extern f64           PreviousHighestLostDeltaTime;
ENGINE_API extern f64           PreviousHighestIdleDeltaTime;
ENGINE_API extern LHrcTimePoint LastStatisticsTime;
ENGINE_API extern u64           StatisticsFrameCount;
ENGINE_API extern f32           StatisticsPeriod;

ENGINE_API extern LHrcTimePoint LastStdOutFlushTime;

} /* ~Namespace Private */

} /* ~Namespace Application */

} /* ~Namespace Jafg */

FORCEINLINE Jafg::Application::LHrcTimePoint Jafg::Application::GetHighestNow() noexcept
{
    return Hrc::now();
}

FORCEINLINE f64 Jafg::Application::GetTimeDifferenceFromStaticStorageInitialization(const LHrcTimePoint& Point) noexcept
{
    return std::chrono::duration<f64>(Point - Private::StaticContainerInitializationTime).count();
}

FORCEINLINE f64 Jafg::Application::GetTimeDiff(const LHrcTimePoint& A, const LHrcTimePoint& B) noexcept
{
    return std::chrono::duration<f64>(B - A).count();
}

FORCEINLINE f64 Jafg::Application::GetTimeDiffFromNow(const LHrcTimePoint& Point) noexcept
{
    return Application::GetTimeDiff(Point, Application::GetHighestNow());
}

FORCEINLINE f64 Jafg::Application::GetDeltaSinceStaticStorageInitialization() noexcept
{
    return GetTimeDifferenceFromStaticStorageInitialization(GetHighestNow());
}

FORCEINLINE f64 Jafg::Application::GetDeltaTime() noexcept
{
    return Private::DeltaTime;
}

FORCEINLINE f32 Jafg::Application::GetDeltaTimeAsFloat() noexcept
{
    return static_cast<f32>(Private::DeltaTime);
}

FORCEINLINE f64 Jafg::Application::GetRealDeltaTime() noexcept
{
    return Private::RealDeltaTime;
}

FORCEINLINE f32 Jafg::Application::GetRealDeltaTimeAsFloat() noexcept
{
    return static_cast<f32>(Private::RealDeltaTime);
}

FORCEINLINE bool Jafg::Application:: HasLostDeltaTime() noexcept
{
    return Private::LostDeltaTime > 0.0;
}

FORCEINLINE f64 Jafg::Application:: GetLostDeltaTime() noexcept
{
    return Private::LostDeltaTime;
}

FORCEINLINE bool Jafg::Application:: HasIdleDeltaTime() noexcept
{
    return Private::IdleDeltaTime > 0.0;
}

FORCEINLINE f64 Jafg::Application:: GetIdleDeltaTime() noexcept
{
    return Private::IdleDeltaTime;
}

FORCEINLINE u64 Jafg::Application::GetFrameCount() noexcept
{
    return Private::FrameCount;
}

FORCEINLINE f64 Jafg::Application::GetCurrentFrameTime() noexcept
{
    return Private::CurrentFrameTime;
}

FORCEINLINE f64 Jafg::Application::GetPreviousFrameTime() noexcept
{
    return Private::PreviousFrameTime;
}

FORCEINLINE f32 Jafg::Application::GetCurrentFps() noexcept
{
    return static_cast<f32>(1.0 / Private::RealDeltaTime);
}

FORCEINLINE f64 Jafg::Application::GetLowestDeltaTime() noexcept
{
    return Private::LowestDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetHighestDeltaTime() noexcept
{
    return Private::HighestDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetHighestLostDeltaTime() noexcept
{
    return Private::HighestLostDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetHighestIdleDeltaTime() noexcept
{
    return Private::HighestIdleDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetRealTimeOfPreviousStatisticsDuration() noexcept
{
    return std::chrono::duration<f64>(Private::LastStatisticsTime - Private::PreviousStatisticsStartTime).count();
}

FORCEINLINE u64 Jafg::Application::GetPreviousFrameCount() noexcept
{
    return Private::PreviousStatisticsFrameCount;
}

FORCEINLINE f64 Jafg::Application::GetPreviousLowestDeltaTime() noexcept
{
    return Private::PreviousLowestDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetPreviousHighestDeltaTime() noexcept
{
    return Private::PreviousHighestDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetPreviousHighestLostDeltaTime() noexcept
{
    return Private::PreviousHighestLostDeltaTime;
}

FORCEINLINE f64 Jafg::Application::GetPreviousHighestIdleDeltaTime() noexcept
{
    return Private::PreviousHighestIdleDeltaTime;
}

FORCEINLINE f32 Jafg::Application::CalculateLowestFps() noexcept
{
    return static_cast<f32>(1.0 / Private::PreviousLowestDeltaTime);
}

FORCEINLINE f32 Jafg::Application::CalculateHighestFps() noexcept
{
    return static_cast<f32>(1.0 / Private::PreviousHighestDeltaTime);
}

FORCEINLINE Jafg::Application::LHrcTimePoint Jafg::Application::GetLastStatisticsTime() noexcept
{
    return Private::LastStatisticsTime;
}

FORCEINLINE void Jafg::Application::SetLastStatisticsTime(const LHrcTimePoint LastStatisticsTime) noexcept
{
    Private::LastStatisticsTime = LastStatisticsTime;
}

FORCEINLINE u64 Jafg::Application::GetStatisticsFrameCount() noexcept
{
    return Private::StatisticsFrameCount;
}

FORCEINLINE void Jafg::Application::ResetStatistics() noexcept
{
    Private::PreviousStatisticsStartTime  = Private::LastStatisticsTime;
    Private::PreviousStatisticsFrameCount = Private::StatisticsFrameCount;
    Private::PreviousLowestDeltaTime      = Private::LowestDeltaTime;
    Private::PreviousHighestDeltaTime     = Private::HighestDeltaTime;
    Private::PreviousHighestLostDeltaTime = Private::HighestLostDeltaTime;
    Private::PreviousHighestIdleDeltaTime = Private::HighestIdleDeltaTime;

    Private::LastStatisticsTime   = Application::GetHighestNow();
    Private::StatisticsFrameCount = 0;
    Private::LowestDeltaTime      = std::numeric_limits<f64>::max();
    Private::HighestDeltaTime     = -1.0;
    Private::HighestLostDeltaTime =  0.0;
    Private::HighestIdleDeltaTime =  0.0;

    return;
}

FORCEINLINE f32 Jafg::Application::GetStatisticsPeriod() noexcept
{
    return Private::StatisticsPeriod;
}
