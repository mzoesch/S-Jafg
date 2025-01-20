// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Level.h"

namespace Jafg
{

class JObject;
class LEngine;
class LLocalEgo;
class LWorld;
class LSurface;
class LCommandLineInterface;
class LApplicationInstance;

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

FORCEINLINE auto IsEngine() -> bool { return GEngine; }
FORCEINLINE auto GetEngine() -> LEngine* { return GEngine; }

FORCEINLINE auto IsEngineExitRequested() -> bool { return bGShouldRequestExit; }
FORCEINLINE auto IsTearingDown() -> bool { return bGEngineRequestingExit; }
FORCEINLINE auto WillShortlyTerminate() -> bool { return bGShouldRequestExit || bGEngineRequestingExit; }

FORCEINLINE auto HasCustomExitStatus() -> bool { return GCustomExitStatusOverride != INDEX_NONE; }
FORCEINLINE auto GetCustomExitStatus() -> int32 { return GCustomExitStatusOverride; }
FORCEINLINE auto HasCustomExitReason() -> bool { return GCustomExitReason.IsEmpty() == false; }
FORCEINLINE auto GetCustomExitReason() -> LSimpleString { return GCustomExitReason; }

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

//#
//# Private engine context wrapper around a world.
//# This struct is internal to the engine and should not be used outside it.
//#
struct LWorldContext
{
    LStringLegacy TravelUrl;
    LWorld* ChildWorld;

    FORCEINLINE bool IsWaitingForTravel() const { return this->TravelUrl.empty() == false; }
};

MAKE_MULTICAST_SIGNATURE(LOnWorldBeginLifeDelegateSignature, LWorld* /* InNewWorld */)

class ENGINE_API LEngine
{
    typedef std::chrono::steady_clock::time_point LSteadyStatisticsTimePoint;

    friend JObject;

public:

    void Initialize();
    void Tick(const float DeltaTime);
    void TearDown();

    void UpdateTime();
    void EnforceTickRate();

    ///////////////////////////////////////////////////////////////////////////////
    // Private function redirects
    ///////////////////////////////////////////////////////////////////////////////

    //# Internal public method. Do not use.
    void BeginExitIfRequested();
    //# Internal public method. Do not use.
    void ReflectForwardedExitRequest();

    void RequestEngineExit();
    void RequestEngineExit(const LSimpleString& Reason);
    void RequestEngineExit(const int32 CustomExitStatus);
    void RequestEngineExit(const int32 CustomExitStatus, const LSimpleString& Reason);

    ///////////////////////////////////////////////////////////////////////////////
    // Client Local Stuff
    ///////////////////////////////////////////////////////////////////////////////

    bool CanEverRender() const;

    bool IsPrimarySurfaceValid() const;
    auto GetPrimarySurface() const -> LSurface*;
    auto GetCheckedPrimarySurface() const -> LSurface*;
    auto GetPanickedPrimarySurface() const -> LSurface*;

    FORCEINLINE bool HasLocalEgo() const { return this->LocalEgo != nullptr; }
    FORCEINLINE auto GetLocalEgo() const -> LLocalEgo* { return this->LocalEgo; }
    FORCEINLINE auto GetCheckedLocalEgo() const -> LLocalEgo* { check( this->LocalEgo ) return this->LocalEgo; }
    FORCEINLINE auto GetPanickedLocalEgo() const -> LLocalEgo*
    {
        if (this->HasLocalEgo()) { return this->GetLocalEgo(); }
        panic( "Could not find local ego instance." )
        return nullptr;
    }

private:

    LLocalEgo* LocalEgo = nullptr;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Context Related
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE static uint8 GetMaxContexts() { return LEngine::MaxContexts; }

    uint8 GetCurrentFreeContexts() const;
    uint8 GetCurrentOccupiedContexts() const;

    auto GetContextFromWorld(const LWorld& World) -> LWorldContext&;

    //# Browse to a new Url at the next opportunity.
    auto Browse(const LWorld& Context, const LStringLegacy& Url) -> void;
    //# @return True if registered successfully.
    auto RegisterLevel(const LLevel& InLevel) -> bool;
    auto RegisterLevel(const LLevel&& InLevel) -> bool;
    auto IsLevelRegistered(const LStringLegacy& Identifier) const -> bool;

    //#
    //# Delegate called when a new world is shortly about to be running inside its beginning life cycle.
    //# The world pointer is guaranteed to be valid.
    //#
    LOnWorldBeginLifeDelegateSignature OnWorldBeginLife;

private:

    auto GetFirstAvailableContextIndex() const -> uint8;
    auto CreateNewWorldContext() -> LWorldContext&;
    auto InitializeContext(LWorldContext& InContext, const LSimpleString& InHumanReadableName) -> void;

    //# Browse to a new Url at the next opportunity.
    void Browse(LWorldContext& Context, const LStringLegacy& Url) const;
    bool IsContextUrlInternal(const LStringLegacy& Url) const;
    void TravelContext(LWorldContext& Context);
    FORCEINLINE auto GetLevelByInternalUrl(const LStringLegacy& Url) -> LLevel* { return this->RegisteredLevels.FindRef(Url); }

    //# The maximum amount of context this engine can handle.
    static constexpr uint8 MaxContexts { 3 };

    //#
    //# All current engine contexts.
    //# An index of a specific context is not guaranteed to stay the same. Always expect a short
    //# lifetime of the index.
    //#
    LWorldContext* Contexts[LEngine::MaxContexts] = { nullptr, nullptr, nullptr, };
    //# The registered levels that this engine can load.
    TdhArray<LLevel> RegisteredLevels = { };

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Misc
    ///////////////////////////////////////////////////////////////////////////////

    bool IsCommandLineInterfaceValid() const;
    auto GetCommandLineInterface() const -> LCommandLineInterface*;
    auto GetCheckedCommandLineInterface() const -> LCommandLineInterface*;
    auto GetPanickedCommandLineInterface() const -> LCommandLineInterface*;

    FORCEINLINE auto IsApplicationInstanceValid() const -> bool { return this->ApplicationInstance; }
    FORCEINLINE auto GetApplicationInstance() const -> LApplicationInstance* { return this->ApplicationInstance; }

private:

    LCommandLineInterface* CommandLineInterface = nullptr;
    LApplicationInstance* ApplicationInstance = nullptr;
};

} /* ~Namespace Jafg */
