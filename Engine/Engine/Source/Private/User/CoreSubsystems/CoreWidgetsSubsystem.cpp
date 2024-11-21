// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/CoreSubsystems/CoreWidgetsSubsystem.h"
#include "Engine/Framework/Hud.h"
#include "User/Frontend/DebugScreen.h"
#include "Widgets/UserWidget.h"

void Jafg::JCoreWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->DebugScreen = ConstructDeferredWidgetNode<WDebugScreen>(this->GetOuter());
    this->DebugScreen->AddToViewport(this->GetHud()->GetMainViewport());
    this->DebugScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->DebugScreen);

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

    return;
}
