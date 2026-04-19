// Copyright mzoesch. All rights reserved.

#include "Nodes/DropDown.h"
#include "Nodes/FloatingWidget.h"
#include "Nodes/HRegion.h"
#include "Nodes/TextBox.h"
#include "Nodes/TextButton.h"
#include "User/UserPreferences.h"

void Jafg::WDropDown::Construct()
{
    Super::Construct();

    BeginStyling(*this)
        .StaticRoot<WHRegion>().SaveTo(&this->RootSubmenuContainer)
        .Anchor(EAnchor::Fill)
        .Tint(*GetSingleton<JUserPreferences>().BackgroundColor)
        ;

    for (LDropDownNodeSubMenu& SubMenu: this->DropDownRoot.Nodes)
    {
        this->RootSubmenuContainer->AddChild(NewStaticNode(WTextBox).SkipBrushDraw(true)
            .Visibility(ENodeVisibility::Visible)
            .Content(SubMenu.DisplayName)
            .Tint(*GetSingleton<JUserPreferences>().PrimaryColor)
            .OnCursorEnter([this, &SubMenu](WNode& Node){ return this->OnMouseEnterInRoot(Node, SubMenu); })
            .OnCursorLeave([this, &SubMenu](WNode& Node){ return this->OnMouseLeaveInRoot(Node, SubMenu); })
            .Unique()
            );
    }

    return;
}

void Jafg::WDropDown::OnDismiss(WFloatingWidget& FloatingWidget)
{
    for (auto& Child: this->RootSubmenuContainer->GetChildren())
    {
        check(Child.get())
        StaticCast<WTextBox>(&*Child)->SetSkipBrushDraw(true);
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

Jafg::LCursorReply Jafg::WDropDown::OnMouseEnterInRoot(WNode& Node, LDropDownNodeSubMenu const& Submenu)
{
    if (this->OpenSubmenus.contains(&Submenu))
    {
        check(this->OpenSubmenus[&Submenu])
        check(Node.As<WTextBox>())
        check(Node.As<WTextBox>()->GetSkipBrushDraw() == false)
        return LCursorReply::Handled();
    }

    for (auto& Child: this->RootSubmenuContainer->GetChildren())
    {
        check(Child.get())
        StaticCast<WTextBox>(&*Child)->SetSkipBrushDraw(true);
    }
    StaticCast<WTextBox>(&Node)->SetSkipBrushDraw(false);

    TArray<TJxxUnique<WFloatingWidget>> SubmenusToClose; SubmenusToClose.reserve(this->OpenSubmenus.size());
    for (auto& FloatingWidget: this->OpenSubmenus | std::views::values) { SubmenusToClose.push_back(std::move(FloatingWidget)); }
    Tasks::Make(ENamedThreads::Master, ETaskTime::Late, [SubmenusToClose = std::move(SubmenusToClose)]() mutable{});
    this->OpenSubmenus.clear();

    WFloatingWidget* FloatingWidget{};
    ConstructDeferredWidget(Jafg::TNodeStaticInit<WDismissibleFloatingWidget>{this->GetViewport()}).Style().SaveTo(&FloatingWidget)
        .Decorate(false)
        .CreateResizeUi(false)
        .InitialWindowSize({100_pt, 0})
        .InitialWindowPosition(
            // TODO: Fix the translation...
            Node.GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)
            + LVec2F{0.0, Node.GetAnchoredSize_v2().y})
        .Content([this, &Submenu](WFloatingWidget& FloatingWidget, WParent& Container)
        {
            WVRegion* Region{};
            BeginStyling(Container).StaticRoot<WVRegion>().SaveTo(&Region)
                .OnCursorLeave(this, &WDropDown::OnMouseLeaveFloatingWidget)
                .Tint(*GetSingleton<JUserPreferences>().OverlayColor);
            if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Warning)) if (Submenu.Children.empty())
            {
                LOG_WARNING(LogWidgetFramework, "Submenu [{}] has no children.", Submenu.DisplayName)
            }
            for (auto& Child : Submenu.Children)
            {
                std::visit([Region]<typename T0>(T0&& Node)
                {
                    typedef std::decay_t<T0> T;
                    if constexpr (std::is_same_v<T, LDropDownNodeSubMenu>)
                    {
                        unimplemented()
                    }
                    else if constexpr (std::is_same_v<T, LDropDownNodeOption>)
                    {
                        auto& Prefs{GetSingleton<JUserPreferences>()};
                        Region->AddChild(NewNode(Region->GetViewport()).Class<WTextButton>()
                            .Anchor(EAnchor::Fill)
                            .NormalSkipBrushDraw(true)
                            .HoverTint(*Prefs.PrimaryColor)
                            .PressTint(*Prefs.PrimaryColor2)
                            .OnPrimaryRelease([Option = &Node](WTextButton&,LKeyEvent const&){ Option->OnAction(*Option); })
                            .Content(Node.Selector.DisplayName)
                            .LeftIcon(Node.Selector.Icon.GetResolved())
                            .Unique()
                            );
                    }
                    else if constexpr (std::is_same_v<T, LDropDownNodeSeparator>)
                    {
                    }
                    else
                    {
                        static_assert(sizeof(T) == 0, "Non-exhaustive visitor.");
                    }
                }, Child);
            }
        })
        .OnDismiss(this, &WDropDown::OnDismiss);
    check(this->OpenSubmenus.contains(&Submenu) == false)
    this->OpenSubmenus.emplace(&Submenu, TJxxUnique<WFloatingWidget>{FloatingWidget});

    return LCursorReply::Handled();
}

Jafg::LCursorReply Jafg::WDropDown::OnMouseLeaveInRoot(WNode& Node, LDropDownNodeSubMenu const& Submenu)
{
    // StaticCast<WTextBox>(&Node)->SetSkipBrushDraw(true);
    // if (this->OpenSubmenus.contains(&Submenu))
    // {
    //     auto& FloatingWidget{*this->OpenSubmenus[&Submenu]};
    //     this->OpenSubmenus.erase(&Submenu);
    //
    //     Tasks::Make(ENamedThreads::Master, ETaskTime::Late, [RemoveMeOuter = &FloatingWidget.GetOuter(), RemoveMe = &FloatingWidget]
    //     {
    //         if (IsValidSlow(RemoveMeOuter, RemoveMe))
    //         {
    //             RemoveMe->MarkAsGarbage_v2();
    //         }
    //     });
    // }
    return LCursorReply::Handled();
}

Jafg::LCursorReply Jafg::WDropDown::OnMouseLeaveFloatingWidget(WNode& Node)
{
    auto& MostOuterParent{Node.GetMostOuterParent()};
    // MostOuterParent.MarkAsGarbage_v2();



    return LCursorReply::Handled();
}
