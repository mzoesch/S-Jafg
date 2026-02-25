// Copyright mzoesch. All rights reserved.

#include "Engine/World.h"
#include "Engine/Carnifex.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Core/Application.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "Physics/PhysicCompontent.h"
#include "User/LocalEgo.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Stats/Stats.h"
#include "Framework/SupremePolicies.h"
#include "Components/RenderComponent.h"

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

        Out.append(Jafg::SprintF("{}={}", Param.Key, Param.Value));
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

void Jafg::LWorld::InitializeWorld(TOptional<LLevel> const& Level /* = {} */, LString&& Url /* = {} */)
{
    STAT_CYCLE_FUNCTION()

    this->RealTimeWhenWorldWasLaunched = static_cast<f32>(Application::GetDeltaSinceStaticStorageInitialization());
    check(this->RealTimeWhenWorldWasLaunched > 0.0f)

    check(this->WorldState == EWorldState::PreInitializing)
    this->WorldState = EWorldState::Initializing;

    this->UnsanitizedUrl = Url;

    /* Remove level name from url. */
    if (const auto Idx{Url.find('?')}; Idx != Url.npos)
    {
        if (algo::is_valid_index(Url, Idx + 1))
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

    this->RealTimeWhenWorldStarted = static_cast<f32>(Application::GetDeltaSinceStaticStorageInitialization());
    check(this->RealTimeWhenWorldStarted >= this->RealTimeWhenWorldWasLaunched)

    if (auto& Track{GEngine->GetTrackFromWorld(this)}; Track.Callbacks.OnWorldPreInit.IsValid())
    {
        Track.Callbacks.OnWorldPreInit(*this);
        Track.Callbacks.OnWorldPreInit.Reset();
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

    this->GetEngine().OnWorldBeginLife.Broadcast(this);
    this->WorldState = EWorldState::Running;

    if (auto& Track{GEngine->GetTrackFromWorld(this)}; Track.Callbacks.OnWorldPostInit.IsValid())
    {
        Track.Callbacks.OnWorldPostInit(*this);
        Track.Callbacks.OnWorldPostInit.Reset();
    }
    this->SupremePolicies->OnWorldPostInit();

    return;
}

Jafg::LEngine& Jafg::LWorld::GetEngine() const noexcept
{
    check(GEngine && "Absence of GEngine when a world exists is undefined behavior.")
    return *GEngine;
}

Jafg::LCommandLineInterface& Jafg::LWorld::GetCommandLineInterface() const noexcept
{
    check(GEngine && "Absence of GEngine when a world exists is undefined behavior.")
    return GEngine->GetCommandLineInterface();
}

Jafg::LLocalEgo& Jafg::LWorld::GetLocalEgo() const noexcept
{
    check(GEngine && "Absence of GEngine when a world exists is undefined behavior.")
    return GEngine->GetLocalEgo();
}

void Jafg::LWorld::Tick(const f32 Dt)
{
    STAT_CYCLE_FUNCTION()

    this->AcquireTickableObjectsLock();
    for (LTickableObject* Tickable : this->TickableObjects)
    {
        Tickable->Tick(Dt);
    }
    this->ReleaseTickableObjectsLock();
    for (LTickableObject* Tickable : this->DeletedTickableObjects)
    {
        algo::erase_once_checked(&this->TickableObjects, Tickable);
    }
    algo::orphan(&this->DeletedTickableObjects);

    return;
}

void Jafg::LWorld::Draw(LRenderInfo const& Info) const
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

    for (auto& Obj : this->GetEmployees())
    {
        if (Obj->IsA<AActor>() == false)
        {
            continue;
        }

        AActor const* Actor{StaticCastChecked<AActor>(&*Obj)};
        check( Actor->_IsGarbage() == false )

        for (auto const& Comp : Actor->GetComponents())
        {
            if (Comp->ShouldRender())
            {
                Comp->Render(Info);
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

    return;
}

Jafg::APersonaController* Jafg::LWorld::Login(LTransientPersona Persona, LString* OutRejectionReason /* = nullptr */)
{
    check(this->SupremePolicies)
    check(this->GetWorldState() == EWorldState::Running)

    checkCode
    (
        if (Persona.Type == EIncomingConnectionRequest::Local)
        {
            check(Persona.Surface)
        }
    )

    if (Persona.Surface)
    {
        if (Persona.Surface->IsOwnedControllerValid())
        {
            LOG_WARNING(LogWorld,
                "Surface [{}] already possesses persona controller [{}]. Rejecting login request.",
                Persona.Surface->GetHumanReadableName(),
                Persona.Surface->GetOwnedControllerChecked()->GetNameAsString()
                )

            if (OutRejectionReason)
            {
                *OutRejectionReason = Jafg::SprintF(
                    "Surface [{}] already possesses persona controller [{}].",
                    Persona.Surface->GetHumanReadableName(),
                    Persona.Surface->GetOwnedControllerChecked()->GetNameAsString()
                    );
            }

            return nullptr;
        }
    }

    auto* Pc{this->SupremePolicies->OnIncomingConnectionRequest(Persona.Type, OutRejectionReason)};
    if (Pc == nullptr)
    {
        return nullptr;
    }

    if (Persona.Type == EIncomingConnectionRequest::Local)
    {
        //# Sideeffect from creation, we do not really care.
        if (Persona.Surface->IsOwnedControllerValid())
        {
            check(Persona.Surface->GetOwnedControllerChecked() == Pc)
            check(Pc->IsOwningSurfaceValid())
        }
        else
        {
            Persona.Surface->PossessController(Pc);
        }
    }
    else if (Persona.Type == EIncomingConnectionRequest::Remote)
    {
        jassertNoEntry()
    }
    else
    {
        jassertNoEntry()
    }

    this->SupremePolicies->OnPersonaControllerCreated(*Pc);

    return Pc;
}

void Jafg::LWorld::RegisterTickableObject(LTickableObject* Tickable)
{
    if (algo::contains(this->TickableObjects, Tickable))
    {
        panic("Found duplicate tickable object")
        return;
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
    return;
}

f32 Jafg::LWorld::GetRealTimeSecondsSinceWorldLaunch() const noexcept
{
    const f32 Now { static_cast<f32>(Application::GetDeltaSinceStaticStorageInitialization()) };
    return Now - this->RealTimeWhenWorldWasLaunched;
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
    check( OutHits )
    check( maths::magnitude(Begin - End) > static_cast<LWorldVec3::value_type>(maths::not_so_small_number_d) && "Why trace small distances." )

    STAT_QUICK_CYCLE_START("LineTraceByChannelImpl")
    /* TODO: Save (as the tickables) the physics in a separate cached vector. */
    LHitResult Dummy;
    for (auto& Obj : this->GetEmployees())
    {
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

        continue;
    }

    return OutHits->empty() == false;
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

Jafg::APersonaController* Jafg::LWorld::GetThisWorldsLocalPersonaControllerSlow() noexcept
{
    for (auto& Surface : this->GetLocalEgo().GetFrontend().GetSurfaces())
    {
        if (auto* Ctrl{Surface->GetOwnedController()})
        {
            if (&Ctrl->GetWorld() == this)
            {
                return Ctrl;
            }
        }
    }

    return nullptr;
}

Jafg::APersonaController const* Jafg::LWorld::GetThisWorldsLocalPersonaControllerSlow() const noexcept
{
    for (auto& Surface : this->GetLocalEgo().GetFrontend().GetSurfaces())
    {
        if (auto* Ctrl{Surface->GetOwnedController()})
        {
            if (&Ctrl->GetWorld() == this)
            {
                return Ctrl;
            }
        }
    }

    return nullptr;
}

void Jafg::LWorld::OnTearDown()
{
    STAT_CYCLE_FUNCTION()

    if (this->WorldState == EWorldState::PreInitializing)
    {
        check( this->Collection.GetSubsystems().size() == 0 )
        check( this->TickableObjects.size() == 0 )
        check( this->DeletedTickableObjects.size() == 0 )
        LClassOuter::OnTearDown();
        this->WorldState = EWorldState::WaitingForKill;
        return;
    }

    check( this->GetWorldState() == EWorldState::Running )
    this->WorldState = EWorldState::TearingDown;

    LOG_VERBOSE(LogWorld, "Tearing down world subsystems for world [{}].", this->GetHumanReadableName())
    this->Collection.TearDownSubsystems([](void) -> void
    {
        Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Early, Tasks::RunAllTasks);
        Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Late, Tasks::RunAllTasks);
        Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Whenever, Tasks::RunAllTasks);

        return;
    });

    // Preserve order!
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Early, Tasks::RunAllTasks);
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Late, Tasks::RunAllTasks);
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Whenever, Tasks::RunAllTasks);

    LOG_VERBOSE(LogWorld, "Killing actors of world [{}].", this->GetHumanReadableName())
#if !IN_SHIPPING
    LSize ActorCount{};
#endif /* !IN_SHIPPING */
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
            Obj->MarkAsGarbage_v2(ECxxRecordTearDownReason::OuterTearDown);
#if !IN_SHIPPING
            ++ActorCount;
#endif /* !IN_SHIPPING */
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

    algo::orphan(&this->TickableObjects);
    algo::orphan(&this->DeletedTickableObjects);

    algo::orphan(&this->UnsanitizedUrl);
    algo::orphan(&this->Url);
    this->Parameters.Reset();

    this->UnderlyingLevel.reset();

    algo::orphan(&this->TickableObjects);
    ensureDiscard(this->DeletedTickableObjects.empty());
    algo::orphan(&this->DeletedTickableObjects);

    check(this->Collection.IsOuterValid() == false && this->Collection.IsClassValid() == false)

    check(this->TickableObjectsPutMutex == false)

    this->RealTimeWhenWorldWasLaunched = -1.0f;
    this->RealTimeWhenWorldStarted = -1.0f;

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
