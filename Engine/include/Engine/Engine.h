// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/EngineCompileTimeConstants.h"
#include "Engine/Level.h"
#include "Cli/CommandLineInterface.h"
#include "Engine/Jxx.h"
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
#include "Storage/Config.h"

namespace Jafg
{

class LEngine;
class LWorld;
class JEngineSubsystem;
class LCommandLineInterface;
class AWorldObject;

} /* ~Namespace Jafg */

//# The engine singleton. Prefer to use #GEngine instead of #GMutableEngine.
ENGINE_API extern Jafg::LEngine const* GEngine;

namespace Jafg
{
namespace Detail
{

ENGINE_API extern LEngine* GMutableEngine;

struct LWorldTrack final
{
    friend LEngine;
    friend LWorld;
    friend AWorldObject;

    //# One time callbacks for initialization.
    struct LCallbacks
    {
        TFunction2<void(LWorld& World)> OnPreInit;
        TFunction2<void(LWorld& World)> OnPostInit;
    };

    struct CreateInfo
    {
        LString HumanReadableName{ "Transient World" };
    };
    LWorldTrack() noexcept = delete;
    constexpr LWorldTrack(LWorldTrack&&) noexcept = default;
    constexpr LWorldTrack(CreateInfo Info)
        : ChildWorld{std::make_unique<LWorld>(std::move(Info.HumanReadableName))}
    {
        check(this->ChildWorld.get() && this->ChildWorld->GetWorldState() == EWorldState::PreInitializing)
    }
    constexpr ~LWorldTrack() noexcept { check(this->ChildWorld.get() || this->ChildWorld->GetWorldState() == EWorldState::WaitingForKill) }

    NODISCARD FORCEINLINE constexpr bool IsWaitingForTravel() const noexcept { return !this->TravelUrl.empty(); }
    FORCEINLINE bool IsValid() const noexcept { return !!this->ChildWorld.get(); }
    NODISCARD FORCEINLINE LWorld& GetWorld() const noexcept { check(this->IsValid()) return *this->ChildWorld; }

private:

    LString TravelUrl;
    TUnique<LWorld> ChildWorld;
    LCallbacks Callbacks;
};

//#
//# After a world has been summoned. They have to be initialized right after. Delaying initialization for even one
//# tick is undefined behavior.
//#
struct LWorldTrackInitializer final
{
    LWorldTrackInitializer() noexcept = delete;
    LWorldTrackInitializer(LWorldStorage Storage) noexcept : Storage{std::move(Storage)} { check(this->Storage.IsValid()) }
    PROHIBIT_COPY(LWorldTrackInitializer)
    DEFAULT_CONSTEXPR_MOVE(LWorldTrackInitializer)
#if JAFG_DO_CHECKS
    inline ~LWorldTrackInitializer() noexcept
    {
        /* Raii check to assert that you have initialized the world. */
        check(this->Storage.IsNull())
    }
#endif /* JAFG_DO_CHECKS */

    //#
    //# Browse the world after initialization. This is required.
    //#
    inline LWorld& Browse(LString Url, LWorldTrack::LCallbacks Callbacks = {});

private:

    LWorldStorage Storage;
};

} /* ~Namespace Detail */

//# The engine - only one will be valid ever. Access its singleton with #GEngine.
class LEngine final
{
public:

    ENGINE_API LEngine();
    ENGINE_API void Initialize();
    ENGINE_API void Tick();
    ENGINE_API void TearDown();

    ///////////////////////////////////////////////////////////////////////////////
    // Time Related Stuff.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API void DefaultTimeAdvance();

    algo::clock::time_point LastStdOutFlush;

    //#
    //# The maximum delta time allowed between frames.
    //# See the #LEngine::TimeStat::LostDeltaTime for the time that is lost when lag spikes occur.
    //#
    static inline constexpr f64 MaxDeltaTime{ 1.0 / 3.0 };

    //# The time when the current frame started. Everything is calculated relative to this.
    algo::clock::time_point FrameStartTimePoint;
    //# The elapsed time from static storage initialization to the current frame.
    f64 FrameStartElapsedTime{};
    //# The last frame's start elapsed time.
    f64 PreviousFrameStartElapsedTime{};

    //# Clamped delta time when encountering spikes.
    f64 DeltaTime{};
    //# The actual time. Never use this for any sort of logic -- only stats, etc.
    f64 RealDeltaTime{};
    //# Time lost while clamping #DeltaTime.
    f64 LostDeltaTime{};
    //# Time the engines master thread spend idle.
    f64 IdleDeltaTime{};
    constexpr bool HasLostDeltaTime() const noexcept { return this->LostDeltaTime > 0.0; }
    constexpr bool HasIdleDeltaTime() const noexcept { return this->IdleDeltaTime > 0.0; }
    constexpr f64 GetRealDeltaTimeAsFps() const noexcept { check(this->RealDeltaTime != 0.0) return 1.0 / this->RealDeltaTime; }

    u64 FrameCount{};

    //# All time related members are measured in seconds except stated otherwise.
    struct TimeStat final
    {
        algo::clock::time_point Start{ algo::now() };
        //# How many frames have passed in this statistic sweep.
        u64 FrameCount{};
        f64 Low{ std::numeric_limits<f64>::max() };
        f64 High{ -1.0 };
        f64 HighestLoss{};
        f64 HighestIdle{};
    };
    //# How often stats should be averaged.
    f64 StatisticsPeriod{ 1.0 };
    algo::clock::time_point LastStatisticsTime{ algo::now() };
    TimeStat CurrentStat;
    TimeStat PreviousStat;

    ///////////////////////////////////////////////////////////////////////////////
    // Client Local Stuff.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API  bool CanEverRender() const noexcept;

#if JAFG_WITH_LOCAL_LAYER

    FORCEINLINE LLocalEgo& GetLocalEgo() noexcept { return this->LocalEgo; }
    FORCEINLINE LLocalEgo const& GetLocalEgo() const noexcept { return this->LocalEgo; }

private:

    LLocalEgo LocalEgo;

#endif /* JAFG_WITH_LOCAL_LAYER */

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

    //#
    //# Summon a completely new fresh world.
    //# @note Master thread of course only.
    //#
    ENGINE_API Detail::LWorldTrackInitializer SummonWorld(Detail::LWorldTrack::CreateInfo Info);

    //# Unlike worlds, track pointer may change location at any time. It is not safe to store them.
    NODISCARD FORCEINLINE Detail::LWorldTrack& GetTrackFromWorld(LWorld const& World)
    {
        return *algo::find_checked(this->Tracks, &World, [](auto const& E){ return E.ChildWorld.get(); });
    }

    //#
    //# Whether your world pointer is still valid.
    //#
    ENGINE_API bool IsWorldValid(LWorld const* World) const noexcept;

    //#
    //# Browse the provided world to a new url at the next opportunity.
    //# The url format is defined as follows:
    //#  Internal (browse the world to a url that is internal to the engine; no networking):
    //#    <LevelName>
    //#    <LevelName>?<option>?... (@see #LWorldParameters for how to format options.)
    //#
    FORCEINLINE LWorld& Browse(LWorld& World, LString Url, Detail::LWorldTrack::LCallbacks Callbacks = {})
    {
        return this->Browse(this->GetTrackFromWorld(World), std::move(Url), std::move(Callbacks));
    }

    //# @return True if registered successfully.
    ENGINE_API  bool RegisterLevel(LLevel Level);
    FORCEINLINE bool IsLevelRegistered(LString const& Identifier) const { return algo::contains(this->RegisteredLevels, Identifier, &LLevel::Identifier); }

    //#
    //# Delegate called when a new world is shortly about to be running inside its beginning life cycle.
    //# The world pointer is guaranteed to be valid.
    //#
    TMulticastDelegate<bool(LWorld* InNewWorld)> OnWorldBeginLife;

    FORCEINLINE auto const& GetTracks() const noexcept { return this->Tracks; }
    FORCEINLINE auto const& GetRegisteredLevels() const noexcept { return this->RegisteredLevels; }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JEngineSubsystem)

private:

    ENGINE_API LWorld& Browse(Detail::LWorldTrack& Track, LString Url, Detail::LWorldTrack::LCallbacks Callbacks);
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
    MULTI_EVENT_DECL_VERBOSE(LEngine, OnForeignPluginLoaded, LLoadedPlugin& Plugin)

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

    LConfig Config;

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

inline LWorld& Detail::LWorldTrackInitializer::Browse(LString Url, LWorldTrack::LCallbacks Callbacks /* = {}*/)
{
    check(this->Storage.IsValid())
    check(GMutableEngine)
    auto& Result{GMutableEngine->Browse(*this->Storage, std::move(Url), std::move(Callbacks))};
    this->Storage.Reset();

    return Result;
}

} /* ~Namespace Jafg */
