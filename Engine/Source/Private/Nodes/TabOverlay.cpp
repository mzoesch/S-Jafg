// Copyright mzoesch. All rights reserved.

#include "Nodes/TabOverlay.h"
#include "User/UserPreferences.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "Nodes/UserWidget.h"

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

Jafg::WTabOverlay::Tab Jafg::WTabOverlay::RegisterTab(LTabCreateInfo&& Descriptor)
{
    if (this->Wrapper == nullptr)
    {
        this->InitializeBoilerplate();
    }
    check(this->Wrapper && this->Selectors && this->Switcher)

    this->Switcher->AddChild(NewNode(this->GetViewport()).Class(Descriptor.Panel.GetClassOrDefault()).Unique());
    WUserWidget* Panel{this->Switcher->GetChildren().back()->AsChecked<WUserWidget>()};

    bool bActivated{};
    if (this->Switcher->GetChildren().size() == 1)
    {
        bActivated = true;
        this->Switcher->SetActiveNodeByIndex(0);
    }

    WTabOverlaySelector* _Selector{};
    BeginStyling(*this->Selectors).StaticRoot<WTabOverlaySelector>(std::move(Descriptor.Selector)).SaveTo(&_Selector)
        .Delegate(this->DefaultSelectorDelegate);
    check(_Selector)
    auto& Selector{*_Selector};
    this->Tabs.emplace_back(&Selector, Panel);
    check(Selector.OnKeyDownEvent.IsValid() == false)
    Selector.OnKeyDownEvent.Bind([this]
    (WNode& Self, LNodeKeyEventData const& Data, LKeyEvent const& Event) -> LReply
    {
        if (Event.PhysicalKey != LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
        {
            return LReply::Unhandled();
        }
        check(this->Switcher)
        this->SetSelectedTab(*StaticCast<WTabOverlaySelector>(&Self));
        return LReply::Unhandled();
    });
    if (bActivated)
    {
        Selector.SetSelected(true);
    }

    for (LTabCreateInfo& Sibling : Descriptor._Siblings)
    {
        this->RegisterTab(std::move(Sibling));
    }

    return {&Selector, Panel};
}

void Jafg::WTabOverlay::CloseTab(WTabOverlaySelector* Selector)
{
    check(this->Switcher)

    for (auto It{this->Tabs.begin()}; It != this->Tabs.end(); ++It)
    {
        if (It->first == Selector)
        {
            It->first->RemoveFromParent2();
            It->second->RemoveFromParent2();
            this->Tabs.erase(It);
            break;
        }
    }

    if (this->Switcher->GetActiveNodeIndex() != WSwitcher::NoActiveNodeIndex)
    {
        auto It{algo::find(this->Tabs, this->Switcher->GetActiveNodeChecked(), &Tab::second)};
        check(It != this->Tabs.end())
        this->SetSelectedTab(*It->first);
    }

    return;
}

Jafg::WUserWidget* Jafg::WTabOverlay::FindWidgetSlow(LCxxClass const& Class) noexcept
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

void Jafg::WTabOverlay::InitializeBoilerplate()
{
    check(!this->Wrapper && !this->Selectors && !this->Switcher)
    BeginStyling(*this).Root(this->WrapperInjection).SaveTo(&this->Wrapper)
        .Anchor(EAnchor::Fill)
        .Inject(this->WrapperInjection)
    [
        NewDynamicNode(this->SelectorsInjection).SaveTo(&this->Selectors)
            .Inject(this->SelectorsInjection)
        +
        NewDynamicNode(this->SwitcherInjection).SaveTo(&this->Switcher)
            .Anchor(EAnchor::Fill)
            .Inject(this->SwitcherInjection)
    ];
    return;
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

void Jafg::WTabOverlaySelector::LoadRightIcon()
{
    this->RightIcon = this->GetFrontend().GetSubsystemChecked<JTextureSubsystem>()->FromTextureViewIdentifier("Icons/Jafg.SmallX");
    this->RightIconStyle.SetEverywhere<&LTextButtonIconBrush::InwardsPadding>(12_spt);
    this->bDecoupledRightIcon = true;
    this->RightIconStyle.Set<EIconStyleBits::Decoupled, &LTextButtonIconBrush::Tint>(*GetSingleton<JUserPreferences>().DangerColor);
    this->DecoupledRightKeyDown = [](auto&, auto&){ return LReply::Handled(); };
    this->DecoupledRightKeyUp = [this](LNodeKeyEventData const& Data, LKeyEvent const& Event)
    {
        auto* TabOverlay{this->GetParentUntilChecked<WTabOverlay>()};
        TabOverlay->CloseTab(this);
        return LReply::Handled();
    };
}
