// Copyright mzoesch. All rights reserved.

#include "Nodes/DropDownTabBar.h"
#include "Nodes/HRegion.h"
#include "Nodes/Text.h"
#include "Nodes/TextBox.h"
#include "Nodes/DropDown.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "User/UserPreferences.h"

void Jafg::WDropDownTabBar::Construct()
{
    Super::Construct();

    BeginStyling(*this)
        .StaticRoot<WHRegion>().SaveTo(&this->RootSubmenuContainer)
        .Anchor(EAnchor::Fill)
        .Tint(*GetSingleton<JUserPreferences>().BackgroundColor)
        ;

    for (auto& SubMenu: this->Tabs.SubMenus)
    {
        LString DisplayName;
        if (std::holds_alternative<LDropDownNodeSubmenu>(SubMenu))
        {
            DisplayName = std::get<LDropDownNodeSubmenu>(SubMenu).Selector.DisplayName;
        }
        else if (std::holds_alternative<LDropDownNodeDeferredSubMenu>(SubMenu))
        {
            DisplayName = std::get<LDropDownNodeDeferredSubMenu>(SubMenu).Selector.DisplayName;
        }
        else
        {
            std::unreachable();
        }
        this->RootSubmenuContainer->AddChild(NewStaticNode(WTextBox).SkipBrushDraw(true)
            .Visibility(ENodeVisibility::Visible)
            .Content(std::move(DisplayName))
            .Tint(*GetSingleton<JUserPreferences>().PrimaryColor)
            .OnCursorEnter([this, &SubMenu](WNode& Node){ return this->OnMouseEnterInRoot(Node, SubMenu); })
            .Unique()
            );
        continue;
    }

    return;
}

void Jafg::WDropDownTabBar::Select(WTextBox* Target /* = nullptr */)
{
    check(this->RootSubmenuContainer)
    for (auto& Child: this->RootSubmenuContainer->GetChildren())
    {
        check(Child.get())
        if (auto* TextBox{StaticCast<WTextBox>(&*Child)}; TextBox != Target)
        {
            TextBox->Brush.bSkipBrushDraw = true;
        }
        continue;
    }

    if (Target)
    {
        check(algo::contains(this->RootSubmenuContainer->GetChildren(), Target, algo::unique_raw))
        StaticCast<WTextBox>(Target)->Brush.bSkipBrushDraw = false;
    }

    return;
}

void Jafg::WDropDownTabBar::OnDismiss(WFloatingWidget& FloatingWidget)
{
    for (auto& Child: this->RootSubmenuContainer->GetChildren())
    {
        check(Child.get())
        StaticCast<WTextBox>(&*Child)->Brush.bSkipBrushDraw = true;
    }
    for (auto& [SubMenu, Widget] : this->OpenSubmenus)
    {
        if (Widget.get() == &FloatingWidget)
        {
            Widget.release();
            this->OpenSubmenus.erase(SubMenu);
            break;
        }
    }

    return;
}

Jafg::LNodeReply Jafg::WDropDownTabBar::OnMouseEnterInRoot(WNode& Node, LDropDownNodeParent const& Submenu)
{
    if (this->OpenSubmenus.contains(&Submenu))
    {
        check(this->OpenSubmenus[&Submenu])
        check(Node.As<WTextBox>())
        check(Node.As<WTextBox>()->Brush.bSkipBrushDraw == false)
        return LNodeReply::Handled();
    }

    this->Select(&Node.AsStatic<WTextBox>());

    TArray<TJxxUnique<WFloatingWidget>> SubmenusToClose; SubmenusToClose.reserve(this->OpenSubmenus.size());
    for (auto& FloatingWidget: this->OpenSubmenus | std::views::values) { SubmenusToClose.push_back(std::move(FloatingWidget)); }
    Tasks::Make(ENamedThreads::Master, ETaskTime::Late, [SubmenusToClose = std::move(SubmenusToClose)]() mutable{});
    this->OpenSubmenus.clear();

    WDismissibleFloatingWidget* FloatingWidget;
    if (std::holds_alternative<LDropDownNodeSubmenu>(Submenu))
    {
        FloatingWidget = &CreateDropDownMenu(
                this->GetViewport(),
                Node.GetAnchoredAndTranslatedTopLeftFromMostOuter(Node.GetTranslationFromMostOuter())
                + LVec2F{0.0, Node.GetAnchoredSize_v2().y},
                {
                    .OnOptionCloseResult = [this](auto&&...)
                    {
                        this->OpenSubmenus.clear();
                        this->Select(nullptr);
                        return algo::reply::handled();
                    }
                },
                std::get<LDropDownNodeSubmenu>(Submenu).Children
                );
    }
    else if (std::holds_alternative<LDropDownNodeDeferredSubMenu>(Submenu))
    {
        check(std::get<LDropDownNodeDeferredSubMenu>(Submenu).OnChildren)
        FloatingWidget = &CreateDropDownMenu(
                this->GetViewport(),
                Node.GetAnchoredAndTranslatedTopLeftFromMostOuter(Node.GetTranslationFromMostOuter())
                + LVec2F{0.0, Node.GetAnchoredSize_v2().y},
                {
                    .OnOptionCloseResult = [this](auto&&...)
                    {
                        this->OpenSubmenus.clear();
                        this->Select(nullptr);
                        return algo::reply::handled();
                    }
                },
                std::get<LDropDownNodeDeferredSubMenu>(Submenu).OnChildren()
                );
    }
    else
    {
        std::unreachable();
    }

    check(FloatingWidget)
    check(!FloatingWidget->OnDismissEvent.IsValid())
    FloatingWidget->OnDismissEvent.Bind(this, &WDropDownTabBar::OnDismiss);
    check(!this->OpenSubmenus.contains(&Submenu))

    this->OpenSubmenus.emplace(&Submenu, TJxxUnique<WFloatingWidget>{FloatingWidget});

    return LNodeReply::Handled();
}
