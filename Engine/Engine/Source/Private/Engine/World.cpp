// Copyright mzoesch. All rights reserved.

#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Core/Application.h"
#include "Engine/ActorUtility.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "Physics/PhysicCompontent.h"
#include "User/LocalEgo.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Debug/DebugTraceLine.h"

Jafg::LWorld::LWorld(const LSimpleString& InHumanReadableName, const EWorldState::Type InWorldType): WorldState(InWorldType)
{
    LObjectContext::operator=(GlobalCarnifex);
    this->SetHumanReadableName(InHumanReadableName);
    check( this->WorldState != EWorldState::None )

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

Jafg::APawn* Jafg::LWorld::GetLocalPawn() const
{
    APawn* Out = this->GetLocalController()->GetPossessed();

    if (Out && Out->GetOuter() == this)
    {
        return Out;
    }

    return nullptr;
}

void Jafg::LWorld::InitializeWorld(const LLevel& Level)
{
    this->RealTimeWhenWorldWasLaunched = static_cast<float>(Application::GetDeltaSinceStaticStorageInitialization());
    check( this->RealTimeWhenWorldWasLaunched > 0.0f )

    this->WorldState = EWorldState::Initializing;

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

void Jafg::LWorld::Tick(const float DeltaTime)
{
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
    for (const AActor* Actor : this->Actors)
    {
        check( Actor->IsGarbage() == false )

        if (Actor->IsRendererComponentValid())
        {
            Actor->GetRendererComponent()->Draw(Viewport, Eye);
        }

        continue;
    }

    for (LTemporalWorldObject* const& TemporalObject : this->TemporalObjects)
    {
        TemporalObject->Draw(*this, Viewport, Eye);
    }

    return;
}

#if AS_CLIENT
void Jafg::LWorld::LateTick(const float DeltaTime)
{
    for (LTemporalWorldObject* const& TemporalObject : this->TemporalObjects)
    {
        TemporalObject->ReduceLifeTime(DeltaTime);
    }
    this->TemporalObjects.RemoveAllByPredicate( [] (LTemporalWorldObject*& TemporalObject)
    {
        if (TemporalObject->IsAlive())
        {
            return false;
        }

        delete TemporalObject;
        TemporalObject = nullptr;
        return true;
    });

    return;
}
#endif /* AS_CLIENT */

void Jafg::LWorld::TearDownContext()
{
    check( this->GetWorldState() == EWorldState::Running )
    this->WorldState = EWorldState::TearingDown;

    LOG_VERBOSE(LogWorld, "Tearing down world subsystems for world [{}].", this->GetHumanReadableName())
    this->Collection.TearDownSubsystems();

    // Preserve order!
    Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::Early, Tasks::Private::RunAllTasks);
    Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::Late, Tasks::Private::RunAllTasks);
    Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::Whenever, Tasks::Private::RunAllTasks);

    LOG_VERBOSE(LogWorld, "Killing {} actors of world [{}].", this->Actors.GetSize(), this->GetHumanReadableName())
    for (AActor* Actor : this->Actors)
    {
        Actor->KillYourSelfNow();
    }

    this->Actors.Empty();
    this->TickableObjects.Empty();
    this->DeletedTickableObjects.Empty();

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

float Jafg::LWorld::GetRealTimeSecondsSinceWorldLaunch() const
{
    const float Now = static_cast<float>(Application::GetDeltaSinceStaticStorageInitialization());
    return Now - this->RealTimeWhenWorldWasLaunched;
}

bool Jafg::LWorld::LineTraceByChannel(
    TdhArray<LHitResult>& OutHits,
    const LVector& Begin,
    const LVector& End,
    const ECollisionChannel::Type Channel,
    const LCollisionQueryParams& Params
) const
{
    check( (Begin - End).Magnitude() > JAFG_NOT_SO_SMALL_NUMBER && "Why trace small distances." )

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
