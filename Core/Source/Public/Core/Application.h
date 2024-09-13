// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Application
{

namespace Private
{

typedef std::chrono::steady_clock::time_point SteadyStatisticsTimePoint;

}

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
FORCEINLINE auto SetCurrentFps(const float CurrentFps) -> void;
FORCEINLINE auto GetLowestFps() -> float;
FORCEINLINE auto SetLowestFps(const float LowestFps) -> void;
FORCEINLINE auto GetHighestFps() -> float;
FORCEINLINE auto SetHighestFps(const float HighestFps) -> void;
FORCEINLINE auto GetLastStatisticsTime() -> Private::SteadyStatisticsTimePoint;
FORCEINLINE auto SetLastStatisticsTime(const Private::SteadyStatisticsTimePoint LastStatisticsTime) -> void;
FORCEINLINE auto GetStatisticsFrameCount() -> uint64;
FORCEINLINE auto ResetStatistics() -> void;

namespace Private
{

extern double DeltaTime;
extern uint64 FrameCount;

extern double CurrentFrameTime;
extern double PreviousFrameTime;

extern float                     CurrentFps;
extern float                     LowestFps;
extern float                     HighestFps;
extern SteadyStatisticsTimePoint LastStatisticsTime;
extern uint64                    StatisticsFrameCount;

}

}

FORCEINLINE void Application::SetDeltaTime(const double DeltaTime)
{
    Private::DeltaTime = DeltaTime;
}

FORCEINLINE double Application::GetDeltaTime()
{
    return Private::DeltaTime;
}

FORCEINLINE float Application::GetDeltaTimeAsFloat()
{
    return static_cast<float>(Private::DeltaTime);
}

FORCEINLINE void Application::UpdateFrameCount()
{
    ++Private::FrameCount;
    ++Private::StatisticsFrameCount;
}

FORCEINLINE uint64 Application::GetFrameCount()
{
    return Private::FrameCount;
}

FORCEINLINE double Application::GetCurrentFrameTime()
{
    return Private::CurrentFrameTime;
}

FORCEINLINE void Application::SetCurrentFrameTime(const double CurrentFrameTime)
{
    Private::CurrentFrameTime = CurrentFrameTime;
}

FORCEINLINE double Application::GetPreviousFrameTime()
{
    return Private::PreviousFrameTime;
}

FORCEINLINE void Application::SetPreviousFrameTime(const double PreviousFrameTime)
{
    Private::PreviousFrameTime = PreviousFrameTime;
}

FORCEINLINE float Application::GetCurrentFps()
{
    return Private::CurrentFps;
}

FORCEINLINE void Application::SetCurrentFps(const float CurrentFps)
{
    Private::CurrentFps = CurrentFps;
}

FORCEINLINE float Application::GetLowestFps()
{
    return Private::LowestFps;
}

FORCEINLINE void Application::SetLowestFps(const float LowestFps)
{
    Private::LowestFps = LowestFps;
}

FORCEINLINE float Application::GetHighestFps()
{
    return Private::HighestFps;
}

FORCEINLINE void Application::SetHighestFps(const float HighestFps)
{
    Private::HighestFps = HighestFps;
}

FORCEINLINE Application::Private::SteadyStatisticsTimePoint Application::GetLastStatisticsTime()
{
    return Private::LastStatisticsTime;
}

FORCEINLINE void Application::SetLastStatisticsTime(const Private::SteadyStatisticsTimePoint LastStatisticsTime)
{
    Private::LastStatisticsTime = LastStatisticsTime;
}

FORCEINLINE uint64 Application::GetStatisticsFrameCount()
{
    return Private::StatisticsFrameCount;
}

FORCEINLINE auto Application::ResetStatistics() -> void
{
    Private::LastStatisticsTime   = std::chrono::steady_clock::now();
    Private::StatisticsFrameCount = 0;
    Private::LowestFps            = -1.0f;
    Private::HighestFps           = -1.0f;

    return;
}
