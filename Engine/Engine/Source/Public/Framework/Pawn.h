// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "Framework/Eye.h"
#include "Pawn.generated.h"

namespace Jafg
{

class APersonaController;
struct LInputActionValue;

//#
//# A pawn is something that can be possessed by a controller.
//#
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API APawn : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    explicit APawn(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Tick(const float DeltaTime) override;
    virtual void EndLife() override;

    FORCEINLINE auto IsPossessed() const -> bool { return this->OwningController != nullptr; }
                bool IsPossessedLocally() const;
                LLocalEgo* GetPossessedEgo() const;
    FORCEINLINE LLocalEgo* GetPossessedEgoChecked() const { LLocalEgo* Out = this->GetPossessedEgo(); check( Out ) return Out; }
    FORCEINLINE LLocalEgo* GetPossessedEgoAsserted() const { LLocalEgo* Out = this->GetPossessedEgo(); jassert( Out ) return Out; }

    FORCEINLINE auto GetOwningController() const -> APersonaController* { return this->OwningController; }
                void DeclareNewPossessor(APersonaController* InNewController);

    FORCEINLINE auto IsEyeValid() const -> bool { return this->Eye.IsOwnerValid(); }
    FORCEINLINE auto GetEye() -> LEye* { return &this->Eye; }
    FORCEINLINE auto GetEye() const -> const LEye* { return &this->Eye; }

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
