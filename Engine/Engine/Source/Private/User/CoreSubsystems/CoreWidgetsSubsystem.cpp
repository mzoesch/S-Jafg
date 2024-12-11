// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/CoreSubsystems/CoreWidgetsSubsystem.h"
#include "Engine/Framework/Hud.h"
#include "Widgets/UserWidget.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "User/Frontend/Hud/Crosshair.h"
#include "User/Frontend/Osd/ChatScreen.h"
#include "User/Frontend/Osd/PauseMenu.h"

void Jafg::JCoreWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->DebugScreen = ConstructDeferredWidgetNode<WDebugScreen>(this->GetOuter());
    this->DebugScreen->AddToViewport(this->GetHud()->GetMainViewport());
    this->DebugScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->DebugScreen);

    this->Crosshair = ConstructDeferredWidgetNode<WCrosshair>(this->GetOuter());
    this->Crosshair->AddToViewport(this->GetHud()->GetMainViewport());
    // this->Crosshair->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
    this->Crosshair->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->Crosshair);

    this->ChatScreen = ConstructDeferredWidgetNode<WConsoleScreen>(this->GetOuter());
    this->ChatScreen->AddToViewport(this->GetHud()->GetMainViewport());
    this->ChatScreen->SetVisibility(EWidgetVisibility::IntransitiveHitTestInvisible);
    MakeDeferredWidgetNodeFinal(this->ChatScreen);

    this->PauseMenu = ConstructDeferredWidgetNode<WPauseMenu>(this->GetOuter());
    this->PauseMenu->AddToViewport(this->GetHud()->GetMainViewport());
    this->PauseMenu->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->PauseMenu);

    return;
}

void Jafg::JCoreWidgetsSubsystem::TearDown()
{
    Super::TearDown();

    if (ensure(this->DebugScreen))
    {
        this->DebugScreen->RemoveFromParent();
        this->DebugScreen = nullptr;
    }

    if (ensure(this->Crosshair))
    {
        this->Crosshair->RemoveFromParent();
        this->Crosshair = nullptr;
    }

    if (ensure(this->ChatScreen))
    {
        this->ChatScreen->RemoveFromParent();
        this->ChatScreen = nullptr;
    }

    return;
}
