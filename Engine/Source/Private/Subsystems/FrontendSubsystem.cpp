// Copyright mzoesch. All rights reserved.

#include "Subsystems/FrontendSubsystem.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"

Jafg::LFrontend& Jafg::JFrontendSubsystem::GetFrontend() noexcept
{
    check(GMutableEngine && "Absence of GEngine while JFrontendSubsystem exists is invalid.")
    return GMutableEngine->GetLocalEgo().GetFrontend();
}

Jafg::LFrontend const& Jafg::JFrontendSubsystem::GetFrontend() const noexcept
{
    check(GEngine && "Absence of GEngine while JFrontendSubsystem exists is invalid.")
    return GEngine->GetLocalEgo().GetFrontend();
}
