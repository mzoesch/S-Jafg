// Copyright mzoesch. All rights reserved.

#include "Jgc/User/Frontend/CustomizationPanel.h"
#include "Nodes/HParent.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/Spacer.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "Nodes/UserWidget.h"
#include "Nodes/VParent.h"
#include "Nodes/VRegion.h"
#include "User/UserPreferences.h"
#include "Widgets/Input_Slider.h"

namespace
{

//# The padding of the content of the panel.
constexpr f32 PanelPadding{ 10.0f };
//# Each part of the body (see #LHumanoidShapeParameter::Group) has a column of at least this width.
constexpr f32 ColumnWidth{ 170.0f };
constexpr f32 ColumnSpace{ 16.0f };
//# Between the content and the right edge of the scroll region, where the scroll bar is.
constexpr f32 ScrollBarGutter{ 12.0f };

//# A button at the bottom of the panel.
NODISCARD TJxxUnique<Jafg::WNode> CreateButton(Jafg::LViewport& Viewport, LString Content, TCopyableFunction<void()> OnClick, Jafg::WUserWidget* Panel)
{
    auto& Prefs{Jafg::GetSingleton<Jafg::JUserPreferences>()};
    return Jafg::NewNode(Viewport).Class<Jafg::WTextButton>()
        .Content(std::move(Content))
        .Palette(*Prefs.SecondaryPaletteSolid)
        .InAllTextBrushes<&Jafg::LTextBrush::TextScale>(Jafg::ETextScale::SubHeader)
        .InAllBrushes<&Jafg::LBoxBrush::Padding>({5_pt, 2})
        .OnKeyEventFocused([OnClick=std::move(OnClick), Panel](Jafg::WNode& Self, Jafg::LNodeKeyEventInfo const& Info, Jafg::LKeyEvent const& Event)
        {
            if (Event.Is<Jafg::ERawInputStateBits::Release>(Jafg::ELogicalKey::LeftMouseButton)
                && Info.CursorLocation && Self.AabbTest({.Translation = Info.Translation}, *Info.CursorLocation))
            {
                if (OnClick)
                {
                    OnClick();
                }
                /* Nobody closed it. */
                if (!Panel->_IsGarbage())
                {
                    Panel->MarkAsGarbage_v2();
                }
                return Jafg::LNodeReply::Handled();
            }
            return Jafg::LNodeReply::Unhandled();
        })
        .Unique();
}

} /* ~Namespace <Anonymous> */

Jafg::WUserWidget& Jgc::OpenCustomizationPanel(Jafg::LViewport& Viewport, LCustomizationPanelInfo Info)
{
    auto& Prefs{Jafg::GetSingleton<Jafg::JUserPreferences>()};
    /* Shared by every slider and button. Lives as long as the panel. */
    auto State{std::make_shared<LCustomizationPanelInfo>(std::move(Info))};

    Jafg::WUserWidget& Panel{*Jafg::ConstructWidget(Jafg::TNodeStaticInit<Jafg::WUserWidget>{Viewport})};
    /* From the top left: As wide as wanted, as high as the viewport. */
    Panel.Anchor = Jafg::LAnchor{0.0f, 0.0f, maths::clamp(State->Width, 0.0f, 1.0f), 1.0f};

    Jafg::WHParent* Columns{};
    Jafg::WHParent* Buttons{};
    Panel.AddChild(Jafg::NewNode(Viewport).Class<Jafg::WVRegion>()
        .Anchor(Jafg::EAnchor::Fill)
        /*
         * Clicks on the panel are not meant for the world behind it (e.g., the input contexts of the station). Gets
         * every press on the panel that nothing in it (e.g., a slider) has used.
         */
        .Visibility(Jafg::ENodeVisibility::Visible)
        .OnKeyEventUnfocused([](Jafg::WNode& Self, Jafg::LNodeKeyEventInfo const& Info, Jafg::LKeyEvent const& Event)
        {
            if ((   Event.Is<Jafg::ERawInputStateBits::Press>(Jafg::ELogicalKey::LeftMouseButton)
                 || Event.Is<Jafg::ERawInputStateBits::Press>(Jafg::ELogicalKey::RightMouseButton))
                && Self.AabbTest(Info))
            {
                return Jafg::LNodeReply::Handled();
            }
            return Jafg::LNodeReply::Unhandled();
        })
        .Tint(*Prefs.ForegroundColor)
        .Padding({Jafg::LNodeSize1{Jafg::ENodeSize::Points, PanelPadding}, 8})
        .Space(4_pt)
    [
        Jafg::NewNode(Viewport).Class<Jafg::WText>()
            .Anchor(Jafg::EAnchor::HFill)
            .TextScale(Jafg::ETextScale::Header)
            .Padding(Jafg::LPadding{Jafg::ENodeSize::Points, 0.0f, 0.0f, 0.0f, 4.0f})
            .Content("Character Customization")
        /*
         * Takes the height that the title and the buttons leave. It does not grow with its content, so that its content
         * scrolls instead of making the panel taller than the viewport. The content keeps away from the right edge,
         * where the scroll bar is drawn over it.
         */
        + Jafg::NewNode(Viewport).Class<Jafg::WScrollRegion>()
            .Anchor(Jafg::EAnchor::Fill)
            .SkipBrushDraw(true)
        [
            Jafg::NewNode(Viewport).Class<Jafg::WHParent>().SaveTo(&Columns)
                .Anchor(Jafg::EAnchor::HFill)
                .Padding(Jafg::LPadding{Jafg::ENodeSize::StaticPoints, 0.0f, 0.0f, ScrollBarGutter, 0.0f})
                .Space(Jafg::LNodeSize1{Jafg::ENodeSize::StaticPoints, ColumnSpace})
        ]
        + Jafg::NewNode(Viewport).Class<Jafg::WHParent>().SaveTo(&Buttons)
            .Anchor(Jafg::EAnchor::HFill)
            .Padding(Jafg::LPadding{Jafg::ENodeSize::Points, 0.0f, 8.0f, 0.0f, 0.0f})
            .Space(8_pt)
        [
            Jafg::NewNode(Viewport).Class<Jafg::WSpacer>()
                .Anchor(Jafg::EAnchor::Fill)
        ]
    ].Unique());
    check(Columns && Buttons)

    /* A column per part of the body. Each value is a name above a slider. */
    Jafg::WVParent* Column{};
    LStringView Group;
    for (LHumanoidShapeParameter const& Parameter: GetHumanoidShapeParameters())
    {
        if (!Column || Parameter.Group != Group)
        {
            Group = Parameter.Group;
            Columns->AddChild(Jafg::NewNode(Viewport).Class<Jafg::WVParent>().SaveTo(&Column)
                .Anchor(Jafg::EAnchor::HFill)
                .MinDesiredSize(Jafg::LNodeSize2{Jafg::ENodeSize::StaticPoints, ColumnWidth, 0.0f})
                .Space(2_pt)
            [
                Jafg::NewNode(Viewport).Class<Jafg::WText>()
                    .Anchor(Jafg::EAnchor::HFill)
                    .TextScale(Jafg::ETextScale::SubHeader)
                    .Content(LString{Group})
            ].Unique());
            check(Column)
        }

        Column->AddChild(Jafg::NewNode(Viewport).Class<Jafg::WText>()
            .Anchor(Jafg::EAnchor::HFill)
            .Padding(Jafg::LPadding{Jafg::ENodeSize::Points, 0.0f, 4.0f, 0.0f, 0.0f})
            .Content(LString{Parameter.DisplayName})
            .Unique());
        Column->AddChild(Jafg::NewNode(Viewport).Class<Jafg::WInput_Slider>()
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

    Buttons->AddChild(::CreateButton(Viewport, "Discard", [State]{ if (State->OnDiscard) { State->OnDiscard(); } }, &Panel));
    Buttons->AddChild(::CreateButton(Viewport, "Apply", [State]{ if (State->OnApply) { State->OnApply(); } }, &Panel));

    return Panel;
}
