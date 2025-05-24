// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectContext.h"
#include "Physics/TraceUtility.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/Level.h"
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
class LCommandLineInterface;
struct LLevel;
struct LSubsystemCollection;

namespace Private
{

struct LWorldMiscellaneousAccessor;

} /* ~Namespace Private */

MAKE_DELEGATE_SIGNATURE(LOnStaticLineTrace, bool,
    TArray<LHitResult>& OutHits,
    const LVector& Start,
    const LVector& End,
    const LCollisionQueryParams& Params
)
MAKE_DELEGATE_SIGNATURE(LOnStaticDraw, void,
    const LViewport& Viewport,
    const LEye& Eye,
    const std::span<LVector>& Corners
)

namespace EWorldState
{

enum Type : u8
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

enum Type : u8
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

    struct LEyeToMatricesMapSecond
    {
        LMatrix P;
        LMatrix V;
    };
    typedef std::unordered_map<const void*, LEyeToMatricesMapSecond> LEyeToMatricesMap;

public:

    LWorld() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LWorld)
    LWorld(const LString& InHumanReadableName, const EWorldState::Type InWorldType);

    virtual bool IsWorld() const override { return true; }

    ENGINE_API auto GetEngine() const -> LEngine*;
    ENGINE_API auto GetLocalEgo() const -> LLocalEgo*;
    ENGINE_API auto GetLocalController() const -> APersonaController*;
    ENGINE_API auto GetCommandLineInterface() const -> LCommandLineInterface*;
    //# Only valid if the pawn is in this world.
    FORCEINLINE bool IsLocalPawnValid() const { return this->GetLocalPawn() != nullptr; }
    ENGINE_API  auto GetLocalPawn() const -> APawn*;
    FORCEINLINE auto GetLocalPawnChecked() const -> APawn* { APawn* Out = this->GetLocalPawn(); check( Out ) return Out; }
    FORCEINLINE auto GetLocalPawnAsserted() const -> APawn* { APawn* Out = this->GetLocalPawn(); jassert( Out ) return Out; }

    FORCEINLINE auto GetWorldState() const -> EWorldState::Type { return this->WorldState; }

    void InitializeWorld(const LLevel& Level);

    FORCEINLINE bool CanTick() const { return this->GetWorldState() == EWorldState::Running; }
    void Tick(const float DeltaTime);

    void Draw(const LViewport& Viewport, const LEye& Eye) const;
    LOnStaticDraw OnStaticDraw;

#if AS_CLIENT
    void LateTick(const float DeltaTime);
#endif /* AS_CLIENT */

    // LObjectContext implementation
    virtual void TearDownContext() override;
    // ~LObjectContext implementation

    FORCEINLINE bool IsUnderlyingLevelValid() const { return this->UnderlyingLevel.IsSet(); }
    FORCEINLINE const LLevel& GetUnderlyingLevel() const { return this->UnderlyingLevel.GetValue(); }
    FORCEINLINE LStringView   GetUnderlyingLevelName() const { return this->IsUnderlyingLevelValid() ? LStringView{this->UnderlyingLevel->Identifier} : LStringView{ }; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameChecked() const { check( this->IsUnderlyingLevelValid() ) return this->IsUnderlyingLevelValid() ? LStringView{this->UnderlyingLevel->Identifier} : LStringView{ }; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameAsserted() const { jassert( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel->Identifier; }

    ENGINE_API void RegisterTickableObject(LTickableObject* Tickable);
    ENGINE_API void UnregisterTickableObject(LTickableObject* Tickable);
    FORCEINLINE auto GetTickableObjects() const -> const TArray<LTickableObject*>& { return this->TickableObjects; }
    FORCEINLINE auto GetActors() const -> const TArray<AActor*>& { return this->Actors; }

    ENGINE_API f32 GetRealTimeSecondsSinceWorldLaunch() const;

    //#
    //# Trace this world for physical hits.
    //# @return True if a blocking hit was found.
    //#
    bool LineTraceByChannel(
        TArray<LHitResult>& OutHits,
        const LVector& Begin,
        const LVector& End,
        const ECollisionChannel::Type Channel,
        const LCollisionQueryParams& Params
    ) const;

    LOnStaticLineTrace OnStaticLineTrace;

#if AS_CLIENT
    template <typename T>
    void AddTemporalObject(T&& InTemporalObject);
#endif /* AS_CLIENT */

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JWorldSubsystem)

private:

    TOptional<LLevel> UnderlyingLevel;

#if AS_CLIENT
    TArray<LTemporalWorldObject*> TemporalObjects;
#endif /* AS_CLIENT */

    TArray<LTickableObject*> TickableObjects;
    TArray<LTickableObject*> DeletedTickableObjects;

    TArray<AActor*> Actors;
    mutable LEyeToMatricesMap EyeToMatrices;
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
    f32 RealTimeWhenWorldWasLaunched { 0.0f };
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

#include "WorldStorage.h"
