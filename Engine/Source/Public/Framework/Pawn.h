// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "Framework/Eye.h"
#include "Pawn.generated.h"

namespace Jafg
{

class APersonaController;
class JSceneComponent;
struct LInputActionValue;

//# A pawn is something that can be possessed by a controller.
DECLARE_JAFG_CLASS()
class ENGINE_API APawn : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    explicit APawn(LCxxObjectInitializer const& CxxObjectInitializer) : Super(CxxObjectInitializer) { this->SetEverTickConstructorOnlyFlag(); }
    DEFAULT_OBJECT_CDR_CTOR(APawn)

public:

    virtual void BeginLife() override;
    virtual void Tick(const float DeltaTime) override;
    virtual void EndLife() override;

    ENGINE_API LEye_v2 GetEye_v2() const noexcept;


    FORCEINLINE bool IsPossessed() const noexcept { return this->OwningController != nullptr; }
    bool IsPossessedLocally() const noexcept;
    LLocalEgo* GetLocalEgoIfPossessed() const noexcept;
    FORCEINLINE LLocalEgo* GetLocalEgoIfPossessedChecked() const noexceptcheck { LLocalEgo* Out{ this->GetLocalEgoIfPossessed() }; check( Out ) return Out; }
    FORCEINLINE LLocalEgo* GetLocalEgoIfPossessedAsserted() const { LLocalEgo* Out{ this->GetLocalEgoIfPossessed() }; jassert( Out ) return Out; }

    FORCEINLINE APersonaController* GetOwningController() noexcept { return this->OwningController; }
    FORCEINLINE APersonaController const* GetOwningController() const noexcept { return this->OwningController; }
    FORCEINLINE APersonaController* GetOwningControllerChecked() noexceptcheck { check( this->IsPossessed() ) return this->OwningController; }
    FORCEINLINE APersonaController const* GetOwningControllerChecked() const noexceptcheck { check( this->IsPossessed() ) return this->OwningController; }
    FORCEINLINE APersonaController* GetOwningControllerAsserted() noexceptcheck { jassert( this->IsPossessed() ) return this->OwningController; }
    FORCEINLINE APersonaController const* GetOwningControllerAsserted() const noexceptcheck { jassert( this->IsPossessed() ) return this->OwningController; }
    virtual void SetOwningController(APersonaController* InNew);

    FORCEINLINE void SetNearFrustum(f32 Value) noexcept { this->NearFrustum = Value; }
    FORCEINLINE void SetFarFrustum(f32 Value) noexcept { this->FarFrustum = Value; }

    //# Cached hit results for this frame. Use this if only generic hit results information is needed.
    // FORCEINLINE auto const& GetCurrentGenericTraceResults() const { return this->CurrentGenericTraceResults; }
    // bool TraceFromEyeByChannel( // TODO
    //     TArray<LHitResult>& OutHits,
    //     const float DistanceInMeters,
    //     const ECollisionChannel::Type Channel,
    //     const LCollisionQueryParams& Params
    // ) const { return false; }

private:

    APersonaController* OwningController{};

    //# The root scene component that specifies the actual transform for this pawn.
    JSceneComponent* RootComponent{};

    f32 VertFov{ 1.0471975511965977461542144610931676280657231331250352736583148641026054687620696662093449417807056893273826955044274355490312815 };
    f32 NearFrustum{ 0.1f };
    f32 FarFrustum{ 1.0f };

    // TArray<LHitResult> CurrentGenericTraceResults;

    // f32  MovementSpeed    = 2.5f;
    f32  MouseSensitivity = 0.1f;
    f64  LastMouseX       = 0.0;
    f64  LastMouseY       = 0.0;
    bool bFirstMouseCallback = true;

};

} /* ~Namespace Jafg */
