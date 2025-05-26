// Copyright mzoesch. All rights reserved.

#include "Widgets/ScrollRegion.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Rhi/OrthographicBoxShader.h"
#include "Rhi/OrthographicRoundedBoxShader.h"
#include "Rhi/OrthographicOutlineBoxShader.h"
#include "Rhi/OrthographicRoundedOutlineBoxShader.h"
#include "Rhi/RendererStateMachine.h"
#include "User/UserPreferences.h"

Jafg::WScrollRegion::WScrollRegion(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::Visible);
    return;
}

void Jafg::WScrollRegion::Draw(LViewport& Context) const
{
    check( this->ScrollPosition.Y >= 0.0f && this->ScrollPosition.Y <= 1.0f )

    const LOrthographicBoxShader* Shader = GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox);
    const LVector2 AnchoredTopLeftFromMostOuter = this->GetAnchoredTopLeftFromMostOuter(Context);

    const LVector2 MostOuterTopLeftContentArea = AnchoredTopLeftFromMostOuter + this->GetPadding().GetTopLeftOffset();
    const LVector2 MaxContentAreaSize = this->GetAnchoredSize() - this->GetPadding().GetDesiredSize();

    const f64 MaxScrollY = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize().Y), 0.0);
    const f32 ScrollOffsetY = this->ScrollPosition.Y * MaxScrollY;
    const f32 ScrollOffsetYPercent = ScrollOffsetY / static_cast<f64>(this->DesiredSizeOfChildren.Y);
    const f32 VisibleY = Maths::Clamp(this->GetAnchoredSize().Y / this->DesiredSizeOfChildren.Y, 0.0f, 1.0f);

    const f64 MaxScrollX = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.X) - static_cast<f64>(this->GetAnchoredSize().X), 0.0);
    const f32 ScrollOffsetX = this->ScrollPosition.X * MaxScrollX;
    const f32 ScrollOffsetXPercent = ScrollOffsetX / static_cast<f64>(this->DesiredSizeOfChildren.X);
    const f32 VisibleX = Maths::Clamp(this->GetAnchoredSize().X / this->DesiredSizeOfChildren.X, 0.0f, 1.0f);

    /* BEGIN Ourselves. */
    if (this->Super::GetBrush().Type == ERegionBrush::Box)
    {
        GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox)->Draw
        (
            Context,
            this->GetAnchoredSize(),
            this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
            this->Super::GetBrush().Tint
        );
    }

    else if (this->Super::GetBrush().Type == ERegionBrush::RoundedBox)
    {
        GEngine->GetShaderChecked<LOrthographicRoundedBoxShader>(Name_ShaderOrthographicRoundedBox)->Draw
        (
            Context,
            this->GetAnchoredSize(),
            this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
            this->Super::GetBrush().Tint,
            this->Super::GetBrush().Radii
        );
    }

    else if (this->Super::GetBrush().Type == ERegionBrush::OutlineBox)
    {
        GEngine->GetShaderChecked<LOrthographicOutlineBoxShader>(Name_ShaderOrthographicOutlineBox)->Draw
        (
            Context,
            this->GetAnchoredSize(),
            this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
            this->Super::GetBrush().Tint,
            this->Super::GetBrush().OutlineThickness,
            this->Super::GetBrush().OutlineTint
        );
    }

    else if (this->Super::GetBrush().Type == ERegionBrush::RoundedOutlineBox)
    {
        GEngine->GetShaderChecked<LOrthographicRoundedOutlineBoxShader>(Name_ShaderOrthographicRoundedOutlineBox)->Draw
        (
            Context,
            this->GetAnchoredSize(),
            this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
            this->Super::GetBrush().Tint,
            this->Super::GetBrush().OutlineThickness,
            this->Super::GetBrush().OutlineTint,
            this->Super::GetBrush().Radii
        );
    }
    /* END Ourselves. */

    Context.ApplyFrameTranslation({-ScrollOffsetX, -ScrollOffsetY});
    RendererStateMachine::ClipOrthographic(Context, MostOuterTopLeftContentArea.Copy(), MaxContentAreaSize);
    WOverlay::Draw(Context);
    RendererStateMachine::DisableClipOrthographic();
    Context.ApplyFrameTranslation({ScrollOffsetX, ScrollOffsetY});

    if (this->DrawVScrollbar(VisibleY))
    {
        if (this->Brush.VScrollBarBackgroundWidth > 0.0f)
        {
            Shader->Draw /* Background.  */
            (
                Context,
                this->GetVBackgroundScrollSize(),
                AnchoredTopLeftFromMostOuter + this->GetVBackgroundScrollPositionFromOuter(),
                this->Brush.VBackgroundTint
            );
        }

        if (this->Brush.VScrollBarWidth > 0.0f)
        {
            Shader->Draw /* Foreground. */
            (
                Context,
                this->GetVForegroundScrollSize(VisibleY),
                AnchoredTopLeftFromMostOuter + this->GetVForegroundScrollPositionFromOuter(ScrollOffsetYPercent),
                this->Brush.VTint
            );
        }
    }

    if (this->DrawHScrollbar(VisibleX))
    {
        if (this->Brush.HScrollBarBackgroundHeight > 0.0f)
        {
            Shader->Draw /* Background.  */
            (
                Context,
                this->GetHBackgroundScrollSize(),
                AnchoredTopLeftFromMostOuter + this->GetHBackgroundScrollPositionFromOuter(),
                this->Brush.HBackgroundTint
            );
        }

        if (this->Brush.HScrollBarHeight > 0.0f)
        {
            Shader->Draw /* Foreground. */
            (
                Context,
                this->GetHForegroundScrollSize(VisibleX),
                AnchoredTopLeftFromMostOuter + this->GetHForegroundScrollPositionFromOuter(ScrollOffsetXPercent),
                this->Brush.HTint
            );
        }
    }

    return;
}

void Jafg::WScrollRegion::UserInterfaceTick(const LViewport& InViewport)
{
    if (InViewport.GetCachedCursorLocation().IsSet() == false)
    {
        return;
    }

    if (this->bUiTickV)
    {
        const f32 VForegroundScrollHeight = this->GetVForegroundScrollSize().Y;
        this->ScrollPosition.Y = Maths::Clamp
        (
            (
                InViewport.GetCachedCursorLocationChecked()->Y
                - (this->GetAnchoredTopLeftFromMostOuter(InViewport) + this->GetVBackgroundScrollPositionFromOuter()).Y
                - (Maths::IsNearlyZero(this->MbVOffset) ? (VForegroundScrollHeight * 0.5f) : 0.0f) + this->MbVOffset
            )
            /
            (this->GetVBackgroundScrollSize().Y - VForegroundScrollHeight),
            0.0f,
            1.0f
        );
    }

    if (this->bUiTickH)
    {
        const f32 HForegroundScrollWidth = this->GetHForegroundScrollSize().X;
        this->ScrollPosition.X = Maths::Clamp
        (
            (
                InViewport.GetCachedCursorLocationChecked()->X
                - (this->GetAnchoredTopLeftFromMostOuter(InViewport) + this->GetHBackgroundScrollPositionFromOuter()).X
                - (Maths::IsNearlyZero(this->MbHOffset) ? (HForegroundScrollWidth * 0.5f) : 0.0f) + this->MbHOffset
            )
            /
            (this->GetHBackgroundScrollSize().X - HForegroundScrollWidth),
            0.0f,
            1.0f
        );
    }

    return;
}

Jafg::LReply Jafg::WScrollRegion::OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
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

Jafg::LReply Jafg::WScrollRegion::OnKeyUp(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
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
    if (const LReply Reply = Super::OnKeyDownNoFocus(InViewport, InKeyEvent); Reply.IsHandled())
    {
        return Reply;
    }

    if (InKeyEvent.GetKey() == EKeys::MouseWheelAxis)
    {
        this->ApplyScroll(InKeyEvent);
        return LReply::Handled();
    }

    return LReply::Unhandled();
}

void Jafg::WScrollRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    this->DesiredSizeOfChildren = this->GetDesiredSize();
    this->SetDesiredSize(this->ScrollRegionSize + this->GetPadding().GetDesiredSize());

    return;
}

void Jafg::WScrollRegion::ApplyScroll(const LKeyEvent& InKeyEvent)
{
    const JUserPreferences* Prefs = GetDefault<JUserPreferences>();

    this->ScrollPosition.Y = Maths::Clamp
    (
        this->ScrollPosition.Y
        + Maths::Sign(InKeyEvent.GetValue())
        *
        (
            Prefs->MouseWheelScrollSpeed
            /
            Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize().Y), 0.0)
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

    if (InViewport.GetCachedCursorLocation().IsSet() == false)
    {
        return false;
    }

    const LVector2 CachedCursorLocation = *InViewport.GetCachedCursorLocationChecked();
    const LVector2 TopLeftMostOuter = this->GetAnchoredTopLeftFromMostOuter(InViewport);

    if (LViewport::IsInBounds
    (
        TopLeftMostOuter + this->GetVInteractiveAreaScrollPositionFromOuter(),
        this->GetVInteractiveAreaScrollSize(),
        CachedCursorLocation
    ))
    {
        if
        (
            const LVector2 TopLeftForeground = TopLeftMostOuter + this->GetVForegroundScrollPositionFromOuter();
            LViewport::IsInBounds
            (
                TopLeftForeground,
                this->GetVForegroundScrollSize(),
                CachedCursorLocation
            )
        )
        {
            this->MbVOffset = Maths::Invert(CachedCursorLocation.Y - TopLeftForeground.Y);
        }

        this->bUiTickV = true;
        this->UserInterfaceTickDelegateHandle = InViewport.OnLateTick.AddMember(this, &WScrollRegion::UserInterfaceTick);
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
            const LVector2 TopLeftForeground = TopLeftMostOuter + this->GetHForegroundScrollPositionFromOuter();
            LViewport::IsInBounds
            (
                TopLeftForeground,
                this->GetHForegroundScrollSize(),
                CachedCursorLocation
            )
        )
        {
            this->MbHOffset = Maths::Invert(CachedCursorLocation.X - TopLeftForeground.X);
        }

        this->bUiTickH = true;
        this->UserInterfaceTickDelegateHandle = InViewport.OnLateTick.AddMember(this, &WScrollRegion::UserInterfaceTick);
        return true;
    }

    return false;
}

bool Jafg::WScrollRegion::MBUpOnScrollbar(const LViewport& InViewport)
{
    if (this->UserInterfaceTickDelegateHandle.IsValid())
    {
        this->bUiTickV = false;
        this->bUiTickH = false;
        this->MbVOffset = 0.0f;
        this->MbHOffset = 0.0f;
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

    return Maths::IsNearlyEqual(InVisibleY, 1.0f) == false;
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

    return Maths::IsNearlyEqual(InVisibleX, 1.0f) == false;
}

Jafg::LVector2 Jafg::WScrollRegion::GetVInteractiveAreaScrollSize() const
{
    return
    {
        Maths::Max(this->Brush.VScrollBarBackgroundWidth, this->Brush.VScrollBarWidth),
        this->GetAnchoredSize().Y - this->Brush.VScrollBarPadding.Y * 2
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetVInteractiveAreaScrollPositionFromOuter() const
{
    return
    {
        this->GetAnchoredSize().X - Maths::Max(this->Brush.VScrollBarBackgroundWidth, this->Brush.VScrollBarWidth) - this->Brush.VScrollBarPadding.X,
        this->Brush.VScrollBarPadding.Y
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetHInteractiveAreaScrollSize() const
{
    return
    {
        this->GetAnchoredSize().X - this->Brush.HScrollBarPadding.X * 2,
        Maths::Max(this->Brush.HScrollBarBackgroundHeight, this->Brush.HScrollBarHeight)
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetHInteractiveAreaScrollPositionFromOuter() const
{
    return
    {
        this->Brush.HScrollBarPadding.X,
        this->GetAnchoredSize().Y - Maths::Max(this->Brush.HScrollBarBackgroundHeight, this->Brush.HScrollBarHeight) - this->Brush.HScrollBarPadding.Y
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetVBackgroundScrollSize() const
{
    return
    {
        this->Brush.VScrollBarBackgroundWidth,
        this->GetAnchoredSize().Y - this->Brush.VScrollBarPadding.Y * 2
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetVBackgroundScrollPositionFromOuter() const
{
    return
    {
        this->GetAnchoredSize().X - this->Brush.VScrollBarBackgroundWidth - this->Brush.VScrollBarPadding.X,
        this->Brush.VScrollBarPadding.Y
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetVForegroundScrollSize() const
{
    return this->GetVForegroundScrollSize(Maths::Clamp(this->GetAnchoredSize().Y / this->DesiredSizeOfChildren.Y, 0.0f, 1.0f));
}

Jafg::LVector2 Jafg::WScrollRegion::GetVForegroundScrollSize(const f32 InVisibleY) const
{
    return
    {
        this->Brush.VScrollBarWidth,
        (this->GetAnchoredSize().Y - this->Brush.VScrollBarPadding.Y * 2) * InVisibleY
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetVForegroundScrollPositionFromOuter() const
{
    const f64 MaxScroll = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize().Y), 0.0);
    const f32 ScrollOffsetY = this->ScrollPosition.Y * MaxScroll;
    const f32 ScrollOffsetYPercent = ScrollOffsetY / static_cast<f64>(this->DesiredSizeOfChildren.Y);

    return this->GetVForegroundScrollPositionFromOuter(ScrollOffsetYPercent);
}

Jafg::LVector2 Jafg::WScrollRegion::GetVForegroundScrollPositionFromOuter(const f32 InScrollOffsetYPercent) const
{
    check( InScrollOffsetYPercent >= 0.0f && InScrollOffsetYPercent <= 1.0f )
    return
    {
        this->GetAnchoredSize().X - this->Brush.VScrollBarWidth - this->Brush.VScrollBarPadding.X
        - Maths::Max(this->Brush.VScrollBarBackgroundWidth - this->Brush.VScrollBarWidth, 0.0f) / 2.0f,
        this->Brush.VScrollBarPadding.Y + InScrollOffsetYPercent * (this->GetAnchoredSize().Y - this->Brush.VScrollBarPadding.Y * 2)
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetHBackgroundScrollSize() const
{
    return
    {
        this->GetAnchoredSize().X - this->Brush.HScrollBarPadding.X * 2,
        this->Brush.HScrollBarBackgroundHeight
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetHBackgroundScrollPositionFromOuter() const
{
    return
    {
        this->Brush.HScrollBarPadding.X,
        this->GetAnchoredSize().Y - this->Brush.HScrollBarBackgroundHeight - this->Brush.HScrollBarPadding.Y
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetHForegroundScrollSize() const
{
    return this->GetHForegroundScrollSize(Maths::Clamp(this->GetAnchoredSize().X / this->DesiredSizeOfChildren.X, 0.0f, 1.0f));
}

Jafg::LVector2 Jafg::WScrollRegion::GetHForegroundScrollSize(const f32 InVisibleX) const
{
    return
    {
        (this->GetAnchoredSize().X - this->Brush.HScrollBarPadding.X * 2) * InVisibleX,
        this->Brush.HScrollBarHeight,
    };
}

Jafg::LVector2 Jafg::WScrollRegion::GetHForegroundScrollPositionFromOuter() const
{
    const f64 MaxScroll = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.X) - static_cast<f64>(this->GetAnchoredSize().X), 0.0);
    const f32 ScrollOffsetX = this->ScrollPosition.X * MaxScroll;
    const f32 ScrollOffsetXPercent = ScrollOffsetX / static_cast<f64>(this->DesiredSizeOfChildren.X);

    return this->GetHForegroundScrollPositionFromOuter(ScrollOffsetXPercent);
}

Jafg::LVector2 Jafg::WScrollRegion::GetHForegroundScrollPositionFromOuter(const f32 InScrollOffsetXPercent) const
{
    check( InScrollOffsetXPercent >= 0.0f && InScrollOffsetXPercent <= 1.0f )
    return
    {
        this->Brush.HScrollBarPadding.X + InScrollOffsetXPercent * (this->GetAnchoredSize().X - this->Brush.HScrollBarPadding.X * 2),
        this->GetAnchoredSize().Y - this->Brush.HScrollBarHeight - this->Brush.HScrollBarPadding.Y
        - Maths::Max(this->Brush.HScrollBarBackgroundHeight - this->Brush.HScrollBarHeight, 0.0f) / 2.0f,
    };
}
