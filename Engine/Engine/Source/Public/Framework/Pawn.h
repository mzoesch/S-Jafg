// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Actor.h"
#include "Framework/Eye.h"
#include "Pawn.generated.h"

namespace Jafg
{

class APersonaController;
struct LInputActionValue;

//#
//# A pawn is something that can be possessed by a controller.
//#
DECLARE_JAFG_CLASS(ECxxClassFlags::Abstract)
class ENGINE_API APawn : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    explicit APawn(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(APawn)

public:

    virtual void Tick(const float DeltaTime) override;
    virtual void EndLife() override;

    FORCEINLINE auto IsPossessed() const -> bool { return this->OwningController != nullptr; }
                bool IsPossessedLocally() const;
                LLocalEgo* GetLocalEgoIfPossessed() const;
    FORCEINLINE LLocalEgo* GetLocalEgoIfPossessedChecked() const { LLocalEgo* Out = this->GetLocalEgoIfPossessed(); check( Out ) return Out; }
    FORCEINLINE LLocalEgo* GetLocalEgoIfPossessedAsserted() const { LLocalEgo* Out = this->GetLocalEgoIfPossessed(); jassert( Out ) return Out; }

    FORCEINLINE APersonaController* GetOwningController() noexcept { return this->OwningController; }
    FORCEINLINE APersonaController const* GetOwningController() const noexcept { return this->OwningController; }
    virtual void SetOwningController(APersonaController* InNew);

    FORCEINLINE bool IsEyeValid() const noexcept { return this->Eye.IsOwningPawnValid(); }
    FORCEINLINE LEye& GetEye() noexcept { return this->Eye; }
    FORCEINLINE LEye const& GetEye() const noexcept { return this->Eye; }

    void OnOngoingMovementInput(LInputActionValue& InValue);
    void OnOngoingRotationInput(LInputActionValue& InValue);
    void OnOngoingVelocityChange(LInputActionValue& InValue);
    void OnOngoingPrimaryInput(LInputActionValue& InValue);
    void OnOngoingSecondaryInput(LInputActionValue& InValue);

    //# Cached hit results for this frame. Use this if only generic hit results information is needed.
    FORCEINLINE auto GetCurrentGenericTraceResults() const -> const TArray<LHitResult>& { return this->CurrentGenericTraceResults; }
    bool TraceFromEyeByChannel(
        TArray<LHitResult>& OutHits,
        const float DistanceInMeters,
        const ECollisionChannel::Type Channel,
        const LCollisionQueryParams& Params
    ) const;

private:

#if WITH_LOCAL_LAYER
    LEye Eye;
#endif /* WITH_LOCAL_LAYER */

    void UpdateRelativeVectors();
    LVector RelativeFront = LVector::ForwardVector;
    LVector RelativeRight = LVector::RightVector;
    LVector RelativeUp    = LVector::UpVector;

    APersonaController* OwningController = nullptr;

    f32  MovementSpeed    = 2.5f;
    f32  MouseSensitivity = 0.1f;
    f64  LastMouseX       = 0.0;
    f64  LastMouseY       = 0.0;
    bool bFirstMouseCallback = true;

    TArray<LHitResult> CurrentGenericTraceResults;
};

} /* ~Namespace Jafg */
