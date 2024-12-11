// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Osd/PauseMenu.h"

#include "Widgets/WidgetRegion.h"

void Jafg::WPauseMenu::Construct()
{
    Super::Construct();

    MakeRootNode(WWidgetRegion)
    .SetPadding({80.0f, 0.0f})
    & LColor(0, 0, 0, 196)
    & (EAnchor::HLeft | EAnchor::VFill)
    FinishWidgetStyling()

    return;
}
