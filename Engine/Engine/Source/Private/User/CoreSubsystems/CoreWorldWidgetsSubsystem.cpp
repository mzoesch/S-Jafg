// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreWorldWidgetsSubsystem.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "Widgets/UserWidget.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "User/Frontend/Osd/DebugMenu.h"
#include "User/Frontend/Hud/Crosshair.h"
#include "User/Frontend/Osd/ConsoleScreen.h"
#include "User/Frontend/Osd/PauseScreen.h"

bool Jafg::JCoreWorldWidgetsSubsystem::ShouldCreateSubsystem(LClassOuter const* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return Super::IsOuterWorld(Outer);
}

void Jafg::JCoreWorldWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    LFrontend& Frontend{ this->GetLocalEgo().GetFrontend() };

    this->DebugScreen = ConstructDeferredWidgetNode<WDebugScreen>(this->GetOuter());

    this->DebugScreen->AddToViewport(&Frontend.GetFocusedSurfaceChecked()->GetViewport());
    this->DebugScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->DebugScreen);

    this->DebugMenu = ConstructDeferredWidgetNode<WDebugMenu>(this->GetOuter());
    this->DebugMenu->AddToViewport(&Frontend.GetFocusedSurfaceChecked()->GetViewport());
    this->DebugMenu->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->DebugMenu);

    this->Crosshair = ConstructDeferredWidgetNode<WCrosshair>(this->GetOuter());
    this->Crosshair->AddToViewport(&Frontend.GetFocusedSurfaceChecked()->GetViewport());
    this->Crosshair->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
    MakeDeferredWidgetNodeFinal(this->Crosshair);

    this->ConsoleScreen = ConstructDeferredWidgetNode<WConsoleScreen>(this->GetOuter());
    this->ConsoleScreen->AddToViewport(&Frontend.GetFocusedSurfaceChecked()->GetViewport());
    this->ConsoleScreen->SetConsoleFrontendState(EConsoleScreenState::Hide);
    MakeDeferredWidgetNodeFinal(this->ConsoleScreen);

    this->PauseScreen = ConstructDeferredWidgetNode<WPauseScreen>(this->GetOuter());
    this->PauseScreen->AddToViewport(&Frontend.GetFocusedSurfaceChecked()->GetViewport());
    this->PauseScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->PauseScreen);

    return;
}
