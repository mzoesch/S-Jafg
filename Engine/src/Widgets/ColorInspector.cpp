// Copyright mzoesch. All rights reserved.

#include "Widgets/ColorInspector.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/VParent.h"
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
        [NewStaticNode(WVParent).SaveTo(&Container).Anchor(EAnchor::Fill)];

    auto& Fields{JUserPreferences::StaticClass().GetFieldsOfThisClassOnly()};
    TArray<LJxxClassField const*> Targets; Targets.reserve(Fields.size());
    Targets.emplace_back(algo::find_pointer_checked(Fields, "BackgroundColor"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "AccentColor"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ForegroundColor"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ForegroundColorVariant"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "InputColor"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "OverlayColor"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "DisabledColor"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "TextColor"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColor"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColorVariant"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColor2"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "PrimaryColorVariant2"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ProximityColorA"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "ProximityColorB"sv, &LJxxClassField::Identifier));
    Targets.emplace_back(algo::find_pointer_checked(Fields, "DangerColor"sv, &LJxxClassField::Identifier));

    for (auto* Target : Targets)
    {
        if (!Target) { continue; }
        LColor Color{ Colors::Black };
        serde::FromString(&Color, Target->Get(Prefs));
        Container->AddChild(NewStaticNode(WTextBox)
            .Anchor(EAnchor::HFill)
            .Tint(Color)
            .Content(LString{Target->Identifier})
            .Unique());
    }

    return;
}
