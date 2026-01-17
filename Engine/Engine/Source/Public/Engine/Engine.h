// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/EngineCompileTimeConstants.h"
#include "Engine/Level.h"
#include "Cli/CommandLineInterface.h"
#include "Engine/CxxClass.h"
#include "Engine/World.h"
#include "User/LocalEgo.h"
#include "Foreign/PluginForward.h"
#include "Subsystems/EngineSubsystem.h"
#if JAFG_WITH_FOREIGN_SUPPORT
    #include "Foreign/Plugin.h"
#endif /* JAFG_WITH_FOREIGN_SUPPORT */
#if JAFG_WITH_REST_CLS
    #include "Cli/ReSTCli.h"
#endif /* JAFG_WITH_REST_CLS */

namespace Jafg
{

class LEngine;
class LWorld;
class JEngineSubsystem;
class LCommandLineInterface;

} /* ~Namespace Jafg */

//#
//# The engine singleton.
//#
ENGINE_API extern Jafg::LEngine* GEngine;

namespace Jafg
{

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

//#
//# Whether the engine should exit at the next opportunity.
//#
ENGINE_API extern bool bGShouldRequestExit;
//#
//# Whether the engine has successfully received an exit request and is now beginning to tear down.
//#
ENGINE_API extern bool bGEngineRequestingExit;

ENGINE_API extern i32     GCustomExitStatusOverride;
ENGINE_API extern LString GCustomExitReason;

FORCEINLINE bool IsEngineValid() { return GEngine; }
FORCEINLINE auto GetEngine() -> LEngine* { return GEngine; }

FORCEINLINE bool IsEngineExitRequested() { return bGShouldRequestExit; }
FORCEINLINE bool IsTearingDown() { return bGEngineRequestingExit; }
FORCEINLINE bool WillShortlyTerminate() { return bGShouldRequestExit || bGEngineRequestingExit; }

FORCEINLINE bool HasCustomExitStatus() { return GCustomExitStatusOverride != INDEX_NONE; }
FORCEINLINE i32  GetCustomExitStatus() { return GCustomExitStatusOverride; }
FORCEINLINE bool HasCustomExitReason() { return GCustomExitReason.empty() == false; }
FORCEINLINE auto GetCustomExitReason() -> LString { return GCustomExitReason; }

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{

struct LWorldTrack final
{
    struct LCallbacks
    {
        TFunction<void(LWorld&)> OnWorldPreInit;
        TFunction<void(LWorld&)> OnWorldPostInit;
    };

    LWorldTrack() noexcept = delete;
    constexpr LWorldTrack(LWorldTrack&&) noexcept = default;
    constexpr LWorldTrack(LString HumanReadableName) noexcept
        : ChildWorld{ std::make_unique<LWorld>(std::move(HumanReadableName)) }
    {
        check( this->ChildWorld.get() != nullptr && this->ChildWorld->GetWorldState() == EWorldState::PreInitializing )
    }
    constexpr ~LWorldTrack() noexcept { check( this->ChildWorld.get() == nullptr || this->ChildWorld->GetWorldState() == EWorldState::WaitingForKill ) }

    ENGINE_API void CreateWorldFromParams();

    FORCEINLINE bool IsWaitingForTravel() const noexcept { return this->TravelUrl.empty() == false; }
    LString TravelUrl;
    LCallbacks Callbacks;

    FORCEINLINE bool IsValid() const noexcept { return this->ChildWorld.get() != nullptr; }
    TUnique<LWorld> ChildWorld;
};

} /* ~Namespace Detail */

//#
//# The engine - only one will be valid ever. Access its singleton with #GEngine.
//#
class LEngine final
{
    typedef std::chrono::steady_clock::time_point LSteadyStatisticsTimePoint;

public:

    ENGINE_API static void PreInitialize();
    ENGINE_API void Initialize();
    ENGINE_API void Tick(const f32 DeltaTime);
    ENGINE_API void TearDown();

    ///////////////////////////////////////////////////////////////////////////////
    // Private Function Redirects.
    ///////////////////////////////////////////////////////////////////////////////

    //# Internal public method. Do not use.
    ENGINE_API static void _BeginExitIfRequested();
    //# Internal public method. Do not use.
    ENGINE_API static void _ReflectForwardedExitRequest();

    ENGINE_API void RequestEngineExit();
    ENGINE_API void RequestEngineExit(const LString& Reason);
    ENGINE_API void RequestEngineExit(const i32 CustomExitStatus);
    ENGINE_API void RequestEngineExit(const i32 CustomExitStatus, const LString& Reason);

    ///////////////////////////////////////////////////////////////////////////////
    // Client Local Stuff.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API  bool CanEverRender() const noexcept;

#if !WITH_LOCAL_LAYER

    FORCEINLINE bool IsLocalEgoValid() const noexcept { return false; }

#else /* !WITH_LOCAL_LAYER */

    FORCEINLINE LLocalEgo& GetLocalEgo() noexcept { return this->LocalEgo; }
    FORCEINLINE LLocalEgo const& GetLocalEgo() const noexcept { return this->LocalEgo; }

private:

    LLocalEgo LocalEgo;

#endif /* WITH_LOCAL_LAYER */

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Class Outer Related.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API  void RegisterClassOuter(LClassOuter* Outer);
    ENGINE_API  void UnregisterClassOuter(LClassOuter* Outer);
    FORCEINLINE bool IsClassOuterKnown(LClassOuter const* Outer) const noexcept { return algo::contains(this->KnownOuters, Outer); }

private:

    TArray<LClassOuter*> KnownOuters;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Track Related.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API Detail::LWorldTrack& GetTrackFromWorld(LWorld const* World);

    //# Summon a completely new fresh world.
    ENGINE_API LWorldStorage SummonWorld(LString const& HumanReadableName);

    ENGINE_API bool IsWorldValid(LWorld const* World) const;

    //#
    //# Browse the provided world to a new url at the next opportunity.
    //# The url format is defined as follows:
    //#  Internal (browse the world to a url that is internal to the engine; no networking):
    //#    <LevelName>
    //#    <LevelName>?<option>?... (@see #LWorldParameters for how to format options.)
    //#
    FORCEINLINE void Browse(LWorld const* World, LString const& Url, Detail::LWorldTrack::LCallbacks Callbacks = {})
        { this->Browse(this->GetTrackFromWorld(World), Url, std::move(Callbacks)); }

    //# @return True if registered successfully.
    ENGINE_API  bool RegisterLevel(LLevel const& Level);
    ENGINE_API  bool RegisterLevel(LLevel&& Level);
    FORCEINLINE bool IsLevelRegistered(LString const& Identifier) const { return algo::contains(this->RegisteredLevels, Identifier, &LLevel::Identifier); }

    //#
    //# Delegate called when a new world is shortly about to be running inside its beginning life cycle.
    //# The world pointer is guaranteed to be valid.
    //#
    TMulticastDelegate<void(LWorld* InNewWorld)> OnWorldBeginLife;

    FORCEINLINE auto const& GetTracks() const noexcept { return this->Tracks; }
    FORCEINLINE auto const& GetRegisteredLevels() const noexcept { return this->RegisteredLevels; }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JEngineSubsystem)

private:

    ENGINE_API void Browse(Detail::LWorldTrack& Track, LString const& Url, Detail::LWorldTrack::LCallbacks Callbacks);
    bool IsTrackUrlInternal(LString const& Url) const;
    bool TravelTrack(Detail::LWorldTrack& Track);
    LLevel* GetLevelByInternalUrl(LString const& Url);

    //#
    //# All current engine tracks.
    //# An index of a specific context is not guaranteed to stay the same. Always expect a short
    //# lifetime of the index.
    //#
    TArray<Detail::LWorldTrack> Tracks;
    //# The registered levels that this engine can load.
    TArray<LLevel> RegisteredLevels;

    LClassOuter Outer{ "Engine" };
    LSubsystemCollection Collection{ "Engine" };

#if JAFG_WITH_FOREIGN_SUPPORT
public:

    ///////////////////////////////////////////////////////////////////////////////
    // Foreign Related.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API void RefetchPlugins(TArray<LString> const& AdditionalPaths);

    //#
    //# Tries to load the plugin with the provided name.
    //#
    ENGINE_API EPluginLoadReturnCode::Type LoadPlugin(LString const& Name);
    ENGINE_API void LoadPluginNoFailure(LString const& Name);

    ENGINE_API auto UnLoadPlugin(const LString& InName, const EPluginShutdownReason::Type InReason) -> EPluginLoadReturnCode::Type;
    ENGINE_API void UnLoadPluginNoFailure(const LString& InName, const EPluginShutdownReason::Type InReason);
    auto UnLoadPlugin(LLoadedPlugin* Plugin, const EPluginShutdownReason::Type Reason) -> EPluginLoadReturnCode::Type;
    void UnLoadPluginNoFailure(LLoadedPlugin* InPlugin, const EPluginShutdownReason::Type InReason);

    //#
    //# This delegate gets called when a foreign plugin has been loaded.
    //# @param InStaticClassContainer All static classes that are registered with the default public context of the new
    //#                               plugin.
    //#
    MULTI_EVENT_DECL_VERBOSE(LEngine, OnForeignPluginLoaded, LLoadedPlugin* Plugin)

private:

    void FetchPlugins(LPath const& Path);
    bool FetchPlugin(LPath const& Path);
    EPluginLoadReturnCode::Type LoadPluginImpl(const LFetchedPlugin& FetchedPlugin);

    TArray<LFetchedPlugin> FetchedPlugins;
    TArray<LLoadedPlugin>  LoadedPlugins;

    NODISCARD FORCEINLINE u32 GetNextPluginUuid() noexcept { return ++this->PluginUuidCounter; }
    u32 PluginUuidCounter { LLoadedPluginHandle::EngineUuid };
#endif /* JAFG_WITH_FOREIGN_SUPPORT */

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Misc.
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE LCommandLineInterface& GetCommandLineInterface() noexcept { return this->CommandLineInterface; }
    FORCEINLINE LCommandLineInterface const& GetCommandLineInterface() const noexcept { return this->CommandLineInterface; }

#if JAFG_WITH_REST_CLS
    ENGINE_API void SetReSTCliCorePaths();
    ENGINE_API void StartReSTCliServer();
    ENGINE_API void StopReSTCliServer(ERunnableStopReason::Type Reason = ERunnableStopReason::EngineTermination);

    FORCEINLINE constexpr LReStCli& GetReSTCli() noexcept { return this->ReSTCli; }
    FORCEINLINE constexpr LReStCli const& GetReSTCli() const noexcept { return this->ReSTCli; }
#endif /* JAFG_WITH_REST_CLS */

private:

    LCommandLineInterface CommandLineInterface;
#if JAFG_WITH_REST_CLS
    LReStCli ReSTCli;
#endif /* JAFG_WITH_REST_CLS */
};

} /* ~Namespace Jafg */
