// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "Engine/Framework/Eye.h"
#include "Pawn.generated.h"

namespace Jafg
{

class APersonaController;
struct LInputActionValue;

/**
 * A pawn is something that can be possessed by a controller.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract,)
class ENGINE_API APawn : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(APawn)

public:

    FORCEINLINE auto IsPossessed() const -> bool { return this->OwningController != nullptr; }
    FORCEINLINE auto GetOwningController() const -> APersonaController* { return this->OwningController; }
                auto DeclareNewPossessor(APersonaController* InNewController) -> void;

    FORCEINLINE auto HasEye() const -> bool { return this->Eye != nullptr; }
    FORCEINLINE auto GetEye() -> LEye* { return this->Eye; }
    FORCEINLINE auto GetEye() const -> const LEye* { return this->Eye; }

    void OnOngoingMovementInput(LInputActionValue& InValue);
    void OnOngoingRotationInput(LInputActionValue& InValue);
    void OnOngoingVelocityChange(LInputActionValue& InValue);
    void OnOngoingPrimaryInput(LInputActionValue& InValue);
    void OnOngoingSecondaryInput(LInputActionValue& InValue);

    bool TraceFromEyeByChannel(
        TdhArray<LHitResult>& OutHits,
        const float DistanceInMeters,
        const ECollisionChannel::Type Channel,
        const LCollisionQueryParams& Params
    ) const;

private:

#if WITH_LOCAL_LAYER
    LEye* Eye = nullptr;
#endif /* WITH_LOCAL_LAYER */

    void UpdateRelativeVectors();
    LVector RelativeFront = LVector::ForwardVector;
    LVector RelativeRight = LVector::RightVector;
    LVector RelativeUp    = LVector::UpVector;

    APersonaController* OwningController = nullptr;

    float MovementSpeed    = 2.5f;
    float MouseSensitivity = 0.1f;

    double LastMouseX = 0.0;
    double LastMouseY = 0.0;
    bool bFirstMouseCallback = true;
};

} /* ~Namespace Jafg */
