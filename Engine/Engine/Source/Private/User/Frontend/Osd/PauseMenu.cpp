// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/PauseMenu.h"
#include "Widgets/WidgetRegion.h"
#include "Widgets/Compound/TabBar.h"

void Jafg::WPauseMenu::Construct()
{
    Super::Construct();

    MakeRootNode(WWidgetRegion)
    .SetAnchor(EAnchor::Fill)
    [
        NewNode(WWidgetRegion)
        .SetAnchor(EAnchor::TopRight)
        .SetTint(LColor::Black)
        .SetPadding(50.0f)
    ]
    // [
    //     NewNode(WTabBar)
    // ]
    FinishWidgetStyling()

    return;
}
