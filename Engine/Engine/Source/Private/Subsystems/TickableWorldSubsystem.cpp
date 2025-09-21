// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "Subsystems/TickableWorldSubsystem.h"
#include "Engine/World.h"

void Jafg::JTickableWorldSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    check( this->GetWorld() )
    this->GetWorld()->RegisterTickableObject(this);

    return;
}

void Jafg::JTickableWorldSubsystem::TearDown()
{
    JWorldSubsystem::TearDown();

    check( this->GetWorld() )
    this->GetWorld()->UnregisterTickableObject(this);

    return;
}
