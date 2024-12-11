// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/ChatScreen.h"
#include "Widgets/EditableTextBlock.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WConsoleScreen::Construct()
{
    Super::Construct();

    this->SetPadding({ 5.0f });

    MakeRootNode(WWidgetRegion)
    .SetAnchor(EAnchor::VBottom | EAnchor::HFill)
    [
        NewNode(WEditableTextBlock)
        .SetPadding({ 10.0f })
        & EAnchor::Fill
        & LColor(0 , 0, 0, 164)
    ]
    FinishWidgetStyling()

    return;
}
