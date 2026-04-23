// Copyright mzoesch. All rights reserved.

#include "Nodes/DropDown.h"
#include "Nodes/FloatingWidget.h"
#include "Nodes/HRegion.h"
#include "Nodes/TextBox.h"
#include "Nodes/TextButton.h"
#include "User/UserPreferences.h"

Jafg::WDismissibleFloatingWidget& Jafg::CreateDropDownMenu(LViewport& Viewport, LVec2F Position, LDropDownMenuCreateInfo CreateInfo, LDropDownNodeSubMenu const& Submenu)
{
    WDismissibleFloatingWidget* Result;
    ConstructDeferredWidget(Jafg::TNodeStaticInit<WDismissibleFloatingWidget>{Viewport}).Style().SaveTo(&Result)
        .Decorate(false)
        .CreateResizeUi(false)
        .InitialWindowSize({100_pt, 0})
        .InitialWindowPosition(Position)
        .Content([Result, &CreateInfo, &Submenu](WFloatingWidget& FloatingWidget, WParent& Container)
        {
            WVRegion* Region;
            BeginStyling(Container).StaticRoot<WVRegion>().SaveTo(&Region)
                .Tint(*GetSingleton<JUserPreferences>().OverlayColor);
            if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Warning)) if (Submenu.Children.empty())
            {
                LOG_WARNING(LogWidgetFramework, "Submenu [{}] has no children.", Submenu.DisplayName)
            }
            for (auto& Child : Submenu.Children)
            {
                std::visit([Result, &CreateInfo, Region]<typename T0>(T0&& Node)
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
                            .InBrush<EStyleBits::Normal, &LBoxBrush::bSkipBrushDraw>(true)
                            .InBrush<EStyleBits::Hover, &LBoxBrush::Tint>(*Prefs.PrimaryColor)
                            .InBrush<EStyleBits::Press, &LBoxBrush::Tint>(*Prefs.PrimaryColor2)
                            .OnKeyUp([Result, OnClose = CreateInfo.OnOptionCloseResult, Action = Node.OnAction](auto&&...)
                            {
                                check(Result)
                                check(!!Action)
                                if (!Action().IsHandled())
                                {
                                    if (OnClose)
                                    {
                                        if (!OnClose(*Result).IsHandled())
                                        {
                                            Result->MarkAsGarbage_v2();
                                        }
                                    }
                                    else
                                    {
                                        Result->MarkAsGarbage_v2();
                                    }
                                }
                                return LReply::Handled();
                            })
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
            return;
        });

    return *Result;
}

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
            .Unique()
            );
    }

    return;
}

void Jafg::WDropDown::Select(WTextBox* Target /* = nullptr */)
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
        check(algo::contains(this->RootSubmenuContainer->GetChildren(), Target, algo::unique_raw{}))
        StaticCast<WTextBox>(Target)->Brush.bSkipBrushDraw = false;
    }

    return;
}

void Jafg::WDropDown::OnDismiss(WFloatingWidget& FloatingWidget)
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

Jafg::LCursorReply Jafg::WDropDown::OnMouseEnterInRoot(WNode& Node, LDropDownNodeSubMenu const& Submenu)
{
    if (this->OpenSubmenus.contains(&Submenu))
    {
        check(this->OpenSubmenus[&Submenu])
        check(Node.As<WTextBox>())
        check(Node.As<WTextBox>()->Brush.bSkipBrushDraw == false)
        return LCursorReply::Handled();
    }

    this->Select(&Node.AsStatic<WTextBox>());

    TArray<TJxxUnique<WFloatingWidget>> SubmenusToClose; SubmenusToClose.reserve(this->OpenSubmenus.size());
    for (auto& FloatingWidget: this->OpenSubmenus | std::views::values) { SubmenusToClose.push_back(std::move(FloatingWidget)); }
    Tasks::Make(ENamedThreads::Master, ETaskTime::Late, [SubmenusToClose = std::move(SubmenusToClose)]() mutable{});
    this->OpenSubmenus.clear();

    auto& FloatingWidget{CreateDropDownMenu(
        this->GetViewport(),
        // TODO: Fix the translation...
        Node.GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)
            + LVec2F{0.0, Node.GetAnchoredSize_v2().y},
        {.OnOptionCloseResult = [this](auto&&...)
        {
            this->OpenSubmenus.clear();
            this->Select(nullptr);
            return LPrimitiveReply::Handled();
        }},
        Submenu
        )};
    check(!FloatingWidget.OnDismissEvent.IsValid())
    FloatingWidget.OnDismissEvent.Bind(this, &WDropDown::OnDismiss);
    check(this->OpenSubmenus.contains(&Submenu) == false)

    this->OpenSubmenus.emplace(&Submenu, TJxxUnique<WFloatingWidget>{&FloatingWidget});

    return LCursorReply::Handled();
}
