// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/Frontend/DebugScreen.h"
#include "Build/EngineBuildInfo.h"
#include "Core/Application.h"
#include "Widgets/TextBlock.h"
#include "Widgets/VBox.h"

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    MakeRootNode(WVBox)
    [
        NewNode(WTextBlock)
        & LSimpleString::SprintF("JAFG {}", BuildInfo::GetEngineVersion().ToString())
        & LTextBlockBrush::MakeDefaultSmall()
    ]
    [
        NewNode(WTextBlock) >> this->TestTextBlock
        & "Hello from Wsdsml."
        & LTextBlockBrush::MakeDefaultSmall()
    ]
    [
        NewNode(WTextBlock)
        & "Hello World."
        & LTextBlockBrush::MakeDefaultSmall()
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
