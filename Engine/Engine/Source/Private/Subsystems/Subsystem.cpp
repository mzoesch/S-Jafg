// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Subsystems/Subsystem.h"

void Jafg::JSubsystem::Initialize(LSubsystemCollection& Collection)
{
    check( this->bIsInitialized == false)
    this->bIsInitialized = true;

    return;
}
