// Copyright mzoesch. All rights reserved.

#include "Widgets/FloatingWindow.h"
#include "Platform/Surface.h"
#include "Widgets/Viewport.h"
#include "Widgets/Region.h"
#include "Widgets/Button.h"

void Jafg::WFloatingWindow::Construct()
{
    Super::Construct();

    WRegion* WindowTopBar{ nullptr };
    MakeRootNode(WVRegion)
        .Anchor(EAnchor::TopLeft)
    [
        NewNode(WRegion).SaveTo(&WindowTopBar)
            .Visibility(EWidgetVisibility::Visible)
            .MinDesiredSize({0_spt, 16})
            .Anchor(EAnchor::HFill)
            .Type(ERegionBrush::Box)
            .Tint(Colors::DarkerGray)
        [
            NewNode(WTextBox).SaveTo(&this->WindowTitle)
                .Anchor(EAnchor::CenterCenter)
                .Content("Floating Window")
                .TextColor(Colors::White)
            +
            NewNode(WTextButton)
                .Anchor(EAnchor::CenterRight)
                .Content("X")
                .OmniTint(Colors::Transparent)
                .OmniType(ERegionBrush::OutlineBox)
                .OmniOutlineThickness(1)
                .NormalType(ERegionBrush::None)
                .TextBlockBrush(LTextBoxBrush::Compact())
                .OnPrimaryRelease([this](WButton* Self, const LKeyEvent& InKeyEvent)
                {
                    if (this->OnWindowClosed.IsValid())
                    {
                        if (this->OnWindowClosed.Invoke(*this))
                        {
                            return;
                        }
                    }

                    this->RemoveFromParent();

                    return;
                })
        ]
    ]
    FinishWidgetStyling()

    this->SetWindowSize({640, 360});

    WindowTopBar->OnKeyDownEvent.BindStrong([](WNode& Self, LViewport& Viewport, LKeyEvent const& KeyEvent) -> LReply
    {
        if (KeyEvent.GetKey() == EKeys::LeftMouseButton)
        {
            Viewport.GetSurface()._SetMouseCursor(EMouseCursor::Hand);

            WFloatingWindow* Window{ StaticCast<WFloatingWindow>(Self.GetParent()->GetParent()) };
            Window->UiTickMoveHandle = Viewport.OnLateTick.Emplace(Window, &WFloatingWindow::UiTickMove);

            return LReply::Handled();
        }

        return {};
    });

    WindowTopBar->OnKeyUpEvent.BindStrong([](WNode& Self, LViewport& Viewport, LKeyEvent const& KeyEvent) -> LReply
    {
        if (KeyEvent.GetKey() == EKeys::LeftMouseButton)
        {
            Viewport.GetSurface()._SetMouseCursor(EMouseCursor::Default);

            auto* Window{ StaticCast<WFloatingWindow>(Self.GetParent()->GetParent()) };
            if (Window->UiTickMoveHandle.IsValid())
            {
                Viewport.OnLateTick.Remove(&Window->UiTickMoveHandle);
            }
            Window->MoveDragOffset.reset();

            return LReply::Handled();
        }

        return {};
    });

    return;
}

void Jafg::WFloatingWindow::SetContentNode(WNode& Content) noexcept
{
    auto* Window{ this->GetWindow() };

    check( Window->GetChildren().size() == 1 )

    Window->AddChild(&Content);
    Content.SetAnchor(EAnchor::Fill);
    Content.SetVisibility(EWidgetVisibility::Visible);

    if (this->bCreateResizeUi && Content.IsA<WParentBase>())
    {
        NewNode(WTextButton)
            .Anchor(EAnchor::BottomRight)
            .Content("#")
            .TextBlockBrush(LTextBoxBrush::Compact())
            .Padding({2_spt})
            .OnPrimaryPress([](WButton* Self, const LKeyEvent& InKeyEvent)
            {
                WFloatingWindow* Window{ StaticCast<WFloatingWindow>(Self->GetParent()->GetParent()->GetParent()) };
                Window->UiTickResizeHandle = Self->GetViewport().OnLateTick.Emplace(Window, &WFloatingWindow::UiTickResize);

                return;
            })
            .OnPrimaryRelease([](WButton* Self, const LKeyEvent& InKeyEvent)
            {
                WFloatingWindow* Window{ StaticCast<WFloatingWindow>(Self->GetParent()->GetParent()->GetParent()) };
                if (Window->UiTickResizeHandle.IsValid())
                {
                    Self->GetViewport().OnLateTick.Remove(&Window->UiTickResizeHandle);
                }
                Window->ResizeDragOffset.reset();

                return;
            })
            .TrailingParent(StaticCast<WParentBase>(&Content))
            ;
    }

    return;
}

void Jafg::WFloatingWindow::UiTickMove(LViewport const& Viewport)
{
    if (this->MoveDragOffset.has_value() == false)
    {
        this->MoveDragOffset =
            Viewport.GetSurface().GetMouseLocationValue()
                - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(Viewport);

        return;
    }

    LVec2F NewPos{Viewport.GetSurface().GetMouseLocationValue() - this->MoveDragOffset.value()};

    if (NewPos.x + 25.0 > Viewport.GetDimensions().x)
    {
        NewPos.x = Viewport.GetDimensions().x - 25.0;
    }
    if (NewPos.y + 25.0 > Viewport.GetDimensions().y)
    {
        NewPos.y = Viewport.GetDimensions().y - 25.0;
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

void Jafg::WFloatingWindow::UiTickResize(LViewport const& Viewport)
{
    if (this->ResizeDragOffset.has_value() == false)
    {
        this->ResizeDragOffset =
            Viewport.GetSurface().GetMouseLocationValue()
                - (this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(Viewport)
                    + this->GetWindow()->GetDesiredSize_v2());

        return;
    }

    LVec2F NewSize{
        Viewport.GetSurface().GetMouseLocationValue().x - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(Viewport).x - this->ResizeDragOffset.value().x,
        Viewport.GetSurface().GetMouseLocationValue().y - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(Viewport).y - this->ResizeDragOffset.value().y
    };

    LVec2F MaxSize{
        Viewport.GetDimensions().x - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(Viewport).x,
        Viewport.GetDimensions().y - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(Viewport).y
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

    check( NewSize.x >= 0.0 && NewSize.y >= 0.0 )

    this->SetWindowSize(NewSize);

    return;
}
