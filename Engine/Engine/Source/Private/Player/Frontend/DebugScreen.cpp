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
    .SetPadding(5.0f)
    [
        NewNode(WTextBlock) >> this->TestTextBlock
        & "Hello from Wsdsml."
        & LColor::Black
        & LTextBlockBrush({.Scale = UserPreferences->GetSmallFontSize()})
    ]
    [
        NewNode(WTextBlock)
        & "My text block."
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
