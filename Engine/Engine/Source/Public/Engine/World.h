// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/ObjectContext.h"
#include "Engine/Actor.h"

struct GLFWwindow;

namespace Jafg
{

class LEngine;
class LShader;
class JWorldSubsystem;
class Camera;
class LTickableObject;
struct LLevel;
struct LSubsystemCollection;

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
class ENGINE_API LWorld final : public ::Jafg::Private::LObjectContext
{
public:

    LWorld() = delete;
    PROHIBIT_REALLOC_OF_ANY_FROM(LWorld)
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

    virtual void TearDownContext() override;

    bool FirstTimeMouseScroll = true;
    bool Key_EscapeDown = false;
    bool bShowMouse = false;
    double LastMouseX = 0.0f;
    double LastMouseY = 0.0f;

    Camera* MainCamera = nullptr;

    LShader* ShaderProgram = nullptr;
    uint32 Texture       = 0;

    void MouseCallback(const double XPos, const double YPos);
    void ScrollCallback(const double YOffset);

    void RegisterTickableObject(LTickableObject* Tickable);
    void UnregisterTickableObject(LTickableObject* Tickable);

private:

    TdhArray<LTickableObject*> TickableObjects;

    TdhArray<AActor*> Actors;
    EWorldState::Type WorldState;

    void InitializeSubsystems();
    void TearDownSubsystems();

    LSubsystemCollection* Collection = nullptr;
};

} /* ~Namespace Jafg */
