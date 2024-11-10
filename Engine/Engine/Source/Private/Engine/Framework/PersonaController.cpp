// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/PersonaController.h"
#include "Engine/World.h"
#include "Engine/Framework/Pawn.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"

void Jafg::APersonaController::BeginLife()
{
    AActor::BeginLife();
}

void Jafg::APersonaController::EndLife()
{
    AActor::EndLife();

    this->Possess(nullptr);
    this->GetWorld()->GetEngine()->GetCheckedLocalEgo()->Possess(nullptr);

    return;
}

void Jafg::APersonaController::Possess(APawn* InNewPawn, const bool bKillOld /* = true */)
{
    if (this->PossessedPawn)
    {
        this->PossessedPawn->DeclareNewPossessor(nullptr);
        if (bKillOld)
        {
            this->PossessedPawn->EndLife();
        }
    }

    this->PossessedPawn = InNewPawn;
    if (this->PossessedPawn)
    {
        this->PossessedPawn->DeclareNewPossessor(this);
    }

    return;
}
