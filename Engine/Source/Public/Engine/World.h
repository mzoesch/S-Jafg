// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Arguments.h"
#include "Engine/ClassOuter.h"
#include "Physics/TraceUtility.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/Level.h"
#include "Cli/CliType.h"
#include "Cli/CliCommand.h"
#include "Platform/SurfaceForward.h"
#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"

namespace Jafg
{

class LLocalEgo;
class AActor;
class LViewport;
class LEngine;
class LShader;
class JWorldSubsystem;
class LTickableObject;
class APersonaController;
class APawn;
class LCommandLineInterface;
class LWorld;
class ASupremePolicies;
struct LEye_v2;
struct LLevel;
struct LSubsystemCollection;
struct LRenderInfo;

enum struct EWorldState : u8
{
    PreInitializing,
    Initializing,
    Running,
    TearingDown,
    WaitingForKill,
};
inline LStringView LexToString(EWorldState Type) noexcept
{
    switch (Type)
    {
        case EWorldState::PreInitializing: return "PreInitializing";
        case EWorldState::Initializing:    return "Initializing";
        case EWorldState::Running:         return "Running";
        case EWorldState::TearingDown:     return "TearingDown";
        case EWorldState::WaitingForKill:  return "WaitingForKill";
        default:                           return "<Unknown>";
    }
}

enum struct EWorldTimeBehavior : u8
{
    //#
    //# Time behaves linearly. This is the default behavior.
    //#
    Linear,

    //#
    //# Time is desisted for this world.
    //# Time will not be forwarded to employees of the world context.
    //#
    Desist,

    //#
    //# Time is simulated and private to world core subsystems.
    //# Time will not be forwarded to employees of the world context.
    //#
    Simulate,
};
inline LStringView LexToString(EWorldTimeBehavior Type) noexcept
{
    switch (Type)
    {
        case EWorldTimeBehavior::Linear:   return "Linear";
        case EWorldTimeBehavior::Desist:   return "Desist";
        case EWorldTimeBehavior::Simulate: return "Simulate";
        default:                           return "<Unknown>";
    }
}

enum struct EIncomingConnectionRequest : u8
{
    Local,
    Remote,
};
inline LStringView LexToString(EIncomingConnectionRequest Type) noexcept
{
    switch (Type)
    {
    case EIncomingConnectionRequest::Local:  return "Local";
    case EIncomingConnectionRequest::Remote: return "Remote";
    default:                                 return "<Unknown>";
    }
}

struct LTransientPersona final
{
    EIncomingConnectionRequest Type;
    LSurface* Surface;
    //# TODO: Net stuff etc.
};

//#
//# The parameters for a world.
//# Parameters are split by a '?'. A special character can be escaped with '\'.
//# A parameter may have the following formats:
//#   <Key>
//#   <Key>=
//#   <Key>=<Value>
//#
//# E.g.: MyCoolLevel?MyFirstParam?MySecondParam=?MyThirdParam=HeyThereIsAValue?Another\=Key\?StillTheFourthKey=ThisIsTheValue\?\=StillTheSameValue
//#
struct LWorldParameters final
{
    friend LWorld;

    struct LWorldParam final
    {
        LString Key;
        LString Value;
    };

    TArray<LWorldParam> Params;

    ENGINE_API LString ToString() const;

private:

    void Reset() noexcept;
};

typedef LGenericArgument LWorldArgument;

template<> FORCEINLINE LCliType LCliType::Type<LWorld>() { return LCliType::Type("World"); }

template<>
struct LCommandArgsTypeRet<LWorld> final
{
    typedef LWorld* type;
};
template<>
FORCEINLINE LCommandArgsTypeRet_t<LWorld> LCommandArgs::GetAs<LWorld>() const;
template<>
struct LCommandArgsTypeRet<const LWorld> final
{
    typedef LWorld const* type;
};
template<>
FORCEINLINE LCommandArgsTypeRet_t<const LWorld> LCommandArgs::GetAs<const LWorld>() const;

//#
//# A world.
//#
//# Once every frame the world will be ticked. It may register itself to the RHI to be used when
//# rendering on any kind of surface. Multiple worlds may draw to the same surface, and a world
//# may draw to multiple surfaces.
//#
class LWorld final : public LClassOuter
{
    friend AActor;

public:

    LWorld() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LWorld)
    LWorld(LString const& InHumanReadableName) noexcept
        : LClassOuter{InHumanReadableName}, WorldState(EWorldState::PreInitializing)
        , Vk_WorldDescriptorSets{nullptr, nullptr, nullptr}
    {
        return;
    }

    // LClassOuter implementation
    virtual bool IsWorld() const noexcept override { return true; }
    // ~LClassOuter implementation

    void InitializeWorld(TOptional<LLevel> const& Level = {}, LString&& Url = {});

    //# There are no checked alternatives, as the engine must be valid at all times if a world exists.
    ENGINE_API LEngine& GetEngine() const noexcept;
    ENGINE_API LCommandLineInterface& GetCommandLineInterface() const noexcept;
    ENGINE_API LLocalEgo& GetLocalEgo() const noexcept;

    //#
    //# The real URL that was used to launch this world. This might not be valid.
    //# @note This URL is not sanitized, so for the most cases you should use the sanitized URL by
    //#       calling #GetBrowsedUrl.
    //#
    FORCEINLINE LString const& GetUnsanitizedUrl() const noexcept { return this->UnsanitizedUrl; }
    //# The URL that was used to launch this world. This might not be valid.
    FORCEINLINE LString const& GetBrowsedUrl() const noexcept { return this->Url; }
    //# The URL but parsed into a structured way. This might not be valid.
    FORCEINLINE LWorldParameters const& GetParameters() const noexcept { return this->Parameters; }

    FORCEINLINE EWorldState GetWorldState() const noexcept { return this->WorldState; }

    FORCEINLINE bool CanTick() const noexcept { return this->GetWorldState() == EWorldState::Running; }
    void Tick(const f32 Dt);

    void Draw(LRenderInfo& Info) const;

    ENGINE_API APersonaController* Login(LTransientPersona Persona, LString* OutRejectionReason = nullptr);

    FORCEINLINE bool IsUnderlyingLevelValid() const noexcept { return this->UnderlyingLevel.has_value(); }
    FORCEINLINE LLevel const& GetUnderlyingLevel() const { return this->UnderlyingLevel.value(); }
    FORCEINLINE LLevel const& GetUnderlyingLevelChecked() const noexceptcheck { check( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel.value(); }
    FORCEINLINE LLevel const& GetUnderlyingLevelAsserted() const { jassert( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel.value(); }
    FORCEINLINE LStringView   GetUnderlyingLevelName() const noexcept { if (this->IsUnderlyingLevelValid()) { return LStringView{this->UnderlyingLevel->Identifier}; } return {}; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameChecked() const noexceptcheck { check( this->IsUnderlyingLevelValid() ) return this->IsUnderlyingLevelValid() ? LStringView{this->UnderlyingLevel->Identifier} : LStringView{ }; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameAsserted() const { jassert( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel->Identifier; }

    ENGINE_API  void RegisterTickableObject(LTickableObject* Tickable);
    ENGINE_API  void UnregisterTickableObject(LTickableObject* Tickable);
    FORCEINLINE bool IsTickableObjectsPutMutexLocked() const { return this->TickableObjectsPutMutex; }
    FORCEINLINE TArray<LTickableObject*> const& GetTickableObjects() const noexcept { return this->TickableObjects; }
    FORCEINLINE TArray<LTickableObject*>& GetMutableTickableObjects() noexcept { return this->TickableObjects; }
    FORCEINLINE TArray<LTickableObject*> const& GetDeletedTickableObjects() const noexcept { return this->DeletedTickableObjects; }
    FORCEINLINE TArray<LTickableObject*>& GetDeletedMutableTickableObjects() noexcept { return this->DeletedTickableObjects; }

    ENGINE_API f32 GetRealTimeSecondsSinceWorldLaunch() const noexcept;

    //#
    //# Trace this world for physical hits.
    //# @return True if a blocking hit was found.
    //#
    bool LineTraceByChannel
    (
          TArray<LHitResult>* OutHits
        , LWorldVec3 const& Begin
        , LWorldVec3 const& End
        , ECollisionChannels Channels
        , LCollisionQueryParams const& Params
    ) const;

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JWorldSubsystem)

    FORCEINLINE ASupremePolicies* GetSupremePolicies() noexcept { return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies const* GetSupremePolicies() const noexcept { return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies* GetSupremePoliciesChecked() noexceptcheck { check(this->SupremePolicies) return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies const* GetSupremePoliciesChecked() const noexceptcheck { check(this->SupremePolicies) return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies* GetSupremePoliciesAsserted() { jassert(this->SupremePolicies) return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies const* GetSupremePoliciesAsserted() const { jassert(this->SupremePolicies) return this->SupremePolicies; }

    FORCEINLINE LLinearColor const& GetBackgroundColor() const noexcept { return this->BackgroundColor; }
    FORCEINLINE void SetBackgroundColor(LLinearColor const& Color) noexcept { this->BackgroundColor = Color; }

    template<typename TRenderInfo> requires std::is_base_of_v<LRenderInfo, TRenderInfo>
    FORCEINLINE auto const& Vk_GetWorldDataDescriptorSet(TRenderInfo const& Info) const noexcept { return this->Vk_WorldDescriptorSets[Info.Frame]; }
    template<typename TRenderInfo> requires std::is_base_of_v<LRenderInfo, TRenderInfo>
    FORCEINLINE auto const& Vk_GetWorldDataBuffer(TRenderInfo const& Info) const noexcept { return this->Vk_WorldBuffers[Info.Frame]; }

    ENGINE_API  static LWorld* GetWorldFromHumanReadableName(LStringView InHumanReadableName) noexcept;
    FORCEINLINE static LWorld* GetWorldFromHumanReadableNameChecked(LStringView InHumanReadableName) noexcept
    {
        auto* Out{GetWorldFromHumanReadableName(InHumanReadableName)};
        check(Out)
        return Out;
    }
    FORCEINLINE static LWorld* GetWorldFromHumanReadableNameAsserted(LStringView InHumanReadableName)
    {
        auto* Out{GetWorldFromHumanReadableName(InHumanReadableName)};
        jassert(Out)
        return Out;
    }

    ENGINE_API  APersonaController* GetThisWorldsLocalPersonaControllerSlow() noexcept;
    FORCEINLINE APersonaController* GetThisWorldsLocalPersonaControllerSlowAsserted() noexceptcheck { auto* Out{ this->GetThisWorldsLocalPersonaControllerSlow() }; check( Out ) return Out; }
    FORCEINLINE APersonaController* GetThisWorldsLocalPersonaControllerSlowChecked() { auto* Out{ this->GetThisWorldsLocalPersonaControllerSlow() }; jassert( Out ) return Out; }
    ENGINE_API  APersonaController const* GetThisWorldsLocalPersonaControllerSlow() const noexcept;
    FORCEINLINE APersonaController const* GetThisWorldsLocalPersonaControllerSlowAsserted() const noexceptcheck { auto* const Out{ this->GetThisWorldsLocalPersonaControllerSlow() }; check( Out ) return Out; }
    FORCEINLINE APersonaController const* GetThisWorldsLocalPersonaControllerSlowChecked() const { auto const* Out{ this->GetThisWorldsLocalPersonaControllerSlow() }; jassert( Out ) return Out; }

protected:

    // LObjectContext implementation
    virtual void OnTearDown() override;
    // ~LObjectContext implementation

private:

    LString UnsanitizedUrl;
    LString Url;
    LWorldParameters Parameters;
    void UpdateUrlParams() noexcept;

    TOptional<LLevel> UnderlyingLevel;

    //# Main thread only.
    FORCEINLINE void AcquireTickableObjectsLock() noexcept { this->TickableObjectsPutMutex = true; }
    FORCEINLINE void ReleaseTickableObjectsLock() noexcept { this->TickableObjectsPutMutex = false; }
    bool TickableObjectsPutMutex = false;
    TArray<LTickableObject*> TickableObjects;
    TArray<LTickableObject*> DeletedTickableObjects;

    EWorldState WorldState;

    LSubsystemCollection Collection{ "World" };

    //#
    //# The real time (not stopped or dilated / clamped) when this world was launched.
    //# Real time is relative to the static storage initialization of the engine shared library.
    //#
    f32 RealTimeWhenWorldWasLaunched { -1.0f };
    f32 RealTimeWhenWorldStarted{ -1.0f };

    //#
    //# Policies for this world. Cannot change. Can only be set during world initialization with the level blueprint.
    //# Only valid on authorities.
    //#
    ASupremePolicies* SupremePolicies{};

    LLinearColor BackgroundColor;
    TFrameArray<vk::raii::DescriptorSet> Vk_WorldDescriptorSets;
    TFrameArray<LMappedDeviceBuffer> Vk_WorldBuffers;
};

template<>
FORCEINLINE LCommandArgsTypeRet_t<LWorld> LCommandArgs::GetAs<LWorld>() const
{
    return LWorld::GetWorldFromHumanReadableNameAsserted(this->Name);
}

template<>
FORCEINLINE LCommandArgsTypeRet_t<const LWorld> LCommandArgs::GetAs<const LWorld>() const
{
    return LWorld::GetWorldFromHumanReadableNameAsserted(this->Name);
}

inline LWorld& LClassOuter::AsWorld() noexcept
{
    check(this->IsWorld())
    return *static_cast<LWorld*>(this);
}

inline LWorld const& LClassOuter::AsWorld() const noexcept
{
    check(this->IsWorld())
    return *static_cast<LWorld const*>(this);
}

} /* ~Namespace Jafg */

#include "Engine/WorldStorage.h"
