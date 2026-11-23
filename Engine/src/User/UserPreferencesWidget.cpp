// Copyright mzoesch. All rights reserved.

#include "User/UserPreferencesWidget.h"
#include "Nodes/Button.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/HButton.h"
#include "Nodes/HParent.h"
#include "User/UserPreferences.h"
#include "User/PreferenceCollection.h"
#include "User/PreferenceValue.h"
#include "Nodes/VRegion.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/Separator.h"
#include "Nodes/Spacer.h"
#include "Nodes/Text.h"
#include "Nodes/Box.h"
#include "Nodes/TextButton.h"
#include "Nodes/VParent.h"

namespace Jafg
{

namespace
{

constexpr LColor RestartTint{ 0xE0A040FF_color };
//# The width of the gutter left of each preference. It holds the restart icon.
constexpr LNodeSize1 IconGutterWidth{ 18_spt };

struct LRestartInfo
{
    //# Not displayed, but matched by the search.
    LStringView Label;
    LStringView Tooltip;
    //# Displayed next to the apply button.
    LStringView Note;
    bool bImminent{};
};

NODISCARD std::optional<LRestartInfo> GetRestartInfo(LPreferenceValue const& Preference) noexcept
{
    if (Preference.RequiresImminentRestart())
    {
        return LRestartInfo{
            .Label = "Requires immediate restart",
            .Tooltip = "App will immediately restart after applying.",
            .Note = "App will immediately restart after applying.",
            .bImminent = true,
            };
    }
    if (Preference.RequiresRestart())
    {
        return LRestartInfo{
            .Label = "Requires restart",
            .Tooltip = "Requires a restart for changes to take affect.",
            .Note = "Some changes will only take effect after a restart.",
            .bImminent = false,
            };
    }
    return {};
}

NODISCARD LColor GetRestartTint(LRestartInfo const& Info) noexcept
{
    return Info.bImminent ? *GetSingleton<JUserPreferences>().DangerColor : RestartTint;
}

NODISCARD LString ToLowerAscii(LStringView Text)
{
    LString Result{Text};
    for (auto& Char: Result)
    {
        if (Char >= 'A' && Char <= 'Z')
        {
            Char = static_cast<char>(Char - 'A' + 'a');
        }
    }
    return Result;
}

//# Splits a query at whitespace into lower case tokens.
NODISCARD TArray<LString> TokenizeQuery(LStringView Query)
{
    TArray<LString> Tokens;
    LString Current;
    for (char Char: ToLowerAscii(Query))
    {
        if (Char == ' ' || Char == '\t' || Char == '\n' || Char == '\r')
        {
            if (!Current.empty())
            {
                Tokens.push_back(std::move(Current));
                Current.clear();
            }
        }
        else
        {
            Current.push_back(Char);
        }
    }
    if (!Current.empty())
    {
        Tokens.push_back(std::move(Current));
    }
    return Tokens;
}

//# The text a search query is matched against. Fields are separated, so that tokens cannot match across them.
NODISCARD LString MakeHaystack(LPreference const& Preference)
{
    LString Haystack{Preference.DisplayName};
    Haystack += '\n';
    Haystack += Preference.Identifier;
    Haystack += '\n';
    Haystack += Preference.Tooltip;
    if (Preference.IsValue())
    {
        if (auto Restart{GetRestartInfo(Preference.AsValue())})
        {
            Haystack += '\n';
            Haystack += Restart->Label;
        }
    }
    return ToLowerAscii(Haystack);
}

NODISCARD TJxxUnique<WNode> CreateRestartIcon(LViewport& Viewport, LRestartInfo const& Info)
{
    return NewNode(Viewport).Class<WBox>()
        /* Hit-testable, so that hovering the icon explains the kind of restart. */
        .Visibility(ENodeVisibility::Visible)
        .Anchor(EAnchor::VFill)
        .MinDesiredSize({IconGutterWidth, 0})
        .Icon(LOptionalTexture2Ref{JafgInfo, "Icons/Warning"})
        .Tint(GetRestartTint(Info))
        .BorderTint(Colors::Transparent)
        .Tooltip(LString{Info.Tooltip})
        .Unique();
}

} /* ~Namespace <Anonymous> */

} /* ~Namespace Jafg */

void Jafg::WUserPreferencesWidget::Construct()
{
    Super::Construct();
    this->Rebuild();
}

void Jafg::WUserPreferencesWidget::Tick(f32 Dt)
{
    Super::Tick(Dt);

    this->DiscardButton->SetEnabled(!this->Changes.empty());
    this->ApplyButton->SetEnabled(!this->Changes.empty());
    this->UpdateRestartNote();
}

void Jafg::WUserPreferencesWidget::UpdateRestartNote()
{
    std::optional<LRestartInfo> Strongest;
    for (auto& Change: this->Changes)
    {
        if (auto Info{GetRestartInfo(*Change.Preference)};
            Info && (!Strongest || (Info->bImminent && !Strongest->bImminent)))
        {
            Strongest = Info;
        }
    }

    if (!Strongest)
    {
        this->RestartNote->SetVisibility(ENodeVisibility::Collapsed);
        return;
    }

    if (this->RestartNoteText->GetContent() != Strongest->Note)
    {
        this->RestartNoteText->SetContent(LString{Strongest->Note});
        this->RestartNoteIcon->Brush.Tint = GetRestartTint(*Strongest);
    }
    this->RestartNote->SetVisibility(ENodeVisibility::TransitiveHitTestInvisible);

    return;
}

void Jafg::WUserPreferencesWidget::Rebuild()
{
    check(this->GetChildren().empty())
    check(this->Changes.empty())

    auto& Prefs{GetSingleton<JUserPreferences>()};

    this->Sections.clear();

    WVRegion* TopLevelPreferencesContainer{};
    WVParent* Content{};
    BeginStyling(*this).StaticRoot<WHParent>()
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
            NewStaticNode(WVParent)
                .Anchor(EAnchor::HFill)
                .Padding({10_pt, 4})
            [
                NewStaticNode(WEditableTextButton).SaveTo(&this->SearchBox)
                    .Palette(*Prefs.InputPaletteSolid)
                    .InAllBrushes<&LBoxBrush::Padding>({3_spt, 1})
                    .Anchor(EAnchor::HFill)
                    .PlaceholderContent("Search all preferences...")
                    .Content(this->SearchQuery)
                    .OnContentChanged([this](WEditableTextButton& Self, LString const& Query)
                    {
                        this->OnSearchQueryChanged(Query);
                    })
                    .OnContentCommitted([this](WEditableTextButton& Self, LString const& Query, ETextCommit Commit)
                    {
                        /* Escape clears the search. */
                        if (Commit == ETextCommit::OnCleared && !Query.empty())
                        {
                            Self.SetContent(LString{});
                            this->OnSearchQueryChanged({});
                        }
                    })
            ]
            +
            NewStaticNode(WScrollRegion).SaveTo(&this->ScrollRegion)
                .Tint(Colors::Transparent)
            [
                NewStaticNode(WVParent).SaveTo(&Content)
                    .Anchor(EAnchor::Fill)
                    .Padding({10_pt, 4})
                    .Space(16_pt)
                [
                    NewStaticNode(WText).SaveTo(&this->NoResultsText)
                        .Visibility(ENodeVisibility::Collapsed)
                        .TextTint(Colors::Gray)
                ]
            ]
            +
            NewStaticNode(WHSeparator).Anchor(EAnchor::HFill)
            +
            NewStaticNode(WHParent)
                .Anchor(EAnchor::HFill)
                .Padding({10_pt, 4})
            [
                NewStaticNode(WSpacer).Anchor(EAnchor::Fill)
                +
                NewStaticNode(WHParent).SaveTo(&this->RestartNote)
                    .Anchor(EAnchor::VFill)
                    .Visibility(ENodeVisibility::Collapsed)
                [
                    NewStaticNode(WBox).SaveTo(&this->RestartNoteIcon)
                        .Anchor(EAnchor::VFill)
                        .MinDesiredSize({IconGutterWidth, 0})
                        .Icon(LOptionalTexture2Ref{JafgInfo, "Icons/Warning"})
                        .BorderTint(Colors::Transparent)
                    +
                    NewStaticNode(WText).SaveTo(&this->RestartNoteText)
                        .Anchor(EAnchor::VFill)
                        .TextAlign(ETextVAlign::Center)
                    +
                    NewStaticNode(WSpacer).MinDesiredSize({8_pt, 0})
                ]
                +
                NewStaticNode(WTextButton).SaveTo(&this->DiscardButton)
                    .Content("Discard")
                    .Palette(*Prefs.SecondaryPaletteSolid)
                    .InAllTextBrushes<&LTextBrush::TextScale>(ETextScale::Header)
                    .InAllBrushes<&LBoxBrush::Padding>({5_pt,2})
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Self.AsStatic<WTextButton>().IsEnabled() && Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                        {
                            LOG_VERBOSE(LogWidgets, "Discarding [{}] modified preferences.", this->Changes.size())
                            this->Changes.clear();
                            this->RebuildInPlace();
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
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Self.AsStatic<WTextButton>().IsEnabled() && Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                        {
                            LOG_VERBOSE(LogWidgets, "Applying [{}] modified preferences.", this->Changes.size())
                            bool bRestartNow{};
                            for (auto& Change: this->Changes)
                            {
                                if (auto Info{GetRestartInfo(*Change.Preference)}; Info && Info->bImminent)
                                {
                                    bRestartNow = true;
                                }
                                LOG_VERBOSE(LogWidgets, "    [{}]: {}", Change.Preference->Identifier, Change.Value)
                                Change.Preference->SaveValue(std::move(Change.Value));
                            }
                            this->Changes.clear();
                            this->RebuildInPlace();

                            if (bRestartNow)
                            {
                                App::RequestEngineRestart("Restarting due to preference changes.");
                            }

                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
            ]
        ]
    ];

    //# Adds the nodes for a preference to the target and returns its search entry.
    auto AddPreference{[Widget=this, Prefs=&Prefs](this auto&& Self, WParent& Target, LPreference& Preference) -> LSearchEntry
    {
        auto& Viewport{Target.GetViewport()};
        LSearchEntry Entry{.Haystack = MakeHaystack(Preference), .bCollection = Preference.IsCollection()};

        if (Preference.IsCollection())
        {
            WVParent* Group;
            Target.AddChild(NewNode(Viewport).Class<WVParent>().SaveTo(&Group)
                .Anchor(EAnchor::HFill)
                [
                    NewNode(Viewport).Class<WText>()
                        /* Text is not hit-testable by default. Without this, the tooltip would never show. */
                        .Visibility(ENodeVisibility::Visible)
                        .Anchor(EAnchor::HFill)
                        .TextScale(ETextScale::SubHeader)
                        .Content(Preference.DisplayName)
                        .Tooltip(Preference.Tooltip)
                ]
                .Unique());
            Entry.Node = Group;

            for (auto& SubPreference: Preference.AsCollection())
            {
                Entry.Children.push_back(Self(*Group, *SubPreference));
            }
        }
        else
        {
            auto& Value{Preference.AsValue()};

            WHButton* Row;
            Target.AddChild(NewNode(Viewport).Class<WHButton>().SaveTo(&Row)
                .Visibility(ENodeVisibility::Visible)
                .Anchor(EAnchor::HFill)
                /* No padding on the left. The icon gutter is the indent. */
                .Padding(LPadding{ENodeSize::Points, 0.0f, 3.0f, 6.0f, 3.0f})
                .InAllBrushesChained<&LRegionBrush::Tint>(Colors::Transparent)
                .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::Tint>(Prefs->PrimaryPaletteSolid->Normal.Tint)
                .Tooltip(Preference.Tooltip)
                .Unique());
            if (auto Restart{GetRestartInfo(Value)})
            {
                Row->AddChild(CreateRestartIcon(Viewport, *Restart));
            }
            else
            {
                Row->AddChild(NewNode(Viewport).Class<WSpacer>().MinDesiredSize({IconGutterWidth, 0}).Unique());
            }
            Row->AddChild(NewNode(Viewport).Class<WText>()
                .Anchor(EAnchor::Fill)
                .TextAlign(ETextVAlign::Center)
                .Content(Preference.DisplayName)
                .Unique());
            Row->AddChild(Value.CreateNode(Viewport, Widget->Changes).Unique());
            Entry.Node = Row;
        }

        Entry.ShownVisibility = Entry.Node->GetVisibility();
        return Entry;
    }};

    for (auto Index{0uz}; Index < Prefs.TopLevelCollections.size(); ++Index)
    {
        auto& Collection{Prefs.TopLevelCollections[Index]};
        check(Collection.IsCollection())

        WVParent* SectionNode;
        Content->AddChild(NewStaticNode(WVParent).SaveTo(&SectionNode)
                .Anchor(EAnchor::HFill)
            [
                NewStaticNode(WText)
                    .Content(Collection.DisplayName)
                    .TextScale(ETextScale::Header)
                +
                NewStaticNode(WHSeparator).Thickness(1).Anchor(EAnchor::HFill)
            ].Unique());

        LSection Section{.Entry = {.Node = SectionNode, .Haystack = MakeHaystack(Collection), .bCollection = true}};
        for (auto& Preference: Collection)
        {
            Section.Entry.Children.push_back(AddPreference(*SectionNode, *Preference));
        }
        Section.Entry.ShownVisibility = SectionNode->GetVisibility();

        TopLevelPreferencesContainer->AddChild(NewStaticNode(WTextButton).SaveTo(&Section.Button)
            .Anchor(EAnchor::HFill)
            .InAllBrushesChained<&LBoxBrush::Tint, &LBoxBrush::Padding>(Colors::Transparent, {6_pt, 3})
            .InBrush<EStyleBits::ActiveCombi, &LBoxBrush::Tint>(0x00000080_color)
            .InBrush<EStyleBits::Selected, &LBoxBrush::Tint>(Prefs.PrimaryPaletteSolid->Selected.Tint)
            .InAllTextBrushes<&LTextBoxBrush::TextScale>(8_pt)
            .InTextBrush<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray)
            .Selectable(true)
            .Content(Collection.DisplayName)
            .Tooltip(Collection.Tooltip)
            .OnKeyEventFocused([this, Index](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                {
                    if (this->IsSearching())
                    {
                        this->SearchBox->SetContent(LString{});
                        this->OnSearchQueryChanged({});
                    }
                    this->SelectSection(Index);
                    return LNodeReply::Handled();
                }
                return LNodeReply::Unhandled();
            })
            .Unique());

        this->Sections.push_back(std::move(Section));
    }

    this->SectionScrollPositions.resize(this->Sections.size(), maths::zero_vector<LVec2F>);
    if (!algo::valid_index(this->Sections, this->SelectedSection))
    {
        this->SelectedSection = 0uz;
    }
    this->SearchTokens = TokenizeQuery(this->SearchQuery);
    this->RefreshVisibility();

    return;
}

void Jafg::WUserPreferencesWidget::RebuildInPlace()
{
    check(this->ScrollRegion)
    const LVec2F ScrollPosition{this->ScrollRegion->GetScrollPosition()};
    this->RemoveChildren();
    this->Rebuild();
    this->ScrollRegion->SetScrollPosition(ScrollPosition);
}

void Jafg::WUserPreferencesWidget::SelectSection(std::size_t Index)
{
    check(!this->IsSearching())
    check(algo::valid_index(this->Sections, Index))

    if (Index == this->SelectedSection)
    {
        return;
    }

    if (algo::valid_index(this->SectionScrollPositions, this->SelectedSection))
    {
        this->SectionScrollPositions[this->SelectedSection] = this->ScrollRegion->GetScrollPosition();
    }
    this->SelectedSection = Index;
    this->RefreshVisibility();
    this->ScrollRegion->SetScrollPosition(this->SectionScrollPositions[Index]);

    return;
}

void Jafg::WUserPreferencesWidget::OnSearchQueryChanged(LStringView Query)
{
    this->SearchQuery = LString{Query};

    auto Tokens{TokenizeQuery(Query)};
    if (Tokens == this->SearchTokens)
    {
        return;
    }

    const bool bWasSearching{this->IsSearching()};
    if (!bWasSearching && algo::valid_index(this->SectionScrollPositions, this->SelectedSection))
    {
        this->SectionScrollPositions[this->SelectedSection] = this->ScrollRegion->GetScrollPosition();
    }

    this->SearchTokens = std::move(Tokens);
    this->RefreshVisibility();

    if (this->IsSearching())
    {
        /* The results changed. Anything but the top is arbitrary. */
        this->ScrollRegion->SetScrollPosition(maths::zero_vector<LVec2F>);
    }
    else if (bWasSearching && algo::valid_index(this->SectionScrollPositions, this->SelectedSection))
    {
        this->ScrollRegion->SetScrollPosition(this->SectionScrollPositions[this->SelectedSection]);
    }

    return;
}

void Jafg::WUserPreferencesWidget::RefreshVisibility()
{
    auto Reset{[](this auto&& Self, LSearchEntry& Entry) -> void
    {
        Entry.Node->SetVisibility(Entry.ShownVisibility);
        for (auto& Child: Entry.Children)
        {
            Self(Child);
        }
    }};

    //# A token matches, if it is found in the entry itself or in any of its ancestors.
    auto Filter{[Tokens=&this->SearchTokens](this auto&& Self, LSearchEntry& Entry, TArray<bool> Matched) -> bool
    {
        for (auto Idx{0uz}; Idx < Tokens->size(); ++Idx)
        {
            Matched[Idx] = Matched[Idx] || Entry.Haystack.contains((*Tokens)[Idx]);
        }

        bool bShown{};
        if (Entry.bCollection)
        {
            for (auto& Child: Entry.Children)
            {
                if (Self(Child, Matched))
                {
                    bShown = true;
                }
            }
        }
        else
        {
            bShown = algo::all_of(Matched, [](bool bMatched){ return bMatched; });
        }

        Entry.Node->SetVisibility(bShown ? Entry.ShownVisibility : ENodeVisibility::Collapsed);
        return bShown;
    }};

    //# A button asserts that it is not selected while changing its enabled state and that it is enabled while changing its selected state.
    auto SetButtonState{[](WTextButton& Button, bool bEnabled, bool bSelected) -> void
    {
        check(bEnabled || !bSelected)
        if (Button.IsSelected() && !bSelected)
        {
            Button.SetSelected(false);
        }
        if (Button.IsEnabled() != bEnabled)
        {
            Button.SetEnabled(bEnabled);
        }
        if (!Button.IsSelected() && bSelected)
        {
            Button.SetSelected(true);
        }
    }};

    const bool bSearching{this->IsSearching()};
    bool bAnyMatch{};
    for (auto Idx{0uz}; Idx < this->Sections.size(); ++Idx)
    {
        auto& Section{this->Sections[Idx]};
        if (bSearching)
        {
            const bool bMatch{Filter(Section.Entry, TArray<bool>(this->SearchTokens.size(), false))};
            bAnyMatch = bAnyMatch || bMatch;
            SetButtonState(*Section.Button, bMatch, false);
        }
        else
        {
            const bool bSelected{Idx == this->SelectedSection};
            for (auto& Child: Section.Entry.Children)
            {
                Reset(Child);
            }
            Section.Entry.Node->SetVisibility(bSelected ? Section.Entry.ShownVisibility : ENodeVisibility::Collapsed);
            SetButtonState(*Section.Button, true, bSelected);
        }
    }

    if (bSearching && !bAnyMatch)
    {
        this->NoResultsText->SetContent("No such preferences.");
        this->NoResultsText->SetVisibility(ENodeVisibility::TransitiveHitTestInvisible);
    }
    else
    {
        this->NoResultsText->SetVisibility(ENodeVisibility::Collapsed);
    }

    return;
}
