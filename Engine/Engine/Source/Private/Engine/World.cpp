// Copyright mzoesch. All rights reserved.

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Core/Application.h"
#include "Engine/ActorUtility.h"
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

Jafg::LString Jafg::LWorldParameters::ToString() const
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
            Out.Append(", ");
        }

        Out.Append(LString::SprintF("{}={}", Param.Key, Param.Value));
        continue;
    }

    Out += ']';
    return Out;
}

void Jafg::LWorldParameters::Reset() noexcept
{
    this->Params.Empty();
    return;
}

Jafg::LWorld::LWorld(const LString& InHumanReadableName, const EWorldState::Type InWorldType) : WorldState(InWorldType)
{
    this->SetHumanReadableName(InHumanReadableName);
    check( this->WorldState == EWorldState::Uninitialized )

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

void Jafg::LWorld::InitializeWorld(const LLevel& Level, LString&& InLaunchedUrl)
{
    STAT_CYCLE_FUNCTION()

    this->RealTimeWhenWorldWasLaunched = static_cast<f32>(Application::GetDeltaSinceStaticStorageInitialization());
    check( this->RealTimeWhenWorldWasLaunched > 0.0f )

    check( this->WorldState == EWorldState::Uninitialized || this->WorldState == EWorldState::WaitingForKill )
    this->WorldState = EWorldState::Initializing;

    this->UnsanitizedUrl = InLaunchedUrl;

    /* Remove level name from url. */
    if (const i32 Idx { InLaunchedUrl.FindFirst('?') }; Idx != INDEX_NONE)
    {
        if (InLaunchedUrl.IsValidIndex(Idx + 1))
        {
            InLaunchedUrl.InlineRightChop(Idx + 1);
        }
        else
        {
            InLaunchedUrl.Empty();
        }
    }
    else
    {
        InLaunchedUrl.Empty();
    }
    this->Url = std::move(InLaunchedUrl);
    this->UpdateUrlParams();
    LOG_VERBOSE(LogWorld, "Initializing new world with [{}].", this->Parameters.ToString())

    this->DeferredInitialize(GlobalCarnifex);

    this->UnderlyingLevel = Level;

    if (this->UnderlyingLevel.IsValid())
    {
        this->bDrawSkyboxFirst = this->UnderlyingLevel->bDrawSkyboxFirst;

        if (this->UnderlyingLevel->bCreateSkybox)
        {
            this->Skybox = LSkybox{this->UnderlyingLevel->Skybox};
            this->Skybox->Upload();
        }
    }

    this->GetEngine()->OnWorldBeginLife.Broadcast(this);

    LOG_VERBOSE(LogWorld, "Initializing {} level actors.", this->Actors.GetSize())
    for (AActor* Actor : this->Actors)
    {
        MakeDeferredActorFinal(Actor);
    }

    this->Collection.DeferredInitialize(this);
    this->Collection.InitializeSubsystems(JWorldSubsystem::StaticClass());

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
        this->TickableObjects.RemoveOnceChecked(Tickable);
    }
    this->DeletedTickableObjects.Empty();

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

    if (this->Skybox.IsValid() && this->bDrawSkyboxFirst)
    {
        this->Skybox->Draw(Viewport, Eye);
    }

    this->OnStaticDraw.InvokeIfBound(Viewport, Eye, CornersSpan);

    for (const AActor* Actor : this->Actors)
    {
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

    for (LTemporalWorldObject* const& TemporalObject : this->TemporalObjects)
    {
        TemporalObject->Draw(*this, Viewport, Eye);
    }

    if (this->Skybox.IsValid() && this->bDrawSkyboxFirst == false)
    {
        this->Skybox->Draw(Viewport, Eye);
    }

    return;
}

#if AS_CLIENT
void Jafg::LWorld::LateTick(const float DeltaTime)
{
    STAT_CYCLE_FUNCTION()

    for (LTemporalWorldObject* const& TemporalObject : this->TemporalObjects)
    {
        TemporalObject->ReduceLifeTime(DeltaTime);
    }
    this->TemporalObjects.RemoveByPredicate([](const LTemporalWorldObject* const& TemporalObject) -> bool
    {
        if (TemporalObject->IsAlive())
        {
            return false;
        }

        delete TemporalObject;
        return true;
    });

    return;
}
#endif /* AS_CLIENT */

void Jafg::LWorld::TearDownContext()
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

    LOG_VERBOSE(LogWorld, "Killing {} actors of world [{}].", this->Actors.GetSize(), this->GetHumanReadableName())
    for (AActor* Actor : this->Actors)
    {
        Actor->MarkAsGarbage();
    }

    this->Actors.Empty();
    this->TickableObjects.Empty();
    this->DeletedTickableObjects.Empty();

    this->OnStaticDraw.Unbind();
    this->OnStaticLineTrace.Unbind();

    this->UnsanitizedUrl.Empty();
    this->Url.Empty();
    this->Parameters.Reset();

    this->UnderlyingLevel.Reset();

    ensureDiscard( this->TemporalObjects.IsEmpty() );
    this->TemporalObjects.Empty();

    ensureDiscard( this->TickableObjects.IsEmpty() );
    this->TickableObjects.Empty();
    ensureDiscard( this->DeletedTickableObjects.IsEmpty() );
    this->DeletedTickableObjects.Empty();

    this->bDrawSkyboxFirst = false;
    this->Skybox.Reset();

    this->EyeToMatrices.clear();

    this->Collection.Reset();

    check( this->TickableObjectsPutMutex == false )

    this->RealTimeWhenWorldWasLaunched = -1.0f;

    LObjectContext::TearDownContext();

    this->WorldState = EWorldState::WaitingForKill;

    return;
}

void Jafg::LWorld::RegisterTickableObject(LTickableObject* Tickable)
{
    if (this->TickableObjects.Contains(Tickable))
    {
        panic( "Found duplicate tickable object" )
        return;
    }

    this->TickableObjects.Add(Tickable);

    return;
}

void Jafg::LWorld::UnregisterTickableObject(LTickableObject* Tickable)
{
    if (this->TickableObjects.RemoveOnce(Tickable))
    {
        return;
    }

    panic( "Failed to find tickable object" )

    return;
}

f32 Jafg::LWorld::GetRealTimeSecondsSinceWorldLaunch() const
{
    const f32 Now = static_cast<f32>(Application::GetDeltaSinceStaticStorageInitialization());
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

    check( (Begin - End).Magnitude() > JAFG_NOT_SO_SMALL_NUMBER && "Why trace small distances." )

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
    LHitResult Dummy;
    for (const AActor* Actor : this->Actors)
    {
        if (Actor->GetPhysicsComponent()->Sweep(Begin, End, Dummy))
        {
            OutHits.Add(Dummy);
            Dummy.Reset();

            if (Params.bSingleHit)
            {
                break;
            }
        }

        continue;
    }

    return OutHits.IsEmpty() == false;
}

void Jafg::LWorld::UpdateUrlParams()
{
    this->Parameters.Reset();

    TArray<LString> Params;

    LString Current;
    char Last { 0 };
    for (const char& C : this->Url)
    {
        if (C == '?' && Last != '\\')
        {
            Params.Emplace(std::move(Current));
        }

        Current += C;
        Last = C;
        continue;
    }

    if (Current.IsEmpty() == false)
    {
        Params.Emplace(std::move(Current));
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

        check( K.IsEmpty() == false )

        this->Parameters.Params.Emplace(std::move(K), std::move(V));

        continue;
    }

    return;
}

Jafg::LWorld* Jafg::LWorld::GetWorldFromHumanReadableName(const LString& InHumanReadableName)
{
    if (GEngine)
    {
        const Private::LWorldContext* Context
        {
            GEngine->GetContexts().FindRefByPredicate([InHumanReadableName](const Private::LWorldContext& InContext) -> bool
            {
                return InContext.ChildWorld->GetHumanReadableName() == InHumanReadableName;
            })
        };

        return Context ? Context->ChildWorld : nullptr;
    }

    return nullptr;
}
