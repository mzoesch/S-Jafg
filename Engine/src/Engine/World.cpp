// Copyright mzoesch. All rights reserved.

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Framework/Surface.h"
#include "Core/App.h"
#include "Framework/Pawn.h"
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
#include "Framework/TextureSubsystem.h"
#include "Framework/Skybox.h"
#include "../Framework/PhysicsForeignCore.h"

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

    auto& Frontend{this->GetEngine().GetLocalEgo().GetFrontend()};

    LOG_TRACE(LogRhi, "Allocating world data descriptor sets.")
    auto& MaterialSubsystem{*Frontend.GetSubsystemChecked<JMaterialSubsystem>()};
    this->Vk_WorldDescriptorSets = Frontend.Vk_CreateFrequentDescriptorSets(MaterialSubsystem.GetSharedDescriptorSetLayout<UBO::WorldData>());
    this->Vk_WorldBuffers = Frontend.Vk_CreateFrequentMappedBuffer(UBO::WorldData::buffer_create_info());

    this->SupremePolicies = SpawnObject(CastTo<ASupremePolicies>{}, {*this, this->CreateInfo.SupremePoliciesClass.GetClassOrDefault()});
    check(this->SupremePolicies)

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
    STAT_FUNCTION()

    this->DeltaTime = Dt;
    if (this->ShouldTickPhysics())
    {
        check(this->PhysicsSystem.IsValid())
        this->PhysicsSystem.Advance(Dt);
    }

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

    auto PollPhysicsState{[&]
    {
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
        if (this->PhysicsSystem.TryUpdate())
        {
            PollPhysicsState();
        }
    }
    else if (this->DormantTicks > 0)
    {
        check(this->IsLinearWorldDormant())
        this->PhysicsSystem.Update(this->DormantTicks);
        this->DormantTicks = 0;
        PollPhysicsState();
    }
}

void Jafg::LWorld::Draw(LRenderInfo const& Info, LWorldEye const& Eye, LMaterialInstance* Instance, algo::transparent_unordered_string_map<vk::DescriptorSet> SharedSets, std::optional<TArray<AActor*>> const& Filter) const
{
    STAT_FUNCTION()

    LActorRenderInfo ActorInfo{Info, Eye, SharedSets};
    auto& Frontend{ActorInfo.Frontend};
    if (auto& Prefs{GetSingleton<JUserPreferences>()}; Prefs.PolygonMode == EPolygonMode::Fill)
    {
        ActorInfo.DefaultPerspectivePolygonMode = vk::PolygonMode::eFill;
    }
    else if (Prefs.PolygonMode == EPolygonMode::Wireframe)
    {
        ActorInfo.DefaultPerspectivePolygonMode = vk::PolygonMode::eLine;
    }
    else
    {
        std::unreachable();
    }

    ActorInfo.PreferredMaterial = Instance;

    auto& WorldData{ActorInfo.WorldData};
    WorldData = {
        .view = maths::look_at(Eye.translation, Eye.translation + Eye.front, Eye.up),
        .proj = maths::perspective<LWorldReal,world_qual>(
            Eye.vert_fov,
            Info.VkViewport.width / Info.VkViewport.height,
            Eye.near_frustum, Eye.far_frustum
            ),
        };
    WorldData.proj[1][1] *= -1.0f;

    // Set up lights
    // Light 1: White light from above
    WorldData.lightPositions[0] = glm::vec4(0.0f, 5.0f, 5.0f, 1.0f);
    WorldData.lightColors[0] = glm::vec4(300.0f, 300.0f, 300.0f, 1.0f);

    // Light 2: Blue light from the left
    WorldData.lightPositions[1] = glm::vec4(-5.0f, 0.0f, 0.0f, 1.0f);
    // WorldData.lightColors[1] = glm::vec4(0.0f, 0.0f, 300.0f, 1.0f);
    WorldData.lightColors[1] = glm::vec4(300.0f, 300.0f, 300.0f, 1.0f);

    // Light 3: Red light from the right
    WorldData.lightPositions[2] = glm::vec4(5.0f, 0.0f, 0.0f, 1.0f);
    // WorldData.lightColors[2] = glm::vec4(300.0f, 0.0f, 0.0f, 1.0f);
    WorldData.lightColors[2] = glm::vec4(300.0f, 300.0f, 300.0f, 1.0f);

    // Light 4: Green light from behind
    WorldData.lightPositions[3] = glm::vec4(0.0f, -5.0f, 0.0f, 1.0f);
    // WorldData.lightColors[3] = glm::vec4(0.0f, 300.0f, 0.0f, 1.0f);
    WorldData.lightColors[3] = glm::vec4(300.0f, 300.0f, 300.0f, 1.0f);

    // Set camera position for view-dependent effects
    WorldData.CameraPosition = Eye.translation;

    WorldData.ViewportHeight = Info.VkViewport.height;
    WorldData.VerticalFov = Eye.vert_fov;

    // Set PBR parameters
    WorldData.exposure = 3.2f;//4.5f;
    WorldData.gamma = 1.3f;//2.2f;
    WorldData.prefilteredCubeMipLevels = 1.0f;
    WorldData.scaleIBLAmbient = 1.0f;

    ActorInfo.WorldDataDescriptorSet = this->Vk_GetWorldDataDescriptorSet(ActorInfo);

    auto& WorldDataBuffer{this->Vk_GetWorldDataBuffer(ActorInfo)};
    WorldData.upload(WorldDataBuffer);
    auto WorldDataWriteInfo{WorldData.write_info(*WorldDataBuffer)};
    std::array Writes{vk::WriteDescriptorSet{
        .dstSet = ActorInfo.WorldDataDescriptorSet,
        .dstBinding = 0, .dstArrayElement = 0, .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo = &WorldDataWriteInfo,
        }};
    Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

    ActorInfo.CommandBuffer.setPolygonModeEXT(ActorInfo.DefaultPerspectivePolygonMode);

    if (Filter)
    {
        check(!Filter->empty())
        for (auto* Actor: *Filter)
        {
            check(!Actor->_IsGarbage())
            for (auto const& Comp: Actor->GetComponents())
            {
                if (Comp->ShouldRender())
                {
                    Comp->Render(ActorInfo);
                }

                continue;
            }

            continue;
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

    if (this->Skybox && !!this->Skybox->GetSkyboxTexture() && !Filter)
    {
        this->Skybox->RenderSkybox(ActorInfo);
    }

    for (auto& F: ActorInfo.PostRenderDelegates)
    {
        F(ActorInfo);
    }

    return;
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
    if (algo::contains(this->TickableObjects, Tickable))
    {
        panic("Found duplicate tickable object")
    }

    this->TickableObjects.push_back(Tickable);

    return;
}

void Jafg::LWorld::UnregisterTickableObject(LTickableObject* Tickable)
{
    if (algo::erase_once(&this->TickableObjects, Tickable))
    {
        return;
    }
    panic("Failed to find tickable object")
}

f32 Jafg::LWorld::GetRealTimeSecondsSinceWorldLaunch() const noexcept
{
    return static_cast<f32>(App::GetElapsedTime()) - this->RealTimeWhenWorldWasLaunched;
}

bool Jafg::LWorld::LineTraceByChannel(
    TArray<LHitResult>* OutHits,
    LWorldVec3 const& Begin,
    LWorldVec3 const& End,
    ECollisionChannels Channels,
    LCollisionQueryParams const& Params
) const
{
    STAT_FUNCTION()
    check(OutHits )
    check(maths::magnitude(Begin - End) > static_cast<LWorldVec3::value_type>(maths::not_so_small_number_d) && "Why trace small distances.")

    /* TODO: Save (as the tickables) the physics in a separate cached vector. */
    // LHitResult Dummy;
    // for (auto& Obj : this->GetEmployees())
    // {
        // if (auto* Actor{ Obj->As<AActor>() }; Actor && Actor->GetPhysicsComponent()->Sweep(Begin, End, Dummy))
        // {
        //     OutHits.push_back(Dummy);
        //     Dummy.Reset();
        //
        //     if (Params.bSingleHit)
        //     {
        //         break;
        //     }
        // }

    //     continue;
    // }

    return OutHits->empty() == false;
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
                        if (auto Intersection{maths::aabb_intersect_ray(Ray, Comp.GetAabbForThisComponentOnly().apply(Comp.GetWorldTransformSlow()))}; Intersection.bHit)
                        {
                            Results.push_back({
                                .Actor = *Actor,
                                .Component = Comp,
                                .GlobalWorldLocation = Intersection.EnterPoint,
                                });
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
