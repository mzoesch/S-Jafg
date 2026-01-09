// Copyright mzoesch. All rights reserved.

#include "Widgets/ScrollRegion.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Rhi/RendererStateMachine.h"
#include "User/UserPreferences.h"
#include "Rhi/OrthographicBoxShader.h"
#include "Rhi/OrthographicImageBoxShader.h"
#include "Rhi/OrthographicRoundedBoxShader.h"
#include "Rhi/OrthographicRoundedImageBoxShader.h"
#include "Rhi/OrthographicOutlineBoxShader.h"
#include "Rhi/OrthographicOutlineImageBoxShader.h"
#include "Rhi/OrthographicRoundedOutlineBoxShader.h"
#include "Rhi/OrthographicRoundedOutlineImageBoxShader.h"

Jafg::WScrollRegion::WScrollRegion(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetVisibility(EWidgetVisibility::Visible);
    return;
}

void Jafg::WScrollRegion::Draw(LViewport& Context) const
{
    check( this->ScrollPosition.Y >= 0.0f && this->ScrollPosition.Y <= 1.0f )

    const LOrthographicBoxShader* Shader { GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox) };
    const LVector2D AnchoredTopLeftFromMostOuter { this->GetAnchoredTopLeftFromMostOuter(Context) };

    const LVector2D MostOuterTopLeftContentArea { AnchoredTopLeftFromMostOuter + this->GetPadding().GetTopLeftOffsetInSpt(Context) };
    const LVector2D MaxContentAreaSize { this->GetAnchoredSize_v2() - this->GetPadding().GetDesiredSizeInSpt(Context) };

    const f64 MaxScrollY { Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize_v2().Y), 0.0) };
    const f64 ScrollOffsetY { this->ScrollPosition.Y * MaxScrollY };
    const f64 ScrollOffsetYPercent { ScrollOffsetY / static_cast<f64>(this->DesiredSizeOfChildren.Y) };
    const f64 VisibleY { Maths::Clamp(this->GetAnchoredSize_v2().Y / this->DesiredSizeOfChildren.Y, 0.0, 1.0) };

    const f64 MaxScrollX { Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.X) - static_cast<f64>(this->GetAnchoredSize_v2().X), 0.0) };
    const f64 ScrollOffsetX { this->ScrollPosition.X * MaxScrollX };
    const f64 ScrollOffsetXPercent { ScrollOffsetX / static_cast<f64>(this->DesiredSizeOfChildren.X) };
    const f64 VisibleX { Maths::Clamp(this->GetAnchoredSize_v2().X / this->DesiredSizeOfChildren.X, 0.0, 1.0) };

    /* BEGIN Ourselves. */
    if (this->Super::GetBrush().Type != ERegionBrush::None)
    {
        if (this->Super::GetBrush().Type == ERegionBrush::Box)
        {
            if (this->Super::GetBrush().Image.IsTextureValid())
            {
                GEngine->GetShaderChecked<LOrthographicImageBoxShader>(Name_ShaderOrthographicImageBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize_v2(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->Super::GetBrush().Tint,
                    this->Super::GetBrush().Image,
                    this->Super::GetBrush().ImageTint,
                    this->Super::GetBrush().ImageScale,
                    this->Super::GetBrush().ImageBehavior,
                    this->Super::GetBrush().ImageOobm,
                    this->Super::GetBrush().ImagePadding
                );
            }
            else
            {
                GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize_v2(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->Super::GetBrush().Tint
                );
            }
        }

        else if (this->Super::GetBrush().Type == ERegionBrush::RoundedBox)
        {
            if (this->Super::GetBrush().Image.IsTextureValid())
            {
                GEngine->GetShaderChecked<LOrthographicRoundedImageBoxShader>(Name_ShaderOrthographicRoundedImageBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize_v2(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->Super::GetBrush().Tint,
                    this->Super::GetBrush().Radii,
                    this->Super::GetBrush().Image,
                    this->Super::GetBrush().ImageTint,
                    this->Super::GetBrush().ImageScale,
                    this->Super::GetBrush().ImageBehavior,
                    this->Super::GetBrush().ImageOobm,
                    this->Super::GetBrush().ImagePadding
                );
            }
            else
            {
                GEngine->GetShaderChecked<LOrthographicRoundedBoxShader>(Name_ShaderOrthographicRoundedBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize_v2(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->Super::GetBrush().Tint,
                    this->Super::GetBrush().Radii
                );
            }
        }

        else if (this->Super::GetBrush().Type == ERegionBrush::OutlineBox)
        {
            if (this->Super::GetBrush().Image.IsTextureValid())
            {
                GEngine->GetShaderChecked<LOrthographicOutlineImageBoxShader>(Name_ShaderOrthographicOutlineImageBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize_v2(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->Super::GetBrush().Tint,
                    this->Super::GetBrush().OutlineThickness,
                    this->Super::GetBrush().OutlineTint,
                    this->Super::GetBrush().Image,
                    this->Super::GetBrush().ImageTint,
                    this->Super::GetBrush().ImageScale,
                    this->Super::GetBrush().ImageBehavior,
                    this->Super::GetBrush().ImageOobm,
                    this->Super::GetBrush().ImagePadding
                );
            }
            else
            {
                GEngine->GetShaderChecked<LOrthographicOutlineBoxShader>(Name_ShaderOrthographicOutlineBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize_v2(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->Super::GetBrush().Tint,
                    this->Super::GetBrush().OutlineThickness,
                    this->Super::GetBrush().OutlineTint
                );
            }
        }

        else if (this->Super::GetBrush().Type == ERegionBrush::RoundedOutlineBox)
        {
            if (this->Super::GetBrush().Image.IsTextureValid())
            {
                GEngine->GetShaderChecked<LOrthographicRoundedOutlineImageBoxShader>(Name_ShaderOrthographicRoundedOutlineImageBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize_v2(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->Super::GetBrush().Tint,
                    this->Super::GetBrush().OutlineThickness,
                    this->Super::GetBrush().OutlineTint,
                    this->Super::GetBrush().Radii,
                    this->Super::GetBrush().Image,
                    this->Super::GetBrush().ImageTint,
                    this->Super::GetBrush().ImageScale,
                    this->Super::GetBrush().ImageBehavior,
                    this->Super::GetBrush().ImageOobm,
                    this->Super::GetBrush().ImagePadding
                );
            }
            else
            {
                GEngine->GetShaderChecked<LOrthographicRoundedOutlineBoxShader>(Name_ShaderOrthographicRoundedOutlineBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize_v2(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->Super::GetBrush().Tint,
                    this->Super::GetBrush().OutlineThickness,
                    this->Super::GetBrush().OutlineTint,
                    this->Super::GetBrush().Radii
                );
            }
        }
    }
    /* END Ourselves. */

    WNode::Draw(Context);

    RendererStateMachine::ClipOrthographic(Context, MostOuterTopLeftContentArea.Copy(), MaxContentAreaSize);
    if (this->bCullNonVisible)
    {
        Context.PushFrameCull(LVector4D{MostOuterTopLeftContentArea.X, MostOuterTopLeftContentArea.Y, this->GetAnchoredSize_v2().X, this->GetAnchoredSize_v2().Y});
    }

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->ShouldNowDraw())
        {
            checkSlow( ChildSlot->Content )

            LVector2D Translation{ -ScrollOffsetX, -ScrollOffsetY };
            if (ChildSlot->Content->GetAnchor().IsPushedHorizontal())
            {
                if (const f64 Diff {ChildSlot->Content->GetAnchoredSize_v2().X - this->GetAnchoredSize_v2().X}; Diff > 0.0)
                {
                    Translation.X += Diff;
                }
            }
            if (ChildSlot->Content->GetAnchor().IsPushedVertical())
            {
                if (const f64 Diff {ChildSlot->Content->GetAnchoredSize_v2().Y - this->GetAnchoredSize_v2().Y}; Diff > 0.0)
                {
                    Translation.Y += Diff;
                }
            }

            Context.ApplyFrameTranslation(Translation);
            ChildSlot->Content->Draw(Context);
            Context.ApplyFrameTranslation(-Translation);
        }
        continue;
    }

    if (this->bCullNonVisible)
    {
        Context.PopFrameCull();
    }
    RendererStateMachine::DisableClipOrthographic();

    if (this->GetAnchoredSize_v2().Y > 0 && this->DrawVScrollbar(VisibleY))
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

    if (this->GetAnchoredSize_v2().X > 0 && this->DrawHScrollbar(VisibleX))
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

Jafg::LCursorReply Jafg::WScrollRegion::SweepMouse(LViewport& Context, const LVector2D& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return WParentBase::SweepMouse(Context, InLocation);
    }

    if (this->IsInBounds(Context, InLocation))
    {
        check( this->ScrollPosition.Y >= 0.0f && this->ScrollPosition.Y <= 1.0f )

        {
            const f64 MaxScrollY = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize_v2().Y), 0.0);
            const f32 ScrollOffsetY = this->ScrollPosition.Y * MaxScrollY;

            const f64 MaxScrollX = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.X) - static_cast<f64>(this->GetAnchoredSize_v2().X), 0.0);
            const f32 ScrollOffsetX = this->ScrollPosition.X * MaxScrollX;

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

Jafg::LReply Jafg::WScrollRegion::SweepFocusTest(const LViewport& Context, const LVector2D& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return WParentBase::SweepFocusTest(Context, InLocation);
    }

    if (this->IsInBounds(Context, InLocation))
    {
        check( this->ScrollPosition.Y >= 0.0f && this->ScrollPosition.Y <= 1.0f )

        {
            const f64 MaxScrollY = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize_v2().Y), 0.0);
            const f32 ScrollOffsetY = this->ScrollPosition.Y * MaxScrollY;

            const f64 MaxScrollX = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.X) - static_cast<f64>(this->GetAnchoredSize_v2().X), 0.0);
            const f32 ScrollOffsetX = this->ScrollPosition.X * MaxScrollX;

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
    check( this->ScrollPosition.Y >= 0.0f && this->ScrollPosition.Y <= 1.0f )

    {
        const f64 MaxScrollY { Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize_v2().Y), 0.0) };
        const f64 ScrollOffsetY { this->ScrollPosition.Y * MaxScrollY };

        const f64 MaxScrollX { Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.X) - static_cast<f64>(this->GetAnchoredSize_v2().X), 0.0) };
        const f64 ScrollOffsetX { this->ScrollPosition.X * MaxScrollX };

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
    check( this->ScrollPosition.Y >= 0.0f && this->ScrollPosition.Y <= 1.0f )

    {
        const f64 MaxScrollY = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize_v2().Y), 0.0);
        const f32 ScrollOffsetY = this->ScrollPosition.Y * MaxScrollY;

        const f64 MaxScrollX = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.X) - static_cast<f64>(this->GetAnchoredSize_v2().X), 0.0);
        const f32 ScrollOffsetX = this->ScrollPosition.X * MaxScrollX;

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
        + (this->bUseChildrenDesiredSize ? this->DesiredSizeOfChildren : LVector2D::ZeroVector)
        );

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
            Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize_v2().Y), 0.0)
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

    const LVector2D CachedCursorLocation = *InViewport.GetCachedCursorLocationChecked();
    const LVector2D TopLeftMostOuter = this->GetAnchoredTopLeftFromMostOuter(InViewport);

    if (LViewport::IsInBounds
    (
        TopLeftMostOuter + this->GetVInteractiveAreaScrollPositionFromOuter(),
        this->GetVInteractiveAreaScrollSize(),
        CachedCursorLocation
    ))
    {
        if
        (
            const LVector2D TopLeftForeground = TopLeftMostOuter + this->GetVForegroundScrollPositionFromOuter();
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
            const LVector2D TopLeftForeground = TopLeftMostOuter + this->GetHForegroundScrollPositionFromOuter();
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

bool Jafg::WScrollRegion::DrawVScrollbar(const f64 InVisibleY) const
{
    if (this->Brush.bAlwaysShowVScrollbar)
    {
        return true;
    }

    if (this->Brush.bAlwaysHideHScrollbar)
    {
        return false;
    }

    return Maths::IsNearlyEqual(InVisibleY, 1.0) == false;
}

bool Jafg::WScrollRegion::DrawHScrollbar(const f64 InVisibleX) const
{
    if (this->Brush.bAlwaysShowHScrollbar)
    {
        return true;
    }

    if (this->Brush.bAlwaysHideVScrollbar)
    {
        return false;
    }

    return Maths::IsNearlyEqual(InVisibleX, 1.0) == false;
}

LVector2D Jafg::WScrollRegion::GetVInteractiveAreaScrollSize() const
{
    return
    {
        Maths::Max(this->Brush.VScrollBarBackgroundWidth, this->Brush.VScrollBarWidth),
        this->GetAnchoredSize_v2().Y - this->Brush.VScrollBarPadding.Y * 2
    };
}

LVector2D Jafg::WScrollRegion::GetVInteractiveAreaScrollPositionFromOuter() const
{
    return
    {
        this->GetAnchoredSize_v2().X - Maths::Max(this->Brush.VScrollBarBackgroundWidth, this->Brush.VScrollBarWidth) - this->Brush.VScrollBarPadding.X,
        this->Brush.VScrollBarPadding.Y
    };
}

LVector2D Jafg::WScrollRegion::GetHInteractiveAreaScrollSize() const
{
    return
    {
        this->GetAnchoredSize_v2().X - this->Brush.HScrollBarPadding.X * 2,
        Maths::Max(this->Brush.HScrollBarBackgroundHeight, this->Brush.HScrollBarHeight)
    };
}

LVector2D Jafg::WScrollRegion::GetHInteractiveAreaScrollPositionFromOuter() const
{
    return
    {
        this->Brush.HScrollBarPadding.X,
        this->GetAnchoredSize_v2().Y - Maths::Max(this->Brush.HScrollBarBackgroundHeight, this->Brush.HScrollBarHeight) - this->Brush.HScrollBarPadding.Y
    };
}

LVector2D Jafg::WScrollRegion::GetVBackgroundScrollSize() const
{
    return
    {
        this->Brush.VScrollBarBackgroundWidth,
        this->GetAnchoredSize_v2().Y - this->Brush.VScrollBarPadding.Y * 2
    };
}

LVector2D Jafg::WScrollRegion::GetVBackgroundScrollPositionFromOuter() const
{
    return
    {
        this->GetAnchoredSize_v2().X - this->Brush.VScrollBarBackgroundWidth - this->Brush.VScrollBarPadding.X,
        this->Brush.VScrollBarPadding.Y
    };
}

LVector2D Jafg::WScrollRegion::GetVForegroundScrollSize() const
{
    return this->GetVForegroundScrollSize(Maths::Clamp(this->GetAnchoredSize_v2().Y / this->DesiredSizeOfChildren.Y, 0.0, 1.0));
}

LVector2D Jafg::WScrollRegion::GetVForegroundScrollSize(const f64 InVisibleY) const
{
    return
    {
        this->Brush.VScrollBarWidth,
        (this->GetAnchoredSize_v2().Y - this->Brush.VScrollBarPadding.Y * 2) * InVisibleY
    };
}

LVector2D Jafg::WScrollRegion::GetVForegroundScrollPositionFromOuter() const
{
    const f64 MaxScroll = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.Y) - static_cast<f64>(this->GetAnchoredSize_v2().Y), 0.0);
    const f64 ScrollOffsetY = this->ScrollPosition.Y * MaxScroll;
    const f64 ScrollOffsetYPercent = ScrollOffsetY / static_cast<f64>(this->DesiredSizeOfChildren.Y);

    return this->GetVForegroundScrollPositionFromOuter(ScrollOffsetYPercent);
}

LVector2D Jafg::WScrollRegion::GetVForegroundScrollPositionFromOuter(const f64 InScrollOffsetYPercent) const
{
    check( InScrollOffsetYPercent >= 0.0 && InScrollOffsetYPercent <= 1.0 )
    return
    {
        this->GetAnchoredSize_v2().X - this->Brush.VScrollBarWidth - this->Brush.VScrollBarPadding.X
        - Maths::Max(this->Brush.VScrollBarBackgroundWidth - this->Brush.VScrollBarWidth, 0.0f) / 2.0,
        this->Brush.VScrollBarPadding.Y + InScrollOffsetYPercent * (this->GetAnchoredSize_v2().Y - this->Brush.VScrollBarPadding.Y * 2)
    };
}

LVector2D Jafg::WScrollRegion::GetHBackgroundScrollSize() const
{
    return
    {
        this->GetAnchoredSize_v2().X - this->Brush.HScrollBarPadding.X * 2,
        this->Brush.HScrollBarBackgroundHeight
    };
}

LVector2D Jafg::WScrollRegion::GetHBackgroundScrollPositionFromOuter() const
{
    return
    {
        this->Brush.HScrollBarPadding.X,
        this->GetAnchoredSize_v2().Y - this->Brush.HScrollBarBackgroundHeight - this->Brush.HScrollBarPadding.Y
    };
}

LVector2D Jafg::WScrollRegion::GetHForegroundScrollSize() const
{
    return this->GetHForegroundScrollSize(Maths::Clamp(this->GetAnchoredSize_v2().X / this->DesiredSizeOfChildren.X, 0.0f, 1.0f));
}

LVector2D Jafg::WScrollRegion::GetHForegroundScrollSize(const f64 InVisibleX) const
{
    return
    {
        (this->GetAnchoredSize_v2().X - this->Brush.HScrollBarPadding.X * 2) * InVisibleX,
        this->Brush.HScrollBarHeight,
    };
}

LVector2D Jafg::WScrollRegion::GetHForegroundScrollPositionFromOuter() const
{
    const f64 MaxScroll = Maths::Max(static_cast<f64>(this->DesiredSizeOfChildren.X) - static_cast<f64>(this->GetAnchoredSize_v2().X), 0.0);
    const f64 ScrollOffsetX = this->ScrollPosition.X * MaxScroll;
    const f64 ScrollOffsetXPercent = ScrollOffsetX / static_cast<f64>(this->DesiredSizeOfChildren.X);

    return this->GetHForegroundScrollPositionFromOuter(ScrollOffsetXPercent);
}

LVector2D Jafg::WScrollRegion::GetHForegroundScrollPositionFromOuter(const f64 InScrollOffsetXPercent) const
{
    check( InScrollOffsetXPercent >= 0.0 && InScrollOffsetXPercent <= 1.0 )
    return
    {
        this->Brush.HScrollBarPadding.X + InScrollOffsetXPercent * (this->GetAnchoredSize_v2().X - this->Brush.HScrollBarPadding.X * 2),
        this->GetAnchoredSize_v2().Y - this->Brush.HScrollBarHeight - this->Brush.HScrollBarPadding.Y
        - Maths::Max(this->Brush.HScrollBarBackgroundHeight - this->Brush.HScrollBarHeight, 0.0f) / 2.0,
    };
}
