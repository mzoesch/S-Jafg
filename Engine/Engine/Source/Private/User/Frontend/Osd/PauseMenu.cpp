// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/PauseMenu.h"
#include "Widgets/WidgetRegion.h"
#include "Widgets/Compound/TabBar.h"

void Jafg::WPauseMenu::Construct()
{
    Super::Construct();

    MakeRootNode(WWidgetRegion)
    .Anchor(EAnchor::Fill)
    [
        NewNode(WWidgetRegion)
        .Anchor(EAnchor::TopRight)
        .Tint(LColor::Black)
        .Padding(50.0f)
    ]
    [
        NewNode(WTabBar) >> this->PauseTabBar
    ]
    FinishWidgetStyling()

    check( this->PauseTabBar )

    return;
}
