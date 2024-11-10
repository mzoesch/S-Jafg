// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/Pawn.h"

#include "Engine/World.h"
#include "Engine/Framework/Camera.h"

void Jafg::APawn::DeclareNewPossessor(APlayerController* InNewController)
{
    this->OwningController = InNewController;
}

Jafg::LVector Jafg::APawn::GetLocation() const
{
    const LVector LocVec = this->GetWorld()->MainCamera->Location;
    const glm::vec3 Loc = glm::vec3(LocVec.X, LocVec.Y, LocVec.Z);
    return LVector(Loc.x, Loc.y, Loc.z);
}

Jafg::LRotator Jafg::APawn::GetRotator() const
{
    return this->GetWorld()->MainCamera->Rotator;
}
