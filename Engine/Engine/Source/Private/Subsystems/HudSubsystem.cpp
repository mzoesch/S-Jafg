// Copyright mzoesch. All rights reserved.

#include "Subsystems/HudSubsystem.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"

void Jafg::JHudSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->OwningHud = GEngine->GetLocalEgoAsserted()->GetHud();
    checkSlow( this->OwningHud )

    return;
}
