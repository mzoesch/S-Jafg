// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreFrontendWidgetsSubsystem.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "User/Frontend/Osd/FrontendScreen.h"
#include "User/Frontend/Osd/ConsoleWindow.h"

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

    auto* Viewport{ &this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetViewport() };

    this->FrontendScreen = ConstructWidgetNode<WFrontendScreen>(Viewport, this->GetOuterChecked());
    ConstructWidgetNode<WConsoleWindow>(Viewport);

    return;
}
