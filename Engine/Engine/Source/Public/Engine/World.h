// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Arguments.h"
#include "Engine/ClassOuter.h"
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
      TArray<LHitResult>& OutHits
    , const LVector& Start
    , const LVector& End
    , const LCollisionQueryParams& Params
    )
MAKE_DELEGATE_SIGNATURE(LOnStaticDraw, void,
      const LViewport& Viewport
    , const LEye& Eye
    , const std::span<LVector>& Corners
    )

namespace EWorldState
{
enum Type : u8
{
    PreInitializing,
    Initializing,
    Running,
    TearingDown,
    WaitingForKill,
};
} /* ~Namespace EWorldState */
inline LStringView LexToString(const EWorldState::Type InType) noexcept
{
    switch (InType)
    {
        case EWorldState::PreInitializing: return "PreInitializing";
        case EWorldState::Initializing:    return "Initializing";
        case EWorldState::Running:         return "Running";
        case EWorldState::TearingDown:     return "TearingDown";
        case EWorldState::WaitingForKill:  return "WaitingForKill";
        default:                           return "<Unknown>";
    }
}

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
inline LStringView LexToString(const EWorldTimeBehavior::Type InType) noexcept
{
    switch (InType)
    {
        case EWorldTimeBehavior::Linear:   return "Linear";
        case EWorldTimeBehavior::Desist:   return "Desist";
        case EWorldTimeBehavior::Simulate: return "Simulate";
        default:                           return "<Unknown>";
    }
}

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

typedef LGenericArgument LWorldArgument;

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
//# A world.
//#
//# Once every frame the world will be ticked. It may register itself to the RHI to be used when
//# rendering on any kind of surface. Multiple worlds may draw to the same surface, and a world
//# may draw to multiple surfaces.
//#
class LWorld final : public LClassOuter
{
    friend AActor;
    friend Private::LWorldMiscellaneousAccessor;

    struct LEyeToMatricesMapSecond { LMatrix P; LMatrix V; };
    typedef std::unordered_map<void const*, LEyeToMatricesMapSecond> LEyeToMatricesMap;

public:

    LWorld() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LWorld)
    constexpr LWorld(LString const& InHumanReadableName) noexcept
        : LClassOuter{InHumanReadableName}, WorldState(EWorldState::PreInitializing)
    {
        return;
    }

    // LClassOuter implementation
    virtual bool IsWorld() const noexcept override { return true; }
    // ~LClassOuter implementation

    void InitializeWorld(TOptional<LLevel> const& Level = {}, LString&& Url = {});

    //# There are no checked alternatives, as the engine must be valid at all times if a world exists.
    ENGINE_API LEngine* GetEngine() const;
    ENGINE_API LCommandLineInterface* GetCommandLineInterface() const;

    ENGINE_API  LLocalEgo* GetLocalEgo() const;
    FORCEINLINE LLocalEgo* GetLocalEgoChecked() const { LLocalEgo* Out{ this->GetLocalEgo() }; check( Out ) return Out; }
    FORCEINLINE LLocalEgo* GetLocalEgoAsserted() const { LLocalEgo* Out{ this->GetLocalEgo() }; jassert( Out ) return Out; }

    //# Get the local controller if any and said local controller is home to this world.
    FORCEINLINE bool                IsLocalControllerValid() const { return this->GetLocalController() != nullptr; }
    ENGINE_API  APersonaController* GetLocalController() const;
    FORCEINLINE APersonaController* GetLocalControllerChecked() const { APersonaController* Out{ this->GetLocalController() }; check( Out ) return Out; }
    FORCEINLINE APersonaController* GetLocalControllerAsserted() const { APersonaController* Out{ this->GetLocalController() }; jassert( Out ) return Out; }

    //# Get the local pawn if any and said local pawn is home to this world.
    FORCEINLINE bool   IsLocalPawnValid() const { return this->GetLocalPawn() != nullptr; }
    ENGINE_API  APawn* GetLocalPawn() const;
    FORCEINLINE APawn* GetLocalPawnChecked() const { APawn* Out{ this->GetLocalPawn() }; check( Out ) return Out; }
    FORCEINLINE APawn* GetLocalPawnAsserted() const { APawn* Out{ this->GetLocalPawn() }; jassert( Out ) return Out; }

    //#
    //# The real URL that was used to launch this world. This might not be valid.
    //# @note This URL is not sanitized, so for the most cases you should use the sanitized URL by
    //#       calling #GetBrowsedUrl.
    //#
    FORCEINLINE LString const& GetUnsanitizedUrl() const noexcept { return this->UnsanitizedUrl; }
    //# The URL that was used to launch this world. This might not be valid.
    FORCEINLINE LString const& GetBrowsedUrl() const noexcept { return this->Url; }
    //# The URL but parsed into a structured way. This might not be valid.
    FORCEINLINE LWorldParameters const& GetParameters() const noexcept { return this->Parameters; }

    FORCEINLINE EWorldState::Type GetWorldState() const noexcept { return this->WorldState; }

    FORCEINLINE bool CanTick() const noexcept { return this->GetWorldState() == EWorldState::Running; }
    void Tick(const f32 DeltaTime);

    void Draw(LViewport const& Viewport, LEye const& Eye) const;
    //# Performance optimization for static objects.
    LOnStaticDraw OnStaticDraw;

#if AS_CLIENT
    void LateTick(const f32 DeltaTime);
#endif /* AS_CLIENT */

    FORCEINLINE bool IsUnderlyingLevelValid() const noexcept { return this->UnderlyingLevel.has_value(); }
    FORCEINLINE LLevel const& GetUnderlyingLevel() const noexceptcheck { check( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel.value(); }
    FORCEINLINE LStringView   GetUnderlyingLevelName() const noexcept { if (this->IsUnderlyingLevelValid()) { return LStringView{this->UnderlyingLevel->Identifier}; } return {}; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameChecked() const noexceptcheck { check( this->IsUnderlyingLevelValid() ) return this->IsUnderlyingLevelValid() ? LStringView{this->UnderlyingLevel->Identifier} : LStringView{ }; }
    FORCEINLINE LStringView   GetUnderlyingLevelNameAsserted() const { jassert( this->IsUnderlyingLevelValid() ) return this->UnderlyingLevel->Identifier; }

#if AS_CLIENT
    template <typename T>
    void AddTemporalObject(T&& InTemporalObject);
    FORCEINLINE TArray<TUnique<LTemporalWorldObject>> const& GetTemporalObjects() const noexcept { return this->TemporalObjects; }
    FORCEINLINE TArray<TUnique<LTemporalWorldObject>>& GetMutableTemporalObjects() noexcept { return this->TemporalObjects; }
#endif /* AS_CLIENT */

    ENGINE_API  void RegisterTickableObject(LTickableObject* Tickable);
    ENGINE_API  void UnregisterTickableObject(LTickableObject* Tickable);
    FORCEINLINE bool IsTickableObjectsPutMutexLocked() const { return this->TickableObjectsPutMutex; }
    FORCEINLINE TArray<LTickableObject*> const& GetTickableObjects() const noexcept { return this->TickableObjects; }
    FORCEINLINE TArray<LTickableObject*>& GetMutableTickableObjects() noexcept { return this->TickableObjects; }
    FORCEINLINE TArray<LTickableObject*> const& GetDeletedTickableObjects() const noexcept { return this->DeletedTickableObjects; }
    FORCEINLINE TArray<LTickableObject*>& GetDeletedMutableTickableObjects() noexcept { return this->DeletedTickableObjects; }

    FORCEINLINE bool ShouldDrawSkyboxFirst() const noexcept { return this->bDrawSkyboxFirst; }
    FORCEINLINE bool IsSkyboxValid() const noexcept { return this->Skybox.has_value(); }
    FORCEINLINE LSkybox const& GetSkybox() const noexceptcheck { check( this->IsSkyboxValid() ) return this->Skybox.value(); }
    FORCEINLINE LSkybox& GetMutableSkybox() noexceptcheck { check( this->IsSkyboxValid() ) return this->Skybox.value(); }

    ENGINE_API f32 GetRealTimeSecondsSinceWorldLaunch() const noexcept;

    //#
    //# Trace this world for physical hits.
    //# @return True if a blocking hit was found.
    //#
    bool LineTraceByChannel
    (
          TArray<LHitResult>& OutHits
        , LVector const& Begin
        , LVector const& End
        , ECollisionChannel::Type Channel
        , LCollisionQueryParams const& Params
    ) const;
    LOnStaticLineTrace OnStaticLineTrace;

    SUBSYSTEM_COLLECTION_OUTER_GETTERS(Collection, JWorldSubsystem)

    ENGINE_API  static LWorld* GetWorldFromHumanReadableName(LString const& InHumanReadableName) noexcept;
    FORCEINLINE static LWorld* GetWorldFromHumanReadableNameChecked(LString const& InHumanReadableName) noexceptcheck;
    FORCEINLINE static LWorld* GetWorldFromHumanReadableNameAsserted(LString const& InHumanReadableName);

protected:

    // LObjectContext implementation
    virtual void OnTearDown() override;
    // ~LObjectContext implementation

private:

    LString UnsanitizedUrl;
    LString Url;
    LWorldParameters Parameters;
    void UpdateUrlParams() noexcept;

    TOptional<LLevel> UnderlyingLevel;

#if AS_CLIENT
    TArray<TUnique<LTemporalWorldObject>> TemporalObjects;
#endif /* AS_CLIENT */

    //# Main thread only.
    void AcquireTickableObjectsLock() { this->TickableObjectsPutMutex = true; }
    void ReleaseTickableObjectsLock() { this->TickableObjectsPutMutex = false; }
    bool TickableObjectsPutMutex = false;
    TArray<LTickableObject*> TickableObjects;
    TArray<LTickableObject*> DeletedTickableObjects;

    bool bDrawSkyboxFirst { false };
    TOptional<LSkybox> Skybox;

    mutable LEyeToMatricesMap EyeToMatrices;
    EWorldState::Type WorldState;

    LSubsystemCollection Collection{ "World" };

    //#
    //# The real time (not stopped or dilated / clamped) when this world was launched.
    //# Real time is relative to the static storage initialization of the engine shared library.
    //#
    f32 RealTimeWhenWorldWasLaunched { -1.0f };
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
    this->TemporalObjects.emplace_back(std::make_unique<T>(std::forward<T>(InTemporalObject)));
    return;
}
#endif /* AS_CLIENT */

FORCEINLINE LWorld* LWorld::GetWorldFromHumanReadableNameChecked(const LString& InHumanReadableName) noexceptcheck
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

FORCEINLINE LWorld* LClassOuter::AsWorld() noexcept
{
    check( this->IsWorld() )
    return static_cast<LWorld*>(this);
}

FORCEINLINE LWorld const* LClassOuter::AsWorld() const noexcept
{
    check( this->IsWorld() )
    return static_cast<LWorld const*>(this);
}

} /* ~Namespace Jafg */

#include "Engine/WorldStorage.h"
