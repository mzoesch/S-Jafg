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
            .Tint(Lal::LColor::DarkerGray)
        [
            NewNode(WTextBox).SaveTo(&this->WindowTitle)
                .Anchor(EAnchor::CenterCenter)
                .Content("Floating Window")
                .TextColor(Lal::LColor::White)
            +
            NewNode(WTextButton)
                .Anchor(EAnchor::CenterRight)
                .Content("X")
                .OmniTint(Lal::LColor::Transparent)
                .OmniType(ERegionBrush::OutlineBox)
                .OmniOutlineThickness(1)
                .NormalType(ERegionBrush::None)
                .TextBlockBrush(LTextBoxBrush::Compact())
                .OnPrimaryRelease([this](WButton* Self, const LKeyEvent& InKeyEvent)
                {
                    if (this->OnWindowClosed.IsBound())
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

    this->SetWindowSize({640_spt, 360});

    WindowTopBar->OnKeyDownEvent.BindStrong([](WNode& Self, LViewport& Viewport, LKeyEvent const& KeyEvent) -> LReply
    {
        if (KeyEvent.GetKey() == EKeys::LeftMouseButton)
        {
            Viewport.GetSurface().SetMouseCursor(EMouseCursor::Hand);

            WFloatingWindow* Window{ StaticCast<WFloatingWindow>(Self.GetParent()->GetParent()) };
            Window->UserInterfaceTickDelegateHandle = Viewport.OnLateTick.AddMember(Window, &WFloatingWindow::UserInterfaceTick);

            return LReply::Handled();
        }

        return {};
    });

    WindowTopBar->OnKeyUpEvent.BindStrong([](WNode& Self, LViewport& Viewport, LKeyEvent const& KeyEvent) -> LReply
    {
        if (KeyEvent.GetKey() == EKeys::LeftMouseButton)
        {
            Viewport.GetSurface().SetMouseCursor(EMouseCursor::Default);

            auto* Window{ StaticCast<WFloatingWindow>(Self.GetParent()->GetParent()) };
            if (Window->UserInterfaceTickDelegateHandle.IsValid())
            {
                Viewport.OnLateTick.Remove(&Window->UserInterfaceTickDelegateHandle);
            }
            Window->DragOffset.reset();

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

    return;
}

void Jafg::WFloatingWindow::UserInterfaceTick(LViewport const& Viewport)
{
    if (this->DragOffset.has_value() == false)
    {
        this->DragOffset =
            Viewport.GetSurface().GetMouseLocation()
                - this->GetWindow()->GetAnchoredAndTranslatedTopLeftFromMostOuter(Viewport);

        return;
    }

    LVector2 NewPos{ Viewport.GetSurface().GetMouseLocation() - this->DragOffset.value() };

    if (NewPos.X + 25.0f > Viewport.GetDimensions().X)
    {
        NewPos.X = Viewport.GetDimensions().X - 25.0f;
    }
    if (NewPos.Y + 25.0f > Viewport.GetDimensions().Y)
    {
        NewPos.Y = Viewport.GetDimensions().Y - 25.0f;
    }

    if (NewPos.X - 75.0f + this->GetWindow()->GetDesiredSize_v2().X < 0.0f)
    {
        NewPos.X = 75.0 - this->GetWindow()->GetDesiredSize_v2().X;
    }
    if (NewPos.Y < 0.0f)
    {
        NewPos.Y = 0.0f;
    }

    this->SetWindowPosition(NewPos);

    return;
}
