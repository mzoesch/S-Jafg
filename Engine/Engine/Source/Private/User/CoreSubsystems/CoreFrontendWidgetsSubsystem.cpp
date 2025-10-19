// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreFrontendWidgetsSubsystem.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "User/Frontend/Osd/FrontendScreen.h"

bool Jafg::JCoreFrontendWidgetsSubsystem::ShouldCreateSubsystem(LClassOuter const* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return Super::IsOuterFrontend(Outer);
}

void Jafg::JCoreFrontendWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->FrontendScreen = ConstructDeferredWidgetNode<WFrontendScreen>(this->GetOuter());
    this->FrontendScreen->AddToViewport(&this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetViewport());
    MakeDeferredWidgetNodeFinal(this->FrontendScreen);

    return;
}
