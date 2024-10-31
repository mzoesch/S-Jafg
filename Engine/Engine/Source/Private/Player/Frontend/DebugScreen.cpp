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
        & "Hello from Wsdsml."
        & LTextBlockBrush { .X = 10.0f, .Y = 10.0f }
        & LColor::Black
    ]
    [
        NewNode(WTextBlock)
        & "Hello from Wsdsml2."
        & LTextBlockBrush { .X = 10.0f, .Y = 30.0f }
        & LColor::Black
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
