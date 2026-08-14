// Copyright mzoesch. All rights reserved.

#include "Nodes/DropDown.h"
#include "Nodes/FloatingWidget.h"
#include "Nodes/HButton.h"
#include "Nodes/HRegion.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "User/UserPreferences.h"
#include "Nodes/TextIconizedSeparator.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "Nodes/Spacer.h"
#include "Nodes/CheckmarkButton.h"

Jafg::WDismissibleFloatingWidget& Jafg::CreateDropDownMenu(LViewport& Viewport, LVec2F Position, LDropDownMenuCreateInfo CreateInfo, TArray<LDropDownNode> const& Children)
{
    Position = maths::round(Position);

    WDismissibleFloatingWidget* Result;
    ConstructDeferredWidget(Jafg::TNodeStaticInit<WDismissibleFloatingWidget>{Viewport}).Style().SaveTo(&Result)
        .Decorate(false)
        .CreateResizeUi(false)
        .InitialWindowSize({100_pt, 0})
        .InitialWindowPosition(Position)
        .Content([Result, &CreateInfo, &Children](WFloatingWidget& FloatingWidget, WParent& Container)
        {
            typedef std::pair<std::size_t, algo::raii_leave> SubmenuWindow_t;
            std::size_t Id{};
            auto SubmenuWindow{std::make_shared<SubmenuWindow_t>(0, []{})};
            WVRegion* Region;
            BeginStyling(Container).StaticRoot<WVRegion>().SaveTo(&Region)
                .Tint(*GetSingleton<JUserPreferences>().OverlayColor)
                .Padding({0, LDropDownMenuCreateInfo::RecommendedPadding});
            if constexpr (LogWidgetFramework.CompilesFor<ELogVerbosity::Warning>) if (Children.empty())
            {
                LOG_WARNING(LogWidgetFramework, "SubMenu has no nodes.")
            }
            for (auto& Child : Children)
            {
                ++Id;
                std::visit([Result, &CreateInfo, Region, SubmenuWindow, Id]<typename T0>(T0&& Node)
                {
                    typedef std::decay_t<T0> T;
                    if constexpr (std::is_same_v<T, LDropDownNodeOption>)
                    {
                        auto& Prefs{GetSingleton<JUserPreferences>()};
                        Region->AddChild(NewNode(Region->GetViewport()).Class<WTextButtonIconizedDouble>()
                            .Anchor(EAnchor::HFill)
                            .InBrush<EStyleBits::Normal|EStyleBits::Disabled, &LBoxBrush::bSkipBrushDraw>(true)
                            .InBrush<EStyleBits::Hover, &LBoxBrush::Tint>(*Prefs.PrimaryColor)
                            .InBrush<EStyleBits::Press, &LBoxBrush::Tint>(*Prefs.PrimaryColorVariant)
                            .InAllBrushes<&LBoxBrush::Padding>({LDropDownMenuCreateInfo::RecommendedPadding, 0})
                            .InTextBrush<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray)
                            .InAllLeftIconBrushes<&LIconBrush::bAlwaysPad>(true)
                            .OnCursorEnter([SubmenuWindow](auto&&...)
                            {
                                check(SubmenuWindow.get())
                                *SubmenuWindow = SubmenuWindow_t{0,[]{}};
                                return LNodeReply::Unhandled();
                            })
                            .OnKeyEventFocused([Result, OnOptionCloseResult=CreateInfo.OnOptionCloseResult, Action=Node.OnAction]
                            (WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                            {
                                check(Result)
                                check(!!Action)
                                if (Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
                                {
                                    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
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
                            .InBrush<EStyleBits::Press, &LBoxBrush::Tint>(*Prefs.PrimaryColorVariant)
                            .OnCursorEnter([SubmenuWindow](auto&&...)
                            {
                                check(SubmenuWindow.get())
                                *SubmenuWindow = SubmenuWindow_t{0,[]{}};
                                return LNodeReply::Unhandled();
                            })
                            .OnKeyEventFocused([Result, OnOptionCloseResult=CreateInfo.OnOptionCloseResult, Action=Node.OnAction]
                                (WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                                {
                                    check(Result)
                                    check(!!Action)
                                    if (Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
                                    {
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
                                        if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                                        {
                                            if (!OnOptionCloseResult || !OnOptionCloseResult(*Result).is_handled())
                                            {
                                                check(Result->IsTopLevel())
                                                Result->MarkAsGarbage_v2();
                                            }
                                            return LNodeReply::Handled();
                                        }
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
                    else if constexpr (std::is_same_v<T, LDropDownNodeSubmenu>)
                    {
                        auto& Prefs{GetSingleton<JUserPreferences>()};
                        Region->AddChild(NewNode(Region->GetViewport()).Class<WTextButtonIconizedDouble>()
                            .Anchor(EAnchor::HFill)
                            .InBrush<EStyleBits::Normal|EStyleBits::Disabled, &LBoxBrush::bSkipBrushDraw>(true)
                            .InBrush<EStyleBits::Hover|EStyleBits::Selected, &LBoxBrush::Tint>(*Prefs.PrimaryColor)
                            .InBrush<EStyleBits::Press, &LBoxBrush::Tint>(*Prefs.PrimaryColorVariant)
                            .InAllBrushes<&LBoxBrush::Padding>({LDropDownMenuCreateInfo::RecommendedPadding, 0})
                            .InTextBrush<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray)
                            .InAllLeftIconBrushes<&LIconBrush::bAlwaysPad>(true)
                            .Selectable(true)
                            .OnCursorEnter([NodeCopy=Node, SubmenuWindow, Id](WNode& Self)
                            {
                                static_assert(std::is_same_v<decltype(NodeCopy), LDropDownNodeSubmenu>);
                                check(SubmenuWindow.get())
                                if (SubmenuWindow->first != Id)
                                {
                                    SubmenuWindow->first = Id;
                                    auto Window{TClassStorage<WDismissibleFloatingWidget>{&CreateDropDownMenu(
                                          Self.GetViewport()
                                        // TODO: Fix translation.
                                        , Self.GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)
                                            + LVec2F{Self.GetAnchoredSize_v2().x, 0.0f}
                                            - LVec2F{0.0f, LDropDownMenuCreateInfo::RecommendedPadding.InStaticPoints(Self.GetViewport())}
                                        , {}
                                        , NodeCopy.Children
                                        )}};
                                    TClassStorage Me{&Self.AsStatic<WTextButtonIconizedDouble>()};
                                    Me->SetSelected(true);
                                    SubmenuWindow->second = algo::raii_leave{[Window=std::move(Window), Me=std::move(Me)] mutable
                                    {
                                        if (Window.IsValidSlow())
                                        {
                                            check(Window->IsTopLevel())
                                            Window->MarkAsGarbage_v2();
                                        }
                                        if (Me.IsValidSlow())
                                        {
                                            Me->SetSelected(false);
                                        }
                                    }};
                                    check(!Window.get_unsafe())
                                    check(!Me.get_unsafe())
                                }
                                return LNodeReply::Handled();
                            })
                            .Content(Node.Selector.DisplayName)
                            .LeftIcon(Node.Selector.Icon.GetResolved())
                            .RightIcon(LTexture2::FromAsset("Icons/Jafg.ExtendRight"))
                            .template InAllRightIconBrushes<&LIconBrush::Alignment>(LIconBrush::Align::Right)
                            .Enabled(Node.IsEnabled)
                            .Unique()
                            );
                    }
                    else if constexpr (std::is_same_v<T, LDropDownNodeDeferredSubMenu>)
                    {
                        auto& Prefs{GetSingleton<JUserPreferences>()};
                        Region->AddChild(NewNode(Region->GetViewport()).Class<WTextButtonIconizedDouble>()
                            .Anchor(EAnchor::HFill)
                            .InBrush<EStyleBits::Normal|EStyleBits::Disabled, &LBoxBrush::bSkipBrushDraw>(true)
                            .InBrush<EStyleBits::Hover|EStyleBits::Selected, &LBoxBrush::Tint>(*Prefs.PrimaryColor)
                            .InBrush<EStyleBits::Press, &LBoxBrush::Tint>(*Prefs.PrimaryColorVariant)
                            .InAllBrushes<&LBoxBrush::Padding>({LDropDownMenuCreateInfo::RecommendedPadding, 0})
                            .InTextBrush<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray)
                            .InAllLeftIconBrushes<&LIconBrush::bAlwaysPad>(true)
                            .Selectable(true)
                            .OnCursorEnter([NodeCopy=Node, SubmenuWindow, Id](WNode& Self)
                            {
                                static_assert(std::is_same_v<decltype(NodeCopy), LDropDownNodeDeferredSubMenu>);
                                check(SubmenuWindow.get())
                                check(NodeCopy.OnChildren)
                                if (SubmenuWindow->first != Id)
                                {
                                    SubmenuWindow->first = Id;
                                    auto Window{TClassStorage<WDismissibleFloatingWidget>{&CreateDropDownMenu(
                                          Self.GetViewport()
                                        // TODO: Fix translation.
                                        , Self.GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)
                                            + LVec2F{Self.GetAnchoredSize_v2().x, 0.0f}
                                            - LVec2F{0.0f, LDropDownMenuCreateInfo::RecommendedPadding.InStaticPoints(Self.GetViewport())}
                                        , {}
                                        , NodeCopy.OnChildren()
                                        )}};
                                    TClassStorage Me{&Self.AsStatic<WTextButtonIconizedDouble>()};
                                    Me->SetSelected(true);
                                    SubmenuWindow->second = algo::raii_leave{[Window=std::move(Window), Me=std::move(Me)] mutable
                                    {
                                        if (Window.IsValidSlow())
                                        {
                                            check(Window->IsTopLevel())
                                            Window->MarkAsGarbage_v2();
                                        }
                                        if (Me.IsValidSlow())
                                        {
                                            Me->SetSelected(false);
                                        }
                                    }};
                                    check(!Window.get_unsafe())
                                    check(!Me.get_unsafe())
                                }
                                return LNodeReply::Handled();
                            })
                            .Content(Node.Selector.DisplayName)
                            .LeftIcon(Node.Selector.Icon.GetResolved())
                            .RightIcon(LTexture2::FromAsset("Icons/Jafg.ExtendRight"))
                            .template InAllRightIconBrushes<&LIconBrush::Alignment>(LIconBrush::Align::Right)
                            .Enabled(Node.IsEnabled)
                            .Unique()
                            );
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

Jafg::LDropDownNodeCustom Jafg::CreateDropDownCheckmark(LString Text, bool bChecked, TCopyableFunction<bool()> OnToggle)
{
    return LDropDownNodeCustom{
            .OnCreate=[bChecked,OnToggle,Text=std::move(Text)](LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)
            {
                return NewNode(Viewport).Class<WSpacer>().Width(2_spt)
                + NewNode(Viewport).Class<WCheckmarkButton>()
                    .Anchor(EAnchor::CenterLeft)
                    .Checked(bChecked)
                    .OnKeyEventFocused([OnToggle](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                            && Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation)
                            )
                        {
                            Self.AsStatic<WCheckmarkButton>().SetChecked(OnToggle());
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewNode(Viewport).Class<WText>()
                    .Anchor(EAnchor::HFill)
                    .Visibility(ENodeVisibility::TransitiveHitTestInvisible)
                    .Padding({6_spt, 0.0f, 0.0f, 0.0f})
                    .Content(std::move(Text));
                },
            .OnAction=[OnToggle](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                {
                    (void)OnToggle();
                    return LDropDownNodeCustom::reply::handled(true);
                }
                return LDropDownNodeCustom::reply::unhandled();
            },};
}
