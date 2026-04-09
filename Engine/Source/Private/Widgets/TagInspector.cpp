// Copyright mzoesch. All rights reserved.

#include "Widgets/TagInspector.h"
#include "Nodes/VRegion.h"
#include "Nodes/TextBox.h"
#include "Nodes/TabOverlayForward.h"
#include "User/UserPreferences.h"

void Jafg::WTagInspector::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};

    WParent* Container;
    BeginStyling(*this).StaticRoot<WVRegion>().SaveTo(&Container)
        .Tint(*Prefs.ForegroundColor)
        .Anchor(EAnchor::Fill);
    for (auto Idx{0uz}; Idx < Detail::GetNameRegistry().GetAllocator().size(); ++Idx)
    {
        Container->AddChild();

        auto r = NewStaticNode(WTextBox).SkipTextBrushDraw(true)
                .Content(SprintF("[{}]: {}", Idx, Detail::GetNameRegistry().GetAllocator()[Idx]))
                .Unique();

    }

    return;
}
