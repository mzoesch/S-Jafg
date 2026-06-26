// Copyright mzoesch. All rights reserved.

#include "Nodes/FloatingWidget.h"
#include "Platform/Surface.h"
#include "Nodes/Viewport.h"
#include "Nodes/Region.h"
#include "Nodes/Text.h"
#include "Nodes/TextButton.h"
#include "Nodes/Button.h"
#include "Nodes/VParent.h"
#include "User/UserPreferences.h"

void Jafg::WFloatingWidget::DestroyFloatingWidgetControlled()
{
    if (this->OnWindowClosedEvent.IsValid())
    {
        if (this->OnWindowClosedEvent.Invoke(*this))
        {
            return;
        }
    }

    check(!this->GetParent())
    this->MarkAsGarbage_v2();

    return;
}

void Jafg::WFloatingWidget::Construct()
{
    Super::Construct();
    check(this->IsTopLevel() && "WFloatingWidgets must be top level.")

    auto& Prefs{GetSingleton<JUserPreferences>()};

    WParent* Container{};
    if (this->bDecorate)
    {
        BeginStyling(*this).StaticRoot<WVRegion>().SaveTo(&Container)
            .Anchor(EAnchor::TopLeft)
            .Visibility(ENodeVisibility::IntransitiveHitTestInvisible)
            .Tint(*Prefs.ForegroundColor)
            .OutlineTint(Colors::Gray)
            .OutlineThickness(1)
        [
            NewStaticNode(WRegion)
                .Visibility(ENodeVisibility::Visible)
                .MinDesiredSize({0_spt, 16})
                .Anchor(EAnchor::HFill)
                .Tint(*Prefs.BackgroundColor)
                .OnKeyEventFocused([](WNode& Self, LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
                {
                    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                    {
                        Data.Surface._SetMouseCursor(ECursor::Hand);
                        WFloatingWidget* Window{StaticCast<WFloatingWidget>(Self.GetParent()->GetParent())};
                        check(Window->UiTickMoveHandle.IsValid() == false)
                        Window->UiTickMoveHandle = Data.Viewport.OnLateTick.Emplace(Window, &WFloatingWidget::UiTickMove);
                        return LNodeReply::Handled();
                    }
                    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                    {
                        Data.Surface._SetMouseCursor(ECursor::Default);
                        auto* Window{StaticCast<WFloatingWidget>(Self.GetParent()->GetParent())};
                        if (Window->UiTickMoveHandle.IsValid())
                        {
                            Data.Viewport.OnLateTick.Remove(&Window->UiTickMoveHandle);
                        }
                        Window->MoveDragOffset.reset();
                        return LNodeReply::Handled();
                    }
                    return LNodeReply::Unhandled();
                })
            [
                NewStaticNode(WText).SaveTo(&this->WindowTitle)
                    .Anchor(EAnchor::CenterCenter)
                    .Content(this->InitialTitle.empty() ? "Floating Window" : std::move(this->InitialTitle))
                    .TextTint(Colors::White)
                +
                NewStaticNode(WButton)
                    .Anchor(EAnchor::CenterRight)
                    .MinDesiredSize(24_spt2)
                    .Style(Prefs.EditorPrimaryButton<LRegionBrush>("Icons/Jafg.X"))
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>())
                        {
                            this->DestroyFloatingWidgetControlled();
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
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
                .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
                {
                    if (Event.Is<ERawInputStateBits::Press>())
                    {
                        this->UiTickResizeHandle = this->GetViewport().OnLateTick.Emplace(this, &WFloatingWidget::UiTickResize);
                        return LNodeReply::Handled();
                    }
                    if (Event.Is<ERawInputStateBits::Release>())
                    {
                        if (this->UiTickResizeHandle.IsValid())
                        {
                            this->GetViewport().OnLateTick.Remove(&this->UiTickResizeHandle);
                        }
                        this->ResizeDragOffset.reset();
                        return LNodeReply::Handled();
                    }
                    return LNodeReply::Unhandled();
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
    auto& Surface{this->GetViewport().GetSurface()};
    if (!Surface.HasMouseLocationForOrtho())
    {
        return {};
    }

    if (!this->MoveDragOffset.has_value())
    {
        this->MoveDragOffset = Surface.GetMouseLocationValue()
                // TODO: This is wrong. How do we get the translation here?
                - this->GetWindow().GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>);
        return {};
    }

    LVec2F NewPos{Surface.GetMouseLocationValue() - this->MoveDragOffset.value()};

    if (NewPos.x + 25.0f > static_cast<f32>(this->GetViewport().GetExtent().width))
    {
        NewPos.x = static_cast<f32>(this->GetViewport().GetExtent().width) - 25.0f;
    }
    if (NewPos.y + 25.0f > static_cast<f32>(this->GetViewport().GetExtent().height))
    {
        NewPos.y = static_cast<f32>(this->GetViewport().GetExtent().height) - 25.0f;
    }

    if (NewPos.x - 75.0f + this->GetWindow().GetDesiredSize_v2().x < 0.0)
    {
        NewPos.x = 75.0f - this->GetWindow().GetDesiredSize_v2().x;
    }
    if (NewPos.y < 0.0f)
    {
        NewPos.y = 0.0f;
    }

    this->SetWindowPosition(NewPos);

    return {};
}

bool Jafg::WFloatingWidget::UiTickResize()
{
    if (!this->ResizeDragOffset.has_value())
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
        static_cast<f32>(this->GetViewport().GetExtent().width) - this->GetWindow().GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).x,
        static_cast<f32>(this->GetViewport().GetExtent().height) - this->GetWindow().GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>).y
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

    this->SetWindowSize({ENodeSize::StaticPoints, NewSize});

    return {};
}
