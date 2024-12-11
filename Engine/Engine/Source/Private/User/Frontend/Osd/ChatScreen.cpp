// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/ChatScreen.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WChatScreen::Construct()
{
    Super::Construct();

    this->SetPadding({ 5.0f });

    MakeRootNode(WWidgetRegion)
    .SetPadding({0.0f, 10.0f})
    & LColor(0, 0, 0, 196)
    & (EAnchor::VBottom | EAnchor::HFill)
    FinishWidgetStyling()

    return;
}
