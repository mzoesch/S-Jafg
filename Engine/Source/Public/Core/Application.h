// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg::Application
{

namespace Private
{

typedef std::chrono::steady_clock::time_point SteadyStatisticsTimePoint;

}

FORCEINLINE ENGINE_API auto SetDeltaTime(const double DeltaTime) -> void;
FORCEINLINE ENGINE_API auto GetDeltaTime() -> double;
FORCEINLINE ENGINE_API auto GetDeltaTimeAsFloat() -> float;
FORCEINLINE ENGINE_API auto UpdateFrameCount() -> void;
FORCEINLINE ENGINE_API auto GetFrameCount() -> uint64;

FORCEINLINE ENGINE_API auto GetCurrentFrameTime() -> double;
FORCEINLINE ENGINE_API auto SetCurrentFrameTime(const double CurrentFrameTime) -> void;
FORCEINLINE ENGINE_API auto GetPreviousFrameTime() -> double;
FORCEINLINE ENGINE_API auto SetPreviousFrameTime(const double PreviousFrameTime) -> void;

FORCEINLINE ENGINE_API auto GetCurrentFps() -> float;
FORCEINLINE ENGINE_API auto SetCurrentFps(const float CurrentFps) -> void;
FORCEINLINE ENGINE_API auto GetLowestFps() -> float;
FORCEINLINE ENGINE_API auto SetLowestFps(const float LowestFps) -> void;
FORCEINLINE ENGINE_API auto GetHighestFps() -> float;
FORCEINLINE ENGINE_API auto SetHighestFps(const float HighestFps) -> void;
FORCEINLINE ENGINE_API auto GetLastStatisticsTime() -> Private::SteadyStatisticsTimePoint;
FORCEINLINE ENGINE_API auto SetLastStatisticsTime(const Private::SteadyStatisticsTimePoint LastStatisticsTime) -> void;
FORCEINLINE ENGINE_API auto GetStatisticsFrameCount() -> uint64;
FORCEINLINE ENGINE_API auto ResetStatistics() -> void;

namespace Private
{

ENGINE_API extern double DeltaTime;
ENGINE_API extern uint64 FrameCount;

ENGINE_API extern double CurrentFrameTime;
ENGINE_API extern double PreviousFrameTime;

ENGINE_API extern float                     CurrentFps;
ENGINE_API extern float                     LowestFps;
ENGINE_API extern float                     HighestFps;
ENGINE_API extern SteadyStatisticsTimePoint LastStatisticsTime;
ENGINE_API extern uint64                    StatisticsFrameCount;

}

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
    return Private::CurrentFps;
}

FORCEINLINE void Jafg::Application::SetCurrentFps(const float CurrentFps)
{
    Private::CurrentFps = CurrentFps;
}

FORCEINLINE float Jafg::Application::GetLowestFps()
{
    return Private::LowestFps;
}

FORCEINLINE void Jafg::Application::SetLowestFps(const float LowestFps)
{
    Private::LowestFps = LowestFps;
}

FORCEINLINE float Jafg::Application::GetHighestFps()
{
    return Private::HighestFps;
}

FORCEINLINE void Jafg::Application::SetHighestFps(const float HighestFps)
{
    Private::HighestFps = HighestFps;
}

FORCEINLINE Jafg::Application::Private::SteadyStatisticsTimePoint Jafg::Application::GetLastStatisticsTime()
{
    return Private::LastStatisticsTime;
}

FORCEINLINE void Jafg::Application::SetLastStatisticsTime(const Private::SteadyStatisticsTimePoint LastStatisticsTime)
{
    Private::LastStatisticsTime = LastStatisticsTime;
}

FORCEINLINE uint64 Jafg::Application::GetStatisticsFrameCount()
{
    return Private::StatisticsFrameCount;
}

FORCEINLINE auto Jafg::Application::ResetStatistics() -> void
{
    Private::LastStatisticsTime   = std::chrono::steady_clock::now();
    Private::StatisticsFrameCount = 0;
    Private::LowestFps            = -1.0f;
    Private::HighestFps           = -1.0f;

    return;
}
