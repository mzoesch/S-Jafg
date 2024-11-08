// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/Framework/Camera.h"
#include "Platform/Surface.h"
#include <glm/gtc/type_ptr.inl>
#include "Engine/ActorUtility.h"
#include "JustTemp.h"
#include "Engine/Framework/PlayerController.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "Player/LocalPlayer.h"
#include "RhiFramework/Shader.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/Framework/Pawn.h"
#if PLATFORM_WINDOWS
    #include <Windows.h>
#endif /* PLATFORM_WINDOWS */

Jafg::LEngine* Jafg::LWorld::GetEngine() const
{
    return GEngine;
}

void Jafg::LWorld::InitializeWorld(const LLevel& Level)
{
    this->WorldState = EWorldState::Initializing;

    ShaderProgram = new LShader("Content/Shaders/Chunk.vert", "Content/Shaders/Chunk.frag");
    ShaderProgram->Use();
    ShaderProgram->SetFloatUniform("texMultiplier", 0.5f);

    JustTemp::A(&Texture);

    MainCamera = new Camera(LVector(0.0f , 0.0f, 25.0f));

    APlayerController* Pc = SpawnDeferredActor<APlayerController>(this);
    GEngine->GetCheckedLocalPlayer()->Possess(Pc);
    APawn* Pawn = SpawnDeferredActor<APawn>(this);
    Pc->Possess(Pawn);

    for (AActor* Actor : this->Actors)
    {
        MakeDeferredActorFinal(Actor);
    }

    this->InitializeSubsystems();

    this->WorldState = EWorldState::Running;
}

void Jafg::LWorld::Tick(const float DeltaTime)
{
    // glm::mat4 View = MainCamera->GetViewMatrix();

    LMatrix View = MainCamera->GetViewMatrix();

    ShaderProgram->Use();
    JustTemp::B(Texture);
    TIntVector2 WindowDimensions = GEngine->GetCheckedLocalPlayer()->GetPrimarySurface()->GetDimensions();
    JustTemp::C(MainCamera->Zoom, ShaderProgram, WindowDimensions, View);

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

    const LViewport* ViewportContext = this->GetEngine()->GetCheckedLocalPlayer()->GetPrimarySurface()->GetViewport();
    check( ViewportContext )
    for (const AActor* Actor : this->Actors)
    {
        check( Actor->IsGarbage() == false )

        if (Actor->HasRendererComponent())
        {
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

void Jafg::LWorld::MouseCallback(double XPos, double YPos)
{
    if (bShowMouse)
    {
        return;
    }

    if (FirstTimeMouseScroll)
    {
        LastMouseX = XPos;
        LastMouseY = YPos;
        FirstTimeMouseScroll = false;
    }

    const double XOffset = XPos - LastMouseX;
    const double YOffset = LastMouseY - YPos;

    LastMouseX = XPos;
    LastMouseY = YPos;

    MainCamera->ProcessMouseMovement(static_cast<float>(XOffset), static_cast<float>(YOffset));
}

void Jafg::LWorld::ScrollCallback(const double YOffset)
{
    MainCamera->ProcessMouseScroll(static_cast<float>(YOffset));
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
