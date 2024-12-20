// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/ConsoleScreen.h"
#include "Widgets/EditableTextBlock.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WConsoleScreen::Construct()
{
    Super::Construct();

    this->SetPadding({ 5.0f });

    this->SetShouldTick(true);

    MakeRootNode(WWidgetRegion)
    .SetAnchor(EAnchor::VBottom | EAnchor::HFill)
    [
        NewNode(WEditableTextBlock)
        .SetTextColor(LColor::Red)
        .SetTextScale(0.5f)
        >> this->EditableTextBlock
        & LPadding({ 5.0f, 4.5f })
        & LColor(0 , 0, 0, 164)
    ]
    FinishWidgetStyling()

    return;
}

void Jafg::WConsoleScreen::OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility)
{
    Super::OnVisibilityChanged(InOldVisibility, InNewVisibility);

    if (EWidgetVisibility::IsDrawn(InNewVisibility))
    {
        checkSlow( this->EditableTextBlock )
        this->GetViewport()->FocusWidgetNode(this->EditableTextBlock);
    }

    return;
}
