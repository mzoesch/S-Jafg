// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/Frontend/DebugScreen.h"
#include "Core/Application.h"
#include "Widgets/TextBlock.h"
#include "Widgets/VBox.h"

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    MakeRootNode(WVBox)
    .SetPadding(5.0f)
    [
        NewNode(WTextBlock) >> this->TestTextBlock
        & "Hello from Wsdsml."
        & LColor::Black
    ]
    [
        NewNode(WTextBlock)
        & "AAAAAAAAAAAAAAAAAAAAAAAAAAA"
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
