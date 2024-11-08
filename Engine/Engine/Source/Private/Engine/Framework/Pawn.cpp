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

Jafg::LSimpleString Jafg::APawn::GetFacingStringDebug() const
{
    const float Yaw = this->GetWorld()->MainCamera->Rotator.Yaw;

    LSimpleString YawAsText = "N/A";
    if (Yaw >= -45.f && Yaw <= 45.f)
    {
        YawAsText = "north (Towards positive X)";
    }
    else if (Yaw > 45.f && Yaw < 135.f)
    {
        YawAsText = "east (Towards positive Y)";
    }
    else if (Yaw >= 135.f || Yaw <= -135.f)
    {
        YawAsText = "south (Towards negative X)";
    }
    else if (Yaw > -135.f && Yaw < -45.f)
    {
        YawAsText = "west (Towards negative Y)";
    }

    return LSimpleString::SprintF(
        "Facing: {} ({:.2f}Y / {:.2f}P)",
        YawAsText,
        this->GetWorld()->MainCamera->Rotator.Yaw,
        this->GetWorld()->MainCamera->Rotator.Pitch
    );
}
