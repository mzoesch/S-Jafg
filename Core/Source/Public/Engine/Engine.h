// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "glad/glad.h" /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h>
#include "Level.h"

class JLocalPlayer;
class JWorld;
class JEngine;
class Surface;

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
FORCEINLINE auto HasCustomExitReason() -> bool { return GCustomExitReason.empty() == false; }
FORCEINLINE auto GetCustomExitReason() -> String { return GCustomExitReason; }

// Engine Globals
///////////////////////////////////////////////////////////////////////////////

/**
 * Private engine context wrapper around a world.
 * This struct is internal to the engine and should not be used outside it.
 */
struct LWorldContext
{
    String  TravelUrl;
    JWorld* ChildWorld;

    FORCEINLINE bool IsWaitingForTravel() const { return this->TravelUrl.empty() == false; }
};

class JEngine
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

    void BeginExitIfRequested();
    void RequestEngineExit();
    void RequestEngineExit(const String& Reason);
    void RequestEngineExit(const int32 CustomExitStatus);
    void RequestEngineExit(const int32 CustomExitStatus, const String& Reason);

    ///////////////////////////////////////////////////////////////////////////////
    // Local Stuff
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE auto HasSurface() const -> bool { return this->SurfaceToDrawOn != nullptr; }
    FORCEINLINE auto GetSurface() const -> Surface* { return this->SurfaceToDrawOn; }

    FORCEINLINE auto HasLocalPlayer() const -> bool { return this->LocalPlayer != nullptr; }
    FORCEINLINE auto GetLocalPlayer() const -> JLocalPlayer* { return this->LocalPlayer; }

private:

    Surface*      SurfaceToDrawOn = nullptr;
    JLocalPlayer* LocalPlayer     = nullptr;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Context Related
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE static auto GetMaxContexts() -> uint8 { return JEngine::MaxContexts; }

    auto GetCurrentFreeContexts() const -> uint8;
    auto GetCurrentOccupiedContexts() const -> uint8;

    auto GetFirstRunningWorld() -> JWorld*;
    auto GetContextFromWorld(const JWorld& World) -> LWorldContext&;

    /** Browse to a new Url at the next opportunity. */
    auto Browse(const JWorld& Context, const String& Url) -> void;
    /** @return True if registered successfully.*/
    auto RegisterLevel(LLevel InLevel) -> bool;
    auto IsLevelRegistered(const String& Identifier) const -> bool;

private:

    auto GetFirstAvailableContextIndex() const -> uint8;
    auto CreateNewWorldContext() -> LWorldContext&;
    auto InitializeContext(LWorldContext& Context) -> void;

    /** Browse to a new Url at the next opportunity. */
    auto Browse(LWorldContext& Context, const String& Url) const -> void;
    auto IsContextUrlInternal(const String& Url) const -> bool;
    void TravelContext(LWorldContext& Context);
    auto GetLevelByInternalUrl(const String& Url) -> LLevel*;

    /** The maximum amount of context this engine can handle. */
    static constexpr uint8 MaxContexts { 3 };

    /**
     * All current engine contexts.
     * An index of a specific context is not guaranteed to stay the same. Always expect a short
     * lifetime of the index.
     */
    LWorldContext* Contexts[JEngine::MaxContexts] = { nullptr, nullptr, nullptr, };
    /** The registered levels that this engine can load. */
    std::vector<LLevel> RegisteredLevels = { };
};
