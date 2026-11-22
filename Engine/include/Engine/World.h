// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Jxx.h"
#include "Engine/Frustum.h"
#include "Core/Arguments.h"
#include "Framework/TraceUtility.h"
#include "Framework/SubsystemCollection.h"
#include "Framework/WorldSubsystem.h"
#include "Cli/CliType.h"
#include "Cli/CliCommand.h"
#include "Framework/SurfaceForward.h"
#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"
#include "Rhi/TextureCube.h"
#include "Framework/EditorForward.h"
#include "Framework/LackeyForward.h"
#include "Framework/PhysicsSystem.h"
#include "User/UserPreferencesForward.h"

namespace Jafg
{

class LLocalEgo;
class AActor;
class AActorComponent;
class ASkybox;
class LViewport;
class LEngine;
class LShader;
class JWorldSubsystem;
class LTickableObject;
class LFixedTickableObject;
class LPresentableObject;
class APersonaController;
class LCommandLineInterface;
class LWorld;
class ASupremePolicies;
class WWorldNode;
class ARigidComponent;
class LShadowAtlas;
struct LSubsystemCollection;
struct LRenderInfo;
struct LNodeRenderInfo;
struct LMaterialInstance;
struct LTextureCube2;

namespace Detail
{

struct LWorldTrack;

} /* ~Namespace Detail */

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

//# The time behaviro of a world is constant and cannot change during the entire lifetime of a world.
enum struct EWorldTimeBehavior: u8
{
    //#
    //# Time behaves linearly. This is the default behavior.
    //# The world can transition into a dormant state, but can never be truly desisted.
    //#
    Linear,
    //# Time is desisted for this world. It may never behave like a linear world. It is always dormant.
    Desist,
};
inline LStringView LexToString(EWorldTimeBehavior Type) noexcept
{
    switch (Type)
    {
        case EWorldTimeBehavior::Linear:   return "Linear";
        case EWorldTimeBehavior::Desist:   return "Desist";
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
#if JAFG_WITH_EDITOR
        bool bEditor{};
#endif /* JAFG_WITH_EDITOR */
    };
    struct Proxy
    {
        //# TODO: Net stuff etc.
    };
    std::variant<Spec, Local, Proxy> Variant;

    NODISCARD constexpr auto& operator*() noexcept { return this->Variant; }
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

struct LWorldCreateInfo final
{
#if JAFG_WITH_EDITOR
    LString HumanReadableName{ "Transient World" };
#endif /* JAFG_WITH_EDITOR */
    TSubclassOf<ASupremePolicies> SupremePoliciesClass;
    EWorldTimeBehavior TimeBehavior{ EWorldTimeBehavior::Linear };
    //# If set, will create a skybox with this cube-map.
    std::shared_ptr<LTextureCube2> TextureCube;
    //# If set, this vector will likely be used.
    std::optional<LVec3F> Gravity;
};

//#
//# A world.
//#
//# Once every frame the world will be ticked. It may register itself to the RHI to be used when
//# rendering on any kind of surface. Multiple worlds may draw to the same surface, and a world
//# may draw to multiple surfaces.
//#
class ENGINE_API LWorld final: public LClassOuter, public LEngineGetters
{
    friend AActor;
    friend ARigidComponent;

public:

    LWorld() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LWorld)
    LWorld(LWorldCreateInfo Info, Detail::LWorldTrack& Track);

    ~LWorld() noexcept override { check(this->WorldState == EWorldState::WaitingForKill) }

    // LClassOuter implementation
    virtual bool IsWorld() const noexcept override { return true; }
    // ~LClassOuter implementation

    void TearDownWithTrack();

    NODISCARD LString GetDetailedHumanReadableName() const noexcept;

    FORCEINLINE constexpr EWorldState GetWorldState() const noexcept { return this->WorldState; }

    FORCEINLINE bool CanTick() const noexcept { return this->GetWorldState() == EWorldState::Running && !this->bPaused; }
    void Tick(f64 Dt);

    //# Explicit device matrices that replace the ones derived from the eye of a draw call.
    struct LViewOverride
    {
        LMat4F View;
        //# Already in the device convention (y down).
        LMat4F Proj;
    };

    struct DrawInfo
    {
        //# The eye to draw from.
        LWorldEye const& Eye;
        //#
        //# If set, used instead of the matrices derived from #Eye and the aspect of the viewport. E.g. to draw from
        //# the view of a light. #Eye should still roughly match, as some drawables read it.
        //#
        std::optional<LViewOverride> ViewOverride;
        //# If set, the volume to cull against instead of the one derived from #Eye and the aspect of the viewport.
        std::optional<LFrustum> FrustumOverride;
        //# The instance to prefer for all supported drawables.
        LMaterialInstance* Instance{};
        //# Shared named sets free to use for this draw call. Might get modified.
        //# TODO: Do we need a second set with ro rights only?
        algo::transparent_unordered_string_map<vk::DescriptorSet> SharedSets;
        //# Shared named buffers free to use for this draw call. Might get modified.
        //# Ptrs must be valid.
        //# TODO: Replace this with a non-owning buffer object. Ptr here is so annoying.
        //# TODO: Do we need a second set with ro rights only?
        algo::transparent_unordered_string_map<rhi::mapped_device_buffer*> SharedBuffers;
        //# An optional filter to only draw a subset of actors.
        std::optional<TArray<AActor*>> const& Filter;
        //#
        //# An optional filter to only draw a subset of components (each with what is attached to it), e.g., a single
        //# selected component of an actor. Mutually exclusive with #Filter. Must outlive the call.
        //#
        TArray<AActorComponent const*> const* ComponentFilter{};

        EWorldRenderFlags Preferences{ EWorldRenderBits::All };
        EPolygonMode PolygonMode{ EPolygonMode::Fill };

        //# Whether to update world device data for this draw call.
        bool bUpdateWorldDataSet{ true };

        //# When using the pbr pipeline. This is the limit.
        std::size_t PbrMaterialLimit{};
        std::size_t PbrLightLimit{};
        //# When using the pbr pipeline. Requires a shared buffer for SSBO::PbrShadowView.
        std::size_t PbrShadowViewLimit{};

        //# The shadows of the lights for this draw call, if any. Lights without a view are not shadowed.
        LShadowAtlas const* Shadows{};

        //# Bindless index of the screen space ambient occlusion for this eye, if any.
        rhi::resource_index_t AmbientOcclusionTexture{ rhi::invalid_resource_index };

        //# The controller whose view is drawn. Null if no controller sees this draw directly (e.g., shadow maps).
        APersonaController const* Viewer{};
    };
    void Draw(LRenderInfo const& Info, DrawInfo DrawInfo) const;

    std::expected<APersonaController*,LString> Login(LTransientPersona Persona);

#if JAFG_WITH_EDITOR
    NODISCARD constexpr LWorldCreateInfo const& GetCreateInfo() const noexcept { return this->CreateInfo; }
#endif /* JAFG_WITH_EDITOR */

    void RegisterTickableObject(LTickableObject* Tickable);
    void UnregisterTickableObject(LTickableObject* Tickable);
    void RegisterPhysicsTickableObject(LFixedTickableObject* Tickable);
    void UnregisterPhysicsTickableObject(LFixedTickableObject* Tickable);
    //# @see #LPresentableObject
    void RegisterPresentableObject(LPresentableObject* Presentable);
    void UnregisterPresentableObject(LPresentableObject* Presentable);
    //# @see #LPresentableObject::Present
    NODISCARD f32 GetPhysicsInterpolationAlpha() const noexcept;
    FORCEINLINE bool IsTickableObjectsPutMutexLocked() const { return this->TickableObjectsPutMutex; }
    FORCEINLINE TArray<LTickableObject*> const& GetTickableObjects() const noexcept { return this->TickableObjects; }
    FORCEINLINE TArray<LTickableObject*>& GetMutableTickableObjects() noexcept { return this->TickableObjects; }
    FORCEINLINE TArray<LTickableObject*> const& GetDeletedTickableObjects() const noexcept { return this->DeletedTickableObjects; }
    FORCEINLINE TArray<LTickableObject*>& GetDeletedMutableTickableObjects() noexcept { return this->DeletedTickableObjects; }

    f32 GetRealTimeSecondsSinceWorldLaunch() const noexcept;

    //# All persona controllers that live in this world.
    NODISCARD FORCEINLINE TArray<APersonaController*> const& GetPersonaControllers() const noexcept { return this->PersonaControllers; }
    //# Jafg internal methods. Do not use. Called by the persona controllers themselves.
    void _RegisterPersonaController(APersonaController* Ctrl);
    void _UnregisterPersonaController(APersonaController* Ctrl);

    //#
    //# Trace the physics of this world for hits.
    //# Bodies that are not owned by a component (rigid components and character movement components) are never hit.
    //# @param OutHits Hits are appended. Unordered unless sorted with #LCollisionQueryParams.
    //# @return True if at least one hit was found.
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
    //#
    //# Traces the ray against what is in the world instead of the physics simulation (e.g., for clicks in the
    //# editor). Every scene component is traced on its own. @see #ASceneComponent::TraceNonPhysical
    //#
    NODISCARD TArray<LHitResult> LineTraceNonPhysical(LWorldMagRay3 const& Ray, TraceConfig const& Config) const;

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JWorldSubsystem)

    //#
    //# The delta time of this world. This value should be used instead of the LEngine::DeltaTime as this
    //# value is dilated according to world physics.
    //#
    NODISCARD FORCEINLINE constexpr f64 GetDeltaTime() const noexcept { return this->DeltaTime; }

    NODISCARD FORCEINLINE constexpr EWorldTimeBehavior GetTimeBehavior() const noexcept { return this->TimeBehavior; }
    NODISCARD FORCEINLINE constexpr bool IsTimeLinear() const noexcept { return this->TimeBehavior == EWorldTimeBehavior::Linear; }
    NODISCARD FORCEINLINE constexpr bool IsTimeDesisted() const noexcept { return this->TimeBehavior == EWorldTimeBehavior::Desist; }
    FORCEINLINE constexpr void SetDormantStateOfLinearWorld(bool b) noexcept
    {
        LOG_TRACE(LogWorld, "[{}]: Setting dormant state of linear world to [{}].", this->GetHumanReadableName(), b)
        check(this->IsTimeLinear())
        this->bDormant = b;
    }
    NODISCARD FORCEINLINE constexpr bool IsLinearWorldDormant() const noexcept { check(this->IsTimeLinear()) return this->bDormant; }
    FORCEINLINE constexpr void SetDormantTicks(u32 Ticks) noexcept { check(this->IsLinearWorldDormant()) check(!this->ShouldTickPhysics()) this->DormantTicks = Ticks; }
    FORCEINLINE constexpr void AddDormantTicks(u32 Ticks = 1) noexcept { check(this->IsLinearWorldDormant()) check(!this->ShouldTickPhysics()) this->DormantTicks += Ticks; }
    NODISCARD FORCEINLINE constexpr u32 GetDormantTicks() const noexcept { check(this->IsLinearWorldDormant()) return this->DormantTicks; }

    //#
    //# If a world is paused. It will no longer be used by any employee. It is completely ignored by the engine.
    //# Even for desisted actors this world will be unreachable.
    //#
    //# @note A paused world might still travel.
    //#
    NODISCARD FORCEINLINE constexpr bool IsPaused() const noexcept { return this->bPaused; }
    FORCEINLINE constexpr void Pause() noexcept
    {
        LOG_TRACE(LogWorld, "[{}]: Pausing world.", this->GetHumanReadableName())
        this->bPaused = true;
    }
    FORCEINLINE constexpr void Unpause() noexcept
    {
        LOG_TRACE(LogWorld, "[{}]: Unpausing world.", this->GetHumanReadableName())
        this->bPaused = false;
    }

    NODISCARD FORCEINLINE constexpr ASupremePolicies* GetSupremePolicies() noexcept { return this->SupremePolicies; }
    NODISCARD FORCEINLINE constexpr ASupremePolicies const* GetSupremePolicies() const noexcept { return this->SupremePolicies; }
    NODISCARD FORCEINLINE constexpr ASupremePolicies* GetSupremePoliciesChecked() noexceptcheck { check(this->SupremePolicies) return this->SupremePolicies; }
    NODISCARD FORCEINLINE constexpr ASupremePolicies const* GetSupremePoliciesChecked() const noexceptcheck { check(this->SupremePolicies) return this->SupremePolicies; }
    NODISCARD FORCEINLINE constexpr ASupremePolicies* GetSupremePoliciesAsserted() { jassert(this->SupremePolicies) return this->SupremePolicies; }
    NODISCARD FORCEINLINE constexpr ASupremePolicies const* GetSupremePoliciesAsserted() const { jassert(this->SupremePolicies) return this->SupremePolicies; }

    ENGINE_API ASkybox& MakeSkybox(std::shared_ptr<LTextureCube2> Texture);
    NODISCARD FORCEINLINE constexpr ASkybox* GetSkybox() noexcept { return this->Skybox; }
    NODISCARD FORCEINLINE constexpr ASkybox const* GetSkybox() const noexcept { return this->Skybox; }
    NODISCARD FORCEINLINE constexpr ASkybox* GetSkyboxChecked() noexceptcheck { check(this->Skybox) return this->Skybox; }
    NODISCARD FORCEINLINE constexpr ASkybox const* GetSkyboxChecked() const noexceptcheck { check(this->Skybox) return this->Skybox; }
    NODISCARD FORCEINLINE constexpr ASkybox* GetSkyboxAsserted() { jassert(this->Skybox) return this->Skybox; }
    NODISCARD FORCEINLINE constexpr ASkybox const* GetSkyboxAsserted() const { jassert(this->Skybox) return this->Skybox; }

    NODISCARD FORCEINLINE constexpr LLinearColor3 const& GetAmbientTint() const noexcept { return this->AmbientTint; }
    NODISCARD FORCEINLINE constexpr LLinearColor3& GetMutableAmbientTint() noexcept { return this->AmbientTint; }
    FORCEINLINE constexpr void SetAmbientTint(LLinearColor3 const& Tint) noexcept { this->AmbientTint = Tint; }
    NODISCARD FORCEINLINE constexpr f32 GetExposure() const noexcept { return this->Exposure; }
    NODISCARD FORCEINLINE constexpr f32& GetMutableExposure() noexcept { return this->Exposure; }
    FORCEINLINE constexpr void SetExposure(f32 E) noexcept { this->Exposure = E; }
    NODISCARD FORCEINLINE constexpr f32 GetGamma() const noexcept { return this->Gamma; }
    NODISCARD FORCEINLINE constexpr f32& GetMutableGamma() noexcept { return this->Gamma; }
    FORCEINLINE constexpr void SetGamma(f32 G) noexcept { this->Gamma = G; }
    NODISCARD FORCEINLINE constexpr f32 GetIblSpecularMaxLod() const noexcept { return this->IblSpecularMaxLod; }
    NODISCARD FORCEINLINE constexpr f32& GetMutableIblSpecularMaxLod() noexcept { return this->IblSpecularMaxLod; }
    FORCEINLINE constexpr void SetIblSpecularMaxLod(f32 L) noexcept { this->IblSpecularMaxLod = L; }
    NODISCARD FORCEINLINE constexpr f32 GetIblDiffuseIntensity() const noexcept { return this->IblDiffuseIntensity; }
    NODISCARD FORCEINLINE constexpr f32& GetMutableIblDiffuseIntensity() noexcept { return this->IblDiffuseIntensity; }
    FORCEINLINE constexpr void SetIblDiffuseIntensity(f32 I) noexcept { this->IblDiffuseIntensity = I; }
    NODISCARD FORCEINLINE constexpr f32 GetIblSpecularIntensity() const noexcept { return this->IblSpecularIntensity; }
    NODISCARD FORCEINLINE constexpr f32& GetMutableIblSpecularIntensity() noexcept { return this->IblSpecularIntensity; }
    FORCEINLINE constexpr void SetIblSpecularIntensity(f32 I) noexcept { this->IblSpecularIntensity = I; }

    NODISCARD FORCEINLINE constexpr bool ShouldTickPhysics() const noexcept { return !this->IsTimeDesisted() && !this->IsLinearWorldDormant(); }
    NODISCARD LPhysicsSystem& GetWorldGlobalPhysicsSystem() noexcept { return this->PhysicsSystem; }
    NODISCARD LPhysicsSystem const& GetWorldGlobalPhysicsSystem() const noexcept { return this->PhysicsSystem; }

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

#if JAFG_WITH_EDITOR
    LWorldCreateInfo CreateInfo;
#endif /* JAFG_WITH_EDITOR */

    //# Main thread only.
    FORCEINLINE void AcquireTickableObjectsLock() noexcept { this->TickableObjectsPutMutex = true; }
    FORCEINLINE void ReleaseTickableObjectsLock() noexcept { this->TickableObjectsPutMutex = false; }
    bool TickableObjectsPutMutex = false;
    TArray<LTickableObject*> TickableObjects;
    TArray<LTickableObject*> DeletedTickableObjects;
#if JAFG_DO_CHECKS
    bool _check_bUpdatingPhysics{};
#endif /* JAFG_DO_CHECKS */
    TArray<LFixedTickableObject*> PhysicsTickableObjects;
    TArray<LPresentableObject*> PresentableObjects;

    EWorldState WorldState{ EWorldState::PreInitializing };
    EWorldTimeBehavior TimeBehavior;
    bool bDormant:1{};
    bool bPaused:1{};
    u32 DormantTicks{};

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

    //# A skybox is optional.
    ASkybox* Skybox{};

    //#
    //# TODO: Kinda awkward here. Where to move?
    //# TODO: Make some preset somewhere. Where in the editor can switch between different states.
    //#             Ambient             Directional
    //# Morning:    {0.06,0.07,0.10}    {1.0,0.75,0.5}
    //# Day:        {0.12,0.15,0.20}    {1.0,0.96,0.9}
    //# Night:      {0.004,0.006,0.012} {0.6,0.70,1.0}
    //#
    LLinearColor3 AmbientTint{ 0.004,0.006,0.012 };
    f32 Exposure{ 1.0f };
    f32 Gamma{ 2.2f };
    f32 IblSpecularMaxLod{ 4.0f };
    f32 IblDiffuseIntensity{ 1.0f };
    f32 IblSpecularIntensity{ 1.0f };

    LPhysicsSystem PhysicsSystem{ ESkipInit::Here };
    TArray<ARigidComponent*> RigidComponents;
    TArray<APersonaController*> PersonaControllers;
};

//#
//# Wrapper around a world raw pointer to store it for a longer period of time. This is essentially a weak pointer.
//# You may check if the world pointer is still valid with #IsValid.
//# It is *not* meant for quick storing (e.g., inside a function) and is also not meant to be passed as an argument
//# to a function.
//#
//# @warning This class might be a victim of an aba state. Unlikely but possible...
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

    ENGINE_API bool IsValid() const noexcept;

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
