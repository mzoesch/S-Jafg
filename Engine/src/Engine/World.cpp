// Copyright mzoesch. All rights reserved.

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Core/App.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "Physics/PhysicCompontent.h"
#include "User/LocalEgo.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Stats/Stats.h"
#include "Framework/SupremePolicies.h"
#include "User/UserPreferences.h"
#include "Components/ActorComponentForward.h"
#include "Components/StaticMeshComponent.h"
#include "Framework/MaterialSubsystem.h"

LString Jafg::LWorldParameters::ToString() const
{
    LString Out { '[' };

    bool bFirst { true };
    for (const LWorldParam& Param : this->Params)
    {
        if (bFirst)
        {
            bFirst = false;
        }
        else
        {
            Out.append(", ");
        }

        Out.append(algo::sprintf("{}={}", Param.Key, Param.Value));
        continue;
    }

    Out += ']';
    return Out;
}

void Jafg::LWorldParameters::Reset() noexcept
{
    algo::orphan(&this->Params);
    return;
}

void Jafg::LWorld::InitializeWorld(std::optional<LLevel> const& Level /* = {} */, LString&& Url /* = {} */)
{
    STAT_CYCLE_FUNCTION()

    this->RealTimeWhenWorldWasLaunched = static_cast<f32>(App::GetElapsedTime());
    check(this->RealTimeWhenWorldWasLaunched > 0.0f)

    check(this->WorldState == EWorldState::PreInitializing)
    this->WorldState = EWorldState::Initializing;

    auto& Frontend{this->GetEngine().GetLocalEgo().GetFrontend()};

    this->UnsanitizedUrl = Url;

    /* Remove level name from url. */
    if (const auto Idx{Url.find('?')}; Idx != Url.npos)
    {
        if (algo::valid_index(Url, Idx + 1))
        {
            algo::inline_right_chop(&Url, Idx + 1);
        }
        else
        {
            algo::orphan(&Url);
        }
    }
    else
    {
        algo::orphan(&Url);
    }
    this->Url = std::move(Url);
    this->UpdateUrlParams();
    LOG_VERBOSE(LogWorld, "Initializing new world with [{}].", this->Parameters.ToString())

    LOG_TRACE(LogRhi, "Allocating world data descriptor sets.")
    auto& MaterialSubsystem{*Frontend.GetSubsystemChecked<JMaterialSubsystem>()};
    this->Vk_WorldDescriptorSets = Frontend.Vk_CreateFrequentDescriptorSets(MaterialSubsystem.GetSharedDescriptorSetLayout<UBO::WorldData>());
    this->Vk_WorldBuffers = Frontend.Vk_CreateFrequentMappedBuffer(UBO::WorldData::buffer_create_info());

    this->UnderlyingLevel = Level;

    if (this->UnderlyingLevel.has_value())
    {
        this->SupremePolicies = SpawnObject(CastTo<ASupremePolicies>{}, {*this, this->UnderlyingLevel->SupremePoliciesClass.GetClassOrDefault()});
    }
    else
    {
        this->SupremePolicies = SpawnObject(TWorldStaticInit<ASupremePolicies>{*this});
    }
    check(this->SupremePolicies)

    this->RealTimeWhenWorldStarted = static_cast<f32>(App::GetElapsedTime());
    check(this->RealTimeWhenWorldStarted >= this->RealTimeWhenWorldWasLaunched)

    if (auto& Track{Detail::GMutableEngine->GetTrackFromWorld(*this)}; Track.Callbacks.OnPreInit)
    {
        Track.Callbacks.OnPreInit(*this);
        algo::swap_default(&Track.Callbacks.OnPreInit);
    }
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

    this->GetMutableEngine().OnWorldBeginLife.Broadcast(this);
    this->WorldState = EWorldState::Running;

    if (auto& Track{Detail::GMutableEngine->GetTrackFromWorld(*this)}; Track.Callbacks.OnPostInit)
    {
        Track.Callbacks.OnPostInit(*this);
        algo::swap_default(&Track.Callbacks.OnPostInit);
    }
    this->SupremePolicies->OnWorldPostInit();

    return;
}

void Jafg::LWorld::Tick(f64 Dt)
{
    STAT_CYCLE_FUNCTION()

    this->DeltaTime = Dt;

    this->AcquireTickableObjectsLock();
    for (LTickableObject* Tickable : this->TickableObjects)
    {
        Tickable->Tick(static_cast<f32>(this->DeltaTime));
    }
    this->ReleaseTickableObjectsLock();
    for (LTickableObject* Tickable : this->DeletedTickableObjects)
    {
        algo::erase_once_checked(&this->TickableObjects, Tickable);
    }
    this->DeletedTickableObjects.clear();
}

void Jafg::LWorld::Draw(LRenderInfo const& Info, LWorldEye const& Eye, LMaterialInstance* Instance, algo::transparent_unordered_string_map<vk::DescriptorSet> SharedSets, std::optional<TArray<AActor*>> const& Filter) const
{
    STAT_CYCLE_FUNCTION()

    // LMatrix P{SkipInit};
    // LMatrix V{SkipInit};
    // if (auto Cache{this->EyeToMatrices.find(&Eye)}; this->GetLocalEgo().GetVariable_UpdateFrustum() || Cache == this->EyeToMatrices.end())
    // {
    //     P = Maths::MakePerspectiveProjectionMatrix(
    //         Maths::ToRadians(Eye.GetDegYFov()),
    //         static_cast<f32>(Info.Surface.GetDimensions().X) / static_cast<f32>(Info.Surface.GetDimensions().X),
    //         Eye.GetNearFrustum(), Eye.GetFarFrustum()
    //     );
    //     V = Eye.GetViewMatrix();
    //     this->EyeToMatrices[&Eye] = { P, V };
    // }
    // else
    // {
    //     P = Cache->second.P;
    //     V = Cache->second.V;
    // }

    // const LMatrix4 InversePV = (P * V).GetInverse();

    // LVector Corners[8];
    // constexpr LVector4 NdcCorners[8]
    // {
    //     {-1, -1, -1,  1}, /* Near Bottom Left */
    //     { 1, -1, -1,  1}, /* Near Bottom Right */
    //     {-1,  1, -1,  1}, /* Near Top Left */
    //     { 1,  1, -1,  1}, /* Near Top Right */
    //     {-1, -1,  1,  1}, /* Far Bottom Left */
    //     { 1, -1,  1,  1}, /* Far Bottom Right */
    //     {-1,  1,  1,  1}, /* Far Top Left */
    //     { 1,  1,  1,  1}  /* Far Top Right */
    // };
    // for (int i = 0; i < 8; i++)
    // {
    //     LVector4 WorldLocation = InversePV * NdcCorners[i];
    //     Corners[i] = WorldLocation.XYZ() / WorldLocation.W; /* Perspective divide */
    // }
    // std::vector<std::pair<int, int>> frustumEdges = {
    //     {0, 1}, {1, 3}, {3, 2}, {2, 0}, // Near Plane Edges
    //     {4, 5}, {5, 7}, {7, 6}, {6, 4}, // Far Plane Edges
    //     {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting Near and Far Planes
    // };
    // const std::span CornersSpan{Corners};

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
    // model...
    WorldData.view = glm::lookAtRH(Eye.translation, Eye.translation + Eye.front, Eye.up);
    WorldData = {
        .view = glm::lookAtRH(Eye.translation, Eye.translation + Eye.front, Eye.up),
        .proj = glm::perspectiveRH_ZO(
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
    WorldData.lightColors[1] = glm::vec4(0.0f, 0.0f, 300.0f, 1.0f);

    // Light 3: Red light from the right
    WorldData.lightPositions[2] = glm::vec4(5.0f, 0.0f, 0.0f, 1.0f);
    WorldData.lightColors[2] = glm::vec4(300.0f, 0.0f, 0.0f, 1.0f);

    // Light 4: Green light from behind
    WorldData.lightPositions[3] = glm::vec4(0.0f, -5.0f, 0.0f, 1.0f);
    WorldData.lightColors[3] = glm::vec4(0.0f, 300.0f, 0.0f, 1.0f);

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

            for (auto const& Comp: Actor->GetComponents())
            {
                if (Comp->ShouldRender())
                {
                    Comp->Render(ActorInfo);
                }

                continue;
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

    auto Pc{this->SupremePolicies->OnIncomingConnectionRequest(
        std::holds_alternative<LTransientPersona::Proxy>(*Persona) ? ASupremePolicies::Proxy : ASupremePolicies::Local
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
    STAT_CYCLE_FUNCTION()
    check(OutHits )
    check(maths::magnitude(Begin - End) > static_cast<LWorldVec3::value_type>(maths::not_so_small_number_d) && "Why trace small distances.")

    STAT_QUICK_CYCLE_START("LineTraceByChannelImpl")
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
    STAT_CYCLE_FUNCTION()

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
            for (auto& Comp: Actor->GetComponents())
            {
                if (auto* Sc{Comp->As<AStaticMeshComponent>()})
                {
                    if (auto r{maths::aabb_intersect_ray(Ray, Sc->GetAabb().apply(Sc->GetTransform()))}; r.bHit)
                    {
                        Results.push_back({
                            .Actor = *Actor,
                            .Component = *Sc,
                            .GlobalWorldLocation = r.EnterPoint,
                            });
                    }
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
    STAT_CYCLE_FUNCTION()

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

void Jafg::LWorld::UpdateUrlParams() noexcept
{
    this->Parameters.Reset();

    TArray<LString> Params;

    LString Current;
    char Last { 0 };
    for (const char& C : this->Url)
    {
        if (C == '?' && Last != '\\')
        {
            Params.emplace_back(std::move(Current));
        }

        Current += C;
        Last = C;
        continue;
    }

    if (Current.empty() == false)
    {
        Params.emplace_back(std::move(Current));
    }

    for (const LString& P : Params)
    {
        bool bAddToKey { true };
        LString K;
        LString V;
        Last = 0;

        /* This is not quite right here. '\' can give wrong resuls. */
        for (const char& C : P)
        {
            if (C == '\\')
            {
                if (bAddToKey)
                {
                    K += C;
                }
                else
                {
                    V += C;
                }

                Last = C;
                continue;
            }

            if (C == '=' && Last != '\\')
            {
                check( bAddToKey )
                bAddToKey = false;
                Last = C;
                continue;
            }

            if (bAddToKey)
            {
                K += C;
            }
            else
            {
                V += C;
            }

            continue;
        }

        check( K.empty() == false )

        this->Parameters.Params.emplace_back(std::move(K), std::move(V));

        continue;
    }

    return;
}
