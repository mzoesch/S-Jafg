// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg::Application
{

namespace Private
{

typedef std::chrono::high_resolution_clock             Hrc;
typedef std::chrono::high_resolution_clock::time_point LHrcTimePoint;

}

/**
 * Global application functions.
 * All time related functions are measured in seconds except stated otherwise.
 */
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

FORCEINLINE auto GetHighestNow() -> Private::LHrcTimePoint;
FORCEINLINE auto GetTimeDifferenceFromStaticStorageInitialization(const Private::LHrcTimePoint& Point) -> double;
FORCEINLINE auto GetTimeDiff(const Private::LHrcTimePoint& A, const Private::LHrcTimePoint& B) -> double;
FORCEINLINE auto GetDeltaSinceStaticStorageInitialization() -> double;

FORCEINLINE auto SetDeltaTime(const double DeltaTime) -> void;
FORCEINLINE auto GetDeltaTime() -> double;
FORCEINLINE auto GetDeltaTimeAsFloat() -> float;
FORCEINLINE auto UpdateFrameCount() -> void;
FORCEINLINE auto GetFrameCount() -> uint64;

FORCEINLINE auto GetCurrentFrameTime() -> double;
FORCEINLINE auto SetCurrentFrameTime(const double CurrentFrameTime) -> void;
FORCEINLINE auto GetPreviousFrameTime() -> double;
FORCEINLINE auto SetPreviousFrameTime(const double PreviousFrameTime) -> void;

FORCEINLINE auto GetCurrentFps() -> float;
FORCEINLINE auto GetLowestDeltaTime() -> double;
FORCEINLINE auto SetLowestDeltaTime(const double LowestDeltaTime) -> void;
FORCEINLINE auto GetHighestDeltaTime() -> double;
FORCEINLINE auto SetHighestDeltaTime(const double HighestDeltaTime) -> void;
FORCEINLINE auto GetRealTimeOfPreviousStatisticsDuration() -> double;
FORCEINLINE auto GetPreviousFrameCount() -> uint64;
FORCEINLINE auto GetPreviousLowestDeltaTime() -> double;
FORCEINLINE auto GetPreviousHighestDeltaTime() -> double;
FORCEINLINE auto CalculateLowestFps() -> float;
FORCEINLINE auto CalculateHighestFps() -> float;
FORCEINLINE auto GetLastStatisticsTime() -> Private::LHrcTimePoint;
FORCEINLINE auto SetLastStatisticsTime(const Private::LHrcTimePoint LastStatisticsTime) -> void;
FORCEINLINE auto GetStatisticsFrameCount() -> uint64;
FORCEINLINE auto ResetStatistics() -> void;
FORCEINLINE auto GetStatisticsPeriod() -> float;

namespace Private
{

ENGINE_API extern double DeltaTime;
ENGINE_API extern uint64 FrameCount;

ENGINE_API extern double CurrentFrameTime;
ENGINE_API extern double PreviousFrameTime;

ENGINE_API extern double        LowestDeltaTime;
ENGINE_API extern double        HighestDeltaTime;
ENGINE_API extern LHrcTimePoint PreviousStatisticsStartTime;
ENGINE_API extern uint64        PreviousStatisticsFrameCount;
ENGINE_API extern double        PreviousLowestDeltaTime;
ENGINE_API extern double        PreviousHighestDeltaTime;
ENGINE_API extern LHrcTimePoint StaticContainerInitializationTime;
ENGINE_API extern LHrcTimePoint LastStatisticsTime;
ENGINE_API extern uint64        StatisticsFrameCount;
ENGINE_API extern float         StatisticsPeriod;

} /* ~Namespace Private */

} /* ~Namespace Jafg::Application */

FORCEINLINE Jafg::Application::Private::LHrcTimePoint Jafg::Application::GetHighestNow()
{
    return Private::Hrc::now();
}

FORCEINLINE double Jafg::Application::GetTimeDifferenceFromStaticStorageInitialization(const Private::LHrcTimePoint& Point)
{
    return std::chrono::duration<double>(Point - Private::StaticContainerInitializationTime).count();
}

FORCEINLINE auto Jafg::Application::GetTimeDiff(const Private::LHrcTimePoint& A, const Private::LHrcTimePoint& B) -> double
{
    return std::chrono::duration<double>(A - B).count();
}

FORCEINLINE double Jafg::Application::GetDeltaSinceStaticStorageInitialization()
{
    return GetTimeDifferenceFromStaticStorageInitialization(GetHighestNow());
}

FORCEINLINE void Jafg::Application::SetDeltaTime(const double DeltaTime)
{
    Private::DeltaTime = DeltaTime;
}

FORCEINLINE double Jafg::Application::GetDeltaTime()
{
    return Private::DeltaTime;
}

FORCEINLINE float Jafg::Application::GetDeltaTimeAsFloat()
{
    return static_cast<float>(Private::DeltaTime);
}

FORCEINLINE void Jafg::Application::UpdateFrameCount()
{
    ++Private::FrameCount;
    ++Private::StatisticsFrameCount;

    return;
}

FORCEINLINE uint64 Jafg::Application::GetFrameCount()
{
    return Private::FrameCount;
}

FORCEINLINE double Jafg::Application::GetCurrentFrameTime()
{
    return Private::CurrentFrameTime;
}

FORCEINLINE void Jafg::Application::SetCurrentFrameTime(const double CurrentFrameTime)
{
    Private::CurrentFrameTime = CurrentFrameTime;
}

FORCEINLINE double Jafg::Application::GetPreviousFrameTime()
{
    return Private::PreviousFrameTime;
}

FORCEINLINE void Jafg::Application::SetPreviousFrameTime(const double PreviousFrameTime)
{
    Private::PreviousFrameTime = PreviousFrameTime;
}

FORCEINLINE float Jafg::Application::GetCurrentFps()
{
    return static_cast<float>(1.0 / Private::DeltaTime);
}

FORCEINLINE double Jafg::Application::GetLowestDeltaTime()
{
    return Private::LowestDeltaTime;
}

FORCEINLINE void Jafg::Application::SetLowestDeltaTime(const double LowestDeltaTime)
{
    Private::LowestDeltaTime = LowestDeltaTime;
}

FORCEINLINE double Jafg::Application::GetHighestDeltaTime()
{
    return Private::HighestDeltaTime;
}

FORCEINLINE void Jafg::Application::SetHighestDeltaTime(const double HighestDeltaTime)
{
    Private::HighestDeltaTime = HighestDeltaTime;
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

FORCEINLINE Jafg::Application::Private::LHrcTimePoint Jafg::Application::GetLastStatisticsTime()
{
    return Private::LastStatisticsTime;
}

FORCEINLINE void Jafg::Application::SetLastStatisticsTime(const Private::LHrcTimePoint LastStatisticsTime)
{
    Private::LastStatisticsTime = LastStatisticsTime;
}

FORCEINLINE uint64 Jafg::Application::GetStatisticsFrameCount()
{
    return Private::StatisticsFrameCount;
}

FORCEINLINE auto Jafg::Application::ResetStatistics() -> void
{
    Private::PreviousStatisticsStartTime  = Private::LastStatisticsTime;
    Private::PreviousStatisticsFrameCount = Private::StatisticsFrameCount;
    Private::PreviousLowestDeltaTime      = Private::LowestDeltaTime;
    Private::PreviousHighestDeltaTime     = Private::HighestDeltaTime;

    Private::LastStatisticsTime   = Application::GetHighestNow();
    Private::StatisticsFrameCount = 0;
    Private::LowestDeltaTime      = std::numeric_limits<double>::max();
    Private::HighestDeltaTime     = -1.0;

    return;
}

FORCEINLINE float Jafg::Application::GetStatisticsPeriod()
{
    return Private::StatisticsPeriod;
}
