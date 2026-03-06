// Copyright mzoesch. All rights reserved.

#include "User/Frontend/JgcPauseScreen.h"
#include "Widgets/Region.h"

void Jgc::WPauseScreen::Construct()
{
    Super::Construct();

    this->SetPadding(Jafg::LPadding{5});

    Jafg::BeginStyling(*this).Root<Jafg::WRegion>()
        .Type(Jafg::ERegionBrush::Box)
        .MinDesiredSize(200_pt2)
        .Tint(Colors::Beige)
        .Radii(LVec4F{0.1f})
        .OutlineThickness(0.01f)
        .OutlineTint(Colors::Orange)
    // [
    //     NewStaticNode(Jafg::WRegion)
    //         .Type(Jafg::ERegionBrush::Box)
    //         .MinDesiredSize(50_pt2)
    //         .Tint(Colors::RebeccaPurple)
    //         .Radii(LVec4F{0.0f,0.0f,0.0f,0.1f})
    // ]
    ;

    return;
}
