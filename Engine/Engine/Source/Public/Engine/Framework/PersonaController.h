// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Framework/Controller.h"
#include "PersonaController.generated.h"

namespace Jafg
{

class LEye;
class APawn;

DECLARE_JAFG_CLASS()
class ENGINE_API APersonaController final : public AController
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(APersonaController)

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
