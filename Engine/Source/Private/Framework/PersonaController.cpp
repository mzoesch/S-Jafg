// Copyright mzoesch. All rights reserved.

#include "Framework/PersonaController.h"
#include "Nodes/WorldNode.h"
#include "Engine/Engine.h"

void Jafg::APersonaController::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    if (this->IsOwningNodeValid())
    {
        if (Application::IsTearingDown() == false)
        {
            LOG_WARNING(LogEgo,
                "Persona controller [{}] is ending life while still possessed by a world node. World node will no longer posses a controller.",
                this->GetNameAsString()
                )
        }

        this->GetOwningNodeChecked()->PossessPersonaController(nullptr, true);
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
        if (bReleaseOld == false)
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
