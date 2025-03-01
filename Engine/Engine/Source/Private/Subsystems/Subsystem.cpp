// Copyright mzoesch. All rights reserved.

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
    check( GEngine )
    return GEngine;
}

Jafg::LLocalEgo* Jafg::JSubsystem::GetLocalEgo() const
{
    return this->GetEngine()->GetLocalEgo();
}

Jafg::LCommandLineInterface* Jafg::JSubsystem::GetCommandLineInterface() const
{
    return this->GetEngine()->GetCommandLineInterface();
}
