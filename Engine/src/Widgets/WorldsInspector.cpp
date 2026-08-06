// Copyright mzoesch. All rights reserved.

#include "Widgets/WorldsInspector.h"
#include "Engine/Engine.h"
#include "Nodes/VParent.h"
#include "Nodes/ScrollRegion.h"
#include "User/UserPreferences.h"

void Jafg::WWorldsInspector::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& Engine{this->GetEngine()};

    WParent* Container;
    BeginStyling(*this).StaticRoot<WScrollRegion>()
        .Tint(*Prefs.ForegroundColor)
        [NewStaticNode(WVParent).SaveTo(&Container).Anchor(EAnchor::Fill)];

    for (auto Idx{0uz}; Idx < Engine.GetTracks().size(); ++Idx)
    {
        auto& Track{Engine.GetTracks()[Idx]};
        check(Track.IsValid())
        auto& World{Track.GetWorld()};

        Container->AddChild(NewStaticNode(WTextBox)
            .Anchor(EAnchor::HFill)
            .Tint(Prefs.GetProximityColor(Idx))
            .Content(algo::sprintf("{}", World.GetHumanReadableName()))
            .Unique());
    }
}
