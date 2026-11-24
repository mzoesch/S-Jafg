// Copyright mzoesch. All rights reserved.

#include "User/UserPreferencesWidget.h"
#include "Core/App.h"
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
    if (!Preference.SearchKeywords.empty())
    {
        Haystack += '\n';
        Haystack += Preference.SearchKeywords;
    }
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

NODISCARD TJxxUnique<WNode> CreateReadOnlyIcon(LViewport& Viewport)
{
    return NewNode(Viewport).Class<WBox>()
        /* Hit-testable, so that hovering the icon explains it. */
        .Visibility(ENodeVisibility::Visible)
        .Anchor(EAnchor::VFill)
        .MinDesiredSize({IconGutterWidth, 0})
        .Icon(LOptionalTexture2Ref{JafgInfo, "Icons/Lock"})
        .Tint(Colors::Gray)
        .BorderTint(Colors::Transparent)
        .Tooltip("Read-only. The application relies on this and it cannot be changed.")
        .Unique();
}

NODISCARD LPreferenceValue* FindValue(LPreferenceCollection& Collection, LStringView Identifier) noexcept
{
    for (auto& Child: Collection)
    {
        if (Child->IsCollection())
        {
            if (auto* Value{FindValue(Child->AsCollection(), Identifier)})
            {
                return Value;
            }
        }
        else if (Child->Identifier == Identifier)
        {
            return &Child->AsValue();
        }
    }
    return nullptr;
}

} /* ~Namespace <Anonymous> */

} /* ~Namespace Jafg */

void Jafg::WUserPreferencesWidget::Construct()
{
    Super::Construct();
    this->Rebuild();
}

void Jafg::WUserPreferencesWidget::Destruct()
{
    for (auto& Collection: GetMutableSingleton<JUserPreferences>().TopLevelCollections)
    {
        if (Collection.ReleaseHost)
        {
            Collection.ReleaseHost(this->Changes);
        }
    }

    Super::Destruct();
}

void Jafg::WUserPreferencesWidget::Tick(f32 Dt)
{
    Super::Tick(Dt);

    if (!this->PendingSectionRebuilds.empty())
    {
        for (const auto Index: std::exchange(this->PendingSectionRebuilds, {}))
        {
            this->RebuildSection(Index);
        }
    }

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
                NewStaticNode(WVParent).SaveTo(&this->Content)
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

    for (auto Index{0uz}; Index < Prefs.TopLevelCollections.size(); ++Index)
    {
        auto& Collection{Prefs.TopLevelCollections[Index]};
        check(Collection.IsCollection())

        this->PrepareCollection(Index);
        LSection Section{.Entry = this->BuildSectionNode(Index, this->Content->GetChildren().size())};

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
                    /* Dynamic collections are refetched every time they are selected. */
                    if (GetSingleton<JUserPreferences>().TopLevelCollections[Index].Refresh)
                    {
                        this->RebuildSection(Index);
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

void Jafg::WUserPreferencesWidget::PrepareCollection(std::size_t Index)
{
    auto& Collection{GetMutableSingleton<JUserPreferences>().TopLevelCollections[Index]};
    if (!Collection.Refresh)
    {
        return;
    }

    Collection.Refresh(Collection, LPreferenceCollectionHost{
        .Changes = &this->Changes,
        .RefreshVisibility = [this]{ this->RefreshVisibility(); },
        .RequestRebuild = [this, Index]
        {
            if (!algo::contains(this->PendingSectionRebuilds, Index))
            {
                this->PendingSectionRebuilds.push_back(Index);
            }
        },
        });

    return;
}

Jafg::WUserPreferencesWidget::LSearchEntry Jafg::WUserPreferencesWidget::BuildSectionNode(std::size_t Index, std::size_t Position)
{
    auto& Collection{GetMutableSingleton<JUserPreferences>().TopLevelCollections[Index]};

    WVParent* SectionNode;
    this->Content->AddChildAt(Position, NewStaticNode(WVParent).SaveTo(&SectionNode)
            .Anchor(EAnchor::HFill)
        [
            NewStaticNode(WText)
                .Content(Collection.DisplayName)
                .TextScale(ETextScale::Header)
            +
            NewStaticNode(WHSeparator).Thickness(1).Anchor(EAnchor::HFill)
        ].Unique());
    if (Collection.CreateHeaderNode)
    {
        SectionNode->AddChild(Collection.CreateHeaderNode(this->GetViewport(), this->Changes).Unique());
    }

    LSearchEntry Entry{.Node = SectionNode, .Haystack = MakeHaystack(Collection), .bCollection = true};
    for (auto& Preference: Collection)
    {
        Entry.Children.push_back(this->AddPreference(*SectionNode, *Preference, 1uz, Collection.Identifier));
    }
    Entry.ShownVisibility = SectionNode->GetVisibility();

    return Entry;
}

Jafg::WUserPreferencesWidget::LSearchEntry Jafg::WUserPreferencesWidget::AddPreference(WParent& Target, LPreference& Preference, std::size_t Depth, LStringView ParentKey)
{
    check(Depth > 0uz)

    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& Viewport{Target.GetViewport()};
    /* Preferences nested deeper than the direct children of a section are indented once more per level. */
    const f32 Indent{static_cast<f32>(Depth - 1uz) * IconGutterWidth.Size};
    LSearchEntry Entry{.Haystack = MakeHaystack(Preference), .bCollection = Preference.IsCollection()};

    if (Preference.IsCollection())
    {
        Entry.CollapseKey = algo::sprintf("{}/{}", ParentKey, Preference.Identifier);

        WVParent* Group;
        Target.AddChild(NewNode(Viewport).Class<WVParent>().SaveTo(&Group)
            .Anchor(EAnchor::HFill)
            [
                /* Pressing the header collapses or expands the collection. Like the categories of the inspector. */
                NewNode(Viewport).Class<WHButton>().SaveTo(&Entry.Row)
                    .Visibility(ENodeVisibility::Visible)
                    .Anchor(EAnchor::HFill)
                    .Padding(LPadding{ENodeSize::Points, 0.0f, 3.0f, 6.0f, 3.0f})
                    .Palette(Prefs.ProximityPaletteSolid(0uz, *Prefs.PalePrimaryPaletteSolid))
                    .Tooltip(Preference.Tooltip)
                    .OnKeyEventFocused([Widget=this, Key=Entry.CollapseKey](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            if (algo::contains(Widget->CollapsedGroups, Key))
                            {
                                algo::erase(&Widget->CollapsedGroups, Key);
                            }
                            else
                            {
                                Widget->CollapsedGroups.push_back(Key);
                            }
                            Widget->RefreshVisibility();
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                [
                    NewNode(Viewport).Class<WSpacer>()
                        .MinDesiredSize({LNodeSize1{ENodeSize::StaticPoints, Indent}, 0})
                    +
                    NewNode(Viewport).Class<WBox>().SaveTo(&Entry.CollapseIcon)
                        .Anchor(EAnchor::VFill)
                        .MinDesiredSize({IconGutterWidth, 0})
                        .Icon(LOptionalTexture2Ref{JafgInfo, "Icons/ExtendDown"})
                        .BorderTint(Colors::Transparent)
                    +
                    NewNode(Viewport).Class<WText>()
                        .Anchor(EAnchor::Fill)
                        .TextAlign(ETextVAlign::Center)
                        .TextScale(Depth > 1uz ? ETextScale::Body : ETextScale::SubHeader)
                        .Content(Preference.DisplayName)
                ]
            ]
            .Unique());
        Entry.Node = Group;

        for (auto& SubPreference: Preference.AsCollection())
        {
            Entry.Children.push_back(this->AddPreference(*Group, *SubPreference, Depth + 1uz, Entry.CollapseKey));
        }

        /* Closes off the top level groups of a section. E.g., each context in the controls. */
        if (Depth == 1uz)
        {
            Group->AddChild(NewNode(Viewport).Class<WHSeparator>().Thickness(1).Anchor(EAnchor::HFill).Unique());
        }
    }
    else
    {
        auto& Value{Preference.AsValue()};
        Entry.Value = &Value;

        WHButton* Row;
        Target.AddChild(NewNode(Viewport).Class<WHButton>().SaveTo(&Row)
            .Visibility(ENodeVisibility::Visible)
            .Anchor(EAnchor::HFill)
            /* No padding on the left. The icon gutter is the indent. */
            .Padding(LPadding{ENodeSize::Points, 0.0f, 3.0f, 6.0f, 3.0f})
            .Palette(Prefs.ProximityPaletteSolid(0uz, *Prefs.PalePrimaryPaletteSolid))
            .Tooltip(Preference.Tooltip)
            .Unique());
        Entry.Row = Row;
        if (Indent > 0.0f)
        {
            Row->AddChild(NewNode(Viewport).Class<WSpacer>().MinDesiredSize({LNodeSize1{ENodeSize::StaticPoints, Indent}, 0}).Unique());
        }
        if (auto Restart{GetRestartInfo(Value)})
        {
            Row->AddChild(CreateRestartIcon(Viewport, *Restart));
        }
        else if (Value.bReadOnly)
        {
            Row->AddChild(CreateReadOnlyIcon(Viewport));
        }
        else
        {
            Row->AddChild(NewNode(Viewport).Class<WSpacer>().MinDesiredSize({IconGutterWidth, 0}).Unique());
        }
        const LColor NameTint{Value.bReadOnly ? Colors::Gray : Colors::White};
        if (Value.Annotation.empty())
        {
            Row->AddChild(NewNode(Viewport).Class<WText>()
                .Anchor(EAnchor::Fill)
                .TextAlign(ETextVAlign::Center)
                .TextTint(NameTint)
                .Content(Preference.DisplayName)
                .Unique());
        }
        else
        {
            /* The annotation follows the display name directly. The rest of the row stays empty. */
            Row->AddChild(NewNode(Viewport).Class<WText>()
                .Anchor(EAnchor::VFill)
                .TextAlign(ETextVAlign::Center)
                .TextTint(NameTint)
                .Content(Preference.DisplayName)
                .Unique());
            Row->AddChild(NewNode(Viewport).Class<WText>()
                .Anchor(EAnchor::VFill)
                .TextAlign(ETextVAlign::Center)
                .TextScale(ETextScale::Compact)
                .TextTint(Colors::Gray)
                .Padding(LPadding{ENodeSize::Points, 4.0f, 0.0f, 0.0f, 0.0f})
                .Content(Value.Annotation)
                .Unique());
            Row->AddChild(NewNode(Viewport).Class<WSpacer>().Anchor(EAnchor::Fill).Unique());
        }
        Row->AddChild(Value.CreateNode(Viewport, this->Changes).Unique());
        Entry.Node = Row;
    }

    Entry.ShownVisibility = Entry.Node->GetVisibility();
    return Entry;
}

void Jafg::WUserPreferencesWidget::RebuildSection(std::size_t Index)
{
    check(algo::valid_index(this->Sections, Index))

    auto& Collection{GetMutableSingleton<JUserPreferences>().TopLevelCollections[Index]};
    auto& Section{this->Sections[Index]};

    /* Refreshing may replace the preferences of the collection. Keep their pending changes by identifier. */
    TArray<LPreferenceValue const*> Values;
    auto CollectValues{[&Values](this auto&& Self, LSearchEntry const& Entry) -> void
    {
        if (Entry.Value)
        {
            Values.push_back(Entry.Value);
        }
        for (auto const& Child: Entry.Children)
        {
            Self(Child);
        }
    }};
    CollectValues(Section.Entry);

    TArray<std::pair<LString, LString>> Kept;
    std::erase_if(this->Changes, [&Values, &Kept](LPreferenceValueChangeRequest& Change)
    {
        if (algo::contains(Values, static_cast<LPreferenceValue const*>(&Change.Preference)))
        {
            Kept.emplace_back(Change.Preference->Identifier, std::move(Change.Value));
            return true;
        }
        return false;
    });

    const auto Position{static_cast<std::size_t>(algo::distance_to(this->Content->GetChildren(), Section.Entry.Node, algo::unique_raw))};
    this->Content->RemoveChild(*Section.Entry.Node);
    Section.Entry = {};

    this->PrepareCollection(Index);
    for (auto& [Identifier, Value]: Kept)
    {
        if (auto* Preference{FindValue(Collection, Identifier)})
        {
            this->Changes.emplace_back(*Preference, std::move(Value));
        }
        else
        {
            LOG_WARNING(LogWidgets, "Dropping the pending change of [{}]. The preference does not exist anymore.", Identifier)
        }
    }

    Section.Entry = this->BuildSectionNode(Index, Position);
    this->RefreshVisibility();

    return;
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

    auto& Collections{GetMutableSingleton<JUserPreferences>().TopLevelCollections};
    //# The collection of the section that is currently filtered. It may hide values on its own.
    LPreferenceCollection* FilteredCollection{};

    //# A token matches, if it is found in the entry itself or in any of its ancestors.
    auto Filter{[Tokens=&this->SearchTokens, Collection=&FilteredCollection, Changes=&this->Changes](this auto&& Self, LSearchEntry& Entry, TArray<bool> Matched) -> bool
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
            check(Entry.Value)
            bShown = algo::all_of(Matched, [](bool bMatched){ return bMatched; })
                && (!*Collection || !(*Collection)->IsValueShown || (*Collection)->IsValueShown(*Entry.Value, *Changes));
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
        FilteredCollection = &Collections[Idx];
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
                if (FilteredCollection->IsValueShown)
                {
                    (void)Filter(Child, TArray<bool>{});
                }
                else
                {
                    Reset(Child);
                }
            }
            Section.Entry.Node->SetVisibility(bSelected ? Section.Entry.ShownVisibility : ENodeVisibility::Collapsed);
            SetButtonState(*Section.Button, true, bSelected);
        }
    }

    /* Collapsed collections keep their header but hide their children. This applies while searching, too. */
    auto Collapse{[Collapsed=&this->CollapsedGroups](this auto&& Self, LSearchEntry& Entry) -> void
    {
        if (!Entry.bCollection)
        {
            return;
        }

        const bool bCollapsed{algo::contains(*Collapsed, Entry.CollapseKey)};
        if (Entry.CollapseIcon)
        {
            Entry.CollapseIcon->Brush.Background = LRegionBrush::Icon({JafgInfo, bCollapsed ? "Icons/ExtendRight" : "Icons/ExtendDown"});
        }
        for (auto& Child: Entry.Children)
        {
            if (bCollapsed)
            {
                Child.Node->SetVisibility(ENodeVisibility::Collapsed);
            }
            else
            {
                Self(Child);
            }
        }
    }};
    for (auto& Section: this->Sections)
    {
        for (auto& Child: Section.Entry.Children)
        {
            Collapse(Child);
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

    this->RepaintRows();

    return;
}

void Jafg::WUserPreferencesWidget::RepaintRows()
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto Paint{[&Prefs](WHButton& Row, std::size_t& Counter)
    {
        Prefs.ProximityPaletteSolid(Counter++, *Prefs.PalePrimaryPaletteSolid).ApplyOn(Row.Style);
        /* Keep the hover state of a row that is under the cursor. */
        Row._ButtonBase_SetBrush((Row._GetNodeState() & Detail::ENodeStateBits::Dispatched) ? EStyleBits::Hover : EStyleBits::Normal);
    }};
    auto Walk{[&Paint](this auto&& Self, LSearchEntry const& Entry, std::size_t& Counter) -> void
    {
        if (Entry.Node->GetVisibility() == ENodeVisibility::Collapsed)
        {
            return;
        }
        if (Entry.Row)
        {
            Paint(*Entry.Row, Counter);
        }
        for (auto const& Child: Entry.Children)
        {
            Self(Child, Counter);
        }
    }};

    for (auto const& Section: this->Sections)
    {
        auto Counter{0uz};
        for (auto const& Child: Section.Entry.Children)
        {
            Walk(Child, Counter);
        }
    }

    return;
}
