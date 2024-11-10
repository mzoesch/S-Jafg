// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "PlayerController.generated.h"

namespace Jafg
{

class LEye;
class APawn;

DECLARE_JAFG_CLASS()
class ENGINE_API APlayerController final : public AActor
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(APlayerController)

public:

    virtual void BeginLife() override;
    virtual void EndLife() override;

    FORCEINLINE auto DoesPossess() const -> bool { return this->PossessedPawn != nullptr; }
    FORCEINLINE auto GetPossessed() const -> APawn* { return this->PossessedPawn; }
                auto Possess(APawn* InNewPawn, const bool bKillOld = true) -> void;

private:

    LEye*  EyeReference  = nullptr;
    APawn* PossessedPawn = nullptr;
};

} /* ~Namespace Jafg */
