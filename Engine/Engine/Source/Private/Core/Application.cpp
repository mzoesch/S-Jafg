// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Core/Application.h"

namespace Jafg::Application::Private
{

ENGINE_API double DeltaTime     { 0.0f };
ENGINE_API uint64 FrameCount    { 0    };

ENGINE_API double CurrentFrameTime  { 0.0f };
ENGINE_API double PreviousFrameTime { 0.0f };

ENGINE_API float                     CurrentFps         { 0.0f };
ENGINE_API float                     LowestFps          { -1.0f };
ENGINE_API float                     HighestFps         { -1.0f };
ENGINE_API SteadyStatisticsTimePoint LastStatisticsTime { std::chrono::steady_clock::now() };
ENGINE_API uint64                    StatisticsFrameCount { 0 };

}
