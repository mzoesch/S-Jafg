// Copyright mzoesch. All rights reserved.

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Core/Application.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "Physics/PhysicCompontent.h"
#include "User/LocalEgo.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Debug/DebugTraceLine.h"
#include "Debug/DebugTraceSphere.h"
#include "Stats/Stats.h"

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

        Out.append(Lal::SprintF("{}={}", Param.Key, Param.Value));
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

Jafg::LEngine* Jafg::LWorld::GetEngine() const
{
    check( GEngine )
    return GEngine;
}

Jafg::LLocalEgo* Jafg::LWorld::GetLocalEgo() const
{
    return this->GetEngine()->GetLocalEgo();
}

Jafg::APersonaController* Jafg::LWorld::GetLocalController() const
{
    return this->GetLocalEgo()->GetPossessed();
}

Jafg::LCommandLineInterface* Jafg::LWorld::GetCommandLineInterface() const
{
    return this->GetEngine()->GetCommandLineInterface();
}

Jafg::APawn* Jafg::LWorld::GetLocalPawn() const
{
    if (APawn* Out = this->GetLocalController()->GetPossessed(); Out && Out->GetOuter() == this)
    {
        return Out;
    }

    return nullptr;
}

void Jafg::LWorld::InitializeWorld(TOptional<LLevel> const& Level /* = {} */, LString&& Url /* = {} */)
{
    STAT_CYCLE_FUNCTION()

    this->RealTimeWhenWorldWasLaunched = static_cast<f32>(Application::GetDeltaSinceStaticStorageInitialization());
    check( this->RealTimeWhenWorldWasLaunched > 0.0f )

    check( this->WorldState == EWorldState::PreInitializing )
    this->WorldState = EWorldState::Initializing;

    this->UnsanitizedUrl = Url;

    /* Remove level name from url. */
    if (const auto Idx { Url.find('?') }; Idx != Url.npos)
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
        this->bDrawSkyboxFirst = this->UnderlyingLevel->bDrawSkyboxFirst;

        if (this->UnderlyingLevel->bCreateSkybox)
        {
            this->Skybox = LSkybox{this->UnderlyingLevel->Skybox};
            this->Skybox->Upload();
        }
    }

    this->GetEngine()->OnWorldBeginLife.Broadcast(this);

    LOG_VERBOSE(LogWorld, "Initializing level actors.")
#if !IN_SHIPPING
    LSize ActorCount{ 0 };
#endif /* !IN_SHIPPING */
    for (auto& Obj : this->GetEmployees())
    {
        if (Obj->IsA<AActor>())
        {
            MakeDeferredActorFinal(StaticCast<AActor>(Obj.get()));
        }

        continue;
    }
#if !IN_SHIPPING
    LOG_VERBOSE(LogWorld, "Initialized [{}] actors.", ActorCount)
#endif /* !IN_SHIPPING */

    this->Collection.InitializeDeferred(this);
    this->Collection.InitializeSubsystems<JWorldSubsystem>();

    this->WorldState = EWorldState::Running;

    return;
}

void Jafg::LWorld::Tick(const f32 DeltaTime)
{
    STAT_CYCLE_FUNCTION()

    this->AcquireTickableObjectsLock();
    for (LTickableObject* Tickable : this->TickableObjects)
    {
        Tickable->Tick(DeltaTime);
    }
    this->ReleaseTickableObjectsLock();
    for (LTickableObject* Tickable : this->DeletedTickableObjects)
    {
        algo::erase_once_checked(&this->TickableObjects, Tickable);
    }
    algo::orphan(&this->DeletedTickableObjects);

    return;
}

void Jafg::LWorld::Draw(const LViewport& Viewport, const LEye& Eye) const
{
    STAT_CYCLE_FUNCTION()

    LMatrix P{SkipInit};
    LMatrix V{SkipInit};
    if (const auto Cache = this->EyeToMatrices.find(&Eye); this->GetLocalEgo()->GetVariable_UpdateFrustum() || Cache == this->EyeToMatrices.end())
    {
        P = Maths::MakePerspectiveProjectionMatrix(
            Maths::ToRadians(Eye.GetDegYFov()),
            static_cast<float>(Viewport.GetDimensions().X) / static_cast<float>(Viewport.GetDimensions().Y),
            Eye.GetNearFrustum(), Eye.GetFarFrustum()
        );
        V = Eye.GetViewMatrix();
        this->EyeToMatrices[&Eye] = { P, V };
    }
    else
    {
        P = Cache->second.P;
        V = Cache->second.V;
    }

    const LMatrix4 InversePV = (P * V).GetInverse();

    LVector Corners[8];
    constexpr LVector4 NdcCorners[8]
    {
        {-1, -1, -1,  1}, /* Near Bottom Left */
        { 1, -1, -1,  1}, /* Near Bottom Right */
        {-1,  1, -1,  1}, /* Near Top Left */
        { 1,  1, -1,  1}, /* Near Top Right */
        {-1, -1,  1,  1}, /* Far Bottom Left */
        { 1, -1,  1,  1}, /* Far Bottom Right */
        {-1,  1,  1,  1}, /* Far Top Left */
        { 1,  1,  1,  1}  /* Far Top Right */
    };
    for (int i = 0; i < 8; i++)
    {
        LVector4 WorldLocation = InversePV * NdcCorners[i];
        Corners[i] = WorldLocation.XYZ() / WorldLocation.W; /* Perspective divide */
    }
    // std::vector<std::pair<int, int>> frustumEdges = {
    //     {0, 1}, {1, 3}, {3, 2}, {2, 0}, // Near Plane Edges
    //     {4, 5}, {5, 7}, {7, 6}, {6, 4}, // Far Plane Edges
    //     {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting Near and Far Planes
    // };
    const std::span CornersSpan{Corners};

    if (this->Skybox.has_value() && this->bDrawSkyboxFirst)
    {
        this->Skybox->Draw(Viewport, Eye);
    }

    this->OnStaticDraw.InvokeIfBound(Viewport, Eye, CornersSpan);

    for (auto& Obj : this->GetEmployees())
    {
        AActor* Actor{ Obj->As<AActor>() };
        if (Actor == nullptr)
        {
            continue;
        }

        check( Actor->IsGarbage() == false )

        if
        (
               Actor->IsRendererComponentValid()
            && Actor->GetRendererComponent()->Cull(CornersSpan) == false
        )
        {
            Actor->GetRendererComponent()->Draw(Viewport, Eye);
        }

        continue;
    }

    if (this->GetLocalEgo()->GetVariable_VisualizeFrustum())
    {
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(Corners[0].X, Corners[0].Y, Corners[0].Z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(Corners[1].X, Corners[1].Y, Corners[1].Z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(Corners[2].X, Corners[2].Y, Corners[2].Z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(Corners[3].X, Corners[3].Y, Corners[3].Z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(Corners[4].X, Corners[4].Y, Corners[4].Z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(Corners[5].X, Corners[5].Y, Corners[5].Z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(Corners[6].X, Corners[6].Y, Corners[6].Z)
        ));
        const_cast<LWorld*>(this)->AddTemporalObject(LDebugTraceSphere(
            LTemporalWorldObject::DrawOnce,
            LVector(Corners[7].X, Corners[7].Y, Corners[7].Z)
        ));
    }

    for (auto& TemporalObject : this->TemporalObjects)
    {
        TemporalObject->Draw(*this, Viewport, Eye);
    }

    if (this->Skybox.has_value() && this->bDrawSkyboxFirst == false)
    {
        this->Skybox->Draw(Viewport, Eye);
    }

    return;
}

#if AS_CLIENT
void Jafg::LWorld::LateTick(const float DeltaTime)
{
    STAT_CYCLE_FUNCTION()

    for (auto& TemporalObject : this->TemporalObjects)
    {
        TemporalObject->ReduceLifeTime(DeltaTime);
    }

    algo::erase_if(&this->TemporalObjects, [](auto& E){ return E->IsAlive() == false; });

    return;
}
#endif /* AS_CLIENT */

void Jafg::LWorld::OnTearDown()
{
    STAT_CYCLE_FUNCTION()

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
    LSize ActorCount{ 0 };
#endif /* !IN_SHIPPING */
    for (TUnique<JCxxClass> const& Obj : this->GetEmployees())
    {
        check( Obj.get() != nullptr )

        if (Obj->IsA<AActor>())
        {
            Obj->MarkAsGarbage_v2(ECxxRecordTearDownReason::OuterTearDown);
#if !IN_SHIPPING
            ++ActorCount;
#endif /* !IN_SHIPPING */

            check( Obj.get() == nullptr )
        }

        continue;
    }

    algo::orphan(&this->TickableObjects);
    algo::orphan(&this->DeletedTickableObjects);

    this->OnStaticDraw.Unbind();
    this->OnStaticLineTrace.Unbind();

    algo::orphan(&this->UnsanitizedUrl);
    algo::orphan(&this->Url);
    this->Parameters.Reset();

    this->UnderlyingLevel.reset();

    ensureDiscard( this->TemporalObjects.empty() );
    algo::orphan(&this->TemporalObjects);

    ensureDiscard( this->TickableObjects.empty() );
    algo::orphan(&this->TickableObjects);
    ensureDiscard( this->DeletedTickableObjects.empty() );
    algo::orphan(&this->DeletedTickableObjects);

    this->bDrawSkyboxFirst = false;
    this->Skybox.reset();

    this->EyeToMatrices.clear();

    check( this->Collection.IsValid() == false )

    check( this->TickableObjectsPutMutex == false )

    this->RealTimeWhenWorldWasLaunched = -1.0f;

    LClassOuter::OnTearDown();

    this->WorldState = EWorldState::WaitingForKill;

    return;
}

void Jafg::LWorld::RegisterTickableObject(LTickableObject* Tickable)
{
    if (algo::contains(this->TickableObjects, Tickable))
    {
        panic( "Found duplicate tickable object" )
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

    panic( "Failed to find tickable object" )

    return;
}

f32 Jafg::LWorld::GetRealTimeSecondsSinceWorldLaunch() const noexcept
{
    const f32 Now { static_cast<f32>(Application::GetDeltaSinceStaticStorageInitialization()) };
    return Now - this->RealTimeWhenWorldWasLaunched;
}

bool Jafg::LWorld::LineTraceByChannel(
    TArray<LHitResult>& OutHits,
    const LVector& Begin,
    const LVector& End,
    const ECollisionChannel::Type Channel,
    const LCollisionQueryParams& Params
) const
{
    STAT_CYCLE_FUNCTION()

    check( (Begin - End).Magnitude() > LAL_NOT_SO_SMALL_NUMBER && "Why trace small distances." )

    if (Channel == ECollisionChannel::Static)
    {
        if (this->OnStaticLineTrace.IsBound())
        {
            STAT_QUICK_CYCLE_START("OnStaticLineTrace")
            return this->OnStaticLineTrace(OutHits, Begin, End, Params);
        }

        return false;
    }

    STAT_QUICK_CYCLE_START("LineTraceByChannelImpl")
    /* TODO: Save (as the tickables) the physics in a separate cached vector. */
    LHitResult Dummy;
    for (auto& Obj : this->GetEmployees())
    {
        if (auto* Actor{ Obj->As<AActor>() }; Actor && Actor->GetPhysicsComponent()->Sweep(Begin, End, Dummy))
        {
            OutHits.push_back(Dummy);
            Dummy.Reset();

            if (Params.bSingleHit)
            {
                break;
            }
        }

        continue;
    }

    return OutHits.empty() == false;
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

Jafg::LWorld* Jafg::LWorld::GetWorldFromHumanReadableName(const LString& InHumanReadableName) noexcept
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
