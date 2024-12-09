// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/CoreSubsystems/CoreWidgetsSubsystem.h"
#include "Engine/Framework/Hud.h"
#include "Widgets/UserWidget.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "User/Frontend/Hud/Crosshair.h"

void Jafg::JCoreWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->DebugScreen = ConstructDeferredWidgetNode<WDebugScreen>(this->GetOuter());
    this->DebugScreen->AddToViewport(this->GetHud()->GetMainViewport());
    this->DebugScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->DebugScreen);

    this->Crosshair = ConstructDeferredWidgetNode<WCrosshair>(this->GetOuter());
    this->Crosshair->AddToViewport(this->GetHud()->GetMainViewport());
    this->Crosshair->SetVisibility(EWidgetVisibility::Visible);
    MakeDeferredWidgetNodeFinal(this->Crosshair);

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

    return;
}
