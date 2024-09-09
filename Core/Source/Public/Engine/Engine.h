// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Runtime/Platform/Surface.h"

class JEngine;
struct LWorldContext;

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

extern JEngine* GEngine;

/**
 * Whether the engine should exit at the next opportunity.
 */
extern bool    bGShouldRequestExit;
/**
 * Whether the engine has successfully received an exit request and is now beginning to tear down.
 */
extern bool    bGEngineRequestingExit;

extern int32   GCustomExitStatusOverride;
extern String  GCustomExitReason;

FORCEINLINE auto IsEngine() -> bool { return GEngine; }
FORCEINLINE auto GetEngine() -> JEngine* { return GEngine; }

FORCEINLINE auto IsEngineExitRequested() -> bool { return bGShouldRequestExit; }
FORCEINLINE auto IsTearingDown() -> bool { return bGEngineRequestingExit; }
FORCEINLINE auto WillShortlyTerminate() -> bool { return bGShouldRequestExit || bGEngineRequestingExit; }

FORCEINLINE auto HasCustomExitStatus() -> bool { return GCustomExitStatusOverride != INDEX_NONE; }
FORCEINLINE auto GetCustomExitStatus() -> int32 { return GCustomExitStatusOverride; }
FORCEINLINE auto HasCustomExitReason() -> bool { return !GCustomExitReason.empty(); }
FORCEINLINE auto GetCustomExitReason() -> String { return GCustomExitReason; }

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

struct LPlatformSpecificCallbacks
{
    std::unique_ptr<Surface> (*CreateSurface)();
    void (*OnTearDown)(std::unique_ptr<Surface>* ActiveSurface);
    double (*GetHighestResolutionMonotonicTime)();
};

class JEngine
{
    typedef std::chrono::steady_clock::time_point SteadyStatisticsTimePoint;

public:

    void Init(std::unique_ptr<LPlatformSpecificCallbacks>&& InPlatformSpecificCallbacks);
    void Tick(const float DeltaSeconds);
    void TearDown();

    void UpdateTime();
    void EnforceTickRate();

    ///////////////////////////////////////////////////////////////////////////////
    // Context Related
    ///////////////////////////////////////////////////////////////////////////////

    void Browse(LWorldContext& Context, const String& Url);

    ///////////////////////////////////////////////////////////////////////////////
    // Private function redirects
    ///////////////////////////////////////////////////////////////////////////////

    void BeginExitIfRequested();
    void RequestEngineExit();
    void RequestEngineExit(const String& Reason);
    void RequestEngineExit(const int32 CustomExitStatus);
    void RequestEngineExit(const int32 CustomExitStatus, const String& Reason);

private:

    void TickForWorldTravel(LWorldContext& Context);
    void InitializeContext(LWorldContext& Context);

    LWorldContext& CreateNewWorldContext();

    ///////////////////////////////////////////////////////////////////////////////
    // Time
    ///////////////////////////////////////////////////////////////////////////////

    double                    CurrentFrameTime          = 0.0;
    double                    LastFrameTime             = 0.0;
    float                     CurrentFps                = 0.0f;
    SteadyStatisticsTimePoint StatisticsSteadyTimeStart = std::chrono::steady_clock::now();
    uint64                    StatisticsFrameCount      = 0;
    float                     LowestFps                 = -1.0f;
    float                     HighestFps                = -1.0f;

    ///////////////////////////////////////////////////////////////////////////////
    // Platform Specific
    ///////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<LPlatformSpecificCallbacks> PlatformSpecificCallbacks = nullptr;
    std::unique_ptr<Surface> ActiveSurface = nullptr;

    ///////////////////////////////////////////////////////////////////////////////
    // Worlds
    ///////////////////////////////////////////////////////////////////////////////

    std::vector<LWorldContext*> Contexts;
};
