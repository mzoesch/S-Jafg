// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_LargeList.h"
#include "Core/TextSearch.h"
#include "Framework/Frontend.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "Nodes/VParent.h"
#include "Nodes/VRegion.h"
#include "User/UserPreferences.h"

namespace
{

//# A token that matches the name of an item ranks higher than one that only matches somewhere in its path.
constexpr i32 NameBonus{ 32 };

NODISCARD LString GetDisplay(Jafg::LInputListItem const& Item)
{
    return Item.DisplayName.empty() ? Item.Identifier : Item.DisplayName;
}

NODISCARD LString GetLastPathSegment(LStringView Text)
{
    if (auto const Slash{Text.find_last_of("/\\")}; Slash != LStringView::npos)
    {
        return LString{Text.substr(Slash + 1uz)};
    }
    return LString{Text};
}

//#
//# How well every token matches the display text, the name or the identifier of an item.
//# @return Empty, if any token does not match. Without tokens, every item matches with zero.
//#
NODISCARD std::optional<i32> ScoreItem(LStringView Display, LStringView Name, LStringView Identifier, TArray<LString> const& Tokens)
{
    i32 Total{};
    for (auto const& Token: Tokens)
    {
        std::optional<i32> Best;
        auto const Consider{[&Best](std::optional<i32> Score, i32 Bonus)
        {
            if (Score && (!Best || *Score + Bonus > *Best))
            {
                Best = *Score + Bonus;
            }
        }};
        Consider(Jafg::TextSearch::FuzzyScore(Name, Token), NameBonus);
        Consider(Jafg::TextSearch::FuzzyScore(Display, Token), 0);
        if (Identifier != Display)
        {
            Consider(Jafg::TextSearch::FuzzyScore(Identifier, Token), 0);
        }

        if (!Best)
        {
            return {};
        }
        Total += *Best;
    }
    return Total;
}

} /* ~Namespace <Anonymous> */

void Jafg::WInput_LargeList::CtorLogic()
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);

    Prefs.InputPaletteSolid->ApplyOn(this->Style);
    Prefs.InputPaletteSolid->ApplyOn(this->TextStyle);

    this->Style.SetEverywhere<&LBoxBrush::Padding>(Prefs.InputPadding);

    this->RightIcon = LTexture2::FromTextureView(JafgInfo, "Icons/ArrowDown");
    this->RightIconStyle.ChainEverywhere<&LIconBrush::bAlwaysPad, &LIconBrush::Alignment>(true, LIconBrush::Align::Right);
    this->RightIconStyle.Set<ETextButtonIconStyleBits::Disabled, &LIconBrush::Tint>(Colors::Gray);
}

Jafg::LNodeReply Jafg::WInput_LargeList::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    auto Reply{Super::OnKeyEventFocused(Info, Event)};

    if (Event.Is<ERawInputStateBits::Press>(ELogicalKey::LeftMouseButton))
    {
        if (this->DeferredWidget.IsValidFast())
        {
            this->DeferredWidget->RemoveFromParent2();
            this->DeferredWidget.Reset();
        }
        else
        {
            this->OpenSelectionWindow(Info);
            /* So that typing filters right away. */
            if (this->SearchBar.IsValidFast())
            {
                return LNodeReply{this->SearchBar};
            }
        }

        if (Reply.IsHandled())
        {
            return Reply;
        }
        return LNodeReply::Handled();
    }

    if (Reply.IsHandled())
    {
        return Reply;
    }
    return LNodeReply::Unhandled();
}

void Jafg::WInput_LargeList::OpenSelectionWindow(LNodeKeyEventInfo const& Info)
{
    this->SearchBar.Reset();
    this->ScrollRegion = nullptr;
    this->List = nullptr;
    this->NoMatches = nullptr;
    this->Rows.clear();
    this->Order.clear();

    ConstructDeferredWidget(Jafg::TNodeStaticInit<WDismissibleFloatingWidget>{Info.Viewport}).Style().SaveTo(&this->DeferredWidget)
        .Decorate(false)
        .CreateResizeUi(false)
        .InitialWindowPosition(
            this->GetAnchoredAndTranslatedTopLeftFromMostOuter(this->GetTranslationFromMostOuter())
            + LVec2F{0,this->GetAnchoredSize_v2().y}
            )
        /* At least as wide as this input. The search bar should not be squeezed. */
        .InitialWindowSize({ENodeSize::StaticPoints, maths::max(this->GetAnchoredSize_v2().x, 100.0f), 20.0f})
        .Content([this](WFloatingWidget& FloatingWidget, WParent& Container)
        {
            auto& Prefs{GetSingleton<JUserPreferences>()};
            auto& Viewport{Container.GetViewport()};

            Container.AddChild(NewNode(Viewport).Class<WVRegion>()
                .Padding(1_spt)
                .Tint(*Prefs.ForegroundColor)
                .OutlineThickness(1)
                .OutlineTint(Colors::White)
            [
                NewNode(Viewport).Class<WEditableTextButtonIconizedLeft>().SaveTo(&this->SearchBar)
                    .Anchor(EAnchor::HFill)
                    .Palette(*Prefs.InputPaletteSolid)
                    .InAllBrushes<&LBoxBrush::Padding>(Prefs.InputPadding)
                    .Icon({JafgInfo, "Icons/Search"})
                    .PlaceholderContent("Search...")
                    .OnContentChanged([this](WEditableTextButton& Self, LString const& Query)
                    {
                        this->Filter(Query);
                    })
                    /* Before the text field handles the keys itself. */
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Press>(Info.Frontend.GetPhysicalKey(ELogicalKey::Escape)))
                        {
                            auto& Bar{*StaticCastChecked<WEditableTextButton>(&Self)};
                            if (!Bar.GetContent().empty())
                            {
                                /* Only the search is cleared. The search bar keeps the focus for the next escape. */
                                Bar.SelectAll();
                                (void)Bar.DeleteSelection();
                                this->Filter({});
                                return LNodeReply::Handled();
                            }
                            this->CloseSelectionWindow();
                            return LNodeReply{TClassStorage<WNode>{}};
                        }
                        if (Event.Is<ERawInputStateBits::Press>(
                              Info.Frontend.GetPhysicalKey(ELogicalKey::Enter)
                            , Info.Frontend.GetPhysicalKey(ELogicalKey::NumPadEnter)))
                        {
                            if (this->SelectFirstMatch())
                            {
                                return LNodeReply{TClassStorage<WNode>{}};
                            }
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                /*
                 * The cap is on a wrapper and not on the scroll region: Its own max size would also cap the size of
                 * its content that it measures, and nothing would be left to scroll.
                 */
                + NewNode(Viewport).Class<WVParent>()
                    .Anchor(EAnchor::HFill)
                    .MaxDesiredSize({0.0f, this->MaxListHeight})
                [
                    NewNode(Viewport).Class<WScrollRegion>().SaveTo(&this->ScrollRegion)
                        .SkipBrushDraw(true)
                        .UseChildrenDesiredSize(true)
                    [
                        NewNode(Viewport).Class<WVParent>().SaveTo(&this->List)
                            .Anchor(EAnchor::HFill)
                    ]
                ]
            ].Unique());
            check(this->List)

            this->Rows.reserve(this->Items.size());
            for (auto const& Item: this->Items)
            {
                check(!Item.Identifier.empty())
                WNode* Row{};
                LString Display{GetDisplay(Item)};
                this->List->AddChild(NewNode(Viewport).Class<WTextButton>().SaveTo(&Row)
                    .Anchor(EAnchor::HFill)
                    .Palette(*Prefs.PrimaryPaletteSolid)
                    .InBrush<EStyleBits::InactiveCombi, &LBoxBrush::bSkipBrushDraw>(true)
                    .InAllBrushes<&LBoxBrush::Padding>(Prefs.InputPadding)
                    .Content(Display)
                    .Enabled(Item.bSelectable)
                    .OnKeyEventFocused([this, Item](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
                        {
                            if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                            {
                                this->Select(Item);
                                return LNodeReply::Handled();
                            }
                        }
                        return LNodeReply::Unhandled();
                    })
                    .Unique());
                check(Row)
                this->Order.push_back(this->Rows.size());
                LString Name{GetLastPathSegment(Display)};
                this->Rows.push_back({.Node = Row, .Item = Item, .Display = std::move(Display), .Name = std::move(Name)});
            }

            this->List->AddChild(NewNode(Viewport).Class<WText>().SaveTo(&this->NoMatches)
                .Anchor(EAnchor::HFill)
                .Padding(Prefs.InputPadding)
                .TextTint(Colors::Gray)
                .Content("No matches.")
                .Visibility(this->Rows.empty() ? ENodeVisibility::Visible : ENodeVisibility::Collapsed)
                .Unique());
        });
}

void Jafg::WInput_LargeList::CloseSelectionWindow()
{
    if (this->DeferredWidget.IsValidFast())
    {
        StaticCastChecked<WFloatingWidget>(this->DeferredWidget.get())->DestroyFloatingWidgetControlled();
    }
    this->DeferredWidget.Reset();
}

void Jafg::WInput_LargeList::Filter(LStringView Query)
{
    if (!this->DeferredWidget.IsValidFast())
    {
        return;
    }
    check(this->ScrollRegion && this->List && this->NoMatches)

    TArray<LString> const Tokens{TextSearch::Tokenize(Query)};
    TArray<std::pair<i32, std::size_t>> Matches;
    for (auto Idx{0uz}; Idx < this->Rows.size(); ++Idx)
    {
        auto const& Row{this->Rows[Idx]};
        check(Row.Node)
        auto const Score{ScoreItem(Row.Display, Row.Name, Row.Item.Identifier, Tokens)};
        Row.Node->SetVisibility(Score ? ENodeVisibility::Visible : ENodeVisibility::Collapsed);
        if (Score)
        {
            Matches.emplace_back(*Score, Idx);
        }
    }

    /* The best first. Equally good ones keep the order of the items. */
    std::ranges::stable_sort(Matches, std::ranges::greater{}, [](auto const& Match) { return Match.first; });
    this->Order.clear();
    for (auto Position{0uz}; auto const& [Score, Idx]: Matches)
    {
        this->List->ReorderChild(*this->Rows[Idx].Node, Position++);
        this->Order.push_back(Idx);
    }

    this->NoMatches->SetVisibility(Matches.empty() ? ENodeVisibility::Visible : ENodeVisibility::Collapsed);
    /* The best matches are at the top. */
    this->ScrollRegion->SetScrollPosition(maths::zero_vector<LVec2F>);
}

void Jafg::WInput_LargeList::Select(LInputListItem Item)
{
    /* First close: The event may change everything, including this list. */
    this->CloseSelectionWindow();
    this->SetContent(GetDisplay(Item));
    (void)this->OnValueChanged.InvokeIfBound(Item);
}

bool Jafg::WInput_LargeList::SelectFirstMatch()
{
    for (std::size_t const Idx: this->Order)
    {
        check(algo::valid_index(this->Rows, Idx))
        if (this->Rows[Idx].Item.bSelectable)
        {
            this->Select(this->Rows[Idx].Item);
            return true;
        }
    }
    return false;
}
