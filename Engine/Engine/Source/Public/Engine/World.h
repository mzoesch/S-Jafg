// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectContext.h"
#include "Physics/TraceUtility.h"
#include "Subsystems/SubsystemCollection.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/Level.h"
#include "Framework/Skybox.h"
#include "Cli/CliType.h"
#include "Cli/CliCommand.h"
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
class LWorld;
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
//# The parameters for a world.
//# Parameters are split by a '?'. A special character can be escaped with '\'.
//# A parameter may have the following formats:
//#   <Key>
//#   <Key>=
//#   <Key>=<Value>
//#
//# E.g.: MyCoolLevel?MyFirstParam?MySecondParam=?MyThirdParam=HeyThereIsAValue?Another\=Key\?StillTheFourthKey=ThisIsTheValue\?\=StillTheSameValue
//#
struct LWorldParameters final
{
    friend LWorld;

    struct LWorldParam final
    {
        LString Key;
        LString Value;
    };

    TArray<LWorldParam> Params;

    ENGINE_API LString ToString() const;

private:

    void Reset() noexcept;
};

template<> FORCEINLINE LCliType LCliType::Type<LWorld>  () { return LCliType::Type("World");   }

template<>
struct LCommandArgsTypeRet<LWorld> final
{
    typedef LWorld* Type;
};
template<>
FORCEINLINE LCommandArgsTypeRet<LWorld>::Type LCommandArgs::GetAs<LWorld>() const;
template<>
struct LCommandArgsTypeRet<const LWorld> final
{
    typedef const LWorld* Type;
};
template<>
FORCEINLINE LCommandArgsTypeRet<const LWorld>::Type LCommandArgs::GetAs<const LWorld>() const;

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

    // LObjectContext implementation
    virtual bool IsWorld() const noexcept override { return true; }
    // ~LObjectContext implementation

    ENGINE_API LEngine* GetEngine() const;
    ENGINE_API LLocalEgo* GetLocalEgo() const;
    ENGINE_API APersonaController* GetLocalController() const;
    ENGINE_API LCommandLineInterface* GetCommandLineInterface() const;

    //# Only valid if the pawn is in this world.
    FORCEINLINE bool IsLocalPawnValid() const { return this->GetLocalPawn() != nullptr; }
    ENGINE_API  APawn* GetLocalPawn() const;
    FORCEINLINE APawn* GetLocalPawnChecked() const { APawn* Out = this->GetLocalPawn(); check( Out ) return Out; }
    FORCEINLINE APawn* GetLocalPawnAsserted() const { APawn* Out = this->GetLocalPawn(); jassert( Out ) return Out; }

    //# The URL that was used to launch this world. This might not be valid.
    FORCEINLINE const LString& GetBrowsedUrl() const noexcept { return this->Url; }
    //# The URL but parsed into a structured way. This might not be valid.
    FORCEINLINE const LWorldParameters& GetParameters() const noexcept { return this->Parameters; }

    FORCEINLINE EWorldState::Type GetWorldState() const noexcept { return this->WorldState; }

    void InitializeWorld(const LLevel& Level, LString&& InLaunchedUrl);

    FORCEINLINE bool CanTick() const { return this->GetWorldState() == EWorldState::Running; }
    void Tick(const f32 DeltaTime);

    void Draw(const LViewport& Viewport, const LEye& Eye) const;
    LOnStaticDraw OnStaticDraw;

#if AS_CLIENT
    void LateTick(const f32 DeltaTime);
#endif /* AS_CLIENT */

    // LObjectContext implementation
    virtual void TearDownContext() override;
    // ~LObjectContext implementation

    FORCEINLINE bool IsUnderlyingLevelValid() const { return this->UnderlyingLevel.IsValid(); }
    FORCEINLINE const LLevel& GetUnderlyingLevel() const { return this->UnderlyingLevel.GetValue(); }
    FORCEINLINE LStringView   GetUnderlyingLevelName() const { return this->IsUnderlyingLevelValid() ? LStringView{this->UnderlyingLevel->Identifier} : LStringView{ }; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameChecked() const { check( this->IsUnderlyingLevelValid() ) return this->IsUnderlyingLevelValid() ? LStringView{this->UnderlyingLevel->Identifier} : LStringView{ }; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameAsserted() const { jassert( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel->Identifier; }

    ENGINE_API  void RegisterTickableObject(LTickableObject* Tickable);
    ENGINE_API  void UnregisterTickableObject(LTickableObject* Tickable);
    FORCEINLINE auto GetTickableObjects() const -> const TArray<LTickableObject*>& { return this->TickableObjects; }
    FORCEINLINE auto GetActors() const -> const TArray<AActor*>& { return this->Actors; }
    FORCEINLINE bool IsSkyboxValid() const noexcept { return this->Skybox.IsValid(); }
    FORCEINLINE auto GetSkybox() noexcept -> LSkybox& { return this->Skybox.GetValue(); }
    FORCEINLINE auto GetSkybox() const noexcept -> const LSkybox& { return this->Skybox.GetValue(); }

    ENGINE_API f32 GetRealTimeSecondsSinceWorldLaunch() const;

    //#
    //# Trace this world for physical hits.
    //# @return True if a blocking hit was found.
    //#
    bool LineTraceByChannel
    (
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

    ENGINE_API  static LWorld* GetWorldFromHumanReadableName(const LString& InHumanReadableName);
    FORCEINLINE static LWorld* GetWorldFromHumanReadableNameChecked(const LString& InHumanReadableName);
    FORCEINLINE static LWorld* GetWorldFromHumanReadableNameAsserted(const LString& InHumanReadableName);

private:

    LString Url;
    LWorldParameters Parameters;
    void UpdateUrlParams();

    TOptional<LLevel> UnderlyingLevel;

#if AS_CLIENT
    TArray<LTemporalWorldObject*> TemporalObjects;
#endif /* AS_CLIENT */

    TArray<LTickableObject*> TickableObjects;
    TArray<LTickableObject*> DeletedTickableObjects;

    TArray<AActor*> Actors;
    bool bDrawSkyboxFirst { false };
    TOptional<LSkybox> Skybox;

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

template <>
FORCEINLINE LCommandArgsTypeRet<LWorld>::Type LCommandArgs::GetAs<LWorld>() const
{
    return LWorld::GetWorldFromHumanReadableNameAsserted(this->Name);
}

template <>
FORCEINLINE LCommandArgsTypeRet<const LWorld>::Type LCommandArgs::GetAs<const LWorld>() const
{
    return LWorld::GetWorldFromHumanReadableNameAsserted(this->Name);
}

#if AS_CLIENT
template <typename T>
FORCEINLINE void LWorld::AddTemporalObject(T&& InTemporalObject)
{
    T* TemporalObject { new T(std::forward<T>(InTemporalObject)) };
    this->TemporalObjects.Add(TemporalObject);
    return;
}
#endif /* AS_CLIENT */

FORCEINLINE LWorld* LWorld::GetWorldFromHumanReadableNameChecked(const LString& InHumanReadableName)
{
    LWorld* Out { LWorld::GetWorldFromHumanReadableName(InHumanReadableName) };
    check( Out )
    return Out;
}

FORCEINLINE LWorld* LWorld::GetWorldFromHumanReadableNameAsserted(const LString& InHumanReadableName)
{
    LWorld* Out { LWorld::GetWorldFromHumanReadableName(InHumanReadableName) };
    jassert( Out )
    return Out;
}

} /* ~Namespace Jafg */

#include "Engine/WorldStorage.h"
