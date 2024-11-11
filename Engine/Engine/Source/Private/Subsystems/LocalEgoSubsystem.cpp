// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Subsystems/LocalEgoSubsystem.h"
#include "Engine/Engine.h"

Jafg::LLocalEgo* Jafg::JLocalEgoSubsystem::GetLocalEgo() const
{
    return GEngine->GetCheckedLocalEgo();
}
