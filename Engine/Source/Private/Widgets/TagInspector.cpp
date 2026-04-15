// Copyright mzoesch. All rights reserved.

#include "Widgets/TagInspector.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/VRegion.h"
#include "Nodes/TextBox.h"
#include "Nodes/TabOverlayForward.h"
#include "User/UserPreferences.h"

void Jafg::WTagInspector::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};

    WParent* Container;
    BeginStyling(*this).StaticRoot<WScrollRegion>()
        .Tint(*Prefs.ForegroundColor)
        .VScrollBarBackgroundWith(4)
    [
        NewStaticNode(WVRegion).SaveTo(&Container)
            .Anchor(EAnchor::Fill)
            .SkipBrushDraw(true)
    ];

    u32 Width{Detail::GetNameRegistry().GetAllocator().empty()
        ? 1
        : static_cast<u32>(maths::log10(Detail::GetNameRegistry().GetAllocator().size() - 1)) + 1
        };
    for (auto Idx{0uz}; Idx < Detail::GetNameRegistry().GetAllocator().size() || Idx < 10; ++Idx)
    {
        Container->AddChild(NewStaticNode(WTextBox).SkipBrushDraw(true)
            .Content(SprintF("{:0{}} -- {}", Idx, Width, Detail::GetNameRegistry().GetAllocator()[Idx]))
            .Unique());
    }

    return;
}
