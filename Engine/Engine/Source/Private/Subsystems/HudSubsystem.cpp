// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Subsystems/HudSubsystem.h"
#include "Player/LocalPlayer.h"
#include "Engine/Engine.h"

void Jafg::JHudSubsystem::Initialize(LSubsystemCollection& Collection)
{
    JSubsystem::Initialize(Collection);

    this->OwningHud = GEngine->GetPanickedLocalPlayer()->GetHud();
    checkSlow( this->OwningHud )

    return;
}

void Jafg::JHudSubsystem::TearDown()
{
    JSubsystem::TearDown();
}

void Jafg::JHudSubsystem::Tick(const float DeltaTime)
{
}
