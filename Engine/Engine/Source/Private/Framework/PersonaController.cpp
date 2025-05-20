// Copyright mzoesch. All rights reserved.

#include "Framework/PersonaController.h"
#include "Engine/World.h"
#include "Framework/Pawn.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"

void Jafg::APersonaController::BeginLife()
{
    Super::BeginLife();
}

void Jafg::APersonaController::EndLife()
{
    Super::EndLife();

    if (this->IsLocalEgoValid())
    {
        this->LocalEgo->Possess(nullptr);
    }

    this->Possess(nullptr);

    return;
}

void Jafg::APersonaController::Possess(APawn* InNewPawn, const bool bKillOld /* = true */)
{
    /* Otherwise, we will get access violations. */
    APawn* OldPawn = bKillOld ? nullptr : this->PossessedPawn;

    if (this->PossessedPawn)
    {
        this->PossessedPawn->DeclareNewPossessor(nullptr);
        if (bKillOld)
        {
            this->PossessedPawn->MarkAsGarbage();
        }
    }

    this->PossessedPawn = InNewPawn;
    if (this->PossessedPawn)
    {
        this->PossessedPawn->DeclareNewPossessor(this);
    }

    if (this->IsLocalEgoValid())
    {
        this->LocalEgo->OnNewPawnPossessed(OldPawn, this->PossessedPawn);
    }

    return;
}
