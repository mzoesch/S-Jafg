// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/EngineCompileTimeConstants.h"
#include "Engine/Level.h"
#include "Cli/CommandLineInterface.h"
#include "Engine/CxxClass.h"
#include "Engine/World.h"
#include "User/LocalEgo.h"
#include "Rhi/EngineShader.h"
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
struct LWorldTrack;

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

namespace Private
{

struct LWorldTrack final
{
    LWorldTrack() = delete;
    LWorldTrack(LWorldTrack&&) = default;
    LWorldTrack(LString HumanReadableName) noexcept
        : ChildWorld{ std::make_unique<LWorld>(std::move(HumanReadableName)) }
    {
        check( this->ChildWorld.get() != nullptr && this->ChildWorld->GetWorldState() == EWorldState::PreInitializing )
    }

    ~LWorldTrack() { check( this->ChildWorld.get() == nullptr || this->ChildWorld->GetWorldState() == EWorldState::WaitingForKill ) }

    FORCEINLINE bool IsValid() const { return this->ChildWorld.get() != nullptr; }

    FORCEINLINE bool IsWaitingForTravel() const { return this->TravelUrl.empty() == false; }
    LString TravelUrl;

    ENGINE_API void CreateWorldFromParams();

#if WITH_LOCAL_LAYER
    bool bSkipThisTick { false };
#endif /* WITH_LOCAL_LAYER */

    TUnique<LWorld> ChildWorld;

    TFunction<void(LWorld&)> OnWorldPreInit;
    TFunction<void(LWorld&)> OnWorldLateInit;
};

} /* ~Namespace Private */

//#
//# The engine - only one will be valid ever. Access its singleton with #GEngine.
//#
class LEngine final
{
    typedef std::chrono::steady_clock::time_point LSteadyStatisticsTimePoint;

    friend LEngineShader;

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

    FORCEINLINE bool IsShaderValid(const LName InName) const noexcept { return this->GetShader(InName) != nullptr; }
    FORCEINLINE auto GetShader(const LName Name) noexcept -> LEngineShader*;
    FORCEINLINE auto GetShader(const LName InName) const noexcept -> const LEngineShader* { return const_cast<LEngine*>(this)->GetShader(InName); }
    FORCEINLINE auto GetShaderChecked(const LName InName) noexceptcheck -> LEngineShader* { LEngineShader* Out = this->GetShader(InName); check( Out ) return Out; }
    FORCEINLINE auto GetShaderChecked(const LName InName) noexceptcheck const -> const LEngineShader* { return const_cast<LEngine*>(this)->GetShaderChecked(InName); }
    FORCEINLINE auto GetShaderAsserted(const LName InName) -> LEngineShader* { LEngineShader* Out = this->GetShader(InName); jassert( Out ) return Out; }
    FORCEINLINE auto GetShaderAsserted(const LName InName) const -> const LEngineShader* { return const_cast<LEngine*>(this)->GetShaderAsserted(InName); }
    FORCEINLINE auto GetShaders() const noexcept -> const std::map<LName, LEngineShader*>& { return this->Shaders; }
    FORCEINLINE bool UnregisterShader(const LName Name, const bool bFree = true) noexcept;
    FORCEINLINE bool UnregisterShaderChecked(const LName Name, const bool bFree = true) noexceptcheck;

    template<typename TShader> requires std::is_base_of_v<LEngineShader, TShader>
    FORCEINLINE TShader*       GetShader(LName Name) noexcept { return static_cast<TShader*>(this->GetShader(Name)); }
    template<typename TShader> requires std::is_base_of_v<LEngineShader, TShader>
    FORCEINLINE TShader const* GetShader(LName Name) const noexcept { return static_cast<const TShader*>(this->GetShader(Name)); }
    template<typename TShader> requires std::is_base_of_v<LEngineShader, TShader>
    FORCEINLINE TShader*       GetShaderChecked(LName Name) noexcept { return static_cast<TShader*>(this->GetShaderChecked(Name)); }
    template<typename TShader> requires std::is_base_of_v<LEngineShader, TShader>
    FORCEINLINE TShader const* GetShaderChecked(LName Name) const noexcept { return static_cast<const TShader*>(this->GetShaderChecked(Name)); }
    template<typename TShader> requires std::is_base_of_v<LEngineShader, TShader>
    FORCEINLINE TShader*       GetShaderAsserted(LName Name) noexcept { return static_cast<TShader*>(this->GetShaderAsserted(Name)); }
    template<typename TShader> requires std::is_base_of_v<LEngineShader, TShader>
    FORCEINLINE TShader const* GetShaderAsserted(LName Name) const noexcept { return static_cast<const TShader*>(this->GetShaderAsserted(Name)); }

    FORCEINLINE void RecompileShader(const LName Name, TArray<LShaderCompileTimeConstant> const& Remove, TArray<LShaderCompileTimeConstant> const& Add)
    {
        this->GetShaderAsserted(Name)->Recompile(Remove, Add);
    }

private:

    FORCEINLINE bool AddShader(const LName Name, LEngineShader* Shader);
    FORCEINLINE bool AddShaderChecked(const LName Name, LEngineShader* Shader) noexceptcheck { const bool bOut{ this->AddShader(Name, Shader) }; check( bOut ) return bOut; }
    FORCEINLINE bool AddShaderAsserted(const LName Name, LEngineShader* Shader) { const bool bOut{ this->AddShader(Name, Shader) }; jassert( bOut ) return bOut; }

    FORCEINLINE bool RemoveShader(const LEngineShader* Shader) noexcept;

    LLocalEgo LocalEgo;
    std::map<LName, LEngineShader*> Shaders;

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

    ENGINE_API Private::LWorldTrack& GetTrackFromWorld(LWorld const* World);

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
    FORCEINLINE void Browse(LWorld const* World, LString const& Url,
        TFunction<void(LWorld&)>&& PreInitCallback = {},
        TFunction<void(LWorld&)>&& PostInitCallback = {}
        ) { this->Browse(this->GetTrackFromWorld(World), Url, std::move(PreInitCallback), std::move(PostInitCallback)); }

    //# @return True if registered successfully.
    ENGINE_API  bool RegisterLevel(LLevel const& Level);
    ENGINE_API  bool RegisterLevel(LLevel&& Level);
    FORCEINLINE bool IsLevelRegistered(LString const& Identifier) const { return algo::contains(this->RegisteredLevels, Identifier, &LLevel::Identifier); }

    //#
    //# Delegate called when a new world is shortly about to be running inside its beginning life cycle.
    //# The world pointer is guaranteed to be valid.
    //#
    TMulticastDelegate<void(LWorld* InNewWorld)> OnWorldBeginLife;

    FORCEINLINE auto GetTracks() const noexcept -> const TArray<Private::LWorldTrack>& { return this->Tracks; }
    FORCEINLINE auto GetRegisteredLevels() const noexcept -> const TArray<LLevel>& { return this->RegisteredLevels; }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JEngineSubsystem)

private:

    ENGINE_API void Browse(Private::LWorldTrack& Track, LString const& Url, TFunction<void(LWorld&)>&& PreInitCallback, TFunction<void(LWorld&)>&& PostInitCallback);
    bool IsTrackUrlInternal(LString const& Url) const;
    bool TravelTrack(Private::LWorldTrack& Track);
    LLevel* GetLevelByInternalUrl(LString const& Url);

    //#
    //# All current engine tracks.
    //# An index of a specific context is not guaranteed to stay the same. Always expect a short
    //# lifetime of the index.
    //#
    TArray<Private::LWorldTrack> Tracks;
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

FORCEINLINE LEngineShader* LEngine::GetShader(const LName Name) noexcept
{
    if (const auto& It = this->Shaders.find(Name); It != this->Shaders.end())
    {
        return It->second;
    }

    return nullptr;
}

FORCEINLINE bool LEngine::UnregisterShader(const LName Name, const bool bFree /* = true */) noexcept
{
    LOG_VERBOSE(LogEngine, "Removing engine shader [{}].", Name)

    if (bFree)
    {
        if (auto const& It{ this->Shaders.find(Name) }; It != this->Shaders.end())
        {
            LEngineShader const* Shader{ It->second };
            this->Shaders.erase(It);
            delete Shader;
            return true;
        }

        LOG_ERROR(LogEngine, "Failed to find engine shader [{}] to free.", Name)
    }

    return this->Shaders.erase(Name) > 0;
}

FORCEINLINE bool LEngine::UnregisterShaderChecked(const LName Name, const bool bFree /* = true */) noexceptcheck
{
    const bool bOut{ this->UnregisterShader(Name, bFree) };
    check( bOut )
    return bOut;
}

FORCEINLINE bool LEngine::AddShader(const LName Name, LEngineShader* Shader)
{
    check( Shader )

    if (this->GetShader(Name))
    {
        return false;
    }

    this->Shaders.emplace(Name, Shader);

    LOG_VERBOSE(LogEngine, "Added new engine shader [{}] to a total of {} shaders.", Name, this->Shaders.size())
    return true;
}

bool LEngine::RemoveShader(LEngineShader const* Shader) noexcept
{
    LName Name;
    for (auto const& It : this->Shaders)
    {
        if (It.second == Shader)
        {
            Name = It.first;
            break;
        }

        continue;
    }

    if (Name.IsSet())
    {
        return this->Shaders.erase(Name) > 0;
    }

    return false;
}

} /* ~Namespace Jafg */
