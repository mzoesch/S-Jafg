// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Subsystems/Subsystem.h"
#include "Async/TaskUtility.h"

void Jafg::JSubsystem::Initialize(LSubsystemCollection& Collection)
{
    check( Tasks::IsOnMasterThread() )
    check( this->bIsInitialized == false)
    this->bIsInitialized = true;

    return;
}
