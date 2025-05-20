// Copyright mzoesch. All rights reserved.

#pragma once

#include "Level.h"
#include "Subsystems/EngineSubsystem.h"
#include "Cli/CommandLineInterface.h"
#include "User/LocalEgo.h"
#include "Engine/World.h"
#include "Rhi/EngineShader.h"
#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #include "Foreign/PluginForward.h"
#include "Foreign/Plugin.h"
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */

namespace Jafg
{

class JObject;
class LEngine;
class LWorld;
class LCommandLineInterface;

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

ENGINE_API extern LEngine* GEngine;

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
FORCEINLINE bool HasCustomExitReason() { return GCustomExitReason.IsEmpty() == false; }
FORCEINLINE auto GetCustomExitReason() -> LString { return GCustomExitReason; }

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

MAKE_MULTICAST_SIGNATURE(LOnWorldBeginLife, LWorld* InNewWorld)

#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
    //#
    //# This delegate gets called when a foreign plugin has been loaded.
    //# @param InStaticClassContainer All static classes that are registered with the default public context of the new
    //#                               plugin.
    MAKE_MULTICAST_SIGNATURE(LOnForeignPluginLoaded, LObjectContext* InStaticClassContainer)
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */

namespace Private
{

//#
//# Private engine context wrapper around a world.
//#
struct LWorldContext
{
    LWorldContext() = delete;
    LWorldContext(const LString& InHumanReadableName)
    {
        this->ChildWorld = new LWorld(InHumanReadableName, EWorldState::Uninitialized);
    }
    ~LWorldContext() { check( this->ChildWorld == nullptr ) }

    FORCEINLINE bool IsValid() const { return this->ChildWorld && this->ChildWorld->IsValid(); }

    LString TravelUrl;
    LWorld* ChildWorld;

    FORCEINLINE bool IsWaitingForTravel() const { return this->TravelUrl.IsEmpty() == false; }
};

} /* ~Namespace Private */

//#
//# The engine - only one will be valid ever. Access its singleton with #GEngine.
//#
class LEngine final
{
    typedef std::chrono::steady_clock::time_point LSteadyStatisticsTimePoint;

    friend JObject;
    friend LEngineShader;

public:

    ENGINE_API void Initialize();
    ENGINE_API void Tick(const f32 DeltaTime);
    ENGINE_API void TearDown();

    ///////////////////////////////////////////////////////////////////////////////
    // Private Function Redirects.
    ///////////////////////////////////////////////////////////////////////////////

    //# Internal public method. Do not use.
    ENGINE_API void BeginExitIfRequested();
    //# Internal public method. Do not use.
    ENGINE_API void ReflectForwardedExitRequest();

    ENGINE_API void RequestEngineExit();
    ENGINE_API void RequestEngineExit(const LString& Reason);
    ENGINE_API void RequestEngineExit(const i32 CustomExitStatus);
    ENGINE_API void RequestEngineExit(const i32 CustomExitStatus, const LString& Reason);

    ///////////////////////////////////////////////////////////////////////////////
    // Client Local Stuff.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API  bool CanEverRender() const noexcept;

    FORCEINLINE bool IsLocalEgoValid() const noexcept { return this->LocalEgo.IsValid(); }
    FORCEINLINE auto GetLocalEgo() -> LLocalEgo* { check( this->IsLocalEgoValid() ) return &this->LocalEgo; }
    FORCEINLINE auto GetLocalEgo() const -> const LLocalEgo* { check( this->IsLocalEgoValid() ) return &this->LocalEgo; }

    FORCEINLINE bool IsShaderValid(const LName InName) const noexcept { return this->GetShader(InName) != nullptr; }
    FORCEINLINE auto GetShader(const LName InName) noexcept -> LEngineShader*;
    FORCEINLINE auto GetShader(const LName InName) const noexcept -> const LEngineShader* { return const_cast<LEngine*>(this)->GetShader(InName); }
    FORCEINLINE auto GetShaderChecked(const LName InName) noexceptcheck -> LEngineShader* { LEngineShader* Out = this->GetShader(InName); check( Out ) return Out; }
    FORCEINLINE auto GetShaderChecked(const LName InName) noexceptcheck const -> const LEngineShader* { return const_cast<LEngine*>(this)->GetShaderChecked(InName); }
    FORCEINLINE auto GetShaderAsserted(const LName InName) -> LEngineShader* { LEngineShader* Out = this->GetShader(InName); jassert( Out ) return Out; }
    FORCEINLINE auto GetShaderAsserted(const LName InName) const -> const LEngineShader* { return const_cast<LEngine*>(this)->GetShaderAsserted(InName); }
    FORCEINLINE auto GetShaders() const noexcept -> const std::map<LName, LEngineShader*>& { return this->Shaders; }
    FORCEINLINE bool UnregisterShader(const LName InName, const bool bFree = true) noexcept;
    FORCEINLINE bool UnregisterShaderChecked(const LName InName, const bool bFree = true) noexceptcheck;

    template <typename TShader> FORCEINLINE       TShader* GetShader(const LName InName) noexcept;
    template <typename TShader> FORCEINLINE const TShader* GetShader(const LName InName) const noexcept;
    template <typename TShader> FORCEINLINE       TShader* GetShaderChecked(const LName InName) noexcept;
    template <typename TShader> FORCEINLINE const TShader* GetShaderChecked(const LName InName) const noexcept;
    template <typename TShader> FORCEINLINE       TShader* GetShaderAsserted(const LName InName) noexcept;
    template <typename TShader> FORCEINLINE const TShader* GetShaderAsserted(const LName InName) const noexcept;

private:

    FORCEINLINE bool AddShader(const LName InName, LEngineShader* InShader);
    FORCEINLINE bool AddShaderChecked(const LName InName, LEngineShader* InShader);
    FORCEINLINE bool AddShaderAsserted(const LName InName, LEngineShader* InShader);

    LLocalEgo LocalEgo;
    std::map<LName, LEngineShader*> Shaders;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Object Context Related.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API  void RegisterObjectContext(LObjectContext* InContext);
    ENGINE_API  void UnregisterObjectContext(LObjectContext* InContext);
    FORCEINLINE bool IsObjectContextKnown(const LObjectContext* InContext) const;

private:

    //#
    //# Known contexts to the engine. These contexts are read-only and should never be accessed through
    //# the engine directly.
    //# We only store the pointers to them here for object life management behind the scenes.
    //#
    TArray<LObjectContext*> KnownObjectContexts;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Context Related.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API Private::LWorldContext& GetContextFromWorld(const LWorld* World);

    ENGINE_API LWorldStorage SummonWorld(const LString& HumanReadableName);

    ENGINE_API bool IsWorldValid(const LWorld* InWorld) const;

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

    FORCEINLINE auto GetContexts() const noexcept -> const TArray<Private::LWorldContext>& { return this->Contexts; }
    FORCEINLINE auto GetRegisteredLevels() const noexcept -> const TArray<LLevel>& { return this->RegisteredLevels; }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JEngineSubsystem)

private:

    Private::LWorldContext& CreateNewWorldContext(const LString& InHumanReadableName);

    void Browse(Private::LWorldContext& Context, const LString& Url) const;
    bool IsContextUrlInternal(const LString& Url) const;
    void TravelContext(Private::LWorldContext& Context);
    FORCEINLINE auto GetLevelByInternalUrl(const LString& Url) -> LLevel* { return this->RegisteredLevels.FindRef(Url); }

    //#
    //# All current engine contexts.
    //# An index of a specific context is not guaranteed to stay the same. Always expect a short
    //# lifetime of the index.
    //#
    TArray<Private::LWorldContext> Contexts;
    //# The registered levels that this engine can load.
    TArray<LLevel> RegisteredLevels;

    LObjectContext ObjectContext { DeferredGlobalCarnifex };
    LSubsystemCollection Collection;

#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
public:

    ///////////////////////////////////////////////////////////////////////////////
    // Foreign Related.
    ///////////////////////////////////////////////////////////////////////////////

    ENGINE_API void RefetchPlugins(const TArray<LString>& InAdditionalPaths);

    //#
    //# Tries to load the plugin with the provided name.
    //#
    ENGINE_API EPluginLoadReturnCode::Type LoadPlugin(const LString& InName);
    ENGINE_API void LoadPluginNoFailure(const LString& InName);

    ENGINE_API auto UnLoadPlugin(const LString& InName, const EPluginShutdownReason::Type InReason) -> EPluginLoadReturnCode::Type;
    ENGINE_API void UnLoadPluginNoFailure(const LString& InName, const EPluginShutdownReason::Type InReason);
    auto UnLoadPlugin(LLoadedPlugin* InPlugin, const EPluginShutdownReason::Type InReason) -> EPluginLoadReturnCode::Type;
    void UnLoadPluginNoFailure(LLoadedPlugin* InPlugin, const EPluginShutdownReason::Type InReason);

    LObjectContext* GetCurrentForeignContext() const;

    LOnForeignPluginLoaded OnForeignPluginLoaded;

private:

    void FetchPlugins(const LPath& InPath);
    bool FetchPlugin(LPath&& InPath);
    EPluginLoadReturnCode::Type LoadPluginImpl(const LFetchedPlugin& InFetchedPlugin);

    TArray<LFetchedPlugin> FetchedPlugins;
    TArray<LLoadedPlugin>  LoadedPlugins;

    NODISCARD FORCEINLINE u32 GetNextPluginUuid() noexcept { return ++this->PluginUuidCounter; }
    u32 PluginUuidCounter { LLoadedPluginHandle::EngineUuid };

    LObjectContext* ForeignContextCursor { nullptr };
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Misc.
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE LCommandLineInterface* GetCommandLineInterface() noexcept { return &this->CommandLineInterface; }
    FORCEINLINE const LCommandLineInterface* GetCommandLineInterface() const noexcept { return &this->CommandLineInterface; }

private:

    LCommandLineInterface CommandLineInterface;
};

template<typename TShader>
FORCEINLINE TShader* LEngine::GetShader(const LName InName) noexcept
{
    return static_cast<TShader*>(this->GetShader(InName));
}

template<typename TShader>
FORCEINLINE const TShader* LEngine::GetShader(const LName InName) const noexcept
{
    return static_cast<const TShader*>(this->GetShader(InName));
}

template<typename TShader>
FORCEINLINE TShader* LEngine::GetShaderChecked(const LName InName) noexcept
{
    return static_cast<TShader*>(this->GetShaderChecked(InName));
}

template<typename TShader>
FORCEINLINE const TShader* LEngine::GetShaderChecked(const LName InName) const noexcept
{
    return static_cast<const TShader*>(this->GetShaderChecked(InName));
}

template<typename TShader>
FORCEINLINE TShader* LEngine::GetShaderAsserted(const LName InName) noexcept
{
    return static_cast<TShader*>(this->GetShaderAsserted(InName));
}

template<typename TShader>
FORCEINLINE const TShader* LEngine::GetShaderAsserted(const LName InName) const noexcept
{
    return static_cast<const TShader*>(this->GetShaderAsserted(InName));
}

FORCEINLINE LEngineShader* LEngine::GetShader(const LName InName) noexcept
{
    if (const auto& It = this->Shaders.find(InName); It != this->Shaders.end())
    {
        return It->second;
    }

    return nullptr;
}

FORCEINLINE bool LEngine::AddShader(const LName InName, LEngineShader* InShader)
{
    check( InShader )

    if (this->GetShader(InName))
    {
        return false;
    }

    this->Shaders.emplace(InName, InShader);

    LOG_VERBOSE(LogEngine, "Added new engine shader [{}] to a total of {} shaders.", InName, this->Shaders.size())
    return true;
}

FORCEINLINE bool LEngine::AddShaderChecked(const LName InName, LEngineShader* InShader)
{
    const bool bOut = this->AddShader(InName, InShader);
    check( bOut )
    return bOut;
}

FORCEINLINE bool LEngine::AddShaderAsserted(const LName InName, LEngineShader* InShader)
{
    const bool bOut = this->AddShader(InName, InShader);
    jassert( bOut )
    return bOut;
}

FORCEINLINE bool LEngine::UnregisterShader(const LName InName, const bool bFree /* = true */) noexcept
{
    LOG_VERBOSE(LogEngine, "Removing engine shader [{}].", InName)

    if (bFree)
    {
        if (auto It = this->Shaders.find(InName); It != this->Shaders.end())
        {
            delete It->second;
        }
        else
        {
            LOG_ERROR(LogEngine, "Failed to find engine shader [{}] to free.", InName)
        }
    }

    return this->Shaders.erase(InName);
}

FORCEINLINE bool LEngine::UnregisterShaderChecked(const LName InName, const bool bFree /* = true */) noexceptcheck
{
    const bool bOut = this->UnregisterShader(InName, bFree);
    check( bOut )
    return bOut;
}

FORCEINLINE bool LEngine::IsObjectContextKnown(const LObjectContext* InContext) const
{
    return this->KnownObjectContexts.Contains(InContext);
}

#if PLATFORM_SUPPORTS_SHARED_LIBRARIES
FORCEINLINE LObjectContext* LEngine::GetCurrentForeignContext() const
{
    check( this->ForeignContextCursor )
    return this->ForeignContextCursor;
}
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */

} /* ~Namespace Jafg */
