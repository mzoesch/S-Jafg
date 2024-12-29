// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Subsystems/Subsystem.h"
#include "Async/TaskUtility.h"
#include "Engine/Engine.h"

void Jafg::JSubsystem::Initialize(LSubsystemCollection& Collection)
{
    check( Tasks::IsOnMasterThread() )
    check( this->bIsInitialized == false)
    this->bIsInitialized = true;

    return;
}

Jafg::LEngine* Jafg::JSubsystem::GetEngine() const
{
    return GEngine;
}

Jafg::LCommandLineInterface* Jafg::JSubsystem::GetCommandLineInterface() const
{
    return this->GetEngine()->GetCommandLineInterface();
}

Jafg::LApplicationInstance* Jafg::JSubsystem::GetApplicationInstance() const
{
    return this->GetEngine()->GetApplicationInstance();
}
