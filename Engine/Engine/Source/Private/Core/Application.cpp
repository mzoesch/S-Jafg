// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Core/Application.h"

namespace Jafg::Application::Private
{

ENGINE_API double DeltaTime     { 0.0 };
ENGINE_API uint64 FrameCount    { 0   };

ENGINE_API double CurrentFrameTime  { 0.0 };
ENGINE_API double PreviousFrameTime { 0.0 };

ENGINE_API double        LowestDeltaTime                   { std::numeric_limits<double>::max() };
ENGINE_API double        HighestDeltaTime                  { -1.0 };
ENGINE_API LHrcTimePoint PreviousStatisticsStartTime       { StaticContainerInitializationTime };
ENGINE_API uint64        PreviousStatisticsFrameCount      { FrameCount       };
ENGINE_API double        PreviousLowestDeltaTime           { LowestDeltaTime  };
ENGINE_API double        PreviousHighestDeltaTime          { HighestDeltaTime };
ENGINE_API LHrcTimePoint StaticContainerInitializationTime { Hrc::now() };
ENGINE_API LHrcTimePoint LastStatisticsTime                { StaticContainerInitializationTime };
ENGINE_API uint64        StatisticsFrameCount              { 0 };
ENGINE_API float         StatisticsPeriod                  { 1.0f };

}
