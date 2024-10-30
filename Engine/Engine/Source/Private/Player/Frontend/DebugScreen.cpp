// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/Frontend/DebugScreen.h"
#include "Widgets/TextBlock.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    this->ReplaceRoot(ConstructWidgetNode<WWidgetRegion>());

    this->GetRoot()->AddChild(ConstructWidgetNode<WTextBlock>());

    return;
}

void Jafg::WDebugScreen::Tick()
{
    Super::Tick();
}
