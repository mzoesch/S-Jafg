// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "IO/Surface.h"

class Engine;

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

extern Engine* GEngine;

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
FORCEINLINE auto GetEngine() -> Engine* { return GEngine; }

FORCEINLINE auto IsEngineExitRequested() -> bool { return bGShouldRequestExit; }
FORCEINLINE auto IsTearingDown() -> bool { return bGEngineRequestingExit; }
FORCEINLINE auto WillShortlyTerminate() -> bool { return bGShouldRequestExit || bGEngineRequestingExit; }

FORCEINLINE auto HasCustomExitStatus() -> bool { return GCustomExitStatusOverride != INDEX_NONE; }
FORCEINLINE auto GetCustomExitStatus() -> int32 { return GCustomExitStatusOverride; }
FORCEINLINE auto HasCustomExitReason() -> bool { return !GCustomExitReason.empty(); }
FORCEINLINE auto GetCustomExitReason() -> String { return GCustomExitReason; }

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

class Engine
{
public:

    void Init();
    void Tick(const float DeltaSeconds);
    void TearDown();

    void UpdateTime();
    void EnforceTickRate();

private:

    std::unique_ptr<Surface> ActiveSurface = nullptr;
};
