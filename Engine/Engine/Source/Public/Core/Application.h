// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg::Application
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
ENGINE_API extern LString CommandLine;
ENGINE_API extern bool    bDebuggerPresent;
ENGINE_API extern bool    bAlwaysReportCrash;

ENGINE_API void UpdateApplicationCommandLineVariables();

} /* ~Namespace Private */

FORCEINLINE auto GetCmdLine() -> const LString& { return Private::CommandLine; }
ENGINE_API  bool HasCmdLineParameter(const LString& Parameter);
FORCEINLINE bool IsDebuggerPresent() { return Private::bDebuggerPresent; }
FORCEINLINE bool IsAlwaysReportCrash() { return Private::bAlwaysReportCrash; }

//#
//# Global application functions.
//# All time related functions are measured in seconds except stated otherwise.
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

FORCEINLINE auto GetHighestNow() -> LHrcTimePoint;
FORCEINLINE auto GetTimeDifferenceFromStaticStorageInitialization(const LHrcTimePoint& Point) -> double;
FORCEINLINE auto GetTimeDiff(const LHrcTimePoint& A, const LHrcTimePoint& B) -> double;
FORCEINLINE auto GetDeltaSinceStaticStorageInitialization() -> double;

FORCEINLINE auto GetDeltaTime() -> double;
FORCEINLINE auto GetDeltaTimeAsFloat() -> float;
FORCEINLINE auto GetRealDeltaTime() -> double;
FORCEINLINE auto GetRealDeltaTimeAsFloat() -> double;
FORCEINLINE bool HasLostDeltaTime();
FORCEINLINE auto GetLostDeltaTime() -> double;
FORCEINLINE bool HasIdleDeltaTime();
FORCEINLINE auto GetIdleDeltaTime() -> double;
FORCEINLINE auto GetFrameCount() -> uint64;

FORCEINLINE auto GetCurrentFrameTime() -> double;
FORCEINLINE auto GetPreviousFrameTime() -> double;

FORCEINLINE auto GetCurrentFps() -> float;
FORCEINLINE auto GetLowestDeltaTime() -> double;
FORCEINLINE auto GetHighestDeltaTime() -> double;
FORCEINLINE auto GetHighestLostDeltaTime() -> double;
FORCEINLINE auto GetRealTimeOfPreviousStatisticsDuration() -> double;
FORCEINLINE auto GetPreviousFrameCount() -> uint64;
FORCEINLINE auto GetPreviousLowestDeltaTime() -> double;
FORCEINLINE auto GetPreviousHighestDeltaTime() -> double;
FORCEINLINE auto CalculateLowestFps() -> float;
FORCEINLINE auto CalculateHighestFps() -> float;
FORCEINLINE auto GetLastStatisticsTime() -> LHrcTimePoint;
FORCEINLINE auto SetLastStatisticsTime(const LHrcTimePoint LastStatisticsTime) -> void;
FORCEINLINE auto GetStatisticsFrameCount() -> uint64;
FORCEINLINE void ResetStatistics();
FORCEINLINE auto GetStatisticsPeriod() -> float;

//#
//# The maximum delta time allowed between frames.
//# See the #Private::LostDeltaTime for the time that is lost when lag spikes occur.
//#
FORCEINLINE constexpr double MaxDeltaTime { 1.0 / 3.0 };

namespace Private
{

ENGINE_API extern double DeltaTime;
ENGINE_API extern double RealDeltaTime;
ENGINE_API extern double LostDeltaTime;
ENGINE_API extern double IdleDeltaTime;
ENGINE_API extern uint64 FrameCount;

ENGINE_API extern double CurrentFrameTime;
ENGINE_API extern double PreviousFrameTime;

ENGINE_API extern LHrcTimePoint StaticContainerInitializationTime;
ENGINE_API extern double        LowestDeltaTime;
ENGINE_API extern double        HighestDeltaTime;
ENGINE_API extern double        HighestLostDeltaTime;
ENGINE_API extern LHrcTimePoint PreviousStatisticsStartTime;
ENGINE_API extern uint64        PreviousStatisticsFrameCount;
ENGINE_API extern double        PreviousLowestDeltaTime;
ENGINE_API extern double        PreviousHighestDeltaTime;
ENGINE_API extern LHrcTimePoint LastStatisticsTime;
ENGINE_API extern uint64        StatisticsFrameCount;
ENGINE_API extern float         StatisticsPeriod;

ENGINE_API extern LHrcTimePoint LastStdOutFlushTime;

} /* ~Namespace Private */

} /* ~Namespace Jafg::Application */

FORCEINLINE Jafg::Application::LHrcTimePoint Jafg::Application::GetHighestNow()
{
    return Hrc::now();
}

FORCEINLINE double Jafg::Application::GetTimeDifferenceFromStaticStorageInitialization(const LHrcTimePoint& Point)
{
    return std::chrono::duration<double>(Point - Private::StaticContainerInitializationTime).count();
}

FORCEINLINE double Jafg::Application::GetTimeDiff(const LHrcTimePoint& A, const LHrcTimePoint& B)
{
    return std::chrono::duration<double>(B - A).count();
}

FORCEINLINE double Jafg::Application::GetDeltaSinceStaticStorageInitialization()
{
    return GetTimeDifferenceFromStaticStorageInitialization(GetHighestNow());
}

FORCEINLINE double Jafg::Application::GetDeltaTime()
{
    return Private::DeltaTime;
}

FORCEINLINE float Jafg::Application::GetDeltaTimeAsFloat()
{
    return static_cast<float>(Private::DeltaTime);
}

FORCEINLINE double Jafg::Application::GetRealDeltaTime()
{
    return Private::RealDeltaTime;
}

FORCEINLINE double Jafg::Application::GetRealDeltaTimeAsFloat()
{
    return static_cast<float>(Private::RealDeltaTime);
}

FORCEINLINE bool Jafg::Application:: HasLostDeltaTime()
{
    return Private::LostDeltaTime > 0.0;
}

FORCEINLINE double Jafg::Application:: GetLostDeltaTime()
{
    return Private::LostDeltaTime;
}

FORCEINLINE bool Jafg::Application:: HasIdleDeltaTime()
{
    return Private::IdleDeltaTime > 0.0;
}

FORCEINLINE double Jafg::Application:: GetIdleDeltaTime()
{
    return Private::IdleDeltaTime;
}

FORCEINLINE uint64 Jafg::Application::GetFrameCount()
{
    return Private::FrameCount;
}

FORCEINLINE double Jafg::Application::GetCurrentFrameTime()
{
    return Private::CurrentFrameTime;
}

FORCEINLINE double Jafg::Application::GetPreviousFrameTime()
{
    return Private::PreviousFrameTime;
}

FORCEINLINE float Jafg::Application::GetCurrentFps()
{
    return static_cast<float>(1.0 / Private::RealDeltaTime);
}

FORCEINLINE double Jafg::Application::GetLowestDeltaTime()
{
    return Private::LowestDeltaTime;
}

FORCEINLINE double Jafg::Application::GetHighestDeltaTime()
{
    return Private::HighestDeltaTime;
}

FORCEINLINE double Jafg::Application::GetHighestLostDeltaTime()
{
    return Private::HighestLostDeltaTime;
}

FORCEINLINE double Jafg::Application::GetRealTimeOfPreviousStatisticsDuration()
{
    return std::chrono::duration<double>(Private::LastStatisticsTime - Private::PreviousStatisticsStartTime).count();
}

FORCEINLINE uint64 Jafg::Application::GetPreviousFrameCount()
{
    return Private::PreviousStatisticsFrameCount;
}

FORCEINLINE double Jafg::Application::GetPreviousLowestDeltaTime()
{
    return Private::PreviousLowestDeltaTime;
}

FORCEINLINE double Jafg::Application::GetPreviousHighestDeltaTime()
{
    return Private::PreviousHighestDeltaTime;
}

FORCEINLINE float Jafg::Application::CalculateLowestFps()
{
    return static_cast<float>(1.0 / Private::PreviousLowestDeltaTime);
}

FORCEINLINE float Jafg::Application::CalculateHighestFps()
{
    return static_cast<float>(1.0 / Private::PreviousHighestDeltaTime);
}

FORCEINLINE Jafg::Application::LHrcTimePoint Jafg::Application::GetLastStatisticsTime()
{
    return Private::LastStatisticsTime;
}

FORCEINLINE void Jafg::Application::SetLastStatisticsTime(const LHrcTimePoint LastStatisticsTime)
{
    Private::LastStatisticsTime = LastStatisticsTime;
}

FORCEINLINE uint64 Jafg::Application::GetStatisticsFrameCount()
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
    Private::LowestDeltaTime      = std::numeric_limits<double>::max();
    Private::HighestDeltaTime     = -1.0;
    Private::HighestLostDeltaTime =  0.0;

    return;
}

FORCEINLINE float Jafg::Application::GetStatisticsPeriod()
{
    return Private::StatisticsPeriod;
}
