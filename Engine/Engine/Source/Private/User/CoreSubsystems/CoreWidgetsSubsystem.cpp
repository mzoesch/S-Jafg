    // Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/CoreSubsystems/CoreWidgetsSubsystem.h"
#include "Engine/Framework/Hud.h"
#include "Widgets/UserWidget.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "User/Frontend/Hud/Crosshair.h"
#include "User/Frontend/Osd/ConsoleScreen.h"
#include "User/Frontend/Osd/PauseScreen.h"

    void Jafg::JCoreWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->DebugScreen = ConstructDeferredWidgetNode<WDebugScreen>(this->GetOuter());
    this->DebugScreen->AddToViewport(&this->GetHud()->GetMainViewport());
    this->DebugScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->DebugScreen);

    this->Crosshair = ConstructDeferredWidgetNode<WCrosshair>(this->GetOuter());
    this->Crosshair->AddToViewport(&this->GetHud()->GetMainViewport());
    this->Crosshair->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
    MakeDeferredWidgetNodeFinal(this->Crosshair);

    this->ConsoleScreen = ConstructDeferredWidgetNode<WConsoleScreen>(this->GetOuter());
    this->ConsoleScreen->AddToViewport(&this->GetHud()->GetMainViewport());
    this->ConsoleScreen->SetConsoleFrontendState(EConsoleScreenState::Hide);
    MakeDeferredWidgetNodeFinal(this->ConsoleScreen);

    this->PauseScreen = ConstructDeferredWidgetNode<WPauseScreen>(this->GetOuter());
    this->PauseScreen->AddToViewport(&this->GetHud()->GetMainViewport());
    this->PauseScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->PauseScreen);

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

    if (ensure(this->ConsoleScreen))
    {
        this->ConsoleScreen->RemoveFromParent();
        this->ConsoleScreen = nullptr;
    }

    return;
}
