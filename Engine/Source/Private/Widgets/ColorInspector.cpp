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
    TArray<LJxxClassField const*> Targets; Targets.reserve(Fields.size());
    Targets.emplace_back(algo::find_pointer_checked(Fields, "BackgroundColor", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "AccentColor", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ForegroundColor", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ForegroundColorVariant", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "InputColor", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "OverlayColor", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "DisabledColor", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "TextColor", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColor", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColorVariant", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColor2", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColorVariant2", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ProximityColorA", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ProximityColorB", &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "DangerColor", &LJxxClassField::Identifier));

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
