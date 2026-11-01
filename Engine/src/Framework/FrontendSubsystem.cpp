// Copyright mzoesch. All rights reserved.

#include "Framework/FrontendSubsystem.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"

bool Jafg::JFrontendSubsystem::ShouldCreateSubsystem() const noexcept
{
    if (!JSubsystem::ShouldCreateSubsystem())
    {
        return false;
    }

    if (this->GetFrontend().IsHeadless())
    {
        return false;
    }

    return true;
}

Jafg::LFrontend& Jafg::JFrontendSubsystem::GetFrontend() noexcept
{
    check(Detail::GMutableEngine && "Absence of GEngine while JFrontendSubsystem exists is invalid.")
    return Detail::GMutableEngine->GetLocalEgo().GetFrontend();
}

Jafg::LFrontend const& Jafg::JFrontendSubsystem::GetFrontend() const noexcept
{
    check(GEngine && "Absence of GEngine while JFrontendSubsystem exists is invalid.")
    return GEngine->GetLocalEgo().GetFrontend();
}
