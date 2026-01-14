// Copyright mzoesch. All rights reserved.

#include "Subsystems/TickableWorldSubsystem.h"
#include "Engine/World.h"

void Jafg::JTickableWorldSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    check( this->GetWorld() )
    this->GetWorld()->RegisterTickableObject(this);

    return;
}

void Jafg::JTickableWorldSubsystem::TearDown(LClassOuter& PreviousOuter)
{
    JWorldSubsystem::TearDown(PreviousOuter);

    check( this->GetWorld() == nullptr )
    PreviousOuter.AsWorld()->UnregisterTickableObject(this);

    return;
}
