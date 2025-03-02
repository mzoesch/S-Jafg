// Copyright mzoesch. All rights reserved.

#pragma once

#include "Level.h"
#include "Platform/SurfaceForward.h"
#include "Subsystems/EngineSubsystem.h"
#include "Engine/Cli/CommandLineInterface.h"
#include "User/LocalEgo.h"
#include "Engine/World.h"
#include "Rhi/EngineShader.h"

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
    friend LEngineShader;

public:

    ENGINE_API void Initialize();
    ENGINE_API void Tick(const float DeltaTime);
    ENGINE_API void TearDown();

    ///////////////////////////////////////////////////////////////////////////////
    // Private Function Redirects
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

    ENGINE_API  bool CanEverRender() const noexcept;

    FORCEINLINE bool IsLocalEgoValid() const noexcept { return this->LocalEgo.IsValid(); }
    FORCEINLINE auto GetLocalEgo() -> LLocalEgo* { check( this->IsLocalEgoValid() ) return &this->LocalEgo; }
    FORCEINLINE auto GetLocalEgo() const -> const LLocalEgo* { check( this->IsLocalEgoValid() ) return &this->LocalEgo; }

    FORCEINLINE auto GetShader(const uint32 InShaderUuid) noexcept -> LEngineShader*;
    FORCEINLINE auto GetShaderChecked(const uint32 InShaderUuid) noexceptcheck -> LEngineShader* { LEngineShader* Out = this->GetShader(InShaderUuid); check( Out ) return Out; }
    FORCEINLINE auto GetShaderAsserted(const uint32 InShaderUuid) -> LEngineShader* { LEngineShader* Out = this->GetShader(InShaderUuid); jassert( Out ) return Out; }
    FORCEINLINE auto GetShaders() noexcept -> TdhArray<LEngineShader*>& { return this->Shaders; }
    FORCEINLINE auto GetShaders() const noexcept -> const TdhArray<LEngineShader*>& { return this->Shaders; }
    FORCEINLINE bool RemoveShader(const uint32 InShaderUuid, const bool bFree = true) noexcept;
    FORCEINLINE bool RemoveShaderChecked(const uint32 InShaderUuid, const bool bFree = true) noexceptcheck;

private:

    FORCEINLINE uint32 AddShader(LEngineShader* InShader);
    FORCEINLINE uint32 MakeShaderUuid() { return ++this->ShaderUuid; }

    uint32 ShaderUuid = NULL;
    LLocalEgo LocalEgo;
    TdhArray<LEngineShader*> Shaders;

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

    FORCEINLINE auto GetContexts() const noexcept -> const TdhArray<LWorldContext>& { return this->Contexts; }
    FORCEINLINE auto GetRegisteredLevels() const noexcept -> const TdhArray<LLevel>& { return this->RegisteredLevels; }

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JEngineSubsystem)

private:

    LWorldContext& CreateNewWorldContext(const LSimpleString& InHumanReadableName);

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
    TdhArray<LLevel> RegisteredLevels;

    LObjectContext ObjectContext { GlobalCarnifex };
    LSubsystemCollection Collection;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // Misc
    ///////////////////////////////////////////////////////////////////////////////

    FORCEINLINE       LCommandLineInterface* GetCommandLineInterface() noexcept { return &this->CommandLineInterface; }
    FORCEINLINE const LCommandLineInterface* GetCommandLineInterface() const noexcept  { return &this->CommandLineInterface; }

private:

    LCommandLineInterface CommandLineInterface;
};

FORCEINLINE LEngineShader* LEngine::GetShader(const uint32 InShaderUuid) noexcept
{
    LEngineShader** Out = this->Shaders.FindByPredicate(
        [InShaderUuid] (const LEngineShader* i)
        {
            return i->GetUuid() == InShaderUuid;
        }
    );

    if (Out)
    {
        return *Out;
    }

    return nullptr;
}

FORCEINLINE uint32 LEngine::AddShader(LEngineShader* InShader)
{
    check( InShader )
    check( InShader->GetUuid() == 0 )
    this->Shaders.Emplace(std::move(InShader));
    InShader->Uuid = this->MakeShaderUuid();
    LOG_VERBOSE(LogEngine, "Added new engine shader [{}] with a total of {} shaders.", InShader->GetUuid(), this->Shaders.GetSize())
    return InShader->GetUuid();
}

FORCEINLINE bool LEngine::RemoveShader(const uint32 InShaderUuid, const bool bFree /* = true */) noexcept
{
    LOG_VERBOSE(LogEngine, "Removing engine shader [{}].", InShaderUuid)

    if (bFree)
    {
        const LEngineShader* Shader = this->GetShader(InShaderUuid);
        delete Shader;
    }

    return this->Shaders.RemoveOnceByPredicate([InShaderUuid](const LEngineShader* i) { return i->GetUuid() == InShaderUuid; });
}

FORCEINLINE bool LEngine::RemoveShaderChecked(const uint32 InShaderUuid, const bool bFree /* = true */) noexceptcheck
{
    const bool bOut = this->RemoveShader(InShaderUuid, bFree);
    check( bOut )
    return bOut;
}

} /* ~Namespace Jafg */
