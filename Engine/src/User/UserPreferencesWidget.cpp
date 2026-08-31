// Copyright mzoesch. All rights reserved.

#include "User/UserPreferencesWidget.h"
#include "Nodes/Button.h"
#include "Nodes/HButton.h"
#include "Nodes/HParent.h"
#include "User/UserPreferences.h"
#include "User/PreferenceCollection.h"
#include "User/PreferenceValue.h"
#include "Nodes/VRegion.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/Separator.h"
#include "Nodes/Spacer.h"
#include "Nodes/Switcher.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "Nodes/VParent.h"

void Jafg::WUserPreferencesWidget::Construct()
{
    Super::Construct();
    this->Rebuild(0uz);
}

void Jafg::WUserPreferencesWidget::Tick()
{
    Super::Tick();

    this->DiscardButton->SetEnabled(!this->Changes.empty());
    this->ApplyButton->SetEnabled(!this->Changes.empty());
}

void Jafg::WUserPreferencesWidget::Rebuild(std::size_t Index)
{
    check(this->GetChildren().empty())
    check(this->Changes.empty())

    auto& Prefs{GetSingleton<JUserPreferences>()};

    WVRegion* TopLevelPreferencesContainer{};
    WHParent* HRegion;
    WSwitcher* Switcher;
    BeginStyling(*this).StaticRoot<WHParent>().SaveTo(&HRegion)
        .Anchor(EAnchor::Fill)
    [
        NewStaticNode(WVRegion).SaveTo(&TopLevelPreferencesContainer)
            .Anchor(EAnchor::VFill)
            .MinDesiredSize({200_spt, 0})
            .Tint(*Prefs.ForegroundColor)
        [
            NewStaticNode(WSpacer).MinDesiredSize({0_pt,25})
        ]
        +
        NewStaticNode(WVRegion)
            .Anchor(EAnchor::Fill)
            .Tint(*Prefs.ForegroundColorVariant)
        [
            NewStaticNode(WSwitcher).SaveTo(&Switcher)
                .Anchor(EAnchor::Fill)
            +
            NewStaticNode(WHSeparator).Anchor(EAnchor::HFill)
            +
            NewStaticNode(WHParent)
                .Anchor(EAnchor::HFill)
                .Padding({10_pt, 4})
            [
                NewStaticNode(WSpacer).Anchor(EAnchor::Fill)
                +
                NewStaticNode(WTextButton).SaveTo(&this->DiscardButton)
                    .Content("Discard")
                    .Palette(*Prefs.SecondaryPaletteSolid)
                    .InAllTextBrushes<&LTextBrush::TextScale>(ETextScale::Header)
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt,2})
                    .OnKeyEventFocused([this,Switcher](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Self.AsStatic<WTextButton>().IsEnabled() && Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                        {
                            LOG_VERBOSE(LogWidgets, "Discarding [{}] modified preferences.", this->Changes.size())
                            auto Index{Switcher->GetActiveNodeIndex()};
                            this->Changes.clear();
                            this->RemoveChildren();
                            this->Rebuild(Index ? *Index : 0uz);
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                +
                NewStaticNode(WTextButton).SaveTo(&this->ApplyButton)
                    .Content("Apply")
                    .Palette(*Prefs.SecondaryPaletteSolid)
                    .InAllTextBrushes<&LTextBrush::TextScale>(ETextScale::Header)
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt,2})
                    .OnKeyEventFocused([this,Switcher](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Self.AsStatic<WTextButton>().IsEnabled() && Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                        {
                            auto Index{Switcher->GetActiveNodeIndex()};
                            LOG_VERBOSE(LogWidgets, "Applying [{}] modified preferences.", this->Changes.size())
                            for (auto& Change: this->Changes)
                            {
                                LOG_VERBOSE(LogWidgets, "    [{}]: {}", Change.Preference->Identifier, Change.Value)
                                Change.Preference->SaveValue(std::move(Change.Value));
                            }
                            this->Changes.clear();
                            this->RemoveChildren();
                            this->Rebuild(Index ? *Index : 0uz);
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
            ]
        ]
    ];

    auto AddPreference{[Prefs=&Prefs](this auto&& Self, WParent& Target, TArray<LPreferenceValueChangeRequest>& Changes, LPreference& Preference) -> void
    {
        if (Preference.IsCollection())
        {
            Target.AddChild(NewNode(Target.GetViewport()).Class<WText>()
                .TextScale(ETextScale::SubHeader)
                .Content(Preference.DisplayName)
                .Unique());
            for (auto& SubPreference: Preference.AsCollection())
            {
                Self(Target, Changes, *SubPreference);
            }
        }
        else
        {
            Target.AddChild(NewNode(Target.GetViewport()).Class<WHButton>()
                .Visibility(ENodeVisibility::Visible)
                .Anchor(EAnchor::HFill)
                .Padding({6_pt, 3})
                .InAllBrushesChained<&LRegionBrush::Tint>(Colors::Transparent)
                .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::Tint>(Prefs->PrimaryPaletteSolid->Normal.Tint)
                [
                    NewNode(Target.GetViewport()).Class<WText>()
                        .Anchor(EAnchor::Fill)
                        .TextAlign(ETextVAlign::Center)
                        .Content(Preference.DisplayName)
                    +
                    Preference.AsValue().CreateNode(Target.GetViewport(), Changes)
                ]
                .Unique()
                );
        }
    }};

    for (auto& Collection: Prefs.TopLevelCollections)
    {
        WNode* Item;
        WParent* PreferenceWrapper;
        Switcher->AddChild(NewStaticNode(WScrollRegion).SaveTo(&Item)
                .Tint(Colors::Transparent)
            [
                NewStaticNode(WVParent).SaveTo(&PreferenceWrapper)
                    .Anchor(EAnchor::Fill)
                    .Padding({10_pt, 4})
                [
                    NewStaticNode(WText)
                        .Content(Collection.DisplayName)
                        .TextScale(ETextScale::Header)
                    +
                    NewStaticNode(WHSeparator).Thickness(1).Anchor(EAnchor::HFill)
                ]
            ].Unique());

        algo::for_each(Collection, [&](std::unique_ptr<LPreference> const& Preference) -> void
        {
            AddPreference(*PreferenceWrapper, this->Changes, *Preference);
        });

        check(Collection.IsCollection())
        TopLevelPreferencesContainer->AddChild(NewStaticNode(WTextButton)
            .Anchor(EAnchor::HFill)
            .InAllBrushesChained<&LBoxBrush::Tint, &LBoxBrush::Padding>(Colors::Transparent, {6_pt, 3})
            .InBrush<EStyleBits::ActiveCombi, &LBoxBrush::Tint>(0x00000080_color)
            .InAllTextBrushes<&LTextBoxBrush::TextScale>(8_pt)
            .Content(Collection.DisplayName)
            .OnKeyEventFocused([Switcher,Item](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                {
                    /* Others could add top level preferences dynamically. Therefore, it is always better to just search by address. */
                    Switcher->SetActiveNode(*Item);
                    return LNodeReply::Handled();
                }
                return LNodeReply::Unhandled();
            })
            .Unique());
    }

    Switcher->SetActiveNodeByIndex(Index);
}
