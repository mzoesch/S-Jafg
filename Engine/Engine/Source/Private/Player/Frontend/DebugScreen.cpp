// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/Frontend/DebugScreen.h"
#include "Core/Application.h"
#include "Widgets/TextBlock.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    MakeRootNode(WWidgetRegion)
    [
        NewNode(WTextBlock) >> this->TestTextBlock
        & LColor::Black
        & "Hello from Wsdsml."
    ]
    FinishWidgetStyling()

    return;
}

void Jafg::WDebugScreen::Tick()
{
    Super::Tick();

    if (this->TestTextBlock)
    {
        this->TestTextBlock->SetContent(LSimpleString(Application::GetFrameCount()));
    }

    return;
}
