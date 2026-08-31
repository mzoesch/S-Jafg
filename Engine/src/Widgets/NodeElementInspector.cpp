// Copyright mzoesch. All rights reserved.

#include "Widgets/NodeElementInspector.h"
#include "Nodes/Button.h"
#include "Nodes/HParent.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "Nodes/VParent.h"
#include "User/UserPreferences.h"

void Jafg::WNodeElementInspector::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto GetButtons{[&]
    {
        return NewStaticNode(WVParent).Anchor(EAnchor::Fill).Space(10_pt)
        [
            NewStaticNode(WHParent).Anchor(EAnchor::HFill).Space(5_pt)
            [
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Button")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PrimaryPaletteSolid)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Selected")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PrimaryPaletteSolid)
                    .Selectable(true).Selected(true)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Disabled")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PrimaryPaletteSolid)
                    .Enabled(false)
                ]
            +
            NewStaticNode(WHParent).Anchor(EAnchor::HFill).Space(5_pt)
            [
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Button")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PrimaryPaletteLine)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Selected")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PrimaryPaletteLine)
                    .Selectable(true).Selected(true)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Disabled")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PrimaryPaletteLine)
                    .Enabled(false)
            ]
            +
            NewStaticNode(WHParent).Anchor(EAnchor::HFill).Space(5_pt)
            [
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Button")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PalePrimaryPaletteSolid)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Selected")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PalePrimaryPaletteSolid)
                    .Selectable(true).Selected(true)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Disabled")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PalePrimaryPaletteSolid)
                    .Enabled(false)
                ]
            +
            NewStaticNode(WHParent).Anchor(EAnchor::HFill).Space(5_pt)
            [
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Button")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PalePrimaryPaletteLine)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Selected")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PalePrimaryPaletteLine)
                    .Selectable(true).Selected(true)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Disabled")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.PalePrimaryPaletteLine)
                    .Enabled(false)
            ]
            +
            NewStaticNode(WHParent).Anchor(EAnchor::HFill).Space(5_pt)
            [
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Button")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.SecondaryPaletteSolid)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Selected")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.SecondaryPaletteSolid)
                    .Selectable(true).Selected(true)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Disabled")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.SecondaryPaletteSolid)
                    .Enabled(false)
                ]
            +
            NewStaticNode(WHParent).Anchor(EAnchor::HFill).Space(5_pt)
            [
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Button")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.SecondaryPaletteLine)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Selected")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.SecondaryPaletteLine)
                    .Selectable(true).Selected(true)
                +
                NewStaticNode(WTextButton)
                    .MinDesiredSize({100_spt, 0})
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign, &LTextBoxBrush::TextVAlign>(ETextHAlign::Center, ETextVAlign::Center)
                    .Content("Disabled")
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt, 2})
                    .Palette(*Prefs.SecondaryPaletteLine)
                    .Enabled(false)
            ]
        ];
    }};

    BeginStyling(*this).StaticRoot<WScrollRegion>()
        .Padding({10_spt})
        .Tint(*Prefs.BackgroundColor)
    [
        NewStaticNode(WHParent).Space(10_spt).Anchor(EAnchor::Fill).MaxDesiredSize({1'000_spt, 0})
        [
            NewStaticNode(WRegion).Anchor(EAnchor::Fill).Padding({10_spt}).Tint(LColor{0x24})[GetButtons()]
            +
            NewStaticNode(WRegion).Anchor(EAnchor::Fill).Padding({10_spt}).Tint(Colors::White)[GetButtons()]
        ]
    ];
}
