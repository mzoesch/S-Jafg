// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "Pawn.generated.h"

namespace Jafg
{

class APersonaController;
class ASceneComponent;
struct LInputActionValue;

//# A pawn is something that can be possessed by a controller.
DECLARE_JAFG_CLASS()
class ENGINE_API APawn : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(APawn)
    {
        this->DefaultInit();
    }

private:

    void DefaultInit();

public:

    virtual void Tick(f32 Dt) override;
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    LWorldEye GetEye() const noexcept;

    bool IsPossessedLocally() const noexcept;
    LLocalEgo* GetLocalEgoIfPossessed() const noexcept;
    FORCEINLINE LLocalEgo* GetLocalEgoIfPossessedChecked() const noexcept { auto* Out{this->GetLocalEgoIfPossessed()}; check(Out) return Out; }
    FORCEINLINE LLocalEgo* GetLocalEgoIfPossessedAsserted() const noexcept { auto* Out{this->GetLocalEgoIfPossessed()}; jassert(Out) return Out; }

    FORCEINLINE bool IsOwningControllerValid() const noexcept { return this->OwningController != nullptr; }
    FORCEINLINE APersonaController* GetOwningController() noexcept { return this->OwningController; }
    FORCEINLINE APersonaController const* GetOwningController() const noexcept { return this->OwningController; }
    FORCEINLINE APersonaController* GetOwningControllerChecked() noexcept{ check(this->IsOwningControllerValid()) return this->OwningController; }
    FORCEINLINE APersonaController const* GetOwningControllerChecked() const noexcept { check(this->IsOwningControllerValid()) return this->OwningController; }
    FORCEINLINE APersonaController* GetOwningControllerAsserted() noexcept { jassert( this->IsOwningControllerValid()) return this->OwningController; }
    FORCEINLINE APersonaController const* GetOwningControllerAsserted() const noexcept { jassert(this->IsOwningControllerValid()) return this->OwningController; }
    //# Jafg internal method. Do not use. @see #APersonaController::PossessPawn.
    virtual void _SetOwningController(APersonaController* New);

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

    f32 VertFov{ 1.0471975511965977461542144610931676280657231331250352736583148641026054687620696662093449417807056893273826955044274355490312815f };
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
