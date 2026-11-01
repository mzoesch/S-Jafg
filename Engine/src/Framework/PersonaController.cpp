// Copyright mzoesch. All rights reserved.

#include "Framework/PersonaController.h"
#include "Framework/PersonaControllerComponent.h"
#include "Engine/Engine.h"

#if JAFG_DO_CHECKS
void Jafg::APersonaControllerComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);

    if (!this->GetOwningActor().IsA<APersonaController>())
    {
        LOG_FATAL(LogUserInput, "Class [{}] requires to be attached a [{}] but is on [{}].",
            this->GetNameAsString(),
            APersonaController::StaticClass().GetFullyQualifiedName(),
            this->GetOwningActor().GetNameAsString()
        )
    }

    return;
}
#endif /* JAFG_DO_CHECKS */

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

TJxxUnique<Jafg::APawn> Jafg::APersonaController::PossessPawn(TJxxUnique<APawn> New, const bool bOrphanOld /* = true */)
{
    check(this->_Lives())

    TJxxUnique<APawn> Result;
    if (this->Pawn)
    {
        this->Pawn->_SetOwningController(nullptr);
        if (!bOrphanOld)
        {
            Result = std::move(this->Pawn);
        }
    }

    this->Pawn = std::move(New);
    if (this->Pawn)
    {
        this->Pawn->_SetOwningController(this);
    }

    return Result;
}
