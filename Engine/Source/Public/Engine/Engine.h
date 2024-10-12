// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg { class LEngine; }

namespace Jafg
{

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

ENGINE_API extern LEngine* GEngine;

/**
 * Whether the engine should exit at the next opportunity.
 */
ENGINE_API extern bool    bGShouldRequestExit;
/**
 * Whether the engine has successfully received an exit request and is now beginning to tear down.
 */
extern bool    bGEngineRequestingExit;

extern int32   GCustomExitStatusOverride;
extern LString GCustomExitReason;

FORCEINLINE auto IsEngine() -> bool { return GEngine; }
FORCEINLINE auto GetEngine() -> LEngine* { return GEngine; }

FORCEINLINE auto IsEngineExitRequested() -> bool { return bGShouldRequestExit; }
FORCEINLINE auto IsTearingDown() -> bool { return bGEngineRequestingExit; }
FORCEINLINE auto WillShortlyTerminate() -> bool { return bGShouldRequestExit || bGEngineRequestingExit; }

FORCEINLINE auto HasCustomExitStatus() -> bool { return GCustomExitStatusOverride != INDEX_NONE; }
FORCEINLINE auto GetCustomExitStatus() -> int32 { return GCustomExitStatusOverride; }
FORCEINLINE auto HasCustomExitReason() -> bool { return GCustomExitReason.empty() == false; }
FORCEINLINE auto GetCustomExitReason() -> LString { return GCustomExitReason; }

// Engine Globals
///////////////////////////////////////////////////////////////////////////////

class ENGINE_API LEngine
{
    typedef std::chrono::steady_clock::time_point SteadyStatisticsTimePoint;

public:

    void Init();
    void Tick(const float DeltaTime);
    void TearDown();

    void UpdateTime();
    void EnforceTickRate();

    ///////////////////////////////////////////////////////////////////////////////
    // Private function redirects
    ///////////////////////////////////////////////////////////////////////////////

    /** Internal public method. Do not use. */
    void BeginExitIfRequested();

    void RequestEngineExit();
    void RequestEngineExit(const LString& Reason);
    void RequestEngineExit(const int32 CustomExitStatus);
    void RequestEngineExit(const int32 CustomExitStatus, const LString& Reason);
};

} /* ~Namespace Jafg */
