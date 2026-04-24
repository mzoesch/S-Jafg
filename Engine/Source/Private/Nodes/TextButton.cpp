// Copyright mzoesch. All rights reserved.

#include "Nodes/TextButton.h"
#include "Engine/Engine.h"
#include "Framework/Frontend.h"
#include "Rhi/NodeRenderInfo.h"

void Jafg::WTextButton::Construct()
{
    Super::Construct();
    if (this->bUpdateBrushOnStateChange)
    {
        if (this->bEnabled)
        {
            if (this->bSelected)
            {
                this->TextBrush = this->TextStyle.SelectedBrush;
                this->LeftIconBrush = this->LeftIconStyle.SelectedBrush;
                this->RightIconBrush = this->RightIconStyle.SelectedBrush;
            }
            else
            {
                this->TextBrush = this->TextStyle.NormalBrush;
                this->LeftIconBrush = this->LeftIconStyle.NormalBrush;
                this->RightIconBrush = this->RightIconStyle.NormalBrush;
            }
        }
        else
        {
            this->TextBrush = this->TextStyle.DisabledBrush;
            this->LeftIconBrush = this->LeftIconStyle.DisabledBrush;
            this->RightIconBrush = this->RightIconStyle.DisabledBrush;
        }
    }
    this->ButtonBase_Construct();
    return;
}

void Jafg::WTextButton::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (this->LeftIcon.get() && this->LeftIconBrush.Scale > 0.0f)
    {
        if (this->LeftIcon->IsBindless() == false)
        {
            this->GetMutableFrontend().Vk_AddTextureToGlobalBindlessArray(&*this->LeftIcon);
            check(this->LeftIcon->IsBindless())
        }
        Info.AddInstance(LVisualInstance{
            .Rect = {maths::round(this->GetLeftIconTopLeft(Info.Translation)),
                     this->LeftIcon->GetExtentAsVec2F() * static_cast<f32>(this->LeftIconBrush.Scale)},
            .Tint = this->LeftIconBrush.Tint.Bits,
            .BackgroundTint = Colors::Black.Bits,
            .Radii = maths::zero_vector<LVec4F>,
            .OutlineTint = Colors::Transparent.Bits,
            .TexCoordRect = {0.0f, 0.0f, 1.0f, 1.0f},
            .OutlineThickness = 0.0f,
            .TextureIndex = this->LeftIcon->GetBindlessIndex(),
            .SamplerIndex = UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx,
            .MsdfPixelRange = 0.0f,
            });
    }
    if (this->RightIcon.get() && this->RightIconBrush.Scale > 0.0f)
    {
        if (this->RightIcon->IsBindless() == false)
        {
            this->GetMutableFrontend().Vk_AddTextureToGlobalBindlessArray(&*this->RightIcon);
            check(this->RightIcon->IsBindless())
        }
        Info.AddInstance(LVisualInstance{
            .Rect = {maths::round(this->GetRightIconTopLeft(Info.Translation)),
                     this->RightIcon->GetExtentAsVec2F() * static_cast<f32>(this->RightIconBrush.Scale)},
            .Tint = this->RightIconBrush.Tint.Bits,
            .BackgroundTint = Colors::Black.Bits,
            .Radii = maths::zero_vector<LVec4F>,
            .OutlineTint = Colors::Transparent.Bits,
            .TexCoordRect = {0.0f, 0.0f, 1.0f, 1.0f},
            .OutlineThickness = 0.0f,
            .TextureIndex = this->RightIcon->GetBindlessIndex(),
            .SamplerIndex = UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx,
            .MsdfPixelRange = 0.0f,
            });
    }

    return;
}

void Jafg::WTextButton::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    if (this->LeftIcon.get() && this->LeftIconBrush.Scale > 0.0f)
    {
        f32 Width{static_cast<f32>(this->LeftIcon->GetExtent().Width * this->LeftIconBrush.Scale)};
        LVec2F Size{Width + InSpt(this->GetViewport(), this->LeftIconBrush.InwardsPadding), 0.0f};
        this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
        this->SetTextDrawOffset(Size);
    }
    if (this->RightIcon.get() && this->RightIconBrush.Scale > 0.0f)
    {
        f32 Width{static_cast<f32>(this->RightIcon->GetExtent().Width * this->RightIconBrush.Scale)};
        LVec2F Size{Width + InSpt(this->GetViewport(), this->RightIconBrush.InwardsPadding), 0.0f};
        this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
    }

    return;
}

Jafg::LCursorReply Jafg::WTextButton::OnCursorEnter()
{
    if (this->bEnabled && this->bSelected == false && this->bUpdateBrushOnStateChange)
    {
        this->TextBrush = this->TextStyle.HoverBrush;
        this->LeftIconBrush = this->LeftIconStyle.HoverBrush;
        this->RightIconBrush = this->RightIconStyle.HoverBrush;
    }
    return this->ButtonBase_OnCursorEnter();
}

Jafg::LCursorReply Jafg::WTextButton::OnCursorMoved(LVec2F const& InLocation)
{
    if (this->bUpdateBrushOnStateChange && this->bEnabled && this->TransformsWidgetLayout())
    {
        if (this->bDecoupledLeftIcon && maths::aabb_point({
            // TODO: Fix translation.
            .Offset = this->GetLeftIconTopLeft(maths::zero_vector<LVec2F>),
            .Extent = {
                static_cast<f32>(this->LeftIcon->GetExtent().Width * this->LeftIconBrush.Scale),
                static_cast<f32>(this->LeftIcon->GetExtent().Height * this->LeftIconBrush.Scale)
                },
            }, InLocation))
        {
            this->LeftIconBrush = this->LeftIconStyle.DecoupledBrush;
            this->DecoupledLeftIconFrame = GEngine->FrameCount;
            if (this->GetViewport().OnLateTick.IsStillBound(this->LeftHandle) == false)
            {
                this->LeftHandle = this->GetViewport().OnLateTick.Emplace([this] -> bool
                {
                    if (this->DecoupledLeftIconFrame != GEngine->FrameCount)
                    {
                        if (this->IsEnabled())
                        {
                            if (this->IsSelected())
                            {
                                this->LeftIconBrush = this->LeftIconStyle.SelectedBrush;
                            }
                            else
                            {
                                this->LeftIconBrush = this->LeftIconStyle.NormalBrush;
                            }
                        }
                        else
                        {
                            check(this->IsSelected() == false)
                            this->LeftIconBrush = this->LeftIconStyle.DisabledBrush;
                        }
                        return true;
                    }
                    return {};
                });
            }
        }
        if (this->bDecoupledRightIcon && maths::aabb_point({
            // TODO: Fix translation.
            .Offset = this->GetRightIconTopLeft(maths::zero_vector<LVec2F>),
            .Extent = {
                static_cast<f32>(this->RightIcon->GetExtent().Width * this->RightIconBrush.Scale),
                static_cast<f32>(this->RightIcon->GetExtent().Height * this->RightIconBrush.Scale)
                },
            }, InLocation))
        {
            this->RightIconBrush = this->RightIconStyle.DecoupledBrush;
            this->DecoupledRightIconFrame = GEngine->FrameCount;
            if (this->GetViewport().OnLateTick.IsStillBound(this->RightHandle) == false)
            {
                this->RightHandle = this->GetViewport().OnLateTick.Emplace([this] -> bool
                {
                    if (this->DecoupledRightIconFrame != GEngine->FrameCount)
                    {
                        if (this->IsEnabled())
                        {
                            if (this->IsSelected())
                            {
                                this->RightIconBrush = this->RightIconStyle.SelectedBrush;
                            }
                            else
                            {
                                this->RightIconBrush = this->RightIconStyle.NormalBrush;
                            }
                        }
                        else
                        {
                            check(this->IsSelected() == false)
                            this->RightIconBrush = this->RightIconStyle.DisabledBrush;
                        }
                        return true;
                    }
                    return {};
                });
            }
        }
    }
    return Super::OnCursorMoved(InLocation);
}

Jafg::LCursorReply Jafg::WTextButton::OnCursorLeave()
{
    if (this->bEnabled && this->bSelected == false && this->bUpdateBrushOnStateChange)
    {
        this->TextBrush = this->TextStyle.NormalBrush;
        this->LeftIconBrush = this->LeftIconStyle.NormalBrush;
        this->RightIconBrush = this->RightIconStyle.NormalBrush;
    }
    return this->ButtonBase_OnCursorLeave();
}

Jafg::LReply Jafg::WTextButton::OnKeyDown(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
{
    if (this->bEnabled)
    {
        if (auto& Surface{this->GetViewport().GetSurface()}; Surface.HasMouseLocation())
        {
            if (this->DecoupledLeftKeyDown && maths::aabb_point({
                .Offset = this->GetLeftIconTopLeft(Data.Translation),
                .Extent = {
                    static_cast<f32>(this->LeftIcon->GetExtent().Width * this->LeftIconBrush.Scale),
                    static_cast<f32>(this->LeftIcon->GetExtent().Height * this->LeftIconBrush.Scale)
                    },
                }, Surface.GetMouseLocationValue()))
            {
                if (auto Reply{this->DecoupledLeftKeyDown(Data, Event)}; Reply.IsHandled())
                {
                    return Reply;
                }
            }
            else if (this->DecoupledRightKeyDown && maths::aabb_point({
                .Offset = this->GetRightIconTopLeft(Data.Translation),
                .Extent = {
                    static_cast<f32>(this->RightIcon->GetExtent().Width * this->RightIconBrush.Scale),
                    static_cast<f32>(this->RightIcon->GetExtent().Height * this->RightIconBrush.Scale)
                    },
                }, Surface.GetMouseLocationValue()))
            {
                if (auto Reply{this->DecoupledRightKeyDown(Data, Event)}; Reply.IsHandled())
                {
                    return Reply;
                }
            }
        }

        if (this->bSelected == false && this->bUpdateBrushOnStateChange)
        {
            if (   Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)
                || Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
            {
                this->TextBrush = this->TextStyle.PressBrush;
                this->LeftIconBrush = this->LeftIconStyle.PressBrush;
                this->RightIconBrush = this->RightIconStyle.PressBrush;
            }
        }
    }
    return this->ButtonBase_OnKeyDown(Data, Event);
}

Jafg::LReply Jafg::WTextButton::OnKeyUp(LNodeKeyEventInfo const& Data, LKeyEvent const& Event)
{
    if (this->bEnabled)
    {
        if (auto& Surface{this->GetViewport().GetSurface()}; Surface.HasMouseLocation())
        {
            if (this->DecoupledLeftKeyUp && maths::aabb_point({
                .Offset = this->GetLeftIconTopLeft(Data.Translation),
                .Extent = {
                    static_cast<f32>(this->LeftIcon->GetExtent().Width * this->LeftIconBrush.Scale),
                    static_cast<f32>(this->LeftIcon->GetExtent().Height * this->LeftIconBrush.Scale)
                    },
                }, Surface.GetMouseLocationValue()))
            {
                if (auto Reply{this->DecoupledLeftKeyUp(Data, Event)}; Reply.IsHandled())
                {
                    return Reply;
                }
            }
            else if (this->DecoupledRightKeyUp && maths::aabb_point({
                .Offset = this->GetRightIconTopLeft(Data.Translation),
                .Extent = {
                    static_cast<f32>(this->RightIcon->GetExtent().Width * this->RightIconBrush.Scale),
                    static_cast<f32>(this->RightIcon->GetExtent().Height * this->RightIconBrush.Scale)
                    },
                }, Surface.GetMouseLocationValue()))
            {
                if (auto Reply{this->DecoupledRightKeyUp(Data, Event)}; Reply.IsHandled())
                {
                    return Reply;
                }
            }
        }

        if (this->bSelected == false && this->bUpdateBrushOnStateChange)
        {
            if (   Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)
                || Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
            {
                this->TextBrush = this->TextStyle.HoverBrush;
                this->LeftIconBrush = this->LeftIconStyle.HoverBrush;
                this->RightIconBrush = this->RightIconStyle.HoverBrush;
            }
        }
    }
    return this->ButtonBase_OnKeyUp(Data, Event);
}

void Jafg::WTextButton::OnEnabledStateChanged()
{
    TButtonBase::OnEnabledStateChanged();
    if (this->bUpdateBrushOnStateChange)
    {
        if (this->bEnabled)
        {
            this->TextBrush = this->TextStyle.NormalBrush;
            this->LeftIconBrush = this->LeftIconStyle.NormalBrush;
            this->RightIconBrush = this->RightIconStyle.NormalBrush;
        }
        else
        {
            this->TextBrush = this->TextStyle.DisabledBrush;
            this->LeftIconBrush = this->LeftIconStyle.DisabledBrush;
            this->RightIconBrush = this->RightIconStyle.DisabledBrush;
        }
    }
    return;
}

void Jafg::WTextButton::OnSelectedStateChanged()
{
    TButtonBase::OnSelectedStateChanged();
    if (this->bUpdateBrushOnStateChange)
    {
        if (this->bSelected)
        {
            this->TextBrush = this->TextStyle.SelectedBrush;
            this->LeftIconBrush = this->LeftIconStyle.SelectedBrush;
            this->RightIconBrush = this->RightIconStyle.SelectedBrush;
        }
        else
        {
            this->TextBrush = this->TextStyle.NormalBrush;
            this->LeftIconBrush = this->LeftIconStyle.NormalBrush;
            this->RightIconBrush = this->RightIconStyle.NormalBrush;
        }
    }
    return;
}

LVec2F Jafg::WTextButton::GetLeftIconTopLeft(LVec2F Translation) const noexcept
{
    check(this->LeftIcon.get())
    f32 TotalHeight{(this->GetAnchoredSize_v2().y - this->Brush.Padding.GetDesiredSizeInSpt(this->GetViewport()).y)};
    return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
        + this->Brush.Padding.GetTopLeftOffsetInSpt(this->GetViewport())
        + LVec2F{0.0f, (TotalHeight - static_cast<f32>(this->LeftIcon->GetExtent().Height * this->LeftIconBrush.Scale)) * 0.5f};
}

LVec2F Jafg::WTextButton::GetRightIconTopLeft(LVec2F Translation) const noexcept
{
    check(this->RightIcon.get())
    f32 TotalHeight{(this->GetAnchoredSize_v2().y - this->Brush.Padding.GetDesiredSizeInSpt(this->GetViewport()).y)};
    return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
        + LVec2F{this->GetAnchoredSize_v2().x, 0.0f}
        - LVec2F{this->Brush.Padding.GetRightOffsetInSpt(this->GetViewport()), this->Brush.Padding.GetTopOffsetInSpt(this->GetViewport())}
        - LVec2F{static_cast<f32>(this->RightIcon->GetExtent().Width * this->RightIconBrush.Scale), 0.0f}
        + LVec2F{0.0f, (TotalHeight - static_cast<f32>(this->RightIcon->GetExtent().Height * this->RightIconBrush.Scale)) * 0.5f};
}
