// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

struct GLFWwindow;

namespace Jafg
{

class LEngine;
class Shader;
class AActor;
class JWorldSubsystem;
class Camera;
struct LLevel;

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

} /* ~Namespace EWorldState */

MAKE_EXTERNAL_TEMPLATE_DHARRAY(ENGINE, ::Jafg::AActor*)

/**
 * Represents a world at its core.
 * Once every frame a world will be ticked. It may register itself to the RHI to be used when
 * rendering on any kind of surface. Multiple worlds may draw to the same surface.
 */
class ENGINE_API LWorld
{
public:

    LWorld()                                        = delete;

    PROHIBIT_REALLOC_OF_ANY_FROM(LWorld)

    // LWorld(LWorld&)                                 = delete;
    // LWorld(const LWorld&)                           = delete;
    // LWorld(LWorld&&)                                = delete;
    // FORCEINLINE LWorld& operator=(LWorld&)          = delete;
    // FORCEINLINE LWorld& operator=(const LWorld&)    = delete;
    // FORCEINLINE LWorld& operator=(LWorld&&)         = delete;

    explicit LWorld(const EWorldState::Type InWorldType)
        : WorldState(InWorldType)
    {
        check( this->WorldState != EWorldState::None )
    }

    LEngine* GetEngine() const;

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

    Shader* ShaderProgram = nullptr;
    uint32 Texture       = 0;

    void MouseCallback(const double XPos, const double YPos);
    void ScrollCallback(const double YOffset);

private:

    TdhArray<AActor*> Actors;

    void InitializeSubsystems();
    void TearDownSubsystems();

    std::vector<JWorldSubsystem*> Subsystems;

    EWorldState::Type WorldState;
};

} /* ~Namespace Jafg */
