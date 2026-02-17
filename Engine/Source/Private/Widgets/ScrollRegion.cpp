// Copyright mzoesch. All rights reserved.

#include "Widgets/ScrollRegion.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"

Jafg::LCursorReply Jafg::WScrollRegion::SweepMouse(LViewport& Context, const LVec2F& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return WParentBase::SweepMouse(Context, InLocation);
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

            for (const LWidgetSlot* ChildSlot : this->GetChildren())
            {
                if (ChildSlot->Content->ShouldCheckForInputs())
                {
                    if (const LCursorReply Reply = ChildSlot->Content->SweepMouse(Context, InLocation); Reply.IsHandled())
                    {
                        return Reply;
                    }
                }

                continue;
            }
        }
    }

    return WParentBase::SweepMouse(Context, InLocation);
}

Jafg::LReply Jafg::WScrollRegion::SweepFocusTest(const LViewport& Context, const LVec2F& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return WParentBase::SweepFocusTest(Context, InLocation);
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

            for (const LWidgetSlot* ChildSlot : this->GetChildren())
            {
                if (ChildSlot->Content->ShouldCheckForInputs())
                {
                    if (const LReply Reply = ChildSlot->Content->SweepFocusTest(Context, InLocation); Reply.IsHandled())
                    {
                        return Reply;
                    }
                }

                continue;
            }
        }
    }

    return WParentBase::SweepFocusTest(Context, InLocation);
}

void Jafg::WScrollRegion::UserInterfaceTick(const LViewport& InViewport)
{
    if (InViewport.GetCachedCursorLocation().has_value() == false)
    {
        return;
    }

    if (this->bUiTickV)
    {
        const f32 VForegroundScrollHeight = this->GetVForegroundScrollSize().y;
        this->ScrollPosition.y = maths::clamp
        (
            (
                InViewport.GetCachedCursorLocationChecked()->y
                - (this->GetAnchoredTopLeftFromMostOuter(InViewport) + this->GetVBackgroundScrollPositionFromOuter()).y
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
                InViewport.GetCachedCursorLocationChecked()->x
                - (this->GetAnchoredTopLeftFromMostOuter(InViewport) + this->GetHBackgroundScrollPositionFromOuter()).x
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

Jafg::LReply Jafg::WScrollRegion::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::MouseWheelAxis)
    {
        this->ApplyScroll(InKeyEvent);
        return LReply::Handled();
    }

    if (InKeyEvent.GetKey() == EKeys::LeftMouseButton)
    {
        if (this->MBDownOnScrollbar(InViewport))
        {
            return LReply::Handled();
        }
    }

    return Super::OnKeyDown(InViewport, InKeyEvent);
}

Jafg::LReply Jafg::WScrollRegion::OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::LeftMouseButton)
    {
       if (this->MBUpOnScrollbar(InViewport))
       {
           return LReply::Handled();
       }
    }

    return Super::OnKeyUp(InViewport, InKeyEvent);
}

Jafg::LReply Jafg::WScrollRegion::OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    check( this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f )

    {
        const f32 maxScrollY { maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f) };
        const f32 ScrollOffsetY { this->ScrollPosition.y * maxScrollY };

        const f32 maxScrollX { maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f) };
        const f32 ScrollOffsetX { this->ScrollPosition.x * maxScrollX };

        LViewportSweepTranslation Translation{InViewport, {-ScrollOffsetX, -ScrollOffsetY}};

        for (const LWidgetSlot* ChildSlot : this->GetChildren())
        {
            check( ChildSlot->Content )

            if (ChildSlot->Content == InViewport.GetFocusedWidget())
            {
                continue;
            }

            if (ChildSlot->Content->ShouldCheckForInputs() == false)
            {
                continue;
            }

            if (ChildSlot->Content->IsInBounds(InViewport, *InViewport.GetCachedCursorLocationChecked()) == false)
            {
                continue;
            }

            if (const LReply Reply = ChildSlot->Content->OnKeyDownNoFocus(InViewport, InKeyEvent); Reply.IsHandled())
            {
                return Reply;
            }

            continue;
        }
    }

    if (InKeyEvent.GetKey() == EKeys::MouseWheelAxis)
    {
        this->ApplyScroll(InKeyEvent);
        return LReply::Handled();
    }

    return WParentBase::OnKeyDownNoFocus(InViewport, InKeyEvent);
}

Jafg::LReply Jafg::WScrollRegion::OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    check( this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f )

    {
        const f32 maxScrollY = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f);
        const f32 ScrollOffsetY = this->ScrollPosition.y * maxScrollY;

        const f32 maxScrollX = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f);
        const f32 ScrollOffsetX = this->ScrollPosition.x * maxScrollX;

        LViewportSweepTranslation Translation{InViewport, {-ScrollOffsetX, -ScrollOffsetY}};

        for (const LWidgetSlot* ChildSlot : this->GetChildren())
        {
            check( ChildSlot->Content )

            if (ChildSlot->Content == InViewport.GetFocusedWidget())
            {
                continue;
            }

            if (ChildSlot->Content->ShouldCheckForInputs() == false)
            {
                continue;
            }

            if (ChildSlot->Content->IsInBounds(InViewport, *InViewport.GetCachedCursorLocationChecked()) == false)
            {
                continue;
            }

            if (const LReply Reply = ChildSlot->Content->OnKeyUpNoFocus(InViewport, InKeyEvent); Reply.IsHandled())
            {
                return Reply;
            }

            continue;
        }
    }

    return WParentBase::OnKeyUpNoFocus(InViewport, InKeyEvent);
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

void Jafg::WScrollRegion::ApplyScroll(const LKeyEvent& InKeyEvent)
{
    const JUserPreferences* Prefs = &GetSingleton<JUserPreferences>();

    this->ScrollPosition.y = maths::clamp
    (
        this->ScrollPosition.y
        + maths::sign(InKeyEvent.GetValue())
        *
        (
            *Prefs->MouseWheelScrollSpeed
            /
            maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f)
        )
        * (Prefs->bInvertVerticalScrollWheel ? -1.0f : 1.0f),
        0.0f,
        1.0f
    );

    return;
}

bool Jafg::WScrollRegion::MBDownOnScrollbar(const LViewport& InViewport)
{
    check( this->UserInterfaceTickDelegateHandle.IsValid() == false )

    if (InViewport.GetCachedCursorLocation().has_value() == false)
    {
        return false;
    }

    const LVec2F CachedCursorLocation = *InViewport.GetCachedCursorLocationChecked();
    const LVec2F TopLeftMostOuter = this->GetAnchoredTopLeftFromMostOuter(InViewport);

    if (LViewport::IsInBounds
    (
        TopLeftMostOuter + this->GetVInteractiveAreaScrollPositionFromOuter(),
        this->GetVInteractiveAreaScrollSize(),
        CachedCursorLocation
    ))
    {
        if
        (
            const LVec2D TopLeftForeground = TopLeftMostOuter + this->GetVForegroundScrollPositionFromOuter();
            LViewport::IsInBounds
            (
                TopLeftForeground,
                this->GetVForegroundScrollSize(),
                CachedCursorLocation
            )
        )
        {
            this->MbVOffset = (CachedCursorLocation.y - TopLeftForeground.y) * -1.0f;
        }

        this->bUiTickV = true;
        this->UserInterfaceTickDelegateHandle = InViewport.OnLateTick.Emplace(this, &WScrollRegion::UserInterfaceTick);
        return true;
    }

    if (LViewport::IsInBounds
    (
        TopLeftMostOuter + this->GetHInteractiveAreaScrollPositionFromOuter(),
        this->GetHInteractiveAreaScrollSize(),
        CachedCursorLocation
    ))
    {
        if
        (
            const LVec2D TopLeftForeground = TopLeftMostOuter + this->GetHForegroundScrollPositionFromOuter();
            LViewport::IsInBounds
            (
                TopLeftForeground,
                this->GetHForegroundScrollSize(),
                CachedCursorLocation
            )
        )
        {
            this->MbHOffset = (CachedCursorLocation.x - TopLeftForeground.x) * -1.0f;
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
