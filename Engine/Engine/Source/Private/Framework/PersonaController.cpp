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
                "Persona controller [{}] is ending life while still possessed by a surface. Surface will no longer posses a controller.",
                this->GetNameAsString()
                )
        }

        this->GetSurfaceChecked()->PossessController(nullptr, false);
    }

    if (this->IsPossessedPawnValid())
    {
        this->PossessPawn(nullptr);
    }

    return;
}

void Jafg::APersonaController::PossessPawn(APawn* New, const bool bKillOld /* = true */)
{
    /* Otherwise, we will get access violations. */
    APawn* OldPawn{ bKillOld ? nullptr : this->Pawn };

    if (this->Pawn)
    {
        if (auto* Surface{ this->GetSurface() })
        {
            algo::erase_once_checked(
                &Surface->GetViewport().GetMutableBackgroundContexts(),
                &this->Pawn->GetEye(),
                &LBackgroundContext::Eye
                );
        }

        this->Pawn->SetOwningController(nullptr);
        if (bKillOld)
        {
            this->Pawn->MarkAsGarbage_v2();
        }
    }

    this->Pawn = New;
    if (this->Pawn)
    {
        this->Pawn->SetOwningController(this);

        if (auto* Surface{ this->GetSurface()})
        {
            Surface->GetViewport().GetMutableBackgroundContexts().emplace_back(
                &this->Pawn->GetEye(),
                this->Pawn->GetWorldChecked()
                );
        }
    }

    this->GetLocalEgo().ForEachMutableSubsystem([OldPawn, New](JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPawnPossessed(OldPawn, New);
    });

    return;
}
