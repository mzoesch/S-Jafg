// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "glad/glad.h" /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Engine/Framework/Camera.h"
#include "Platform/Surface.h"
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.inl>
#include "MyWorld/Chunk/ChunkGenerationSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#if PLATFORM_WINDOWS
    #include <Windows.h>
#endif /* PLATFORM_WINDOWS */

void Jafg::JWorld::InitializeWorld(const LLevel& Level)
{
    this->WorldState = EWorldState::Initializing;

    ShaderProgram = Shader("Content/Shaders/vertex_shader.shader", "Content/Shaders/fragment_shader.shader");
    ShaderProgram.Use();
    ShaderProgram.SetFloat("texMultiplier", 0.5f);

    glGenTextures(1, &Texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_set_flip_vertically_on_load(true);

    LString ExecPath(PLATFORM_MAX_PATH, '\0');
    GetModuleFileNameA(nullptr, ExecPath.data(), static_cast<uint32>(ExecPath.size()));
    ExecPath = ExecPath.substr(0, ExecPath.find_last_of('\\'));

    LString TexPath = ExecPath + '/' + "Content/Textures/Map.png";

    int32 Width, Height, NrChannels;
    uint8* Data = stbi_load(&TexPath[0], &Width, &Height, &NrChannels, 0);
    if (Data == nullptr)
    {
        GEngine->RequestEngineExit(EPlatformExit::Fatal, "Failed to load texture.");
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(Data);

    MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 25.0f));

    this->InitializeSubsystems();

    this->WorldState = EWorldState::Running;
}

void Jafg::JWorld::Tick(const float DeltaTime)
{
    glm::mat4 View = MainCamera->GetViewMatrix();

    ShaderProgram.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);

    TIntVector2 WindowDimensions = GEngine->GetSurface()->GetDimensions();

    glm::mat4 Projection = glm::perspective(glm::radians(MainCamera->Zoom),
        static_cast<float>(WindowDimensions.X) / static_cast<float>(WindowDimensions.Y), 0.1f, 2000.0f); // change clipping here
    const int32 ViewLoc = glGetUniformLocation(ShaderProgram.ID, "view");
    glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, glm::value_ptr(View));
    const int32 ProjectionLoc = glGetUniformLocation(ShaderProgram.ID, "projection");
    glUniformMatrix4fv(ProjectionLoc, 1, GL_FALSE, glm::value_ptr(Projection));

    for (JWorldSubsystem* const& Subsystem : this->Subsystems)
    {
        if (LTickableObject* Tickable = dynamic_cast<LTickableObject*>(Subsystem); Tickable)
        {
            Tickable->Tick(DeltaTime);
        }
    }

    // std::cout << "Cam: " << MainCamera->Position.x << ", " << MainCamera->Position.y << ", " << MainCamera->Position.z <<
    //     '\n';
    // std::cout.flush();

    return;
}

void Jafg::JWorld::TearDownWorld()
{
    check( this->GetWorldState() == EWorldState::Running )

    this->WorldState = EWorldState::TearingDown;

    this->TearDownSubsystems();

    this->WorldState = EWorldState::WaitingForKill;

    return;
}

void Jafg::JWorld::MouseCallback(double XPos, double YPos)
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

void Jafg::JWorld::ScrollCallback(const double YOffset)
{
    MainCamera->ProcessMouseScroll(static_cast<float>(YOffset));
}

void Jafg::JWorld::InitializeSubsystems()
{
    JChunkGenerationSubsystem* ChunkGenerationSubsystem = new JChunkGenerationSubsystem();
    this->Subsystems.emplace_back(ChunkGenerationSubsystem);
    LSubsystemCollection Collection;
    ChunkGenerationSubsystem->Initialize(Collection);

    return;
}

void Jafg::JWorld::TearDownSubsystems()
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
