// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/Frontend/DebugScreen.h"
#include "Build/EngineBuildInfo.h"
#include "Core/Application.h"
#include "User/UserPreferences.h"
#include "Widgets/TextBlock.h"
#include "Widgets/VBox.h"

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    const JUserPreferences* UserPreferences = GetDefault<JUserPreferences>();

    MakeRootNode(WVBox)
    [
        NewNode(WTextBlock)
        .SetPadding(5.0f)
        & LSimpleString::SprintF("JAFG {}", BuildInfo::GetEngineVersion().ToString())
        & LTextBlockBrush({.Tint = LColor(0, 0, 0, 128), .Scale = UserPreferences->GetSmallFontSize()})
        & LColor::White
    ]
    [
        NewNode(WTextBlock) >> this->TestTextBlock
        & "Hello from Wsdsml."
        & LTextBlockBrush({.Scale = UserPreferences->GetSmallFontSize()})
        & LColor::White
    ]
    [
        NewNode(WTextBlock)
        & "Hello World."
        & LTextBlockBrush({.Scale = UserPreferences->GetSmallFontSize()})
        & LColor::White
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
