// Copyright mzoesch. All rights reserved.

#pragma once

#include "Level.h"
#include "Platform/SurfaceForward.h"
#include "Subsystems/EngineSubsystem.h"
#include "Engine/Cli/CommandLineInterface.h"
#include "User/LocalEgo.h"
#include "Engine/World.h"

namespace Jafg
{

class JObject;
class LEngine;
class LWorld;
class LCommandLineInterface;

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

ENGINE_API extern LEngine*      GEngine;

//#
//# Whether the engine should exit at the next opportunity.
//#
ENGINE_API extern bool          bGShouldRequestExit;
//#
//# Whether the engine has successfully received an exit request and is now beginning to tear down.
//#
ENGINE_API extern bool          bGEngineRequestingExit;

ENGINE_API extern int32         GCustomExitStatusOverride;
ENGINE_API extern LSimpleString GCustomExitReason;

FORCEINLINE bool IsEngineValid() { return GEngine; }
FORCEINLINE auto GetEngine() -> LEngine* { return GEngine; }

FORCEINLINE bool IsEngineExitRequested() { return bGShouldRequestExit; }
FORCEINLINE bool IsTearingDown() { return bGEngineRequestingExit; }
FORCEINLINE bool WillShortlyTerminate() { return bGShouldRequestExit || bGEngineRequestingExit; }

FORCEINLINE bool HasCustomExitStatus() { return GCustomExitStatusOverride != INDEX_NONE; }
FORCEINLINE auto GetCustomExitStatus() -> int32 { return GCustomExitStatusOverride; }
FORCEINLINE bool HasCustomExitReason() { return GCustomExitReason.IsEmpty() == false; }
FORCEINLINE auto GetCustomExitReason() -> LSimpleString { return GCustomExitReason; }

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

//#
//# Private engine context wrapper around a world.
//# This struct is internal to the engine and should not be used outside it.
//#
struct LWorldContext
{
    LWorldContext() = delete;
    LWorldContext(const LSimpleString& InHumanReadableName)
    {
        this->ChildWorld = new LWorld(InHumanReadableName, EWorldState::Uninitialized);
    }
    ~LWorldContext() { check( this->ChildWorld == nullptr ) }

    FORCEINLINE bool IsValid() const { return this->ChildWorld && this->ChildWorld->IsValid(); }

    LString TravelUrl;
    LWorld* ChildWorld;

    FORCEINLINE bool IsWaitingForTravel() const { return this->TravelUrl.IsEmpty() == false; }
};

MAKE_MULTICAST_SIGNATURE(LOnWorldBeginLife, LWorld* /* InNewWorld */)

class LEngine final
{
    typedef std::chrono::steady_clock::time_point LSteadyStatisticsTimePoint;

    friend JObject;

public:

    ENGINE_API void Initialize();
    ENGINE_API void Tick(const float DeltaTime);
    ENGINE_API void TearDown();

    ///////////////////////////////////////////////////////////////////////////////
    // Private function redirects
    ///////////////////////////////////////////////////////////////////////////////

    //# Internal public method. Do not use.
    ENGINE_API void BeginExitIfRequested();
    //# Internal public method. Do not use.
    ENGINE_API void ReflectForwardedExitRequest();

    ENGINE_API void RequestEngineExit();
    ENGINE_API void RequestEngineExit(const LSimpleString& Reason);
    ENGINE_API void RequestEngineExit(const int32 CustomExitStatus);
    ENGINE_API void RequestEngineExit(const int32 CustomExitStatus, const LSimpleString& Reason);

    ///////////////////////////////////////////////////////////////////////////////
    // Client Local Stuff
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API  bool CanEverRender() const;

    FORCEINLINE bool IsLocalEgoValid() const { return this->LocalEgo.IsValid(); }
    FORCEINLINE auto GetLocalEgo() -> LLocalEgo* { check( this->IsLocalEgoValid() ) return &this->LocalEgo; }
    FORCEINLINE auto GetLocalEgo() const -> const LLocalEgo* { check( this->IsLocalEgoValid() ) return &this->LocalEgo; }

private:

    LLocalEgo LocalEgo;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Context Related
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API auto GetContextFromWorld(const LWorld* World) -> LWorldContext&;

    //# Browse to a new Url at the next opportunity.
    ENGINE_API void Browse(const LWorld* World, const LString& Url);

    //# @return True if registered successfully.
    ENGINE_API bool RegisterLevel(const LLevel& InLevel);
    ENGINE_API bool RegisterLevel(LLevel&& InLevel);
    ENGINE_API bool IsLevelRegistered(const LString& Identifier) const;

    //#
    //# Delegate called when a new world is shortly about to be running inside its beginning life cycle.
    //# The world pointer is guaranteed to be valid.
    //#
    LOnWorldBeginLife OnWorldBeginLife;

    FORCEINLINE auto GetContexts() const -> const TdhArray<LWorldContext>& { return this->Contexts; }
    FORCEINLINE auto GetRegisteredLevels() const -> const TdhArray<LLevel>& { return this->RegisteredLevels; }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JEngineSubsystem)

private:

    LWorldContext& CreateNewWorldContext(const LSimpleString& InHumanReadableName);

    //# Browse to a new Url at the next opportunity.
    void Browse(LWorldContext& Context, const LString& Url) const;
    bool IsContextUrlInternal(const LString& Url) const;
    void TravelContext(LWorldContext& Context);
    FORCEINLINE auto GetLevelByInternalUrl(const LString& Url) -> LLevel* { return this->RegisteredLevels.FindRef(Url); }

    //#
    //# All current engine contexts.
    //# An index of a specific context is not guaranteed to stay the same. Always expect a short
    //# lifetime of the index.
    //#
    TdhArray<LWorldContext> Contexts;
    //# The registered levels that this engine can load.
    TdhArray<LLevel> RegisteredLevels = { };

    LObjectContext ObjectContext = GlobalCarnifex;
    LSubsystemCollection Collection;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Misc
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE       LCommandLineInterface* GetCommandLineInterface() { return &this->CommandLineInterface; }
    FORCEINLINE const LCommandLineInterface* GetCommandLineInterface() const { return &this->CommandLineInterface; }

private:

    LCommandLineInterface CommandLineInterface;
};

} /* ~Namespace Jafg */
