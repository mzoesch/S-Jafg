// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Hud/Crosshair.h"

#include "Widgets/WidgetRegion.h"

void Jafg::WCrosshair::Construct()
{
    Super::Construct();

    MakeRootNode(WWidgetRegion)
    .SetAnchor(EAnchor::Fill)
    [
        NewNode(WWidgetRegion)
        .SetPadding({1.0f, 4.0f})
        & LColor(255, 255, 255, 255)
        & EAnchor::CenterCenter
    ]
    [
        NewNode(WWidgetRegion)
        .SetPadding({4.0f, 1.0f})
        & LColor(255, 255, 255, 255)
        & EAnchor::CenterCenter
    ]
    FinishWidgetStyling()

    return;
}
