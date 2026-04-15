// Copyright mzoesch. All rights reserved.

#include "Nodes/FloatingWindow.h"
#include "Platform/Surface.h"
#include "Nodes/Viewport.h"
#include "Nodes/Region.h"
#include "Nodes/TextButton.h"

void Jafg::WFloatingWindow::Construct()
{
    Super::Construct();

    BeginStyling(*this).StaticRoot<WVRegion>()
        .Anchor(EAnchor::TopLeft)
    [
        NewStaticNode(WRegion)
            .Visibility(ENodeVisibility::Visible)
            .MinDesiredSize({0_spt, 16})
            .Anchor(EAnchor::HFill)
            .Tint(Colors::DarkerGray)
            .OnKeyDown([](WNode& Self, LNodeKeyEventData const& Data, LKeyEvent const& Event) -> LReply
            {
                if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton))
                {
                    Data.Surface._SetMouseCursor(EMouseCursor::Hand);
                    WFloatingWindow* Window{StaticCast<WFloatingWindow>(Self.GetParent()->GetParent())};
                    check(Window->UiTickMoveHandle.IsValid() == false)
                    Window->UiTickMoveHandle = Data.Viewport.OnLateTick.Emplace(Window, &WFloatingWindow::UiTickMove);
                    return LReply::Handled();
                }
                return {};
            })
            .OnKeyUp([](WNode& Self, LNodeKeyEventData const& Data, LKeyEvent const& Event) -> LReply
            {
                if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton))
                {
                    Data.Surface._SetMouseCursor(EMouseCursor::Default);
                    auto* Window{StaticCast<WFloatingWindow>(Self.GetParent()->GetParent())};
                    if (Window->UiTickMoveHandle.IsValid())
                    {
                        Data.Viewport.OnLateTick.Remove(&Window->UiTickMoveHandle);
                    }
                    Window->MoveDragOffset.reset();
                    return LReply::Handled();
                }
                return {};
            })
        [
            NewStaticNode(WTextBox).SaveTo(&this->WindowTitle)
                .Anchor(EAnchor::CenterCenter)
                .Content("Floating Window")
                .TextTint(Colors::White)
            +
            NewStaticNode(WTextButton)
                .Anchor(EAnchor::CenterRight)
                .Content("X")
                .OmniTint(Colors::Transparent)
                .OmniOutlineThickness(1)
                .TextBrush({ETextScale::Compact})
                .OnPrimaryRelease([this](auto&, LKeyEvent const& KeyEvent)
                {
                    if (this->OnWindowClosed.IsValid())
                    {
                        if (this->OnWindowClosed.Invoke(*this))
                        {
                            return;
                        }
                    }
                    this->RemoveFromParent2();
                })
        ]
    ]
    ;

    this->SetWindowSize({640, 360});

    return;
}

// void Jafg::WFloatingWindow::SetContentNode(WNode& Content) noexcept
// {
//     auto* Window{this->GetWindow()};
//
//     check(Window->GetChildren().size() == 1)
//
//     Window->AddChild(&Content);
//     Content.SetAnchor(EAnchor::Fill);
//     Content.SetVisibility(ENodeVisibility::Visible);
//
//     if (this->bCreateResizeUi && Content.IsA<WParentBase>())
//     {
//         BeginStyling(*StaticCast<WParentBase>(&Content)).Root<WTextButton>()
//             .Anchor(EAnchor::BottomRight)
//             .Content("#")
//             .TextBlockBrush(LTextBoxBrush::Compact())
//             .Padding({2_spt})
//             .OnPrimaryPress([](WButton* Self, const LKeyEvent& InKeyEvent)
//             {
//                 WFloatingWindow* Window{ StaticCast<WFloatingWindow>(Self->GetParent()->GetParent()->GetParent()) };
//                 Window->UiTickResizeHandle = Self->GetViewport().OnLateTick.Emplace(Window, &WFloatingWindow::UiTickResize);
//
//                 return;
//             })
//             .OnPrimaryRelease([](WButton* Self, const LKeyEvent& InKeyEvent)
//             {
//                 WFloatingWindow* Window{ StaticCast<WFloatingWindow>(Self->GetParent()->GetParent()->GetParent()) };
//                 if (Window->UiTickResizeHandle.IsValid())
//                 {
//                     Self->GetViewport().OnLateTick.Remove(&Window->UiTickResizeHandle);
//                 }
//                 Window->ResizeDragOffset.reset();
//
//                 return;
//             })
//             ;
//     }
//
//     return;
// }

void Jafg::WFloatingWindow::UiTickMove()
{
    if (this->MoveDragOffset.has_value() == false)
    {
        this->MoveDragOffset =
            this->GetViewport().GetSurface().GetMouseLocationValue()
                // TODO: This is wrong. How do we get the translation here?
                - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>);

        return;
    }

    LVec2F NewPos{this->GetViewport().GetSurface().GetMouseLocationValue() - this->MoveDragOffset.value()};

    if (NewPos.x + 25.0 > this->GetViewport().GetDimensions().x)
    {
        NewPos.x = this->GetViewport().GetDimensions().x - 25.0;
    }
    if (NewPos.y + 25.0 > this->GetViewport().GetDimensions().y)
    {
        NewPos.y = this->GetViewport().GetDimensions().y - 25.0;
    }

    if (NewPos.x - 75.0 + this->GetWindow()->GetDesiredSize_v2().x < 0.0)
    {
        NewPos.x = 75.0 - this->GetWindow()->GetDesiredSize_v2().x;
    }
    if (NewPos.y < 0.0)
    {
        NewPos.y = 0.0;
    }

    this->SetWindowPosition(NewPos);

    return;
}

void Jafg::WFloatingWindow::UiTickResize()
{
    if (this->ResizeDragOffset.has_value() == false)
    {
        this->ResizeDragOffset =
            this->GetViewport().GetSurface().GetMouseLocationValue()
                // TODO: This is wrong. How do we get the translation here? Same for below
                - (this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)
                    + this->GetWindow()->GetDesiredSize_v2());
        return;
    }

    LVec2F NewSize{
        this->GetViewport().GetSurface().GetMouseLocationValue().x
        - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).x - this->ResizeDragOffset.value().x,
        this->GetViewport().GetSurface().GetMouseLocationValue().y
        - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).y - this->ResizeDragOffset.value().y
        };

    LVec2F MaxSize{
        this->GetViewport().GetDimensions().x - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).x,
        this->GetViewport().GetDimensions().y - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).y
        };

    if (NewSize.x < 0.0)
    {
        NewSize.x = 0.0;
    }
    if (NewSize.y < 0.0)
    {
        NewSize.y = 0.0;
    }

    if (NewSize.x > MaxSize.x)
    {
        NewSize.x = MaxSize.x;
    }
    if (NewSize.y > MaxSize.y)
    {
        NewSize.y = MaxSize.y;
    }

    check(NewSize.x >= 0.0 && NewSize.y >= 0.0)

    this->SetWindowSize(NewSize);

    return;
}
