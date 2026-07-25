// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Jxx.h"
#include "Core/Arguments.h"
#include "Physics/TraceUtility.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/Level.h"
#include "Cli/CliType.h"
#include "Cli/CliCommand.h"
#include "Platform/SurfaceForward.h"
#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"
#include "Framework/LackeyForward.h"

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
class WWorldNode;
struct LLevel;
struct LSubsystemCollection;
struct LRenderInfo;
struct LNodeRenderInfo;
struct LMaterialInstance;

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
    }
    std::unreachable();
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
    }
    std::unreachable();
}

struct LTransientPersona final
{
    struct Spec
    {
    };
    struct Local
    {
        LLocalLackey& Lackey;
    };
    struct Proxy
    {
        //# TODO: Net stuff etc.
    };
    std::variant<Spec, Local, Proxy> Variant;

    NODISCARD constexpr auto& operator*() noexcept { return this->Variant; }
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
class ENGINE_API LWorld final : public LClassOuter, public LEngineGetters
{
    friend AActor;

public:

    LWorld() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LWorld)
    LWorld(LString HumanReadableName) noexcept : LClassOuter{std::move(HumanReadableName)} {}
    ~LWorld() noexcept override { check(this->WorldState == EWorldState::WaitingForKill) }

    // LClassOuter implementation
    virtual bool IsWorld() const noexcept override { return true; }
    // ~LClassOuter implementation

    void InitializeWorld(std::optional<LLevel> const& Level = {}, LString&& Url = {});

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
    void Tick(f64 Dt);

    void Draw(LRenderInfo const& Info, LWorldEye const& Eye, LMaterialInstance* Instance, algo::transparent_unordered_string_map<vk::DescriptorSet> SharedSets, std::optional<TArray<AActor*>> const& Filter) const;

    std::expected<APersonaController*,LString> Login(LTransientPersona Persona);

    FORCEINLINE bool IsUnderlyingLevelValid() const noexcept { return this->UnderlyingLevel.has_value(); }
    FORCEINLINE LLevel const& GetUnderlyingLevel() const { return this->UnderlyingLevel.value(); }
    FORCEINLINE LLevel const& GetUnderlyingLevelChecked() const noexceptcheck { check( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel.value(); }
    FORCEINLINE LLevel const& GetUnderlyingLevelAsserted() const { jassert( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel.value(); }
    FORCEINLINE LStringView   GetUnderlyingLevelName() const noexcept { if (this->IsUnderlyingLevelValid()) { return LStringView{this->UnderlyingLevel->Identifier}; } return {}; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameChecked() const noexceptcheck { check( this->IsUnderlyingLevelValid() ) return this->IsUnderlyingLevelValid() ? LStringView{this->UnderlyingLevel->Identifier} : LStringView{ }; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameAsserted() const { jassert( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel->Identifier; }

    void RegisterTickableObject(LTickableObject* Tickable);
    void UnregisterTickableObject(LTickableObject* Tickable);
    FORCEINLINE bool IsTickableObjectsPutMutexLocked() const { return this->TickableObjectsPutMutex; }
    FORCEINLINE TArray<LTickableObject*> const& GetTickableObjects() const noexcept { return this->TickableObjects; }
    FORCEINLINE TArray<LTickableObject*>& GetMutableTickableObjects() noexcept { return this->TickableObjects; }
    FORCEINLINE TArray<LTickableObject*> const& GetDeletedTickableObjects() const noexcept { return this->DeletedTickableObjects; }
    FORCEINLINE TArray<LTickableObject*>& GetDeletedMutableTickableObjects() noexcept { return this->DeletedTickableObjects; }

    f32 GetRealTimeSecondsSinceWorldLaunch() const noexcept;

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
    struct TraceConfig final
    {
        //#
        //# Allows non-uniform direction for the trace. If false, it is expected that the direction is normalized.
        //# This is asserted.
        //#
        bool bAllowNonUniformDirection:1{};
        //#
        //# If true, the trace will stop at the first blocking hit. If false, all hits will be returned.
        //# Might be more performant.
        //#
        bool bSingleHit:1{};
    };
    NODISCARD TArray<LHitResult> LineTraceNonPhysical(LWorldMagRay3 const& Ray, TraceConfig const& Config) const;

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JWorldSubsystem)

    //#
    //# The delta time of this world. This value should be used instead of the LEngine::DeltaTime as this
    //# value is dilated according to world physics.
    //#
    FORCEINLINE constexpr f64 GetDeltaTime() const noexcept { return this->DeltaTime; }

    FORCEINLINE ASupremePolicies* GetSupremePolicies() noexcept { return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies const* GetSupremePolicies() const noexcept { return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies* GetSupremePoliciesChecked() noexceptcheck { check(this->SupremePolicies) return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies const* GetSupremePoliciesChecked() const noexceptcheck { check(this->SupremePolicies) return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies* GetSupremePoliciesAsserted() { jassert(this->SupremePolicies) return this->SupremePolicies; }
    FORCEINLINE ASupremePolicies const* GetSupremePoliciesAsserted() const { jassert(this->SupremePolicies) return this->SupremePolicies; }

    template<typename TRenderInfo> requires std::is_base_of_v<LRenderInfo, TRenderInfo>
    FORCEINLINE auto const& Vk_GetWorldDataDescriptorSet(TRenderInfo const& Info) const noexcept { return this->Vk_WorldDescriptorSets[Info.Frame]; }
    template<typename TRenderInfo> requires std::is_base_of_v<LRenderInfo, TRenderInfo>
    FORCEINLINE auto const& Vk_GetWorldDataBuffer(TRenderInfo const& Info) const noexcept { return this->Vk_WorldBuffers[Info.Frame]; }

    static LWorld* GetWorldFromHumanReadableName(LStringView InHumanReadableName) noexcept;
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

protected:

    // LObjectContext implementation
    virtual void OnTearDown() override;
    // ~LObjectContext implementation

private:

    LString UnsanitizedUrl;
    LString Url;
    LWorldParameters Parameters;
    void UpdateUrlParams() noexcept;

    std::optional<LLevel> UnderlyingLevel;

    //# Main thread only.
    FORCEINLINE void AcquireTickableObjectsLock() noexcept { this->TickableObjectsPutMutex = true; }
    FORCEINLINE void ReleaseTickableObjectsLock() noexcept { this->TickableObjectsPutMutex = false; }
    bool TickableObjectsPutMutex = false;
    TArray<LTickableObject*> TickableObjects;
    TArray<LTickableObject*> DeletedTickableObjects;

    EWorldState WorldState{ EWorldState::PreInitializing };

    LSubsystemCollection Collection{ "World" };

    //#
    //# The real time (not stopped or dilated / clamped) when this world was launched.
    //# Real time is relative to the static storage initialization of the engine shared library.
    //#
    f32 RealTimeWhenWorldWasLaunched{ -1.0f };
    f32 RealTimeWhenWorldStarted{ -1.0f };
    f64 DeltaTime{};

    //#
    //# Policies for this world. Cannot change. Can only be set during world initialization with the level blueprint.
    //# Only valid on authorities.
    //#
    ASupremePolicies* SupremePolicies{};

    rhi::frame_array<vk::raii::DescriptorSet> Vk_WorldDescriptorSets JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    rhi::frame_array<rhi::mapped_device_buffer> Vk_WorldBuffers;
};

//#
//# Wrapper around a world raw pointer to store it for a longer period of time. This is essentially a weak pointer.
//# You may check if the world pointer is still valid with #IsValid.
//# It is *not* meant for quick storing (e.g., inside a function) and is also not meant to be passed as an argument
//# to a function.
//#
struct LWorldStorage final
{
    FORCEINLINE explicit constexpr LWorldStorage() noexcept : World(nullptr) { }
    FORCEINLINE explicit constexpr LWorldStorage(std::nullptr_t) noexcept : World(nullptr) { }
    FORCEINLINE explicit constexpr LWorldStorage(LWorld* InWorld) noexcept : World(InWorld) { }
    DEFAULT_REALLOC_OF_ANY_FORM(LWorldStorage)
    FORCEINLINE constexpr ~LWorldStorage() noexcept { }

    FORCEINLINE constexpr bool operator==(std::nullptr_t) const noexcept { return this->World == nullptr; }
    FORCEINLINE constexpr bool operator==(LWorldStorage const& Other) const noexcept { return this->World == Other.World; }
    FORCEINLINE constexpr bool operator==(LWorld const* Other) const noexcept { return this->World == Other; }

    FORCEINLINE constexpr void Reset() noexcept { this->World = nullptr; }
    FORCEINLINE constexpr bool IsNull() const noexcept { return this->World == nullptr; }
    FORCEINLINE constexpr bool IsNotNull() const noexcept { return this->IsNull() == false; }

    ENGINE_API  bool IsValid() const noexcept;

    FORCEINLINE constexpr LWorld* Get() noexcept { return this->World; }
    FORCEINLINE constexpr LWorld const* Get() const noexcept { return this->World; }

    FORCEINLINE constexpr LWorld* operator->() noexcept { return this->Get(); }
    FORCEINLINE constexpr LWorld const* operator->() const noexcept { return this->Get(); }

    FORCEINLINE constexpr LWorld& operator*() noexcept { return *this->Get(); }
    FORCEINLINE constexpr LWorld const& operator*() const noexcept { return *this->Get(); }

    FORCEINLINE constexpr operator bool() const noexcept { return this->IsNotNull(); }

    FORCEINLINE operator LWorld*() noexcept { return this->Get(); }
    FORCEINLINE operator LWorld const*() const noexcept { return this->Get(); }

private:

    LWorld* World;
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
