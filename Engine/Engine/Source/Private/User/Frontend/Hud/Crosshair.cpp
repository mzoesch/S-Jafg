// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Frontend/Hud/Crosshair.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WCrosshair::Construct()
{
    Super::Construct();

    MakeRootNode(WWidgetRegion).Anchor(EAnchor::Fill)
    [
        NewNode(WWidgetRegion)
            .Anchor(EAnchor::CenterCenter)
            .Padding({1.0f, 4.0f})
            .Tint(LColor::White)
        + NewNode(WWidgetRegion)
            .Anchor(EAnchor::CenterCenter)
            .Padding({4.0f, 1.0f})
            .Tint(LColor::White)
    ]
    FinishWidgetStyling()

    return;
}
