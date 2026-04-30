// Copyright mzoesch. All rights reserved.

#include "Nodes/TabOverlay.h"
#include "User/UserPreferences.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "Nodes/UserWidget.h"
#include "Nodes/DropDown.h"
#include "Nodes/HRegion.h"

namespace
{
auto NewOverlay{[](Jafg::LTabOverlayPossibilities& Self) {
    return Jafg::NewNode(Self.Owner.GetViewport()).Class<Jafg::WTabOverlay>()
        .Possibilities(Self)
        .KillIfNoChildren(true)
        .Visibility(Jafg::ENodeVisibility::Visible)
        .MinDesiredSize({25_pt, 0.0f});
    }};

} /* ~Namespace <Anonymous> */

void Jafg::LTabOverlayPossibilities::FindNewOverlay()
{
    WTabOverlayHParent* TabOverlayHParent{this->Owner.FindNodeInVisiblePathChecked<WTabOverlayHParent>()};
    TabOverlayHParent->AddChild(NewOverlay(*this).Unique());

    return;
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
            this->FindNewOverlay();
        }
        Overlay = &this->Overlays.front();
    }

    WUserWidget* Result{Overlay->RegisterTab(std::move(Info)).second};
    check(Result)
    auto It{algo::find_checked(Overlay->GetTabs(), Result, algo::pair_second{})};
    Overlay->SetSelectedTab(*It->first);
    return *Result;
}

void Jafg::WTabOverlay::Construct()
{
    Super::Construct();

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
    BeginStyling(*this).StaticRoot<WVRegion>().SaveTo(&this->Wrapper)
        .Anchor(EAnchor::Fill)
        .SkipBrushDraw(true)
    [
          NewStaticNode(WHRegion).SaveTo(&this->Selectors)
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

    if (this->bKillIfNoChildren && this->Switcher->GetChildren().empty())
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
                    .Icon = "Icons/Jafg.SplitX",
                    },
                .OnAction = [/*this,*/ &Selector]
                {
                    Selector.GetParentUntilChecked<WTabOverlayHParent>()->Move(Selector, WTabOverlayHParent::EDirection::Right);
                    return algo::reply::unhandled();
                },},
            LDropDownNodeOption{
                .Selector = {
                    .DisplayName = "Move Left",
                    .Icon = "Icons/Jafg.SplitX",
                    },
                },
            LDropDownNodeOption{
                .Selector = {
                    .DisplayName = "Move Up",
                    .Icon = "Icons/Jafg.SplitY",
                    },
                },
            LDropDownNodeOption{
                    .Selector = {
                    .DisplayName = "Move Down",
                    .Icon = "Icons/Jafg.SplitY",
                    },
                },
            LDropDownNodeOption{
                .Selector = {
                    .DisplayName = "Split Right",
                    .Icon = "Icons/Jafg.SplitX",
                    },
                },
            LDropDownNodeOption{
                .Selector = {
                    .DisplayName = "Split Left",
                    .Icon = "Icons/Jafg.SplitX",
                    },
                },
            LDropDownNodeOption{
                .Selector = {
                    .DisplayName = "Split Up",
                    .Icon = "Icons/Jafg.SplitY",
                    },
                },
            LDropDownNodeOption{
                    .Selector = {
                    .DisplayName = "Split Down",
                    .Icon = "Icons/Jafg.SplitY",
                    },
                },
        },}
        );

    return;
}

void Jafg::WTabOverlaySelector::LoadRightIcon()
{
    this->RightIcon = this->GetMutableFrontend().GetSubsystemChecked<JTextureSubsystem>()->FromTextureViewIdentifier("Icons/Jafg.SmallX");
    this->RightIconStyle.SetEverywhere<&LTextButtonIconBrush::InwardsPadding>(12_spt);
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

void Jafg::WTabOverlayHParent::Move(WTabOverlaySelector& Selector, EDirection Direction)
{
    check(this->Possibilities)
    check(Direction == EDirection::Right)

    WTabOverlay& Target{this->AddChild(NewOverlay(*this->Possibilities).Unique()).AsStatic<WTabOverlay>()};

    WTabOverlay& PreviousTabOverlay{*Selector.GetParentUntilChecked<WTabOverlay>()};
    check(algo::find(PreviousTabOverlay.GetTabs(), &Selector, algo::pair_first{}) != PreviousTabOverlay.GetTabs().end())
    auto Panel{PreviousTabOverlay.CloseTabImpl(&Selector, true)};
    check(Panel.has_value())

    Target.RegisterTab({
        .Selector = {
            .DisplayName = Selector.GetContent(),
            .Icon = std::move(Selector.LeftIcon),
            },
        .Panel = std::move(*Panel),
        });

    return;
}
