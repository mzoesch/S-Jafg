// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectContext.h"
#include "Physics/TraceUtility.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#if AS_CLIENT
    #include "Debug/TemporalWorldObject.h"
#endif /* AS_CLIENT */

namespace Jafg
{

class LLocalEgo;
class AActor;
class LViewport;
class LEngine;
class LShader;
class JWorldSubsystem;
class LTickableObject;
class APersonaController;
class APawn;
class LEye;
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

namespace EWorldTimeBehavior
{

enum Type : uint8
{
    //#
    //# Time behaves linearly. This is the default behavior.
    //#
    Linear,

    //#
    //# Time is desisted for this world.
    //# Time will not be forwarded to employees of the world context.
    //#
    Desist,

    //#
    //# Time is simulated and private to world core subsystems.
    //# Time will not be forwarded to employees of the world context.
    //#
    Simulate,
};

} /* ~Namespace EWorldTimeBehavior */

//#
//# Represents a world at its core.
//# Once every frame a world will be ticked. It may register itself to the RHI to be used when
//# rendering on any kind of surface. Multiple worlds may draw to the same surface, and a world
//# may draw to multiple surfaces.
//#
class LWorld final : public LObjectContext
{
    friend AActor;
    friend Private::LWorldMiscellaneousAccessor;

public:

    LWorld() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LWorld)
    LWorld(const LSimpleString& InHumanReadableName, const EWorldState::Type InWorldType);

    ENGINE_API auto GetEngine() const -> LEngine*;
    ENGINE_API auto GetLocalEgo() const -> LLocalEgo*;
    ENGINE_API auto GetLocalController() const -> APersonaController*;
    //# Only valid if the pawn is in this world.
    ENGINE_API  auto GetLocalPawn() const -> APawn*;
    FORCEINLINE auto GetLocalPawnChecked() const -> APawn* { APawn* Out = this->GetLocalPawn(); check( Out ) return Out; }
    FORCEINLINE auto GetLocalPawnAsserted() const -> APawn* { APawn* Out = this->GetLocalPawn(); jassert( Out ) return Out; }

    FORCEINLINE auto GetWorldState() const -> EWorldState::Type { return this->WorldState; }

    void InitializeWorld(const LLevel& Level);

    FORCEINLINE bool CanTick() const { return this->GetWorldState() == EWorldState::Running; }
    void Tick(const float DeltaTime);
    void Draw(const LViewport& Viewport, const LEye& Eye) const;
#if AS_CLIENT
    void LateTick(const float DeltaTime);
#endif /* AS_CLIENT */

    // LObjectContext implementation
    virtual void TearDownContext() override;
    // ~LObjectContext implementation

    ENGINE_API void RegisterTickableObject(LTickableObject* Tickable);
    ENGINE_API void UnregisterTickableObject(LTickableObject* Tickable);
    FORCEINLINE auto GetTickableObjects() const -> const TdhArray<LTickableObject*>& { return this->TickableObjects; }
    FORCEINLINE auto GetActors() const -> const TdhArray<AActor*>& { return this->Actors; }

    ENGINE_API float GetRealTimeSecondsSinceWorldLaunch() const;

    //#
    //# Trace this world for physical hits.
    //# @return True if a blocking hit was found.
    //#
    bool LineTraceByChannel(
        TdhArray<LHitResult>& OutHits,
        const LVector& Begin,
        const LVector& End,
        const ECollisionChannel::Type Channel,
        const LCollisionQueryParams& Params
    ) const;

#if AS_CLIENT
    template <typename T>
    void AddTemporalObject(T&& InTemporalObject);
#endif /* AS_CLIENT */

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JWorldSubsystem)

private:

#if AS_CLIENT
    TdhArray<LTemporalWorldObject*> TemporalObjects;
#endif /* AS_CLIENT */

    TdhArray<LTickableObject*> TickableObjects;
    TdhArray<LTickableObject*> DeletedTickableObjects;

    TdhArray<AActor*> Actors;
    EWorldState::Type WorldState;

    LSubsystemCollection Collection;

    //# Main thread only.
    bool TickableObjectsPutMutex = false;
    bool IsTickableObjectsPutMutexLocked() const { return this->TickableObjectsPutMutex; }
    void AcquireTickableObjectsLock() { this->TickableObjectsPutMutex = true; }
    void ReleaseTickableObjectsLock() { this->TickableObjectsPutMutex = false; }

    //#
    //# The real time (not stopped or dilated / clamped) when this world was launched.
    //# Real time is relative to the static storage initialization of the engine shared library.
    //#
    float RealTimeWhenWorldWasLaunched = 0.0f;
};

#if AS_CLIENT
template <typename T>
void LWorld::AddTemporalObject(T&& InTemporalObject)
{
    T* TemporalObject = new T(std::forward<T>(InTemporalObject));
    this->TemporalObjects.Add(TemporalObject);
    return;
}
#endif /* AS_CLIENT */

} /* ~Namespace Jafg */
