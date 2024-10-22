// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/PlayerController.h"
#include "Engine/World.h"
#include "Player/LocalPlayer.h"

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

    this->GetWorld()->GetEngine()->GetCheckedLocalPlayer()->Possess(nullptr);

    return;
}
