// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "RHI/Shader.h"

class JWorldSubsystem;
class Camera;
class Planet;
struct LLevel;
struct GLFWwindow;

namespace EWorldState
{

enum Type : uint8
{
    None,
    Uninitialized,
    Initializing,
    Running,
    TearingDown,
    WaitingForKill,
};

}

/**
 * Represents a world at its core.
 * Once every frame a world will be ticked. It may register itself to the RHI to be used when
 * rendering on any kind of surface. Multiple worlds may draw to the same surface.
 */
class JWorld
{
public:

    JWorld()                                        = delete;
    JWorld(JWorld&)                                 = delete;
    JWorld(const JWorld&)                           = delete;
    JWorld(JWorld&&)                                = delete;
    FORCEINLINE JWorld& operator=(JWorld&)          = delete;
    FORCEINLINE JWorld& operator=(const JWorld&)    = delete;
    FORCEINLINE JWorld& operator=(JWorld&&)         = delete;

    explicit JWorld(const EWorldState::Type InWorldType)
        : WorldState(InWorldType)
    {
        check( this->WorldState != EWorldState::None )
    }

    FORCEINLINE EWorldState::Type GetWorldState() const { return this->WorldState; }

    void InitializeWorld(const LLevel& Level);

    FORCEINLINE bool CanTick() const { return this->GetWorldState() == EWorldState::Running; }
    void Tick(const float DeltaTime);

    void TearDownWorld();

    bool FirstTimeMouseScroll = true;
    bool Key_EscapeDown = false;
    bool bShowMouse = false;
    double LastMouseX = 0.0f;
    double LastMouseY = 0.0f;

    Camera* MainCamera = nullptr;
    Planet* MainPlanet = nullptr;

    Shader ShaderProgram = Shader();
    uint32 Texture       = 0;

    void MouseCallback(const double XPos, const double YPos);
    void ScrollCallback(const double YOffset);

private:

    void InitializeSubsystems();
    void TearDownSubsystems();

    std::vector<JWorldSubsystem*> Subsystems;

    EWorldState::Type WorldState;
};
