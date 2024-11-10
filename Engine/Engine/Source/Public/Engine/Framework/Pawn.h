// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "Engine/Framework/Eye.h"
#include "Pawn.generated.h"

namespace Jafg
{

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class APlayerController;

/**
 * A pawn is something that can be possessed by a controller.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API APawn : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(APawn)

public:

    FORCEINLINE auto IsPossessed() const -> bool { return this->OwningController != nullptr; }
    FORCEINLINE auto GetOwningController() const -> APlayerController* { return this->OwningController; }
                auto DeclareNewPossessor(APlayerController* InNewController) -> void;

    FORCEINLINE auto HasEye() const -> bool { return this->Eye != nullptr; }
    FORCEINLINE auto GetEye() -> LEye* { return this->Eye; }
    FORCEINLINE auto GetEye() const -> const LEye* { return this->Eye; }

    void ProcessKeyboard(const Camera_Movement Dir, const float DeltaTime);
    void ProcessMouseMovement(const float XOffset, const float YOffset);
    void ProcessMouseScroll(const float YOffset);

private:

#if WITH_LOCAL_LAYER
    LEye* Eye = nullptr;
#endif /* WITH_LOCAL_LAYER */

    void UpdateRelativeVectors();
    LVector RelativeFront = LVector::ForwardVector;
    LVector RelativeRight = LVector::RightVector;
    LVector RelativeUp    = LVector::UpVector;

    APlayerController* OwningController = nullptr;

    float MovementSpeed    = 2.5f;
    float MouseSensitivity = 0.1f;
};

} /* ~Namespace Jafg */
