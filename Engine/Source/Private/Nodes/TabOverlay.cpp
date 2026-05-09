// Copyright mzoesch. All rights reserved.

#include "Nodes/TabOverlay.h"
#include "User/UserPreferences.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "Nodes/UserWidget.h"
#include "Nodes/DropDown.h"
#include "Nodes/HParent.h"
#include "Nodes/HRegion.h"
#include "Nodes/VParent.h"
#include "Rhi/NodeRenderInfo.h"

namespace
{

inline constexpr LColor HighlightColor{0xFF, 0xFF, 0xFF, 0x10,};

} /* ~Namespace <Anonymous> */

Jafg::LFactoryTabOverlayParent Jafg::LTabOverlayPossibilities::GetNewOverlayParent()
{
    return NewNode(this->Owner.GetViewport()).Class<WTabOverlayParent>()
        // .SetInitialState(LInitialHDragRegionState{100_pt,{},100_pt})
        .Anchor(EAnchor::TopLeft)
        .Possibilities(*this)
        .Tint(*GetSingleton<JUserPreferences>().BackgroundColor)
        ;
}

Jafg::LFactoryTabOverlay Jafg::LTabOverlayPossibilities::GetNewOverlay()
{
    return NewNode(this->Owner.GetViewport()).Class<WTabOverlay>()
        .Possibilities(*this)
        .Visibility(ENodeVisibility::Visible)
        .MinDesiredSize({25_pt, 0.0f})
        ;
}

Jafg::WTabOverlay& Jafg::LTabOverlayPossibilities::FindNewOverlay()
{
    if (auto* Parent{this->Owner.FindNodeInVisiblePath<WTabOverlayParent>()})
    {
        return Parent->AddChild(this->GetNewOverlay().Unique()).AsStatic<WTabOverlay>();
    }
    this->GetOverlayRoot().AddChild(this->GetNewOverlayParent()
        .Anchor(EAnchor::Fill)
        .Padding({ENodeSize::StaticPoints
            , static_cast<f32>(*GetSingleton<JUserPreferences>().PreferredDragPadding), 0.0f
            , static_cast<f32>(*GetSingleton<JUserPreferences>().PreferredDragPadding), static_cast<f32>(*GetSingleton<JUserPreferences>().PreferredDragPadding)}
            )
        .Unique()
        );
    return this->FindNewOverlay();
}

Jafg::WUserWidget& Jafg::LTabOverlayPossibilities::AddWindow(LTabCreateInfo Info, bool bFocus)
{
    if (std::holds_alternative<TSubclassOf<WUserWidget>>(Info.Panel))
    {
        auto& Panel{std::get<TSubclassOf<WUserWidget>>(Info.Panel)};
        if (this->Selected)
        {
            if (auto* Widget{this->Selected->FindWidgetSlow(Panel.GetClassOrDefault())})
            {
                auto It{algo::find_checked(this->Selected->GetTabs(), Widget, algo::pair_second{})};
                this->Selected->SetSelectedTab(*It->first);
                return *Widget;
            }
        }
        for (auto& Overlay : this->Overlays)
        {
            if (auto* Widget{Overlay->FindWidgetSlow(Panel.GetClassOrDefault())})
            {
                auto It{algo::find_checked(Overlay->GetTabs(), Widget, algo::pair_second{})};
                Overlay->SetSelectedTab(*It->first);
                return *Widget;
            }
        }
    }

    WTabOverlay* Overlay{this->Selected};
    if (!Overlay)
    {
        if (this->Overlays.empty())
        {
            Overlay = &this->FindNewOverlay();
            check(!this->Overlays.empty())
        }
        else
        {
            Overlay = &this->Overlays.front();
        }
    }

    WUserWidget* Result{Overlay->RegisterTab(std::move(Info)).second};
    check(Result)
    auto It{algo::find_checked(Overlay->GetTabs(), Result, algo::pair_second{})};
    Overlay->SetSelectedTab(*It->first);
    return *Result;
}

void Jafg::WTabOverlay::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (!!this->DrawOption)
    {
        check(this->Switcher)
        LRect2F Rect{
            .Offset = this->Switcher->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
            .Extent = this->Switcher->GetAnchoredSize_v2(),
            };
        if (*this->DrawOption == StepResult::L)
        {
            Rect.Extent.x = StepResult::GetMaxDistanceFromEdge<StepResult::L>(Rect.Extent.x);
        }
        else if (*this->DrawOption == StepResult::U)
        {
            Rect.Extent.y = StepResult::GetMaxDistanceFromEdge<StepResult::U>(Rect.Extent.y);
        }
        else if (*this->DrawOption == StepResult::R)
        {
            Rect.Offset.x += StepResult::GetMaxDistanceFromEdge<StepResult::R>(Rect.Extent.x);
            Rect.Extent.x = StepResult::GetMaxDistanceFromEdge<StepResult::R>(Rect.Extent.x);
        }
        else if (*this->DrawOption == StepResult::D)
        {
            Rect.Offset.y += StepResult::GetMaxDistanceFromEdge<StepResult::D>(Rect.Extent.y);
            Rect.Extent.y = StepResult::GetMaxDistanceFromEdge<StepResult::D>(Rect.Extent.y);
        }
        Info.AddInstance({
            .Rect = Rect,
            .Tint = HighlightColor,
            .BackgroundTint = Colors::Black,
            .Radii = maths::zero_vector<LVec4F>,
            .OutlineTint = Colors::White,
            .TexCoordRect = {0.0f, 0.0f, 1.0f, 1.0f},
            .OutlineThickness = 0.0f,
            .TextureIndex = UBO::BindlessTextureArray::IdentityMulIdx,
            .SamplerIndex = UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx,
            .MsdfPixelRange = 0.0f,
            });
    }

    return;
}

void Jafg::WTabOverlay::Construct()
{
    Super::Construct();

    check(this->Possibilities)
    this->Possibilities->RegisterOverlay(*this);

    if (this->Wrapper == nullptr)
    {
        this->InitializeBoilerplate();
    }
    check(this->Wrapper && this->Selectors && this->Switcher)

    return;
}

Jafg::WTabOverlay::Tab Jafg::WTabOverlay::RegisterTab(LTabCreateInfo&& Info)
{
    if (this->Wrapper == nullptr)
    {
        this->InitializeBoilerplate();
    }
    check(this->Wrapper && this->Selectors && this->Switcher)

    if (std::holds_alternative<TSubclassOf<WUserWidget>>(Info.Panel))
    {
        this->Switcher->AddChild(NewNode(this->GetViewport()).Class(std::get<TSubclassOf<WUserWidget>>(Info.Panel).GetClassOrDefault()).Unique());
    }
    else
    {
        this->Switcher->AddConstructedChild(std::get<TJxxUnique<WUserWidget>>(std::move(Info.Panel)));
    }
    this->Tabs.emplace_back(nullptr, this->Switcher->GetChildren().back()->AsChecked<WUserWidget>());

    bool bActivated{};
    if (this->Switcher->GetChildren().size() == 1)
    {
        bActivated = true;
        this->Switcher->SetActiveNodeByIndex(0);
    }

    BeginStyling(*this->Selectors).StaticRoot<WTabOverlaySelector>(std::move(Info.Selector)).SaveTo(&this->Tabs.back().first)
        .Selected(bActivated)
        .OnKeyDownFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
        {
            if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
            {
                check(this->Switcher)
                this->SetSelectedTab(*StaticCast<WTabOverlaySelector>(&Self));
                if (!this->UiTickHandle.IsValid())
                {
                    this->UiTickHandle = this->GetViewport().OnLateTick.Emplace([this, &Self]
                    {
                        return this->MouseTabMoveTick(Self.AsStatic<WTabOverlaySelector>());
                    });
                }
                return LNodeReply::Handled();
            }
            if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
            {
                this->CreateTabMenu(Info.CursorLocation, *StaticCast<WTabOverlaySelector>(&Self));
                return LNodeReply::Handled();
            }
            return LNodeReply::Unhandled();
        })
        .OnKeyUpFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
        {
            if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MiddleMouseButton))
            {
                this->CloseTab(StaticCast<WTabOverlaySelector>(&Self));
                return LNodeReply::Handled();
            }
            return LNodeReply::Unhandled();
        })
        .OnKeyDownUnfocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
        {
            if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
            {
                this->CreateTabMenu(Info.CursorLocation, *StaticCast<WTabOverlaySelector>(&Self));
                return LNodeReply::Handled();
            }
            return LNodeReply::Unhandled();
        })
        .OnKeyUpUnfocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
        {
            if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MiddleMouseButton))
            {
                this->CloseTab(StaticCast<WTabOverlaySelector>(&Self));
                return LNodeReply::Handled();
            }
            return LNodeReply::Unhandled();
        })
        .Delegate(this->DefaultSelectorDelegate);

    auto Result{this->Tabs.back()};

    for (LTabCreateInfo& Sibling : Info._Siblings)
    {
        this->RegisterTab(std::move(Sibling));
    }

    check(algo::contains(this->Tabs, Result.first, algo::pair_first{}))
    check(algo::find(this->Tabs, Result.first, algo::pair_first{})->second == Result.second)
    return Result;
}

Jafg::WUserWidget* Jafg::WTabOverlay::FindWidgetSlow(LJxxClass const& Class) noexcept
{
    check(Class.DerivesFrom<WUserWidget>())
    for (auto& Panel: this->Tabs | std::views::values)
    {
        if (Panel->GetVirtualTable().DerivesFrom(Class))
        {
            return Panel;
        }
    }
    return nullptr;
}

void Jafg::WTabOverlay::SetSelectedTab(WTabOverlaySelector& Target)
{
    for (auto& Child : this->Selectors->GetChildren())
    {
        check(Child.get())
        if (auto* Selector{StaticCast<WTabOverlaySelector>(&*Child)}; Selector != &Target)
        {
            Selector->SetSelected(false);
        }
    }
    Target.SetSelected(true);

    auto It{algo::find(this->Tabs, &Target, &Tab::first)};
    check(It != this->Tabs.end())
    this->Switcher->SetActiveNode(*It->second);

    return;
}

void Jafg::WTabOverlay::InitializeBoilerplate()
{
    check(!this->Wrapper && !this->Selectors && !this->Switcher)
    BeginStyling(*this).StaticRoot<WVParent>().SaveTo(&this->Wrapper).Anchor(EAnchor::Fill)
    [
          NewStaticNode(WHParent).SaveTo(&this->Selectors).Anchor(EAnchor::HFill)
        + NewStaticNode(WSwitcher).SaveTo(&this->Switcher).Anchor(EAnchor::Fill)
    ];
    return;
}

std::optional<TJxxUnique<Jafg::WUserWidget>> Jafg::WTabOverlay::CloseTabImpl(WTabOverlaySelector* Selector, bool bRelease)
{
    check(this->Switcher)

    std::optional<TJxxUnique<WUserWidget>> Result;

    auto It{this->Tabs.begin()};
#if JAFG_DO_CHECKS
    bool bRemoved{};
#endif /* JAFG_DO_CHECKS */
    for (;It != this->Tabs.end(); ++It)
    {
        if (It->first == Selector)
        {
            It->first->RemoveFromParent2();
            if (bRelease)
            {
                Result = It->second->RemoveFromTree();
            }
            else
            {
                It->second->RemoveFromParent2();
            }
            It = this->Tabs.erase(It);
            checkCode(bRemoved = true)
            break;
        }
    }
    check(bRemoved)

    if (this->Switcher->GetChildren().empty())
    {
        check(!this->_IsGarbage())
        this->RemoveFromParent2();
        return Result;
    }

    if (It != this->Tabs.begin())
    {
        It = algo::prev(It);
    }
    if (It != this->Tabs.end())
    {
        this->SetSelectedTab(*It->first);
    }

    return Result;
}

void Jafg::WTabOverlay::CreateTabMenu(std::optional<LVec2F> Hint, WTabOverlaySelector& Selector)
{
    LVec2F Location{Hint.has_value() ? *Hint : maths::zero_vector<LVec2F>};

    CreateDropDownMenu(
        this->GetViewport(),
        Location,
        {
            .OnOptionCloseResult = [](WDismissibleFloatingWidget& Self)
            {
                return algo::reply::unhandled();
            }
        }, {.Children = {
            LDropDownNodeOption{
                .Selector = {
                    .DisplayName = "Close Tab",
                    },
                .OnAction = [&Selector]
                {
                    Selector.GetParentUntilChecked<WTabOverlay>()->CloseTab(&Selector);
                    return algo::reply::unhandled();
                },},
            LDropDownNodeOption{
                .Selector = {
                    .DisplayName = "Move Right",
                    .Icon = "Icons/Jafg.ArrowRight",
                    },
                .OnAction = [&Selector]
                {
                    Selector.GetParentUntilChecked<WTabOverlayParent>()->MoveHere(
                        Selector, *Selector.GetParentUntilChecked<WTabOverlay>(), WTabOverlayParent::EDirection::Right, false
                        );
                    return algo::reply::unhandled();
                },},
            LDropDownNodeOption{
                    .Selector = {
                    .DisplayName = "Move Down",
                    .Icon = "Icons/Jafg.ArrowDown",
                    },
                .OnAction = [&Selector]
                {
                    Selector.GetParentUntilChecked<WTabOverlayParent>()->MoveHere(
                        Selector, *Selector.GetParentUntilChecked<WTabOverlay>(), WTabOverlayParent::EDirection::Down, false
                        );
                    return algo::reply::unhandled();
                },},
            LDropDownNodeOption{
                .Selector = {
                    .DisplayName = "Split Right",
                    .Icon = "Icons/Jafg.SplitX",
                    },
                .OnAction = [&Selector]
                {
                    Selector.GetParentUntilChecked<WTabOverlayParent>()->MoveHere(
                        Selector, *Selector.GetParentUntilChecked<WTabOverlay>(), WTabOverlayParent::EDirection::Right, true
                        );
                    return algo::reply::unhandled();
                },},
            LDropDownNodeOption{
                    .Selector = {
                    .DisplayName = "Split Down",
                    .Icon = "Icons/Jafg.SplitY",
                    },
                .OnAction = [&Selector]
                {
                    Selector.GetParentUntilChecked<WTabOverlayParent>()->MoveHere(
                        Selector, *Selector.GetParentUntilChecked<WTabOverlay>(), WTabOverlayParent::EDirection::Down, true
                        );
                    return algo::reply::unhandled();
                },},
        },}
        );

    return;
}

bool Jafg::WTabOverlay::MouseTabMoveTick(WTabOverlaySelector& Selector)
{
    check(this->Selectors)

    auto ResetDrawOption{[this]
    {
        if (!!this->LastStepResult)
        {
            check(this->LastStepResult->Overlay)
            this->LastStepResult->Overlay->DrawOption = {};
            this->LastStepResult.reset();
            check(!this->DrawOption)
            this->LastMouseFlags = EModBits::Identity;
        }
    }};
    auto ResetTempBox{[this, &Selector]
    {
        if (this->TempBox)
        {
            Selector.SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
            this->TempBox->RemoveFromTree().release()->KillYourSelfNow_v2();
            this->TempBox = nullptr;
            this->CachedMoveTabSize = {};
        }
        else
        {
            check(Selector.IsDerivedHitTestInvisible())
        }
    }};

    if (Selector._IsGarbage())
    {
        ResetDrawOption();
        if (this->TempBox)
        {
            this->TempBox->RemoveFromTree().release()->KillYourSelfNow_v2();
            this->TempBox = nullptr;
            this->CachedMoveTabSize = {};
        }
        return true;
    }

    if (!this->TempBox)
    {
        check(Selector.IsDerivedHitTestInvisible())
        this->CachedMoveTabSize = Selector.GetAnchoredSize_v2();
    }

    auto GetIndex{[](WHParent& Parent, LVec2F Location) -> std::size_t
    {
        check(Parent.AabbTest({.Translation=maths::zero_vector<LVec2F>}, Location))
        for (auto Idx{0uz}; Idx < Parent.GetChildren().size(); ++Idx)
        {
            auto& Child{Parent.GetChildren()[Idx]};

            if (!Child->TransformsWidgetLayout())
            {
                continue;
            }

            auto TopLeft{Child->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)};

            if (TopLeft.x < Location.x && Location.x < TopLeft.x + Child->GetAnchoredSize_v2().x)
            {
                return Idx;
            }
        }
        check(!Parent.GetChildren().empty())
        return Parent.GetChildren().size() - 1;
    }};

    auto& Inputs{this->GetViewport().GetSurface().GetRawInputs()};

    auto It{algo::find(Inputs, LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton), &LRawInput::PhysicalKey)};
    if (It == Inputs.end() || (It->State & (ERawInputStateBits::Press | ERawInputStateBits::Hold)) == ERawInputStateBits::Identity)
    {
        if (this->TempBox)
        {
            check(this->Selectors == this->TempBox->GetParentChecked())
            auto Idx{algo::distance_to(this->Selectors->GetChildren(), this->TempBox, algo::unique_raw{})};
            this->Selectors->ReorderChild(Selector, Idx);
        }

        ResetTempBox();
        this->CachedMoveTabSize = {};
        this->UiTickHandle.Reset();
        if (!!this->LastStepResult)
        {
            check(this->LastStepResult->Overlay)
            check(this == Selector.GetParentUntilChecked<WTabOverlay>())
            std::optional<WTabOverlayParent::EDirection> Direction;
            switch (this->LastStepResult->Direction)
            {
            case StepResult::L: { Direction = WTabOverlayParent::EDirection::Left; break; }
            case StepResult::U: { Direction = WTabOverlayParent::EDirection::Up; break; }
            case StepResult::R: { Direction = WTabOverlayParent::EDirection::Right; break; }
            case StepResult::D: { Direction = WTabOverlayParent::EDirection::Down; break; }
            default: break;
            }
            this->LastStepResult->Overlay->GetParentChecked()->AsStatic<WTabOverlayParent>()
                .MoveHere(Selector, *this->LastStepResult->Overlay, Direction, !!(this->LastMouseFlags & EModBits::Control));
        }
        ResetDrawOption();
        return true;
    }

    this->LastMouseFlags = It->Mods;

    if (!this->GetViewport().GetSurface().HasMouseLocation())
    {
        ResetDrawOption();
        ResetTempBox();
        return {};
    }

    auto Location{this->GetViewport().GetSurface().GetMouseLocationValue()};

    //# TODO: Again: Translation...
    // Identity op; just discard.
    if (Selector.AabbTest({.Translation=maths::zero_vector<LVec2F>}, Location))
    {
        ResetDrawOption();
        return {};
    }

    check(Selector.GetParentChecked() == this->Selectors)
    //# TODO: Again: Translation...
    // Inline tab reorder.
    if (this->Selectors->AabbTest({.Translation=maths::zero_vector<LVec2F>}, Location))
    {
        if (!this->TempBox)
        {
            Selector.SetVisibility(ENodeVisibility::Collapsed);
            this->TempBox = &this->Selectors->AddChildAt(algo::distance_to(this->Selectors->GetChildren(), &Selector, algo::unique_raw{}),
                NewNode(this->GetViewport()).Class<WBox>()
                .Anchor(EAnchor::VFill)
                .MinDesiredSize(
                    this->CachedMoveTabSize
                    ? LNodeSize2{ENodeSize::StaticPoints, maths::max(this->CachedMoveTabSize->x, 20.0f), 0.0f}
                    : LNodeSize2{100_spt, 0.0f}
                    )
                .Tint(HighlightColor)
                .Unique()).AsStatic<WBox>();
        }

        auto Idx{GetIndex(this->Selectors->AsStatic<WHParent>(), Location)};
        this->Selectors->ReorderChild(*this->TempBox, Idx);

        ResetDrawOption();
        return {};
    }

    ResetTempBox();

    // Tab move.
    for (auto It{this->GetViewport().GetTopLevelWidgets().rbegin()}; It != this->GetViewport().GetTopLevelWidgets().rend(); ++It)
    {
        if (auto Result{this->StepThrough(**It, Location)}; Result.Overlay)
        {
            if (!!this->LastStepResult)
            {
                check(this->LastStepResult->Overlay)
                this->LastStepResult->Overlay->DrawOption = {};
            }
            this->LastStepResult = Result;
            this->LastStepResult->Overlay->DrawOption = Result.Direction;
            return {};
        }
    }

    ResetDrawOption();
    return {};
}

Jafg::WTabOverlay::StepResult Jafg::WTabOverlay::StepThrough(WParent& Node, LVec2F Location) const noexcept
{
    for (auto It{Node.GetChildren().rbegin()}; It != Node.GetChildren().rend(); ++It)
    {
        check(It->get())
        auto& Child{**It};
        if (Child.ShouldCheckForInputs())
        {
            if (!Child.IsA<WParent>())
            {
                continue;
            }
            if (auto Result{StepThrough(Child.AsStatic<WParent>(), Location)}; Result.Overlay)
            {
                return Result;
            }
            if (Child.IsA<WTabOverlay>())
            {
                auto& Candidate{Child.AsStatic<WTabOverlay>()};
                check(Candidate.Switcher)
                auto& CandidateSwitcher{*Candidate.Switcher};

                //# TODO: Wrong translation. Fix.
                if (CandidateSwitcher.AabbTest({.Translation=maths::zero_vector<LVec2F>}, Location))
                {
                    auto Offset{CandidateSwitcher.GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)}; // TODO: Wrong translation. Fix.
                    auto Size{CandidateSwitcher.GetAnchoredSize_v2()};
                    auto RelativeLocation{Location - Offset};
                    if (RelativeLocation.x < 0.0f || RelativeLocation.y < 0.0f || RelativeLocation.x > Size.x || RelativeLocation.y > Size.y)
                    {
                        continue;
                    }
                    if (RelativeLocation.x < StepResult::GetMaxDistanceFromEdge<StepResult::L>(Size.x))
                    {
                        return {&Candidate, StepResult::L};
                    }
                    if (RelativeLocation.y < StepResult::GetMaxDistanceFromEdge<StepResult::U>(Size.y))
                    {
                        return {&Candidate, StepResult::U};
                    }
                    if (RelativeLocation.x > StepResult::GetMaxDistanceFromEdge<StepResult::R>(Size.x))
                    {
                        return {&Candidate, StepResult::R};
                    }
                    if (RelativeLocation.y > StepResult::GetMaxDistanceFromEdge<StepResult::D>(Size.y))
                    {
                        return {&Candidate, StepResult::D};
                    }
                    return {&Candidate, StepResult::C};
                }
            }
            continue;
        }
    }
    return {.Overlay={}};
}

void Jafg::WTabOverlaySelector::LoadRightIcon()
{
    this->RightIcon = this->GetMutableFrontend().GetSubsystemChecked<JTextureSubsystem>()->FromTextureViewIdentifier("Icons/Jafg.SmallX");
    this->RightIconStyle.SetEverywhere<&LTextButtonIconBrush::InwardsPadding>(16_spt);
    this->RightIconStyle.SetEverywhere<&LTextButtonIconBrush::MinIconSize>(16_spt);
    this->RightIconStyle.SetEverywhere<&LTextButtonIconBrush::Alignment>(LTextButtonIconBrush::Align::Center);
    this->bDecoupledRightIcon = true;
    this->RightIconStyle.Set<EIconStyleBits::Decoupled, &LTextButtonIconBrush::Tint>(*GetSingleton<JUserPreferences>().DangerColor);
    this->DecoupledRightKeyDown = [](auto&, auto&){ return LNodeReply::Handled(); };
    this->DecoupledRightKeyUp = [this](LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
    {
        auto* TabOverlay{this->GetParentUntilChecked<WTabOverlay>()};
        TabOverlay->CloseTab(this);
        return LNodeReply::Handled();
    };

    return;
}

void Jafg::WTabOverlayParent::OnRemoveChildPost(WNode& Child)
{
    Super::OnRemoveChildPost(Child);

    if (!this->bPreventAutoKillOnChildLoss)
    {
        if (this->GetChildren().empty())
        {
            this->RemoveFromParent2();
        }
        else if (this->GetChildren().size() == 1)
        {
            if (this->GetParent() && this->GetParent()->IsA<WTabOverlayParent>())
            {
                WTabOverlayParent& Parent{this->GetParent()->AsStatic<WTabOverlayParent>()};
                LOG_TRACE(LogWidgets, "Removing redundant overlay parent.")
                auto Idx{algo::distance(Parent.GetChildren(), algo::find(Parent.GetChildren(), this, algo::unique_raw{}))};
                auto bOld{std::exchange(Parent.bPreventAutoKillOnChildLoss, true)};
                auto Child{this->GetChildren()[0]->RemoveFromTree()};
                check(this->GetChildren().empty())
                check(this->_IsGarbage() && !this->IsParentValid())
                Parent.AddConstructedChildAt(Idx, std::move(Child));
                Parent.bPreventAutoKillOnChildLoss = bOld;
            }
        }
    }

    return;
}

void Jafg::WTabOverlayParent::MoveHere(WTabOverlaySelector& WhoSelector, WTabOverlay& Where, std::optional<EDirection> Direction, bool bDuplicate)
{
    check(this->Possibilities)
    check(!this->_IsGarbage())
    check(algo::contains(this->GetChildren(), &Where, algo::unique_raw{}))
    check(!Where._IsGarbage())

    WTabOverlay& Who{*WhoSelector.GetParentUntilChecked<WTabOverlay>()};
    bool bWhoInline{algo::contains(this->GetChildren(), &Who, algo::unique_raw{})};
    if (bWhoInline && !bDuplicate)
    {
        if (&Who == &Where)
        {
            if (Who.Tabs.size() == 1)
            {
                LOG_TRACE(LogWidgetFramework, "Inline moving a tab results in a noop.")
                return;
            }
            if (!Direction)
            {
                LOG_TRACE(LogWidgetFramework, "No direction provided for inline tab move. This results in a noop.")
                return;
            }
        }
    }

    algo::raii_leave _{[this, bOld = std::exchange(this->bPreventAutoKillOnChildLoss, true)]{ this->bPreventAutoKillOnChildLoss = bOld; }};
    std::size_t WhereDistance(algo::distance_to(this->GetChildren(), &Where, algo::unique_raw{}));
    std::size_t WhoDistance(algo::distance_to(this->GetChildren(), &Who, algo::unique_raw{}));

    LTabCreateInfo CreateInfo{.Selector={
        .DisplayName = WhoSelector.GetContent(),
        .Icon = bDuplicate ? WhoSelector.LeftIcon : std::move(WhoSelector.LeftIcon),
        },};
    if (bDuplicate)
    {
        CreateInfo.Panel = algo::find_checked(Who.GetTabs(), &WhoSelector, algo::pair_first{})
            ->second->GetVirtualTable();
    }
    else
    {
        CreateInfo.Panel = *Who.CloseTabImpl(&WhoSelector, true);
    }

    if (Direction)
    {
        TReference<WTabOverlayParent> Target{*this};

        // If the size is one, we can trivially change the control flow without needing to create sub parents.
        if (this->GetChildren().size() == 1)
        {
            if (this->Cf == ENodePrimitiveControlFlow::Horizontal && (Direction == EDirection::Up || Direction == EDirection::Down))
            {
                this->Cf = ENodePrimitiveControlFlow::Vertical;
            }
            else if (this->Cf == ENodePrimitiveControlFlow::Vertical && (Direction == EDirection::Left || Direction == EDirection::Right))
            {
                this->Cf = ENodePrimitiveControlFlow::Horizontal;
            }
        }

        std::optional<std::size_t> Idx;
        if ((this->Cf == ENodePrimitiveControlFlow::Horizontal && (Direction == EDirection::Left || Direction == EDirection::Right))
         || (this->Cf == ENodePrimitiveControlFlow::Vertical   && (Direction == EDirection::Up   || Direction == EDirection::Down)))
        {
            Idx = (Direction == EDirection::Left || Direction == EDirection::Up) ? WhereDistance : WhereDistance + 1;
            if (Where._IsGarbage())
            {
                Idx = maths::max(*Idx, 1uz) - 1;
            }
            if (bWhoInline && Who._IsGarbage() && &Who != &Where)
            {
                if (WhoDistance < WhereDistance)
                {
                    Idx = maths::max(*Idx, 1uz) - 1;
                }
            }
        }
        else
        {
            check(!Where._IsGarbage())

            LOG_TRACE(LogWidgets, "Creating new overlay parent for tab movement.")
            Target = *StaticCast<WTabOverlayParent>(&this->AddChildAt(WhereDistance, this->Possibilities->GetNewOverlayParent().Unique()));
            if (Direction == EDirection::Left || Direction == EDirection::Right)
            {
                Target->Cf = ENodePrimitiveControlFlow::Horizontal;
            }
            else if (Direction == EDirection::Up || Direction == EDirection::Down)
            {
                Target->Cf = ENodePrimitiveControlFlow::Vertical;
            }

            auto RemovedTabOverlay{Where.RemoveFromTree()};
            check(&*RemovedTabOverlay == &Where)
            /* Resetting anchor to default. Modifying the anchor of children of overlay parents is not allowed. */
            RemovedTabOverlay->Anchor = EAnchor::TopLeft;
            RemovedTabOverlay->MinDesiredSize = {};
            RemovedTabOverlay->MaxDesiredSize = {};
            Target->AddConstructedChild(std::move(RemovedTabOverlay));

            Idx = Direction == EDirection::Left || Direction == EDirection::Up ? 0 : Target->GetChildren().size();
        }

        if (Idx)
        {
            Target->AddChildAt(*Idx, this->GetPossibilitiesChecked()->GetNewOverlay().Unique())
                .AsStatic<WTabOverlay>().RegisterTab(std::move(CreateInfo));
        }
    }
    else if (Where._IsGarbage())
    {
        this->AddChildAt(WhereDistance, this->GetPossibilitiesChecked()->GetNewOverlay().Unique())
            .AsStatic<WTabOverlay>().RegisterTab(std::move(CreateInfo));
    }
    else
    {
        Where.SetSelectedTab(*Where.RegisterTab(std::move(CreateInfo)).first);
    }

    return;
}
