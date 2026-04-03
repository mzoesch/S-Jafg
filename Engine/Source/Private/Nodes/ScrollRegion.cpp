// Copyright mzoesch. All rights reserved.

#include "Nodes/ScrollRegion.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"

Jafg::LCursorReply Jafg::WScrollRegion::SweepMouse(LViewport& Context, const LVec2F& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return WNode::SweepMouse(Context, InLocation);
    }

    if (this->IsInBounds(Context, InLocation))
    {
        check( this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f )

        {
            const f32 maxScrollY = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f);
            const f32 ScrollOffsetY = this->ScrollPosition.y * maxScrollY;

            const f32 maxScrollX = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f);
            const f32 ScrollOffsetX = this->ScrollPosition.x * maxScrollX;

            LViewportSweepTranslation Translation{Context, {-ScrollOffsetX, -ScrollOffsetY}};

            for (auto& Child : this->GetChildren())
            {
                if (Child->ShouldCheckForInputs())
                {
                    if (const LCursorReply Reply = Child->SweepMouse(Context, InLocation); Reply.IsHandled())
                    {
                        return Reply;
                    }
                }

                continue;
            }
        }
    }

    return WNode::SweepMouse(Context, InLocation);
}

Jafg::LReply Jafg::WScrollRegion::SweepFocusTest(const LViewport& Context, const LVec2F& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return WNode::SweepFocusTest(Context, InLocation);
    }

    if (this->IsInBounds(Context, InLocation))
    {
        check( this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f )

        {
            const f32 maxScrollY = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f);
            const f32 ScrollOffsetY = this->ScrollPosition.y * maxScrollY;

            const f32 maxScrollX = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f);
            const f32 ScrollOffsetX = this->ScrollPosition.x * maxScrollX;

            LViewportSweepTranslation Translation{Context, {-ScrollOffsetX, -ScrollOffsetY}};

            for (auto& Child : this->GetChildren())
            {
                if (Child->ShouldCheckForInputs())
                {
                    if (const LReply Reply = Child->SweepFocusTest(Context, InLocation); Reply.IsHandled())
                    {
                        return Reply;
                    }
                }

                continue;
            }
        }
    }

    return WNode::SweepFocusTest(Context, InLocation);
}

void Jafg::WScrollRegion::UserInterfaceTick()
{
    if (this->GetViewport().GetSurface().GetMouseLocation().has_value() == false)
    {
        return;
    }

    auto MouseLocation{this->GetViewport().GetSurface().GetMouseLocationValue()};

    if (this->bUiTickV)
    {
        const f32 VForegroundScrollHeight = this->GetVForegroundScrollSize().y;
        this->ScrollPosition.y = maths::clamp
        (
            (
                MouseLocation.y
                - (this->GetAnchoredTopLeftFromMostOuter(this->GetViewport()) + this->GetVBackgroundScrollPositionFromOuter()).y
                - (maths::eq_zero_e(this->MbVOffset) ? (VForegroundScrollHeight * 0.5f) : 0.0f) + this->MbVOffset
            )
            /
            (this->GetVBackgroundScrollSize().y - VForegroundScrollHeight),
            0.0f,
            1.0f
        );
    }

    if (this->bUiTickH)
    {
        const f32 HForegroundScrollWidth = this->GetHForegroundScrollSize().x;
        this->ScrollPosition.x = maths::clamp
        (
            (
                MouseLocation.x
                - (this->GetAnchoredTopLeftFromMostOuter(this->GetViewport()) + this->GetHBackgroundScrollPositionFromOuter()).x
                - (maths::eq_zero_e(this->MbHOffset) ? (HForegroundScrollWidth * 0.5f) : 0.0f) + this->MbHOffset
            )
            /
            (this->GetHBackgroundScrollSize().x - HForegroundScrollWidth),
            0.0f,
            1.0f
        );
    }

    return;
}

Jafg::LReply Jafg::WScrollRegion::OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event)
{
    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::MouseWheelUp))
    {
        this->HandleMouseWheelUp(Event.Value);
        return LReply::Handled();
    }
    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::MouseWheelDown))
    {
        this->HandleMouseWheelDown(Event.Value);
        return LReply::Handled();
    }

    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton))
    {
        if (this->MBDownOnScrollbar(Data.Viewport))
        {
            return LReply::Handled();
        }
    }

    return Super::OnKeyDown(Data, Event);
}

Jafg::LReply Jafg::WScrollRegion::OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event)
{
    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::LeftMouseButton))
    {
       if (this->MBUpOnScrollbar(Data.Viewport))
       {
           return LReply::Handled();
       }
    }

    return Super::OnKeyUp(Data, Event);
}

Jafg::LReply Jafg::WScrollRegion::OnKeyDownNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event)
{
    check(this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f)

    {
        const f32 maxScrollY{maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f)};
        const f32 ScrollOffsetY{this->ScrollPosition.y * maxScrollY};

        const f32 maxScrollX{maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f)};
        const f32 ScrollOffsetX{this->ScrollPosition.x * maxScrollX};

        LViewportSweepTranslation Translation{Data.Viewport, {-ScrollOffsetX, -ScrollOffsetY}};

        for (auto& Child : this->GetChildren())
        {
            check(Child.get())
            if (&*Child == Data.Viewport.GetFocusedWidget())
            {
                continue;
            }
            if (Child->ShouldCheckForInputs() == false)
            {
                continue;
            }
            if (Child->IsInBounds(Data.Viewport, Data.Surface.GetMouseLocationValue()) == false)
            {
                continue;
            }

            if (LReply Reply{Child->OnKeyDownNoFocus(Data, Event)}; Reply.IsHandled())
            {
                return Reply;
            }
            continue;
        }
    }

    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::MouseWheelUp))
    {
        this->HandleMouseWheelUp(Event.Value);
        return LReply::Handled();
    }
    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::MouseWheelDown))
    {
        this->HandleMouseWheelDown(Event.Value);
        return LReply::Handled();
    }

    /* Not super. */
    return WNode::OnKeyDownNoFocus(Data, Event);
}

Jafg::LReply Jafg::WScrollRegion::OnKeyUpNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& InKeyEvent)
{
    check( this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f )

    {
        const f32 maxScrollY = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f);
        const f32 ScrollOffsetY = this->ScrollPosition.y * maxScrollY;

        const f32 maxScrollX = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f);
        const f32 ScrollOffsetX = this->ScrollPosition.x * maxScrollX;

        LViewportSweepTranslation Translation{Data.Viewport, {-ScrollOffsetX, -ScrollOffsetY}};

        for (auto& Child : this->GetChildren())
        {
            check(Child.get())

            if (&*Child == Data.Viewport.GetFocusedWidget())
            {
                continue;
            }

            if (Child->ShouldCheckForInputs() == false)
            {
                continue;
            }

            if (Child->IsInBounds(Data.Viewport, Data.Surface.GetMouseLocationValue()) == false)
            {
                continue;
            }

            if (const LReply Reply = Child->OnKeyUpNoFocus(Data, InKeyEvent); Reply.IsHandled())
            {
                return Reply;
            }

            continue;
        }
    }

    return WNode::OnKeyUpNoFocus(Data, InKeyEvent);
}

void Jafg::WScrollRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    this->DesiredSizeOfChildren = this->GetDesiredSize_v2();
    this->SetDesiredSizeInSpt(
          InSpt(this->GetViewport(), this->ScrollRegionSize)
        + this->GetPadding().GetDesiredSizeInSpt(*this)
        + (this->bUseChildrenDesiredSize ? this->DesiredSizeOfChildren : maths::zero_vector<LVec2F>)
        );

    return;
}

bool Jafg::WScrollRegion::MBDownOnScrollbar(LViewport const& InViewport)
{
    check( this->UserInterfaceTickDelegateHandle.IsValid() == false )

    if (InViewport.GetSurface().GetMouseLocation().has_value() == false)
    {
        return false;
    }

    auto MouseLocation{InViewport.GetSurface().GetMouseLocationValue()};
    LVec2F TopLeftMostOuter{this->GetAnchoredTopLeftFromMostOuter(InViewport)};

    if (LViewport::IsInBounds
    (
        TopLeftMostOuter + this->GetVInteractiveAreaScrollPositionFromOuter(),
        this->GetVInteractiveAreaScrollSize(),
        MouseLocation
    ))
    {
        if
        (
            const LVec2D TopLeftForeground = TopLeftMostOuter + this->GetVForegroundScrollPositionFromOuter();
            LViewport::IsInBounds
            (
                TopLeftForeground,
                this->GetVForegroundScrollSize(),
                MouseLocation
            )
        )
        {
            this->MbVOffset = (MouseLocation.y - TopLeftForeground.y) * -1.0f;
        }

        this->bUiTickV = true;
        this->UserInterfaceTickDelegateHandle = InViewport.OnLateTick.Emplace(this, &WScrollRegion::UserInterfaceTick);
        return true;
    }

    if (LViewport::IsInBounds
    (
        TopLeftMostOuter + this->GetHInteractiveAreaScrollPositionFromOuter(),
        this->GetHInteractiveAreaScrollSize(),
        MouseLocation
    ))
    {
        if
        (
            const LVec2D TopLeftForeground = TopLeftMostOuter + this->GetHForegroundScrollPositionFromOuter();
            LViewport::IsInBounds
            (
                TopLeftForeground,
                this->GetHForegroundScrollSize(),
                MouseLocation
            )
        )
        {
            this->MbHOffset = (MouseLocation.x - TopLeftForeground.x) * -1.0f;
        }

        this->bUiTickH = true;
        this->UserInterfaceTickDelegateHandle = InViewport.OnLateTick.Emplace(this, &WScrollRegion::UserInterfaceTick);
        return true;
    }

    return false;
}

bool Jafg::WScrollRegion::MBUpOnScrollbar(LViewport const& InViewport)
{
    if (this->UserInterfaceTickDelegateHandle.IsValid())
    {
        this->bUiTickV = false;
        this->bUiTickH = false;
        this->MbVOffset = 0.0;
        this->MbHOffset = 0.0;
        InViewport.OnLateTick.Remove(&this->UserInterfaceTickDelegateHandle);
        return true;
    }

    return false;
}

bool Jafg::WScrollRegion::DrawVScrollbar(const f32 InVisibleY) const
{
    if (this->Brush.bAlwaysShowVScrollbar)
    {
        return true;
    }

    if (this->Brush.bAlwaysHideHScrollbar)
    {
        return false;
    }

    return maths::eq_e(InVisibleY, 1.0) == false;
}

bool Jafg::WScrollRegion::DrawHScrollbar(const f32 InVisibleX) const
{
    if (this->Brush.bAlwaysShowHScrollbar)
    {
        return true;
    }

    if (this->Brush.bAlwaysHideVScrollbar)
    {
        return false;
    }

    return maths::eq_e(InVisibleX, 1.0) == false;
}

LVec2F Jafg::WScrollRegion::GetVInteractiveAreaScrollSize() const
{
    return
    {
        maths::max(this->Brush.VScrollBarBackgroundWidth, this->Brush.VScrollBarWidth),
        this->GetAnchoredSize_v2().y - this->Brush.VScrollBarPadding.y * 2
    };
}

LVec2F Jafg::WScrollRegion::GetVInteractiveAreaScrollPositionFromOuter() const
{
    return
    {
        this->GetAnchoredSize_v2().x - maths::max(this->Brush.VScrollBarBackgroundWidth, this->Brush.VScrollBarWidth) - this->Brush.VScrollBarPadding.x,
        this->Brush.VScrollBarPadding.y
    };
}

LVec2F Jafg::WScrollRegion::GetHInteractiveAreaScrollSize() const
{
    return
    {
        this->GetAnchoredSize_v2().x - this->Brush.HScrollBarPadding.x * 2,
        maths::max(this->Brush.HScrollBarBackgroundHeight, this->Brush.HScrollBarHeight)
    };
}

LVec2F Jafg::WScrollRegion::GetHInteractiveAreaScrollPositionFromOuter() const
{
    return
    {
        this->Brush.HScrollBarPadding.x,
        this->GetAnchoredSize_v2().y - maths::max(this->Brush.HScrollBarBackgroundHeight, this->Brush.HScrollBarHeight) - this->Brush.HScrollBarPadding.y
    };
}

LVec2F Jafg::WScrollRegion::GetVBackgroundScrollSize() const
{
    return
    {
        this->Brush.VScrollBarBackgroundWidth,
        this->GetAnchoredSize_v2().y - this->Brush.VScrollBarPadding.y * 2
    };
}

LVec2F Jafg::WScrollRegion::GetVBackgroundScrollPositionFromOuter() const
{
    return
    {
        this->GetAnchoredSize_v2().x - this->Brush.VScrollBarBackgroundWidth - this->Brush.VScrollBarPadding.x,
        this->Brush.VScrollBarPadding.y
    };
}

LVec2F Jafg::WScrollRegion::GetVForegroundScrollSize() const
{
    return this->GetVForegroundScrollSize(maths::clamp(this->GetAnchoredSize_v2().y / this->DesiredSizeOfChildren.y, 0.0f, 1.0f));
}

LVec2F Jafg::WScrollRegion::GetVForegroundScrollSize(const f32 InVisibleY) const
{
    return
    {
        this->Brush.VScrollBarWidth,
        (this->GetAnchoredSize_v2().y - this->Brush.VScrollBarPadding.y * 2) * InVisibleY
    };
}

LVec2F Jafg::WScrollRegion::GetVForegroundScrollPositionFromOuter() const
{
    const f32 maxScroll = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f);
    const f32 ScrollOffsetY = this->ScrollPosition.y * maxScroll;
    const f32 ScrollOffsetYPercent = ScrollOffsetY / static_cast<f32>(this->DesiredSizeOfChildren.y);

    return this->GetVForegroundScrollPositionFromOuter(ScrollOffsetYPercent);
}

LVec2F Jafg::WScrollRegion::GetVForegroundScrollPositionFromOuter(const f32 InScrollOffsetYPercent) const
{
    check( InScrollOffsetYPercent >= 0.0 && InScrollOffsetYPercent <= 1.0 )
    return
    {
        this->GetAnchoredSize_v2().x - this->Brush.VScrollBarWidth - this->Brush.VScrollBarPadding.x
        - maths::max(this->Brush.VScrollBarBackgroundWidth - this->Brush.VScrollBarWidth, 0.0f) / 2.0,
        this->Brush.VScrollBarPadding.y + InScrollOffsetYPercent * (this->GetAnchoredSize_v2().y - this->Brush.VScrollBarPadding.y * 2)
    };
}

LVec2F Jafg::WScrollRegion::GetHBackgroundScrollSize() const
{
    return
    {
        this->GetAnchoredSize_v2().x - this->Brush.HScrollBarPadding.x * 2,
        this->Brush.HScrollBarBackgroundHeight
    };
}

LVec2F Jafg::WScrollRegion::GetHBackgroundScrollPositionFromOuter() const
{
    return
    {
        this->Brush.HScrollBarPadding.x,
        this->GetAnchoredSize_v2().y - this->Brush.HScrollBarBackgroundHeight - this->Brush.HScrollBarPadding.y
    };
}

LVec2F Jafg::WScrollRegion::GetHForegroundScrollSize() const
{
    return this->GetHForegroundScrollSize(maths::clamp(this->GetAnchoredSize_v2().x / this->DesiredSizeOfChildren.x, 0.0f, 1.0f));
}

LVec2F Jafg::WScrollRegion::GetHForegroundScrollSize(const f32 InVisibleX) const
{
    return
    {
        (this->GetAnchoredSize_v2().x - this->Brush.HScrollBarPadding.x * 2) * InVisibleX,
        this->Brush.HScrollBarHeight,
    };
}

LVec2F Jafg::WScrollRegion::GetHForegroundScrollPositionFromOuter() const
{
    const f32 maxScroll = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f);
    const f32 ScrollOffsetX = this->ScrollPosition.x * maxScroll;
    const f32 ScrollOffsetXPercent = ScrollOffsetX / static_cast<f32>(this->DesiredSizeOfChildren.x);

    return this->GetHForegroundScrollPositionFromOuter(ScrollOffsetXPercent);
}

LVec2F Jafg::WScrollRegion::GetHForegroundScrollPositionFromOuter(const f32 InScrollOffsetXPercent) const
{
    check( InScrollOffsetXPercent >= 0.0 && InScrollOffsetXPercent <= 1.0 )
    return
    {
        this->Brush.HScrollBarPadding.x + InScrollOffsetXPercent * (this->GetAnchoredSize_v2().x - this->Brush.HScrollBarPadding.x * 2),
        this->GetAnchoredSize_v2().y - this->Brush.HScrollBarHeight - this->Brush.HScrollBarPadding.y
        - maths::max(this->Brush.HScrollBarBackgroundHeight - this->Brush.HScrollBarHeight, 0.0f) / 2.0,
    };
}

void Jafg::WScrollRegion::HandleMouseWheelUp(f32 Value)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    this->ScrollPosition.y = maths::clamp(
        this->ScrollPosition.y
        + maths::sign(Value)
        *   (*Prefs.MouseWheelScrollSpeed /
            maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f))
        * (Prefs.bInvertVerticalScrollWheel ? -1.0f : 1.0f),
        0.0f,
        1.0f
        );

    return;
}

void Jafg::WScrollRegion::HandleMouseWheelDown(f32 Value)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    this->ScrollPosition.y = maths::clamp(
        this->ScrollPosition.y
        + maths::sign(-Value)
        *   (*Prefs.MouseWheelScrollSpeed /
            maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f))
        * (Prefs.bInvertVerticalScrollWheel ? -1.0f : 1.0f),
        0.0f,
        1.0f
        );

    return;
}
