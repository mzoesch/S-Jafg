// Copyright mzoesch. All rights reserved.

#include "Subsystems/TickableWorldSubsystem.h"
#include "Engine/World.h"

void Jafg::JTickableWorldSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->GetWorld().RegisterTickableObject(this);
    return;
}

void Jafg::JTickableWorldSubsystem::TearDown()
{
    Super::TearDown();
    this->GetWorld().UnregisterTickableObject(this);
    return;
}
