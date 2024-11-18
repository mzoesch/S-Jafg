// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Core/Application.h"
#include "Engine/ActorUtility.h"
#include "Engine/Framework/Pawn.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "User/LocalEgo.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"

Jafg::LEngine* Jafg::LWorld::GetEngine() const
{
    return GEngine;
}

Jafg::LApplicationInstance* Jafg::LWorld::GetApplicationInstance() const
{
    return this->GetEngine()->GetApplicationInstance();
}

void Jafg::LWorld::InitializeWorld(const LLevel& Level)
{
    this->RealTimeWhenWorldWasLaunched = static_cast<float>(Application::GetDeltaSinceStaticStorageInitialization());
    check( this->RealTimeWhenWorldWasLaunched > 0.0f )

    this->WorldState = EWorldState::Initializing;

    this->GetEngine()->OnWorldBeginLife.Broadcast(this);

    for (AActor* Actor : this->Actors)
    {
        MakeDeferredActorFinal(Actor);
    }

    this->InitializeSubsystems();

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

    const LViewport* ViewportContext = this->GetEngine()->GetCheckedLocalEgo()->GetPrimarySurface()->GetViewport();
    check( ViewportContext )
    for (const AActor* Actor : this->Actors)
    {
        check( Actor->IsGarbage() == false )

        if (Actor->IsRendererComponentValid())
        {
            // And add eye here.
            Actor->GetRendererComponent()->Draw(*ViewportContext);
        }

        continue;
    }

    return;
}

void Jafg::LWorld::TearDownContext()
{
    check( this->GetWorldState() == EWorldState::Running )
    this->WorldState = EWorldState::TearingDown;

    this->TearDownSubsystems();

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

void Jafg::LWorld::InitializeSubsystems()
{
    check( this->Collection == nullptr )
    this->Collection = new LSubsystemCollection(this);
    this->Collection->LocateAllSubsystemsOfClass(JWorldSubsystem::StaticClass());
    this->Collection->InitializeSubsystems();

    return;
}

void Jafg::LWorld::TearDownSubsystems()
{
    this->Collection->TearDownSubsystems();
    delete this->Collection;
    this->Collection = nullptr;

    return;
}
