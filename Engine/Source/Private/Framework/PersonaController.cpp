// Copyright mzoesch. All rights reserved.

#include "Framework/PersonaController.h"
#include "Framework/Pawn.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"

void Jafg::APersonaController::OnGarbage(ECxxRecordTearDownReason::Type Reason)
{
    Super::OnGarbage(Reason);

    if (this->IsOwningSurfaceValid())
    {
        if (Jafg::IsTearingDown() == false)
        {
            LOG_WARNING(LogEgo,
                "Persona controller [{}] is ending life while still possessed by a surface. Surface will no longer posses a controller.",
                this->GetNameAsString()
                )
        }

        this->GetOwningSurfaceChecked()->PossessController(nullptr, false);
    }

    if (this->IsOwnedPawnValid())
    {
        this->PossessPawn(nullptr);
    }

    return;
}

void Jafg::APersonaController::PossessPawn(APawn* New, const bool bKillOld /* = true */)
{
    check(this->_Lives())

    if (this->Pawn)
    {
        this->Pawn->_SetOwningController(nullptr);
        if (bKillOld)
        {
            this->Pawn->MarkAsGarbage_v2();
        }
    }

    this->Pawn = New;
    if (this->Pawn)
    {
        this->Pawn->_SetOwningController(this);
    }

    return;
}
