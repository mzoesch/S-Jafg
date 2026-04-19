// Copyright mzoesch. All rights reserved.

#include "Nodes/FloatingWidget.h"
#include "Platform/Surface.h"
#include "Nodes/Viewport.h"
#include "Nodes/Region.h"
#include "Nodes/TextButton.h"

void Jafg::WFloatingWidget::Construct()
{
    Super::Construct();
    check(this->IsTopLevel() && "WFloatingWidgets must be top level.")

    WParent* Container{};
    if (this->bDecorate)
    {
        BeginStyling(*this).StaticRoot<WVRegion>().SaveTo(&Container)
            .SkipBrushDraw(true)
            .Anchor(EAnchor::TopLeft)
            .Visibility(ENodeVisibility::IntransitiveHitTestInvisible)
        [
            NewStaticNode(WRegion)
                .Visibility(ENodeVisibility::Visible)
                .MinDesiredSize({0_spt, 16})
                .Anchor(EAnchor::HFill)
                .Tint(Colors::DarkerGray)
                .OnKeyDown([](WNode& Self, LNodeKeyEventData const& Data, LKeyEvent const& Event) -> LReply
                {
                    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                    {
                        Data.Surface._SetMouseCursor(EMouseCursor::Hand);
                        WFloatingWidget* Window{StaticCast<WFloatingWidget>(Self.GetParent()->GetParent())};
                        check(Window->UiTickMoveHandle.IsValid() == false)
                        Window->UiTickMoveHandle = Data.Viewport.OnLateTick.Emplace(Window, &WFloatingWidget::UiTickMove);
                        return LReply::Handled();
                    }
                    return {};
                })
                .OnKeyUp([](WNode& Self, LNodeKeyEventData const& Data, LKeyEvent const& Event) -> LReply
                {
                    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                    {
                        Data.Surface._SetMouseCursor(EMouseCursor::Default);
                        auto* Window{StaticCast<WFloatingWidget>(Self.GetParent()->GetParent())};
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
                    .Content(this->InitialTitle.empty() ? "Floating Window" : std::move(this->InitialTitle))
                    .TextTint(Colors::White)
                +
                NewStaticNode(WTextButton)
                    .Anchor(EAnchor::CenterRight)
                    .Content("X")
                    .InAllBrushes<&LBoxBrush::Tint>(Colors::Transparent)
                    .InAllBrushes<&LBoxBrush::OutlineThickness>(1)
                    .TextBrush({ETextScale::Compact})
                    .OnPrimaryRelease([this](auto&, LKeyEvent const& KeyEvent)
                    {
                        if (this->OnWindowClosedEvent.IsValid())
                        {
                            if (this->OnWindowClosedEvent.Invoke(*this))
                            {
                                return;
                            }
                        }
                        this->RemoveFromParent2();
                    })
            ]
        ];
    }

    check(this->ContentEvent.IsValid())
    this->ContentEvent.Invoke(*this, Container ? *Container : *this);
    check(&this->GetWindow())
    this->ContentEvent.Reset();
    this->GetWindow().Anchor = this->InitialWindowAnchor;
    this->GetWindow().SetVisibility(this->InitialWindowVisibility);
    if (this->bCreateResizeUi)
    {
        if (WParent* Parent{this->GetWindow().As<WParent>()})
        {
            BeginStyling(*Parent).StaticRoot<WTextButton>()
                .Anchor(EAnchor::BottomRight)
                .Content("#")
                .TextScale(ETextScale::Compact)
                .Padding({2_spt})
                .OnPrimaryPress([this](WTextButton&, LKeyEvent const&)
                {
                    this->UiTickResizeHandle = this->GetViewport().OnLateTick.Emplace(this, &WFloatingWidget::UiTickResize);
                })
                .OnPrimaryRelease([this](WTextButton&, LKeyEvent const&)
                {
                    if (this->UiTickResizeHandle.IsValid())
                    {
                        this->GetViewport().OnLateTick.Remove(&this->UiTickResizeHandle);
                    }
                    this->ResizeDragOffset.reset();
                });
        }
        else
        {
            LOG_WARNING(LogWidgetFramework
                , "[]: Cannot create resize UI for window content of type [{}] as it is not a WParent."
                , this->GetNameAsString(), this->GetWindow().GetNameAsString())
        }
    }

    this->SetWindowSize(this->InitialWindowSize);
    this->SetWindowPosition(this->InitialWindowPosition);

    return;
}

bool Jafg::WFloatingWidget::UiTickMove()
{
    if (this->MoveDragOffset.has_value() == false)
    {
        this->MoveDragOffset =
            this->GetViewport().GetSurface().GetMouseLocationValue()
                // TODO: This is wrong. How do we get the translation here?
                - this->GetWindow().GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>);
        return {};
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

    if (NewPos.x - 75.0 + this->GetWindow().GetDesiredSize_v2().x < 0.0)
    {
        NewPos.x = 75.0 - this->GetWindow().GetDesiredSize_v2().x;
    }
    if (NewPos.y < 0.0)
    {
        NewPos.y = 0.0;
    }

    this->SetWindowPosition(NewPos);

    return {};
}

bool Jafg::WFloatingWidget::UiTickResize()
{
    if (this->ResizeDragOffset.has_value() == false)
    {
        this->ResizeDragOffset =
            this->GetViewport().GetSurface().GetMouseLocationValue()
                // TODO: This is wrong. How do we get the translation here? Same for below
                - (this->GetWindow().GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)
                    + this->GetWindow().GetDesiredSize_v2());
        return {};
    }

    LVec2F NewSize{
        this->GetViewport().GetSurface().GetMouseLocationValue().x
        - this->GetWindow().GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).x - this->ResizeDragOffset.value().x,
        this->GetViewport().GetSurface().GetMouseLocationValue().y
        - this->GetWindow().GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).y - this->ResizeDragOffset.value().y
        };

    LVec2F MaxSize{
        this->GetViewport().GetDimensions().x - this->GetWindow().GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).x,
        this->GetViewport().GetDimensions().y - this->GetWindow().GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).y
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

    this->SetWindowSize({EWidgetSize::StaticPoints, NewSize});

    return {};
}
