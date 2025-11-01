// Copyright mzoesch. All rights reserved.

#include "Svw/SvwPawn.h"

void Jgc::ASvwPawn::SetOwningController(Jafg::APersonaController* InNew)
{
    Super::SetOwningController(InNew);

    if (InNew == nullptr || InNew->IsLocallyPossessed() == false)
    {
        return;
    }

    return;
}
