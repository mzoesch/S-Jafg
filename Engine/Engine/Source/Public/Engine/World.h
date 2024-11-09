// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectContext.h"

namespace Jafg
{

class AActor;
class LEngine;
class LShader;
class JWorldSubsystem;
class Camera;
class LTickableObject;
struct LLevel;
struct LSubsystemCollection;

namespace Private
{

struct LWorldMiscellaneousAccessor;

} /* ~Namespace Private */

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

/**
 * Represents a world at its core.
 * Once every frame a world will be ticked. It may register itself to the RHI to be used when
 * rendering on any kind of surface. Multiple worlds may draw to the same surface.
 */
class ENGINE_API LWorld final : public ::Jafg::Private::LObjectContext
{
    friend AActor;
    friend Private::LWorldMiscellaneousAccessor;

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

    void MouseCallback(const double XPos, const double YPos);
    void ScrollCallback(const double YOffset);

    void RegisterTickableObject(LTickableObject* Tickable);
    void UnregisterTickableObject(LTickableObject* Tickable);

    FORCEINLINE auto GetTickableObjects() const -> const TdhArray<LTickableObject*>& { return this->TickableObjects; }
    FORCEINLINE auto GetActors() const -> const TdhArray<AActor*>& { return this->Actors; }

    float GetRealTimeSecondsSinceWorldLaunch() const;

private:

    TdhArray<LTickableObject*> TickableObjects;
    TdhArray<LTickableObject*> DeletedTickableObjects;

    TdhArray<AActor*> Actors;
    EWorldState::Type WorldState;

    void InitializeSubsystems();
    void TearDownSubsystems();

    LSubsystemCollection* Collection = nullptr;

    /** Main thread only. */
    bool TickableObjectsPutMutex : 1 = false;
    bool IsTickableObjectsPutMutexLocked() const { return this->TickableObjectsPutMutex; }
    void AcquireTickableObjectsLock() { this->TickableObjectsPutMutex = true; }
    void ReleaseTickableObjectsLock() { this->TickableObjectsPutMutex = false; }

    /**
     * The real time (not stopped or dilated / clamped) when this world was launched.
     * Real time is relative to the static storage initialization of the engine shared library.
     */
    float RealTimeWhenWorldWasLaunched = 0.0f;
};

} /* ~Namespace Jafg */
