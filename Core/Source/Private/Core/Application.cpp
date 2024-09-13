// Copyright mzoesch. All rights reserved.

#include "Core/Application.h"

namespace Application::Private
{

double DeltaTime     { 0.0f };
uint64 FrameCount    { 0    };

double CurrentFrameTime  { 0.0f };
double PreviousFrameTime { 0.0f };

float                     CurrentFps         { 0.0f };
float                     LowestFps          { -1.0f };
float                     HighestFps         { -1.0f };
SteadyStatisticsTimePoint LastStatisticsTime { std::chrono::steady_clock::now() };
uint64                    StatisticsFrameCount { 0 };

}
