// Copyright mzoesch. All rights reserved.

#include "Framework/PersonaController.h"
#include "Engine/Engine.h"

void Jafg::APersonaController::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    if (this->IsOwningLackeyValid())
    {
        if (!App::IsTearingDown())
        {
            LOG_WARNING(LogEgo, "[{}]: Ending life while still possessed by a lackey."
                , this->GetNameAsString())
        }

        algo::leak(this->GetOwningBaseLackey().PossessPersonaController(nullptr, true));
    }

    if (this->IsOwnedPawnValid())
    {
        this->PossessPawn(nullptr);
    }

    return;
}

void Jafg::APersonaController::PossessPawn(TJxxUnique<APawn> New, const bool bReleaseOld /* = true */)
{
    check(this->_Lives())

    if (this->Pawn)
    {
        this->Pawn->_SetOwningController(nullptr);
        if (!bReleaseOld)
        {
            this->Pawn->MarkAsGarbage_v2();
        }
    }

    this->Pawn = std::move(New);
    if (this->Pawn)
    {
        this->Pawn->_SetOwningController(this);
    }

    return;
}
