// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Hud/Crosshair.h"
#include "Widgets/Region.h"

void Jafg::WCrosshair::Construct()
{
    Super::Construct();

    MakeRootNode(WRegion).Anchor(EAnchor::Fill)
    [
        NewNode(WRegion)
            .Anchor(EAnchor::CenterCenter)
            .Type(ERegionBrush::Box)
            .Padding({1_spt, 4})
            .Tint(Lal::LColor::White)
        + NewNode(WRegion)
            .Anchor(EAnchor::CenterCenter)
            .Type(ERegionBrush::Box)
            .Padding({4_spt, 1})
            .Tint(Lal::LColor::White)
    ]
    FinishWidgetStyling()

    return;
}
