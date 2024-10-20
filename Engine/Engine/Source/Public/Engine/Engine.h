// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "glad/glad.h" /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h>
#include "Level.h"

namespace Jafg
{

class LEngine;
class JLocalPlayer;
class JWorld;
class Surface;

ENGINE_EXTERN template class ENGINE_API TArray<::Jafg::LLevel, ::Jafg::ResizePolicy::Dynamic, ::Jafg::AllocationPolicy::Heap>;

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

ENGINE_API extern LEngine*      GEngine;

/**
 * Whether the engine should exit at the next opportunity.
 */
ENGINE_API extern bool          bGShouldRequestExit;
/**
 * Whether the engine has successfully received an exit request and is now beginning to tear down.
 */
ENGINE_API extern bool          bGEngineRequestingExit;

ENGINE_API extern int32         GCustomExitStatusOverride;
ENGINE_API extern LString       GCustomExitReason;

FORCEINLINE ENGINE_API auto IsEngine() -> bool { return GEngine; }
FORCEINLINE ENGINE_API auto GetEngine() -> LEngine* { return GEngine; }

FORCEINLINE ENGINE_API auto IsEngineExitRequested() -> bool { return bGShouldRequestExit; }
FORCEINLINE ENGINE_API auto IsTearingDown() -> bool { return bGEngineRequestingExit; }
FORCEINLINE ENGINE_API auto WillShortlyTerminate() -> bool { return bGShouldRequestExit || bGEngineRequestingExit; }

FORCEINLINE ENGINE_API auto HasCustomExitStatus() -> bool { return GCustomExitStatusOverride != INDEX_NONE; }
FORCEINLINE ENGINE_API auto GetCustomExitStatus() -> int32 { return GCustomExitStatusOverride; }
FORCEINLINE ENGINE_API auto HasCustomExitReason() -> bool { return GCustomExitReason.empty() == false; }
FORCEINLINE ENGINE_API auto GetCustomExitReason() -> LString { return GCustomExitReason; }

// Engine Globals
///////////////////////////////////////////////////////////////////////////////

/**
 * Private engine context wrapper around a world.
 * This struct is internal to the engine and should not be used outside it.
 */
struct LWorldContext
{
    LString TravelUrl;
    JWorld* ChildWorld;

    FORCEINLINE bool IsWaitingForTravel() const { return this->TravelUrl.empty() == false; }
};

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

    ///////////////////////////////////////////////////////////////////////////////
    // Client Local Stuff
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

    FORCEINLINE static auto GetMaxContexts() -> uint8 { return LEngine::MaxContexts; }

    auto GetCurrentFreeContexts() const -> uint8;
    auto GetCurrentOccupiedContexts() const -> uint8;

    auto GetFirstRunningWorld() -> JWorld*;
    auto GetContextFromWorld(const JWorld& World) -> LWorldContext&;

    /** Browse to a new Url at the next opportunity. */
    auto Browse(const JWorld& Context, const LString& Url) -> void;
    /** @return True if registered successfully.*/
    auto RegisterLevel(const LLevel& InLevel) -> bool;
    auto RegisterLevel(const LLevel&& InLevel) -> bool;
    auto IsLevelRegistered(const LString& Identifier) const -> bool;

private:

    auto GetFirstAvailableContextIndex() const -> uint8;
    auto CreateNewWorldContext() -> LWorldContext&;
    auto InitializeContext(LWorldContext& Context) -> void;

    /** Browse to a new Url at the next opportunity. */
    auto Browse(LWorldContext& Context, const LString& Url) const -> void;
    auto IsContextUrlInternal(const LString& Url) const -> bool;
    void TravelContext(LWorldContext& Context);
    auto GetLevelByInternalUrl(const LString& Url) -> LLevel*;

    /** The maximum amount of context this engine can handle. */
    static constexpr uint8 MaxContexts { 3 };

    /**
     * All current engine contexts.
     * An index of a specific context is not guaranteed to stay the same. Always expect a short
     * lifetime of the index.
     */
    LWorldContext* Contexts[LEngine::MaxContexts] = { nullptr, nullptr, nullptr, };
    /** The registered levels that this engine can load. */
    TdhArray<LLevel> RegisteredLevels = { };
};

} /* Namespace Jafg */
