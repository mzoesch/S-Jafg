// Copyright mzoesch. All rights reserved.

#include "Nodes/DropDown.h"
#include "Nodes/FloatingWidget.h"
#include "Nodes/HButton.h"
#include "Nodes/HRegion.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "User/UserPreferences.h"
#include "Nodes/TextIconizedSeparator.h"

Jafg::WDismissibleFloatingWidget& Jafg::CreateDropDownMenu(LViewport& Viewport, LVec2F Position, LDropDownMenuCreateInfo CreateInfo, LDropDownNodeSubMenu const& Submenu)
{
    Position = maths::round(Position);

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
                .Tint(*GetSingleton<JUserPreferences>().OverlayColor)
                .Padding({0, LDropDownMenuCreateInfo::RecommendedPadding});
            if constexpr (IS_COMPILED_LOG(LogWidgetFramework, Warning)) if (Submenu.Children.empty())
            {
                LOG_WARNING(LogWidgetFramework, "Submenu [{}] has no children.", Submenu.DisplayName)
            }
            for (auto& Child : Submenu.Children)
            {
                std::visit([Result, &CreateInfo, Region]<typename T0>(T0&& Node)
                {
                    typedef std::decay_t<T0> T;
                    if constexpr (std::is_same_v<T, LDropDownNodeOption>)
                    {
                        auto& Prefs{GetSingleton<JUserPreferences>()};
                        Region->AddChild(NewNode(Region->GetViewport()).Class<WTextButtonIconizedDouble>()
                            .Anchor(EAnchor::HFill)
                            .InBrush<EStyleBits::Normal|EStyleBits::Disabled, &LBoxBrush::bSkipBrushDraw>(true)
                            .InBrush<EStyleBits::Hover, &LBoxBrush::Tint>(*Prefs.PrimaryColor)
                            .InBrush<EStyleBits::Press, &LBoxBrush::Tint>(*Prefs.PrimaryColor2)
                            .InAllBrushes<&LBoxBrush::Padding>({LDropDownMenuCreateInfo::RecommendedPadding, 0})
                            .InTextBrush<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray)
                            .InAllLeftIconBrushes<&LIconBrush::bAlwaysPad>(true)
                            .OnKeyUpFocused([Result, OnOptionCloseResult=CreateInfo.OnOptionCloseResult, Action=Node.OnAction]
                                (WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                                {
                                    check(Result)
                                    check(!!Action)
                                    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                                    {
                                        if (Action(Self, Info, Event).is_handled())
                                        {
                                            return LNodeReply::Handled();
                                        }
                                        if (!OnOptionCloseResult || !OnOptionCloseResult(*Result).is_handled())
                                        {
                                            check(Result->IsTopLevel())
                                            Result->MarkAsGarbage_v2();
                                        }
                                        return LNodeReply::Handled();
                                    }
                                    return LNodeReply::Unhandled();
                                })
                            .Content(Node.Selector.DisplayName)
                            .LeftIcon(Node.Selector.Icon.GetResolved())
                            .Enabled(Node.IsEnabled)
                            .Unique()
                            );
                    }
                    else if constexpr (std::is_same_v<T, LDropDownNodeInformation>)
                    {
                        Region->AddChild(NewNode(Region->GetViewport())
                            .Class<WText>()
                            .Padding(std::holds_alternative<LDropDownNodeInformation::IconPadding>(Node.Padding)
                                    ? LPadding{LDropDownMenuCreateInfo::RecommendedTextPadding, 0, LDropDownMenuCreateInfo::RecommendedPadding.Size, 0}
                                : std::holds_alternative<LDropDownNodeInformation::MinPadding>(Node.Padding)
                                    ? LPadding{LDropDownMenuCreateInfo::RecommendedPadding, 0.0f}
                                : std::holds_alternative<LPadding>(Node.Padding)
                                    ? std::get<LPadding>(Node.Padding)
                                    : LPadding{}
                                )
                            .TextTint(Colors::Gray)
                            .Content(Node.What)
                            .Unique()
                            );
                    }
                    else if constexpr (std::is_same_v<T, LDropDownNodeCustom>)
                    {
                        check(!!Node.OnCreate)
                        auto& Prefs{GetSingleton<JUserPreferences>()};
                        Region->AddChild(NewNode(Region->GetViewport()).Class<LDropDownNodeCustom::Parent>()
                            .Anchor(EAnchor::HFill)
                            .Visibility(ENodeVisibility::Visible) /* Allow custom nodes to be swept. */
                            .Padding({LDropDownMenuCreateInfo::RecommendedPadding, 0})
                            .InBrush<EStyleBits::Normal|EStyleBits::Disabled, &LBoxBrush::bSkipBrushDraw>(true)
                            .InBrush<EStyleBits::Hover, &LBoxBrush::Tint>(*Prefs.PrimaryColor)
                            .InBrush<EStyleBits::Press, &LBoxBrush::Tint>(*Prefs.PrimaryColor2)
                            .OnKeyUpFocused([Result, OnOptionCloseResult=CreateInfo.OnOptionCloseResult, Action=Node.OnAction]
                                (WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                                {
                                    check(Result)
                                    check(!!Action)
                                    if (auto reply{Action(Self, Info, Event)}; reply.is_handled())
                                    {
                                        if (reply.should_kill())
                                        {
                                            if (!OnOptionCloseResult || !OnOptionCloseResult(*Result).is_handled())
                                            {
                                                check(Result->IsTopLevel())
                                                Result->MarkAsGarbage_v2();
                                            }
                                        }
                                        return LNodeReply::Handled();
                                    }
                                    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                                    {
                                        if (!OnOptionCloseResult || !OnOptionCloseResult(*Result).is_handled())
                                        {
                                            check(Result->IsTopLevel())
                                            Result->MarkAsGarbage_v2();
                                        }
                                        return LNodeReply::Handled();
                                    }
                                    return LNodeReply::Unhandled();
                                })
                            [Node.OnCreate(Region->GetViewport(), *Result)]
                            .Enabled(Node.IsEnabled)
                            .Unique()
                            );
                    }
                    else if constexpr (std::is_same_v<T, LDropDownNodeScratch>)
                    {
                        Region->AddChild(Node.OnCreate(Region->GetViewport(), *Result).Unique());
                    }
                    else if constexpr (std::is_same_v<T, LDropDownNodeSeparator>)
                    {
                        Region->AddChild(NewNode(Region->GetViewport())
                            .Class<WTextIconizedLeftSeparator>()
                            .Padding({5_spt, 0})
                            .TextScale(ETextScale::Small)
                            .TextTint(Colors::Gray)
                            .Content(Node.DisplayName)
                            .Icon(Node.Icon.GetResolved())
                            .Thickness(Node.Thickness)
                            .Unique()
                            );
                    }
                    else if constexpr (std::is_same_v<T, LDropDownNodeSubMenu>)
                    {
                        std::unreachable();
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

Jafg::LNodeReply Jafg::WDropDown::OnMouseEnterInRoot(WNode& Node, LDropDownNodeSubMenu const& Submenu)
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

    auto& FloatingWidget{CreateDropDownMenu(
        this->GetViewport(),
        // TODO: Fix the translation...
        Node.GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)
            + LVec2F{0.0, Node.GetAnchoredSize_v2().y},
        {.OnOptionCloseResult = [this](auto&&...)
        {
            this->OpenSubmenus.clear();
            this->Select(nullptr);
            return algo::reply::handled();
        }},
        Submenu
        )};
    check(!FloatingWidget.OnDismissEvent.IsValid())
    FloatingWidget.OnDismissEvent.Bind(this, &WDropDown::OnDismiss);
    check(this->OpenSubmenus.contains(&Submenu) == false)

    this->OpenSubmenus.emplace(&Submenu, TJxxUnique<WFloatingWidget>{&FloatingWidget});

    return LNodeReply::Handled();
}
