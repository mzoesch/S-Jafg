// Copyright mzoesch. All rights reserved.

#include "Framework/PersonaController.h"
#include "Engine/World.h"
#include "Framework/Pawn.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"

void Jafg::APersonaController::EndLife()
{
    Super::EndLife();

    if (this->IsSurfaceValid())
    {
        if (Jafg::IsTearingDown() == false)
        {
            LOG_WARNING(LogEgo,
                "Persona controller [{}] is ending life while still possessed by local ego. Local ego will no longer posses a controller.",
                this->GetNameAsString()
                )
        }

        this->GetSurfaceChecked()->Possess(nullptr);
    }

    if (this->DoesPossess())
    {
        this->Possess(nullptr);
    }

    return;
}

void Jafg::APersonaController::Possess(APawn* NewPawn, const bool bKillOld /* = true */)
{
    /* Otherwise, we will get access violations. */
    APawn* OldPawn{ bKillOld ? nullptr : this->Pawn };

    if (this->Pawn)
    {
        if (auto* Surface{ this->GetSurface()})
        {
            algo::erase_once_checked(
                &Surface->GetViewport().GetMutableBackgroundContexts(),
                &this->Pawn->GetEye(),
                &LBackgroundContext::Eye
                );
        }

        this->Pawn->DeclareNewPossessor(nullptr);
        if (bKillOld)
        {
            this->Pawn->MarkAsGarbage_v2();
        }
    }

    this->Pawn = NewPawn;
    if (this->Pawn)
    {
        this->Pawn->DeclareNewPossessor(this);

        if (auto* Surface{ this->GetSurface()})
        {
            Surface->GetViewport().GetMutableBackgroundContexts().emplace_back(
                &this->Pawn->GetEye(),
                this->Pawn->GetWorldChecked()
                );
        }
    }

    this->GetLocalEgo().ForEachMutableSubsystem([OldPawn, NewPawn](JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPawnPossessed(OldPawn, NewPawn);
    });

    return;
}
