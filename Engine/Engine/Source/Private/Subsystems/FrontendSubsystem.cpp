// Copyright mzoesch. All rights reserved.

#include "Subsystems/FrontendSubsystem.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"

void Jafg::JFrontendSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->OwningFrontend = GEngine->GetLocalEgoAsserted()->GetFrontend();
    checkSlow( this->OwningFrontend )

    return;
}
