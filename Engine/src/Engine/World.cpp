// Copyright mzoesch. All rights reserved.

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Framework/Surface.h"
#include "Core/App.h"
#include "Framework/AvatarComponent.h"
#include "Framework/PersonaController.h"
#include "User/LocalEgo.h"
#include "Framework/SubsystemCollection.h"
#include "Framework/WorldSubsystem.h"
#include "Framework/SupremePolicies.h"
#include "User/UserPreferences.h"
#include "Framework/ActorComponentForward.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/PhysicsSystem.h"
#include "Framework/PhysicsSystemActor.h"
#include "Framework/RigidComponent.h"
#include "Framework/CharacterMovementComponent.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/Skybox.h"
#include "Framework/LightSubsystem.h"
#include "Rhi/Shadow.h"
#include "../Framework/PhysicsForeignCore.h"

namespace
{

//# The device representation of a light. @see Content/Shaders/include/Jafg/Pbr.slang
NODISCARD Jafg::SSBO::BindlessPbrLight MakeDeviceLight(Jafg::LLight const& Light) noexcept
{
    using namespace Jafg;

    SSBO::BindlessPbrLight Result{
        .Translation = Light.Translation,
        .Direction = Light.GetDirection(),
        .Type = Light.Type,
        .Tint = Light.Tint.ToDeviceReadyVector3(),
        .Intensity = Light.Intensity,
        .Right = Light.GetRight(),
        .Flags = SSBO::ELightFlagBits::Identity,
        .SpecularScale = maths::max(Light.SpecularScale, 0.0f),
        };

    switch (Light.Type)
    {
    case ELight::Directional:
    {
        //# The device expects the tangent of the angular radius of the source.
        Result.SourceRadius = maths::tan(maths::clamp(Light.SourceAngle, 0.0f, maths::half_pi_f) * 0.5f);
        break;
    }
    case ELight::Spot:
    {
        f32 const Outer{maths::clamp(Light.OuterConeAngle, 0.0f, maths::pi_f)};
        f32 const Inner{maths::clamp(Light.InnerConeAngle, 0.0f, Outer)};
        f32 const CosOuter{maths::cos(Outer)};
        Result.SpotScale = 1.0f / maths::max(maths::cos(Inner) - CosOuter, 0.001f);
        Result.SpotOffset = -CosOuter * Result.SpotScale;
        [[fallthrough]];
    }
    case ELight::Point:
    {
        Result.Range = maths::max(Light.Range, 0.0f);
        Result.SourceRadius = maths::max(Light.SourceRadius, 0.0f);
        break;
    }
    case ELight::Rect:
    {
        Result.Range = maths::max(Light.Range, 0.0f);
        Result.Width = maths::max(Light.Width, 0.0f);
        Result.Height = maths::max(Light.Height, 0.0f);
        break;
    }
    }

    return Result;
}

} /* ~Namespace <Anonymous> */

Jafg::LWorld::LWorld(LWorldCreateInfo Info, Detail::LWorldTrack& Track)
    : LClassOuter{std::move(Info.HumanReadableName)}, CreateInfo{std::move(Info)}
{
    STAT_FUNCTION()

    this->RealTimeWhenWorldWasLaunched = static_cast<f32>(App::GetElapsedTime());
    check(this->RealTimeWhenWorldWasLaunched > 0.0f)

    this->TimeBehavior = this->CreateInfo.TimeBehavior;

    check(this->WorldState == EWorldState::PreInitializing)
    this->WorldState = EWorldState::Initializing;

    check(this->CreateInfo.HumanReadableName.empty())

    check(!Track.ChildWorld.get())
    Track.ChildWorld = TUnique<LWorld>{this};

    LOG_VERBOSE(LogWorld, "[{}]: Initializing world with supreme policies [{}]."
        , this->GetHumanReadableName()
        , this->CreateInfo.SupremePoliciesClass.GetClassOrDefault().GetFullyQualifiedName()
        )
    this->SupremePolicies = SpawnObject(CastTo<ASupremePolicies>{}, {*this, this->CreateInfo.SupremePoliciesClass.GetClassOrDefault()});
    check(this->SupremePolicies)
    if (Info.Gravity)
    {
        this->SupremePolicies->PreferredGravity = *Info.Gravity;
    }

    LOG_TRACE(LogPhysics, "Initializing physics system.")
    check(!this->PhysicsSystem.IsValid())
    this->PhysicsSystem = LPhysicsSystemCreateInfo{
        .RigidBodyLimit = this->SupremePolicies->PreferredRigidBodyLimit
            ? *this->SupremePolicies->PreferredRigidBodyLimit : this->SupremePolicies->GetPreferredRigidBodyLimit(),
        .BodyMutexNumber = this->SupremePolicies->PreferredBodyMutexNumber
            ? *this->SupremePolicies->PreferredBodyMutexNumber : this->SupremePolicies->GetPreferredBodyMutexNumber(),
        .BodyPairLimit = this->SupremePolicies->PreferredBodyPairLimit
            ? *this->SupremePolicies->PreferredBodyPairLimit : this->SupremePolicies->GetPreferredBodyPairLimit(),
        .ContactConstraintLimit = this->SupremePolicies->PreferredContactConstraintLimit
            ? *this->SupremePolicies->PreferredContactConstraintLimit : this->SupremePolicies->GetPreferredContactConstraintLimit(),

        .TemporalUpdateStackSizeLimit = this->SupremePolicies->PreferredTemporalUpdateStackSizeLimit
            ? *this->SupremePolicies->PreferredTemporalUpdateStackSizeLimit : this->SupremePolicies->GetPreferredTemporalUpdateStackSizeLimit(),

        .JobLimit = this->SupremePolicies->PreferredJobLimit
            ? *this->SupremePolicies->PreferredJobLimit : this->SupremePolicies->GetPreferredJobLimit(),
        .BarrierLimit = this->SupremePolicies->PreferredBarrierLimit
            ? *this->SupremePolicies->PreferredBarrierLimit : this->SupremePolicies->GetPreferredBarrierLimit(),
        .ThreadLimit = this->SupremePolicies->PreferredThreadLimit
            ? *this->SupremePolicies->PreferredThreadLimit : this->SupremePolicies->GetPreferredThreadLimit(),

        .PhysicsStep = this->SupremePolicies->PreferredPhysicsStep
            ? *this->SupremePolicies->PreferredPhysicsStep : this->SupremePolicies->GetPreferredPhysicsStep(),
        .AccumulatorLimit = this->SupremePolicies->PreferredAccumulatorLimit
            ? *this->SupremePolicies->PreferredAccumulatorLimit : this->SupremePolicies->GetPreferredAccumulatorLimit(),
        .PhysicsSteps = this->SupremePolicies->PreferredPhysicsSteps
            ? *this->SupremePolicies->PreferredPhysicsSteps : this->SupremePolicies->GetPreferredPhysicsSteps(),

        .Gravity = this->SupremePolicies->PreferredGravity
            ? *this->SupremePolicies->PreferredGravity : this->SupremePolicies->GetPreferredGravity()
        };
    check(this->PhysicsSystem.IsValid())

    this->RealTimeWhenWorldStarted = static_cast<f32>(App::GetElapsedTime());
    check(this->RealTimeWhenWorldStarted >= this->RealTimeWhenWorldWasLaunched)

    this->SupremePolicies->OnWorldPreInit();

    checkCode
    (
        for (auto& Obj : this->GetEmployees())
        {
            check(Obj->_HasBegunLife())
            if (auto* Actor{Obj->As<AActor>()})
            {
                check(Actor->_Lives())
                check(Obj->IsA<APersonaController>() == false)
            }
        }
    )

    this->Collection.InitializeDeferred(this);
    this->Collection.InitializeSubsystems<JWorldSubsystem>();

    if (this->CreateInfo.TextureCube)
    {
        this->MakeSkybox(this->CreateInfo.TextureCube);
    }

    this->GetMutableEngine().OnWorldBeginLife.Broadcast(this);
    this->WorldState = EWorldState::Running;

    this->SupremePolicies->OnWorldPostInit();

    this->PhysicsSystem->OptimizeBroadPhase();

    return;
}

void Jafg::LWorld::TearDownWithTrack()
{
    this->TearDown();
    auto& Engine{this->GetMutableEngine()};
    Engine.SilentlyRemoveTrack(Engine.GetTrackFromWorld(*this));
}

LString Jafg::LWorld::GetDetailedHumanReadableName() const noexcept
{
    return algo::sprintf("{}@{}"
        , this->GetHumanReadableName()
        , this->CreateInfo.SupremePoliciesClass.GetClassOrDefault().GetFullyQualifiedName()
        );
}

void Jafg::LWorld::Tick(f64 Dt)
{
    STAT_CUSTOM_ZONE_FMT("WorldTick::{}", this->GetHumanReadableName())

    this->DeltaTime = Dt;
    if (this->ShouldTickPhysics())
    {
        check(this->PhysicsSystem.IsValid())
        this->PhysicsSystem.Advance(Dt);
    }

    {
        STAT_CUSTOM_ZONE_FMT("WorldTick::TickableObjects::{}", this->GetHumanReadableName())
        this->AcquireTickableObjectsLock();
        for (LTickableObject* Tickable: this->TickableObjects)
        {
            Tickable->Tick(static_cast<f32>(this->DeltaTime));
        }
        this->ReleaseTickableObjectsLock();
        for (LTickableObject* Tickable: this->DeletedTickableObjects)
        {
            algo::erase_once_checked(&this->TickableObjects, Tickable);
        }
        this->DeletedTickableObjects.clear();
    }

    bool bFollowup{};
    auto PhysicsUpdate{[&](f32 PhysicsStep)
    {
        STAT_CUSTOM_ZONE_FMT("WorldTick::PhysicsTick::{}", this->GetHumanReadableName())
        for (auto* Tickable: this->PhysicsTickableObjects)
        {
            Tickable->FixedTick(PhysicsStep, bFollowup);
        }
        bFollowup = true;
    }};
    auto PollPhysicsState{[&]
    {
        STAT_CUSTOM_ZONE_FMT("WorldTick::PollPhysicsState::{}", this->GetHumanReadableName())

        auto& Interface{this->PhysicsSystem->GetBodyInterfaceNoLock()};

        JPH::RVec3 Position;
        JPH::Quat Rotation;
        for (auto* Comp: this->RigidComponents)
        {
            check(IsValidFast(*this, Comp))
            check(Comp->GetRigidObject().IsValid())
            check(&Comp->GetOwningActor().GetRootComponent() == Comp && "Currently only supported as root component.")

            Interface.GetPositionAndRotation(Comp->GetRigidObject()->GetID(), Position, Rotation);

            Comp->LocalTransform.t = {Position.GetX(), Position.GetY(), Position.GetZ()};
            Comp->LocalTransform.r = {Rotation.GetX(), Rotation.GetY(), Rotation.GetZ(), Rotation.GetW()};
            Comp->_detail_OnPhysicsPoll(ETransformChangeBits::Translation|ETransformChangeBits::Rotator);
        }
    }};
    if (this->ShouldTickPhysics())
    {
        checkCode(this->_check_bUpdatingPhysics = true)
        if (this->PhysicsSystem.TryUpdate(PhysicsUpdate))
        {
            PollPhysicsState();
        }
        check(this->DormantTicks == 0)
        checkCode(this->_check_bUpdatingPhysics = false)
    }
    else if (this->DormantTicks > 0)
    {
        check(this->IsLinearWorldDormant())
        checkCode(this->_check_bUpdatingPhysics = true)
        this->PhysicsSystem.Update(this->DormantTicks, PhysicsUpdate);
        this->DormantTicks = 0;
        PollPhysicsState();
        checkCode(this->_check_bUpdatingPhysics = false)
    }

    /* Last, so that the views follow everything that moved in this tick, including the physics. */
    {
        STAT_CUSTOM_ZONE_FMT("WorldTick::Views::{}", this->GetHumanReadableName())
        for (TArray<APersonaController*> const Controllers{this->PersonaControllers}; APersonaController* Ctrl: Controllers)
        {
            Ctrl->_UpdateView(static_cast<f32>(this->DeltaTime));
        }
    }
}

void Jafg::LWorld::_RegisterPersonaController(APersonaController* Ctrl)
{
    check(Ctrl)
    check(!algo::contains(this->PersonaControllers, Ctrl))
    this->PersonaControllers.emplace_back(Ctrl);
}

void Jafg::LWorld::_UnregisterPersonaController(APersonaController* Ctrl)
{
    algo::erase_once(&this->PersonaControllers, Ctrl);
}

void Jafg::LWorld::Draw(LRenderInfo const& Info, DrawInfo DrawInfo) const
{
    STAT_FUNCTION()

    check(!DrawInfo.SharedSets.contains(SSBO::BindlessPbrMaterial::name()))
    DrawInfo.SharedSets.emplace(UBO::Bindless::name(), Info.TextureSubsystem.Vk_GetBindlessTextureArrayDescriptorSet());
    DrawInfo.SharedSets.emplace(SSBO::BindlessPbrMaterial::name(), Info.MaterialSubsystem.GetBindlessPbrInstanceDescriptorSet(Info.Frame));

    LActorRenderInfo ActorInfo{Info, DrawInfo.Eye, DrawInfo.SharedSets, DrawInfo.SharedBuffers};
    auto& Frontend{ActorInfo.Frontend};
    auto& Eye{ActorInfo.PerspectiveEye};

    if (DrawInfo.PolygonMode == EPolygonMode::Fill)
    {
        ActorInfo.DefaultPerspectivePolygonMode = vk::PolygonMode::eFill;
    }
    else if (DrawInfo.PolygonMode == EPolygonMode::Wireframe)
    {
        ActorInfo.DefaultPerspectivePolygonMode = vk::PolygonMode::eLine;
    }
    else
    {
        std::unreachable();
    }

    ActorInfo.PreferredMaterial = DrawInfo.Instance;

    {
        auto& WorldData{ActorInfo.WorldData};

        if (DrawInfo.ViewOverride)
        {
            WorldData.View = DrawInfo.ViewOverride->View;
            WorldData.Proj = DrawInfo.ViewOverride->Proj;
        }
        else
        {
            WorldData.View = maths::look_at(Eye.translation, Eye.translation + Eye.front, Eye.up);
            WorldData.Proj = maths::perspective<LWorldReal,world_qual>(
                Eye.vert_fov,
                Info.VkViewport.width / Info.VkViewport.height,
                Eye.near_frustum, Eye.far_frustum
                );
            WorldData.Proj[1][1] *= -1.0f;
        }

        WorldData.Ambient = this->AmbientTint;
        WorldData.Exposure = this->Exposure;
        WorldData.Gamma = this->Gamma;
        WorldData.IblSpecularMaxLod = this->IblSpecularMaxLod;
        WorldData.IblDiffuseIntensity = this->IblDiffuseIntensity;
        WorldData.IblSpecularIntensity = this->IblSpecularIntensity;

        WorldData.AmbientOcclusionTexture = DrawInfo.AmbientOcclusionTexture;

        WorldData.Flags = DrawInfo.Preferences;

        //# TODO: Is that not inside the view?
        WorldData.CameraPosition = Eye.translation;

        WorldData.ViewportHeight = Info.VkViewport.height;
        WorldData.VerticalFov = Eye.vert_fov;

        if (DrawInfo.bUpdateWorldDataSet)
        {
            ActorInfo.WorldData.upload_and_update(Frontend.Vk_GetDevice()
                , DrawInfo.SharedSets.at(UBO::WorldData::name())
                , 0
                , *DrawInfo.SharedBuffers.at(UBO::WorldData::name())
                );
        }
    }

    if (DrawInfo.PbrLightLimit > 0)
    {
        auto& LightSubsystem{*this->GetSubsystemChecked<JLightSubsystem>()};

        rhi::object_range<SSBO::BindlessPbrLight> Lights{16, DrawInfo.PbrLightLimit};
        for (auto* Light: LightSubsystem.GetLights())
        {
            check(Light->bCanEverAffectWorld)
            if (Light->bEnabled && Light->Intensity > 0.0f)
            {
                auto& DeviceLight{Lights->emplace_back(::MakeDeviceLight(*Light))};
                if (DrawInfo.Shadows)
                {
                    auto const Views{DrawInfo.Shadows->FindViews(*Light)};
                    DeviceLight.Shadow = Views.First;
                    DeviceLight.ShadowCount = Views.Count;
                }
            }
        }
        if (Lights->empty())
        {
            //# The descriptor range must never be empty. The shader skips lights without intensity.
            Lights->push_back({});
        }
        Lights.upload_and_update(Info.Frontend.Vk_GetDevice()
            , DrawInfo.SharedSets.at(SSBO::BindlessPbrLight::name())
            , SSBO::BindlessPbrLight::LightBinding
            , *DrawInfo.SharedBuffers.at(SSBO::BindlessPbrLight::name())
            );

        //# The shadow views share the set of the lights. Every binding of it has to be valid.
        check(DrawInfo.PbrShadowViewLimit > 0)
        rhi::object_range<SSBO::PbrShadowView> ShadowViews{4, DrawInfo.PbrShadowViewLimit};
        if (DrawInfo.Shadows)
        {
            auto const& Views{DrawInfo.Shadows->GetDeviceViews()};
            ShadowViews->insert(ShadowViews->end(), Views.begin(), Views.end());
        }
        if (ShadowViews->empty())
        {
            //# The descriptor range must never be empty. No light references this view.
            ShadowViews->push_back({});
        }
        ShadowViews.upload_and_update(Info.Frontend.Vk_GetDevice()
            , DrawInfo.SharedSets.at(SSBO::BindlessPbrLight::name())
            , SSBO::BindlessPbrLight::ShadowViewBinding
            , *DrawInfo.SharedBuffers.at(SSBO::PbrShadowView::name())
            );
    }

    ActorInfo.CommandBuffer.setPolygonModeEXT(ActorInfo.DefaultPerspectivePolygonMode);

    if (DrawInfo.Filter)
    {
        check(!DrawInfo.Filter->empty())
        for (auto* Actor: *DrawInfo.Filter)
        {
            check(!Actor->_IsGarbage())
            check(&Actor->GetOuter() == this)
            for (auto const& Comp: Actor->GetComponents())
            {
                if (Comp->ShouldRender())
                {
                    Comp->Render(ActorInfo);
                }
            }
        }
    }
    else
    {
        for (auto& Obj: this->GetEmployees())
        {
            if (!Obj->IsA<AActor>())
            {
                continue;
            }

            AActor const* Actor{StaticCastChecked<AActor>(&*Obj)};
            check(!Actor->_IsGarbage())
            if (Actor->AllowsRendering())
            {
                for (auto const& Comp: Actor->GetComponents())
                {
                    if (Comp->ShouldRender())
                    {
                        Comp->Render(ActorInfo);
                    }

                    continue;
                }
            }

            // if
            // (
            //        Actor->IsRendererComponentValid()
            //     && Actor->GetRendererComponent()->Cull(CornersSpan) == false
            // )
            // {
            //     Actor->GetRendererComponent()->Draw(Viewport, Eye);
            // }

            continue;
        }
    }

    //# The skybox cannot be rendered with any other material than its own.
    if (this->Skybox && !!this->Skybox->GetSkyboxTexture() && !DrawInfo.Filter && !DrawInfo.Instance)
    {
        this->Skybox->RenderSkybox(ActorInfo);
    }

    while (!ActorInfo.PostRenderDelegates.empty())
    {
        TArray Now{std::move(ActorInfo.PostRenderDelegates)};
        for (auto& F: Now)
        {
            F(ActorInfo);
        }
    }
}

std::expected<Jafg::APersonaController*,LString> Jafg::LWorld::Login(LTransientPersona Persona)
{
    check(Tasks::IsOnMasterThread())
    check(this->GetWorldState() == EWorldState::Running)
    check(this->SupremePolicies)

    if (std::holds_alternative<LTransientPersona::Local>(*Persona))
    {
        if (auto& Lackey{std::get<LTransientPersona::Local>(*Persona).Lackey}; Lackey.IsOwnedPersonaControllerValid())
        {
            return std::unexpected(algo::sprintf(
                "Lackey already possesses persona controller [{}]. Rejecting login request."
                , Lackey.GetOwnedPersonaControllerChecked()->GetNameAsString()
                ));
        }
    }

    auto Pc{this->SupremePolicies->OnIncomingConnectionRequest(std::holds_alternative<LTransientPersona::Proxy>(*Persona)
        ? ASupremePolicies::Proxy
#if JAFG_WITH_EDITOR
        : std::get<LTransientPersona::Local>(*Persona).bEditor
            ? ASupremePolicies::Editor
#endif /* JAFG_WITH_EDITOR */
            : ASupremePolicies::Local
        )};
    if (!Pc)
    {
        return std::unexpected{Pc.error()};
    }
    auto& Result{*Pc.value()};

    if (std::holds_alternative<LTransientPersona::Local>(*Persona))
    {
        auto& Local{std::get<LTransientPersona::Local>(*Persona)};
        check(!Local.Lackey.IsOwnedPersonaControllerValid())
        Local.Lackey.PossessPersonaController(std::move(Pc).value());
    }
    else
    {
        std::unreachable();
    }
    check(!Pc.value().get())

    this->SupremePolicies->OnPersonaControllerCreated(Result);
    return &Result;
}

void Jafg::LWorld::RegisterTickableObject(LTickableObject* Tickable)
{
    check(!this->IsTickableObjectsPutMutexLocked())

    if (algo::contains(this->TickableObjects, Tickable))
    {
        LOG_FATAL(LogWorld, "[{}]: Found duplicate tickable object.", this->GetHumanReadableName())
    }

    this->TickableObjects.push_back(Tickable);
}

void Jafg::LWorld::UnregisterTickableObject(LTickableObject* Tickable)
{
    if (this->IsTickableObjectsPutMutexLocked())
    {
        this->DeletedTickableObjects.push_back(Tickable);
    }
    else
    {
        if (algo::erase_once(&this->TickableObjects, Tickable))
        {
            return;
        }
        LOG_FATAL(LogWorld, "[{}]: Failed to find tickable object.", this->GetHumanReadableName())
    }
}

void Jafg::LWorld::RegisterPhysicsTickableObject(LFixedTickableObject* Tickable)
{
    check(!this->_check_bUpdatingPhysics)
    if (algo::contains(this->PhysicsTickableObjects, Tickable))
    {
        LOG_FATAL(LogWorld, "[{}: Found duplicate fixed tickable object.", this->GetHumanReadableName())
    }
    this->PhysicsTickableObjects.push_back(Tickable);
}

void Jafg::LWorld::UnregisterPhysicsTickableObject(LFixedTickableObject* Tickable)
{
    check(!this->_check_bUpdatingPhysics)
    if (algo::erase_once(&this->PhysicsTickableObjects, Tickable))
    {
        return;
    }
    LOG_FATAL(LogWorld, "[{}]: Failed to find fixed tickable object.", this->GetHumanReadableName())
}

f32 Jafg::LWorld::GetRealTimeSecondsSinceWorldLaunch() const noexcept
{
    return static_cast<f32>(App::GetElapsedTime()) - this->RealTimeWhenWorldWasLaunched;
}

namespace
{

//# @see Jafg::Detail::CharacterBodyUserDataTag
NODISCARD Jafg::AActorComponent* GetBodyOwner(u64 UserData) noexcept
{
    if (UserData == 0)
    {
        return nullptr;
    }
    if (UserData & Jafg::Detail::CharacterBodyUserDataTag)
    {
        return reinterpret_cast<Jafg::ACharacterMovementComponent*>(UserData & ~Jafg::Detail::CharacterBodyUserDataTag);
    }
    return reinterpret_cast<Jafg::ARigidComponent*>(UserData);
}

class trace_object_layer_filter final : public JPH::ObjectLayerFilter
{
public:

    explicit trace_object_layer_filter(Jafg::ECollisionChannels InChannels) noexcept : Channels{InChannels} {}

    virtual bool ShouldCollide(JPH::ObjectLayer Layer) const override
    {
        switch (Layer)
        {
        case Jafg::EPhysicsLayer::Static: return !!(this->Channels & Jafg::ECollisionChannelBits::Static);
        case Jafg::EPhysicsLayer::Dynamic: return !!(this->Channels & (Jafg::ECollisionChannelBits::Dynamic | Jafg::ECollisionChannelBits::Pawn));
        default: return false;
        }
    }

private:

    Jafg::ECollisionChannels Channels;
};

//# Bodies of characters are in the pawn channel. All other bodies are in the channel of their layer.
class trace_body_filter final : public JPH::BodyFilter
{
public:

    trace_body_filter(Jafg::ECollisionChannels InChannels, TArray<Jafg::AActor const*> const& InIgnoredActors) noexcept
        : Channels{InChannels}, IgnoredActors{InIgnoredActors}
    {
    }

    virtual bool ShouldCollideLocked(JPH::Body const& Body) const override
    {
        Jafg::AActorComponent const* Owner{::GetBodyOwner(Body.GetUserData())};
        if (!Owner || !Owner->IsOwningActorValid())
        {
            return false;
        }

        Jafg::AActor const& Actor{Owner->GetOwningActor()};
        if (algo::contains(this->IgnoredActors, &Actor))
        {
            return false;
        }

        if (Body.GetUserData() & Jafg::Detail::CharacterBodyUserDataTag)
        {
            return !!(this->Channels & Jafg::ECollisionChannelBits::Pawn);
        }
        if (Body.GetObjectLayer() == Jafg::EPhysicsLayer::Static)
        {
            return !!(this->Channels & Jafg::ECollisionChannelBits::Static);
        }
        return !!(this->Channels & Jafg::ECollisionChannelBits::Dynamic);
    }

private:

    Jafg::ECollisionChannels Channels;
    TArray<Jafg::AActor const*> const& IgnoredActors;
};

} /* ~Namespace <Anonymous> */

bool Jafg::LWorld::LineTraceByChannel(
    TArray<LHitResult>* OutHits,
    LWorldVec3 const& Begin,
    LWorldVec3 const& End,
    ECollisionChannels Channels,
    LCollisionQueryParams const& Params
) const
{
    STAT_FUNCTION()
    check(OutHits)
    check(Tasks::IsOnMasterThread())
    check(maths::magnitude(Begin - End) > static_cast<LWorldVec3::value_type>(maths::not_so_small_number_d) && "Why trace small distances.")

    if (!this->PhysicsSystem.IsValid() || !Channels)
    {
        return false;
    }

    JPH::PhysicsSystem const& System{*this->PhysicsSystem};
    LWorldVec3 const Delta{End - Begin};
    JPH::RRayCast const Ray{
          JPH::RVec3{Begin.x, Begin.y, Begin.z}
        , JPH::Vec3{static_cast<f32>(Delta.x), static_cast<f32>(Delta.y), static_cast<f32>(Delta.z)}
        };
    JPH::RayCastSettings const Settings{};
    ::trace_object_layer_filter const ObjectFilter{Channels};
    ::trace_body_filter const BodyFilter{Channels, Params.IgnoredActors};

    JPH::Array<JPH::RayCastResult> Results;
    if (Params.bSingleHit)
    {
        JPH::ClosestHitCollisionCollector<JPH::CastRayCollector> Collector;
        System.GetNarrowPhaseQuery().CastRay(Ray, Settings, Collector, {}, ObjectFilter, BodyFilter);
        if (Collector.HadHit())
        {
            Results.push_back(Collector.mHit);
        }
    }
    else
    {
        JPH::AllHitCollisionCollector<JPH::CastRayCollector> Collector;
        System.GetNarrowPhaseQuery().CastRay(Ray, Settings, Collector, {}, ObjectFilter, BodyFilter);
        if (Params.bSortByDistance)
        {
            Collector.Sort();
            if (Params.bSortDescending)
            {
                std::ranges::reverse(Collector.mHits);
            }
        }
        Results = std::move(Collector.mHits);
    }

    std::size_t const Previous{OutHits->size()};
    for (JPH::RayCastResult const& Hit: Results)
    {
        JPH::BodyLockRead const Lock{System.GetBodyLockInterface(), Hit.mBodyID};
        if (!Lock.Succeeded())
        {
            continue;
        }

        JPH::Body const& Body{Lock.GetBody()};
        AActorComponent* Owner{::GetBodyOwner(Body.GetUserData())};
        if (!Owner || !Owner->IsOwningActorValid())
        {
            continue;
        }

        JPH::RVec3 const Point{Ray.GetPointOnRay(Hit.mFraction)};
        JPH::Vec3 const Normal{Body.GetWorldSpaceSurfaceNormal(Hit.mSubShapeID2, Point)};
        OutHits->push_back({
            .Actor = Owner->GetOwningActor(),
            .Component = *Owner,
            .GlobalWorldLocation = LWorldVec3{Point.GetX(), Point.GetY(), Point.GetZ()},
            .SurfaceNormal = LWorldVec3{Normal.GetX(), Normal.GetY(), Normal.GetZ()},
            });
    }

    return OutHits->size() > Previous;
}

TArray<Jafg::LHitResult> Jafg::LWorld::LineTraceNonPhysical(LWorldMagRay3 const& Ray, TraceConfig const& Config) const
{
    STAT_FUNCTION()

    checkCode
    (
        if (!Config.bAllowNonUniformDirection)
        {
            check(maths::normalized(Ray.direction))
        }
    )
    check(maths::magnitude(Ray.origin - (Ray.origin + Ray.direction * Ray.magnitude))
        > static_cast<LWorldVec3::value_type>(maths::not_so_small_number_d) && "Why trace small distances.")

    TArray<LHitResult> Results;
    for (auto& Obj: this->GetEmployees())
    {
        if (auto* Actor{Obj->As<AActor>()})
        {
            if (LWorldAabb3 Aabb{Actor->GetTransformedActorAabb()}; !Aabb.empty())
            {
                if (maths::aabb_intersect_ray(Ray, Aabb).bHit)
                {
                    check(Actor->HasRootComponent())
                    auto TraverseActor{[&](this auto&& Self, ASceneComponent& Comp) -> void
                    {
                        if (!Comp.GetAabbForThisComponentOnly().empty())
                        {
                            if (auto Intersection{maths::aabb_intersect_ray(
                                Ray,
                                Comp.GetAabbForThisComponentOnly().apply(Comp.GetWorldTransformSlow())
                                )}; Intersection.bHit)
                            {
                                Results.push_back({
                                    .Actor = *Actor,
                                    .Component = Comp,
                                    .GlobalWorldLocation = Intersection.EnterPoint,
                                    });
                            }
                        }
                        for (auto& Child: Comp.GetChildren())
                        {
                            Self(*Child);
                        }
                    }};
                    TraverseActor(Actor->GetRootComponent());
                }
            }
        }
    }

    checkCode
    (
        for (auto& Hit: Results)
        {
            check(&Hit.Actor)
            check(&Hit.Component)
            check(&Hit.Actor == &Hit.Component.GetOwningActor())
        }
    )

    if (Config.bSingleHit && Results.size() > 1uz)
    {
        auto* ClosestHit{&Results.front()};
        f32 ClosestDistance{maths::magnitude(Ray.origin - ClosestHit->GlobalWorldLocation)};
        for (auto& Hit: Results)
        {
            if (f32 Distance{maths::magnitude(Ray.origin - Hit.GlobalWorldLocation)}; Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestHit = &Hit;
            }
        }
        LHitResult Dummy{*ClosestHit};
        Results.clear();
        Results.emplace_back(std::move(Dummy));
    }

    return Results;
}

Jafg::ASkybox& Jafg::LWorld::MakeSkybox(std::shared_ptr<LTextureCube2> Texture)
{
#if !JAFG_IN_SHIPPING
    if (Texture)
    {
        LOG_TRACE(LogWorld, "[{}]: Making skybox with [{}].", this->GetHumanReadableName(), Texture->GetDebugName())
    }
    else
    {
        LOG_TRACE(LogWorld, "[{}]: Making skybox with [nullptr].", this->GetHumanReadableName())
    }
#endif /* !JAFG_IN_SHIPPING */
    if (!this->Skybox)
    {
        this->Skybox = SpawnObject(TWorldStaticInit<ASkybox>{*this});
        check(this->Skybox)
    }

    check(IsValidFast(*this, this->Skybox))
    this->Skybox->SetSkyboxTexture(std::move(Texture));

    return *this->Skybox;
}

Jafg::LWorld* Jafg::LWorld::GetWorldFromHumanReadableName(LStringView InHumanReadableName) noexcept
{
    if (GEngine)
    {
        const auto* Track
        {
            algo::find_pointer(GEngine->GetTracks(), InHumanReadableName, [](auto const& E){ return E.ChildWorld->GetHumanReadableName(); })
        };

        return Track ? Track->ChildWorld.get() : nullptr;
    }

    return nullptr;
}

void Jafg::LWorld::OnTearDown()
{
    STAT_FUNCTION()

    check(this->WorldState == EWorldState::Running)
    this->WorldState = EWorldState::TearingDown;

    LOG_VERBOSE(LogWorld, "Tearing down world subsystems for world [{}].", this->GetHumanReadableName())
    this->Collection.TearDownSubsystems([](void) -> void
    {
        /* Preserve order. */
        Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Early, Tasks::RunAllTasks);
        Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Late, Tasks::RunAllTasks);
        Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Whenever, Tasks::RunAllTasks);
    });
    /* Preserve order. */
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Early, Tasks::RunAllTasks);
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Late, Tasks::RunAllTasks);
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Whenever, Tasks::RunAllTasks);

    LOG_VERBOSE(LogWorld, "Killing actors of world [{}].", this->GetHumanReadableName())
    for (auto Idx{0uz}; Idx < this->GetEmployees().size();)
    {
        TUnique<JCxxClass> const& Obj{this->GetEmployees()[Idx]};

        /* Null only allowed in tear down. */
        if (Obj.get() == nullptr)
        {
            ++Idx;
            continue;
        }

        if (Obj->IsA<AActor>())
        {
            Obj->MarkAsGarbage_v2(EJxxRecordTearDownReason::OuterTearDown);
            check(Obj.get() == nullptr)
            Idx = 0;
            continue;
        }

        ++Idx;
        continue;
    }
    checkCode
    (
        for (auto const& Obj : this->GetEmployees())
        {
            check(Obj.get() == nullptr || Obj->IsA<AActor>() == false)
        }
    )
    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

    check(this->Collection.IsOuterValid() == false && this->Collection.IsClassValid() == false)
    check(this->TickableObjectsPutMutex == false)

    LClassOuter::OnTearDown();

    this->WorldState = EWorldState::WaitingForKill;

    return;
}
