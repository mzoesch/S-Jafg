// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/Frontend/DebugScreen.h"
#include "Widgets/TextBlock.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    this->ReplaceRoot(ConstructDeferredWidgetNode<WWidgetRegion>());

    this->GetRoot()->AddChild(ConstructDeferredWidgetNode<WTextBlock>());

    ConstructDeferredWidgetNode(this->GetRoot());

    return;
}

void Jafg::WDebugScreen::Tick()
{
    Super::Tick();
}
