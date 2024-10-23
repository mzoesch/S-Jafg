// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/Framework/Camera.h"
#include "Platform/Surface.h"
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.inl>
#include "JustTemp.h"
#include "Engine/Framework/PlayerController.h"
#include "MyWorld/Chunk/ChunkGenerationSubsystem.h"
#include "Player/LocalPlayer.h"
#include "RhiFramework/Shader.h"
#include "Subsystems/WorldSubsystem.h"
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

    ShaderProgram = new Shader("Content/Shaders/vertex_shader.shader", "Content/Shaders/fragment_shader.shader");
    ShaderProgram->Use();
    ShaderProgram->SetFloat("texMultiplier", 0.5f);

    JustTemp::A(&Texture);

    MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 25.0f));

    APlayerController* Pc = new APlayerController(this);
    this->Actors.Add(Pc);
    GEngine->GetCheckedLocalPlayer()->Possess(Pc);

    for (AActor* Actor : this->Actors)
    {
        Actor->BeginLife();
    }

    this->InitializeSubsystems();

    this->WorldState = EWorldState::Running;
}

void Jafg::LWorld::Tick(const float DeltaTime)
{
    glm::mat4 View = MainCamera->GetViewMatrix();

    ShaderProgram->Use();

    JustTemp::B(Texture);

    TIntVector2 WindowDimensions = GEngine->GetCheckedLocalPlayer()->GetPrimarySurface()->GetDimensions();

    JustTemp::C(MainCamera->Zoom, ShaderProgram, WindowDimensions, View);

    // glm::mat4 Projection = glm::perspective(glm::radians(MainCamera->Zoom),
    //     static_cast<float>(WindowDimensions.X) / static_cast<float>(WindowDimensions.Y), 0.1f, 2000.0f); // change clipping here
    // const int32 ViewLoc = glGetUniformLocation(ShaderProgram->ID, "view");
    // glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(View));
    // const int32 ProjectionLoc = glGetUniformLocation(ShaderProgram->ID, "projection");
    // glUniformMatrix4fv(ProjectionLoc, 1, GL_FALSE, glm::value_ptr(Projection));

    for (AActor* Actor : this->Actors)
    {
        Actor->Tick(DeltaTime);
    }

    for (JWorldSubsystem* const& Subsystem : this->Subsystems)
    {
        if (LTickableObject* Tickable = dynamic_cast<LTickableObject*>(Subsystem); Tickable)
        {
            Tickable->Tick(DeltaTime);
        }
    }

    return;
}

void Jafg::LWorld::TearDownWorld()
{
    check( this->GetWorldState() == EWorldState::Running )

    this->WorldState = EWorldState::TearingDown;

    for (AActor* Actor : this->Actors)
    {
        Actor->EndLife();
    }
    for (const AActor* Actor : this->Actors)
    {
        delete Actor;
    }
    this->Actors.Reset(0);

    this->TearDownSubsystems();

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

void Jafg::LWorld::InitializeSubsystems()
{
    JChunkGenerationSubsystem* ChunkGenerationSubsystem = new JChunkGenerationSubsystem(this);
    this->Subsystems.emplace_back(ChunkGenerationSubsystem);
    LSubsystemCollection Collection;
    ChunkGenerationSubsystem->Initialize(Collection);

    return;
}

void Jafg::LWorld::TearDownSubsystems()
{
    for (JWorldSubsystem* const& Subsystem : this->Subsystems)
    {
        Subsystem->TearDown();
    }

    for (const JWorldSubsystem* const& Subsystem : this->Subsystems)
    {
        delete Subsystem;
    }

    this->Subsystems.clear();

    return;
}
