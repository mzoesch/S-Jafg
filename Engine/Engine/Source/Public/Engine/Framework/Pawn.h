// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "Pawn.generated.h"

namespace Jafg
{

class APlayerController;

DECLARE_JAFG_CLASS()
class ENGINE_API APawn final : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(APawn)

public:

    FORCEINLINE auto IsPossessed() const -> bool { return this->OwningController != nullptr; }
    FORCEINLINE auto GetOwningController() const -> APlayerController* { return this->OwningController; }
                auto DeclareNewPossessor(APlayerController* InNewController) -> void;

    LVector GetLocation() const;

    LSimpleString GetFacingStringDebug() const;

private:

    APlayerController* OwningController = nullptr;
};

} /* ~Namespace Jafg */
