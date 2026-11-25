// Copyright mzoesch. All rights reserved.

#include "Jgc/User/Frontend/JgcCustomizationWindow.h"
#include "Nodes/FloatingWidget.h"
#include "Nodes/HParent.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/Spacer.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "Nodes/VParent.h"
#include "User/UserPreferences.h"
#include "Widgets/Input_Slider.h"

namespace
{

//# In static points.
constexpr f32 WindowMargin{ 16.0f };
//# The padding of the content of the window.
constexpr f32 WindowPadding{ 10.0f };
//# Each part of the body (see #LHumanoidShapeParameter::Group) has a column of at least this width.
constexpr f32 ColumnWidth{ 170.0f };
constexpr f32 ColumnSpace{ 16.0f };
//# Wider windows only spread the columns further apart.
constexpr f32 MaxWindowWidth{ 1100.0f };
//# Between the content and the right edge of the scroll region, where the scroll bar is.
constexpr f32 ScrollBarGutter{ 12.0f };
//# What the window needs besides the values: the title, the paddings and the buttons. Measured.
constexpr f32 WindowChrome{ 140.0f };
//# How high the values may be without an area to fit into.
constexpr f32 DefaultListHeight{ 560.0f };

//# A button at the bottom of the window. #OnClick gets the window.
NODISCARD TJxxUnique<Jafg::WNode> CreateButton(Jafg::LViewport& Viewport, LString Content, TCopyableFunction<void()> OnClick, Jafg::WFloatingWidget* Window)
{
    auto& Prefs{Jafg::GetSingleton<Jafg::JUserPreferences>()};
    return Jafg::NewNode(Viewport).Class<Jafg::WTextButton>()
        .Content(std::move(Content))
        .Palette(*Prefs.SecondaryPaletteSolid)
        .InAllTextBrushes<&Jafg::LTextBrush::TextScale>(Jafg::ETextScale::SubHeader)
        .InAllBrushes<&Jafg::LBoxBrush::Padding>({5_pt, 2})
        .OnKeyEventFocused([OnClick=std::move(OnClick), Window](Jafg::WNode& Self, Jafg::LNodeKeyEventInfo const& Info, Jafg::LKeyEvent const& Event)
        {
            if (Event.Is<Jafg::ERawInputStateBits::Release>(Jafg::ELogicalKey::LeftMouseButton)
                && Info.CursorLocation && Self.AabbTest({.Translation = Info.Translation}, *Info.CursorLocation))
            {
                if (OnClick)
                {
                    OnClick();
                }
                /* Nobody closed it. */
                if (!Window->_IsGarbage())
                {
                    Window->MarkAsGarbage_v2();
                }
                return Jafg::LNodeReply::Handled();
            }
            return Jafg::LNodeReply::Unhandled();
        })
        .Unique();
}

} /* ~Namespace <Anonymous> */

Jafg::WFloatingWidget& Jgc::OpenCustomizationWindow(Jafg::LViewport& Viewport, LCustomizationWindowInfo Info)
{
    /* Shared by every slider and button. Lives as long as the window. */
    auto State{std::make_shared<LCustomizationWindowInfo>(std::move(Info))};

    std::size_t Columns{};
    LStringView LastGroup;
    for (LHumanoidShapeParameter const& Parameter: GetHumanoidShapeParameters())
    {
        Columns += Parameter.Group != LastGroup ? 1uz : 0uz;
        LastGroup = Parameter.Group;
    }
    f32 const MinWindowWidth{
        static_cast<f32>(Columns) * ColumnWidth + static_cast<f32>(Columns - 1uz) * ColumnSpace
        + 2.0f * WindowPadding + ScrollBarGutter};

    LVec2F Position{WindowMargin};
    f32 Width{MinWindowWidth};
    /* Taller values scroll. */
    f32 ListHeight{DefaultListHeight};
    if (State->Area)
    {
        LRect2F const& Area{*State->Area};
        Position = Area.offset + LVec2F{WindowMargin};
        Width = maths::clamp(Area.extent.x - 2.0f * WindowMargin, MinWindowWidth, maths::max(MaxWindowWidth, MinWindowWidth));
        ListHeight = maths::max(Area.extent.y - 2.0f * WindowMargin - WindowChrome, 100.0f);
    }

    Jafg::WFloatingWidget* Result{};
    Jafg::ConstructDeferredWidget(Jafg::TNodeStaticInit<Jafg::WFloatingWidget>{Viewport}).Style().SaveTo(&Result)
        .InitialTitle("Character Customization")
        .CreateResizeUi(false)
        .InitialWindowSize(Jafg::LNodeSize2{Jafg::ENodeSize::StaticPoints, Width, 20.0f})
        .InitialWindowPosition(maths::round(Position))
        .Content([State, ListHeight](Jafg::WFloatingWidget& Window, Jafg::WParent& Container)
        {
            Jafg::WVParent* Outer{};
            Jafg::BeginStyling(Container).StaticRoot<Jafg::WVParent>().SaveTo(&Outer)
                .Anchor(Jafg::EAnchor::HFill)
                .Padding({Jafg::LNodeSize1{Jafg::ENodeSize::Points, WindowPadding}, 8})
                .Space(4_pt)
                ;
            check(Outer)
            auto& ContentViewport{Outer->GetViewport()};

            /*
             * The cap is on a wrapper and not on the scroll region: Its own max size would also cap the size of its
             * content that it measures, and nothing would be left to scroll (as in #Jafg::WInput_LargeList).
             * The content keeps away from the right edge, where the scroll bar is drawn over it.
             */
            Jafg::WHParent* Columns{};
            Outer->AddChild(Jafg::NewNode(ContentViewport).Class<Jafg::WVParent>()
                .Anchor(Jafg::EAnchor::HFill)
                .MaxDesiredSize(Jafg::LNodeSize2{Jafg::ENodeSize::StaticPoints, 0.0f, ListHeight})
            [
                Jafg::NewNode(ContentViewport).Class<Jafg::WScrollRegion>()
                    .SkipBrushDraw(true)
                    .UseChildrenDesiredSize(true)
                [
                    Jafg::NewNode(ContentViewport).Class<Jafg::WHParent>().SaveTo(&Columns)
                        .Anchor(Jafg::EAnchor::HFill)
                        .Padding(Jafg::LPadding{Jafg::ENodeSize::StaticPoints, 0.0f, 0.0f, ScrollBarGutter, 0.0f})
                        .Space(Jafg::LNodeSize1{Jafg::ENodeSize::StaticPoints, ColumnSpace})
                ]
            ].Unique());
            check(Columns)

            /* A column per part of the body. Each value is a name above a slider. */
            Jafg::WVParent* Column{};
            LStringView Group;
            for (LHumanoidShapeParameter const& Parameter: GetHumanoidShapeParameters())
            {
                if (!Column || Parameter.Group != Group)
                {
                    Group = Parameter.Group;
                    Columns->AddChild(Jafg::NewNode(ContentViewport).Class<Jafg::WVParent>().SaveTo(&Column)
                        .Anchor(Jafg::EAnchor::HFill)
                        .MinDesiredSize(Jafg::LNodeSize2{Jafg::ENodeSize::StaticPoints, ColumnWidth, 0.0f})
                        .Space(2_pt)
                    [
                        Jafg::NewNode(ContentViewport).Class<Jafg::WText>()
                            .Anchor(Jafg::EAnchor::HFill)
                            .TextScale(Jafg::ETextScale::SubHeader)
                            .Content(LString{Group})
                    ].Unique());
                    check(Column)
                }

                Column->AddChild(Jafg::NewNode(ContentViewport).Class<Jafg::WText>()
                    .Anchor(Jafg::EAnchor::HFill)
                    .Padding(Jafg::LPadding{Jafg::ENodeSize::Points, 0.0f, 4.0f, 0.0f, 0.0f})
                    .Content(LString{Parameter.DisplayName})
                    .Unique());
                Column->AddChild(Jafg::NewNode(ContentViewport).Class<Jafg::WInput_Slider>()
                    .Anchor(Jafg::EAnchor::HFill)
                    .Value(std::round(static_cast<f64>(State->Shape.*Parameter.Value) * 100.0))
                    .Step(1.0)
                    .Minimum(std::round(static_cast<f64>(Parameter.Min) * 100.0))
                    .Maximum(std::round(static_cast<f64>(Parameter.Max) * 100.0))
                    .OnValueChanged([State, Value=Parameter.Value](f64 Percent)
                    {
                        State->Shape.*Value = static_cast<f32>(Percent / 100.0);
                        if (State->OnChanged)
                        {
                            State->OnChanged(State->Shape);
                        }
                    })
                    .Unique());
            }

            Jafg::WHParent* Buttons{};
            Outer->AddChild(Jafg::NewNode(ContentViewport).Class<Jafg::WHParent>().SaveTo(&Buttons)
                .Anchor(Jafg::EAnchor::HFill)
                .Padding(Jafg::LPadding{Jafg::ENodeSize::Points, 0.0f, 8.0f, 0.0f, 0.0f})
                .Space(8_pt)
            [
                Jafg::NewNode(ContentViewport).Class<Jafg::WSpacer>()
                    .Anchor(Jafg::EAnchor::Fill)
            ].Unique());
            check(Buttons)
            Buttons->AddChild(::CreateButton(ContentViewport, "Discard", [State]{ if (State->OnDiscard) { State->OnDiscard(); } }, &Window));
            Buttons->AddChild(::CreateButton(ContentViewport, "Apply", [State]{ if (State->OnApply) { State->OnApply(); } }, &Window));
        })
        /* Closing is discarding. */
        .OnWindowClosed([State](Jafg::WFloatingWidget& Window)
        {
            if (State->OnDiscard)
            {
                State->OnDiscard();
            }
            /* Handled if the callback closed it. Otherwise, it is destroyed by default. */
            return Window._IsGarbage();
        });

    check(Result)
    return *Result;
}
