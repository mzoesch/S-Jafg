// Copyright mzoesch. All rights reserved.

#include "Widgets/ColorInspector.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/VRegion.h"
#include "Nodes/TextBox.h"
#include "Nodes/GenericTabInfos.h"
#include "User/UserPreferences.h"

void Jafg::WColorInspector::Construct()
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

    auto& Fields{JUserPreferences::StaticClass().GetFields()};
    TArray<LCxxClassField const*> Targets; Targets.reserve(Fields.size());
    Targets.emplace_back(algo::find_pointer_checked(Fields, "BackgroundColor", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "AccentColor", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ForegroundColor", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ForegroundColorVariant", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "InputColor", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "OverlayColor", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "DisabledColor", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "TextColor", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColor", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColorVariant", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColor2", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColorVariant2", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ProximityColorA", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ProximityColorB", &LCxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "DangerColor", &LCxxClassField::Identifier));

    for (auto* Target : Targets)
    {
        if (!Target) { continue; }
        LColor Color{ Colors::Black };
        Serde::FromString(&Color, Target->Get(Prefs));
        Container->AddChild(NewStaticNode(WTextBox)
            .Anchor(EAnchor::HFill)
            .Tint(Color)
            .Content(LString{Target->Identifier})
            .Unique());
    }

    return;
}
