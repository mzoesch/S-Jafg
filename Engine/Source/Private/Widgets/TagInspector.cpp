// Copyright mzoesch. All rights reserved.

#include "Widgets/TagInspector.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/VRegion.h"
#include "Nodes/TextBox.h"
#include "Nodes/GenericTabInfos.h"
#include "User/UserPreferences.h"

void Jafg::WTagInspector::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};

    WParent* Container;
    BeginStyling(*this).StaticRoot<WScrollRegion>()
        .Tint(*Prefs.ForegroundColor)
    [
        NewStaticNode(WVRegion).SaveTo(&Container)
            .Anchor(EAnchor::Fill)
            .SkipBrushDraw(true)
    ];

    u32 Width{Detail::GetJxxTagRegistry().GetAllocator().empty()
        ? 1
        : static_cast<u32>(maths::log10(Detail::GetJxxTagRegistry().GetAllocator().size() - 1)) + 1
        };
    for (auto Idx{0uz}; Idx < Detail::GetJxxTagRegistry().GetAllocator().size(); ++Idx)
    {
        Container->AddChild(NewStaticNode(WTextBox)
            .Anchor(EAnchor::HFill)
            .Tint((!!(Idx % 2)) ? *Prefs.ProximityColorA : *Prefs.ProximityColorB)
            .Content(SprintF("{:0{}} -- {}", Idx, Width, Detail::GetJxxTagRegistry().GetAllocator()[Idx]))
            .Unique());
    }
    return;
}
