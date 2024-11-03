// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/Frontend/DebugScreen.h"
#include "Core/Application.h"
#include "User/UserPreferences.h"
#include "Widgets/TextBlock.h"
#include "Widgets/VBox.h"

void Jafg::WDebugScreen::Construct()
{
    Super::Construct();

    const JUserPreferences* UserPreferences = GetDefault<JUserPreferences>();

    MakeRootNode(WVBox)
    .SetRegionPadding(5.0f)
    .SetBrush(LRegionBrush({.Tint = LColor::Cyan}))
    [
        NewNode(WTextBlock) >> this->TestTextBlock
        & "Hello from Wsdsml."
        & LColor::Black
        & LTextBlockBrush({.Scale = UserPreferences->GetSmallFontSize()})
    ]
    [
        NewNode(WTextBlock)
        & "Hello World."
        & LColor::Black
        & LTextBlockBrush({.Scale = UserPreferences->GetSmallFontSize()})
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
