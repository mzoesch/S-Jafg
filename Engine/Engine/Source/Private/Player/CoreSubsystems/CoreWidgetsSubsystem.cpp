// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/CoreSubsystems/CoreWidgetsSubsystem.h"
#include "Engine/Framework/Hud.h"
#include "Player/Frontend/DebugScreen.h"
#include "Widgets/UserWidget.h"

void Jafg::JCoreWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->DebugScreen = ConstructUserWidget<WDebugScreen>(this->GetContext());
    this->DebugScreen->AddToViewport(this->GetHud()->GetMainViewport());

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
