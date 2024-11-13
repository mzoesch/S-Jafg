// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Subsystems/ApplicationInstanceSubsystem.h"
#include "Engine/Engine.h"

Jafg::LApplicationInstance* Jafg::JApplicationInstanceSubsystem::GetApplicationInstance() const
{
    return GEngine->GetApplicationInstance();
}
