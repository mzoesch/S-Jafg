// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/Framework/Pawn.h"
#include "Player/LocalPlayer.h"
#include "Engine/Engine.h"

void Jafg::APlayerController::BeginLife()
{
    AActor::BeginLife();
}

void Jafg::APlayerController::Tick(const float DeltaTime)
{
    AActor::Tick(DeltaTime);
}

void Jafg::APlayerController::EndLife()
{
    AActor::EndLife();

    this->Possess(nullptr);
    this->GetWorld()->GetEngine()->GetCheckedLocalPlayer()->Possess(nullptr);

    return;
}

void Jafg::APlayerController::Possess(APawn* InNewPawn, const bool bKillOld /* = true */)
{
    if (bKillOld && this->PossessedPawn)
    {
        this->PossessedPawn->EndLife();
    }

    this->PossessedPawn = InNewPawn;

    return;
}
