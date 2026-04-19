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
                this->SetTextBrush(this->TextStyle.SelectedBrush);
                this->SetLeftIconBrush(this->LeftIconStyle.SelectedBrush);
                this->SetRightIconBrush(this->RightIconStyle.SelectedBrush);
            }
            else
            {
                this->SetTextBrush(this->TextStyle.NormalBrush);
                this->SetLeftIconBrush(this->LeftIconStyle.NormalBrush);
                this->SetRightIconBrush(this->RightIconStyle.NormalBrush);
            }
        }
        else
        {
            this->SetTextBrush(this->TextStyle.DisabledBrush);
            this->SetLeftIconBrush(this->LeftIconStyle.DisabledBrush);
            this->SetRightIconBrush(this->RightIconStyle.DisabledBrush);
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
            this->GetFrontend().Vk_AddTextureToGlobalBindlessArray(&*this->LeftIcon);
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
            this->GetFrontend().Vk_AddTextureToGlobalBindlessArray(&*this->RightIcon);
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
        this->SetTextBrush(this->TextStyle.HoverBrush);
        this->SetLeftIconBrush(this->LeftIconStyle.HoverBrush);
        this->SetRightIconBrush(this->RightIconStyle.HoverBrush);
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
            this->SetLeftIconBrush(this->DecoupledLeftIconBrush);
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
                                this->SetLeftIconBrush(this->LeftIconStyle.SelectedBrush);
                            }
                            else
                            {
                                this->SetLeftIconBrush(this->LeftIconStyle.NormalBrush);
                            }
                        }
                        else
                        {
                            check(this->IsSelected() == false)
                            this->SetLeftIconBrush(this->LeftIconStyle.DisabledBrush);
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
            this->SetRightIconBrush(this->DecoupledRightIconBrush);
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
                                this->SetRightIconBrush(this->RightIconStyle.SelectedBrush);
                            }
                            else
                            {
                                this->SetRightIconBrush(this->RightIconStyle.NormalBrush);
                            }
                        }
                        else
                        {
                            check(this->IsSelected() == false)
                            this->SetRightIconBrush(this->RightIconStyle.DisabledBrush);
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
        this->SetTextBrush(this->TextStyle.NormalBrush);
        this->SetLeftIconBrush(this->LeftIconStyle.NormalBrush);
        this->SetRightIconBrush(this->RightIconStyle.NormalBrush);
    }
    return this->ButtonBase_OnCursorLeave();
}

Jafg::LReply Jafg::WTextButton::OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event)
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
                this->SetTextBrush(this->TextStyle.PressBrush);
                this->SetLeftIconBrush(this->LeftIconStyle.PressBrush);
                this->SetRightIconBrush(this->RightIconStyle.PressBrush);
            }
        }
    }
    return this->ButtonBase_OnKeyDown(Data, Event);
}

Jafg::LReply Jafg::WTextButton::OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event)
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
                this->SetTextBrush(this->TextStyle.HoverBrush);
                this->SetLeftIconBrush(this->LeftIconStyle.HoverBrush);
                this->SetRightIconBrush(this->RightIconStyle.HoverBrush);
            }
        }
    }
    return this->ButtonBase_OnKeyUp(Data, Event);
}

void Jafg::WTextButton::OnEnabledStateChanged()
{
    SuperButton::OnEnabledStateChanged();
    if (this->bUpdateBrushOnStateChange)
    {
        if (this->bEnabled)
        {
            this->SetTextBrush(this->TextStyle.NormalBrush);
            this->SetLeftIconBrush(this->LeftIconStyle.NormalBrush);
            this->SetRightIconBrush(this->RightIconStyle.NormalBrush);
        }
        else
        {
            this->SetTextBrush(this->TextStyle.DisabledBrush);
            this->SetLeftIconBrush(this->LeftIconStyle.DisabledBrush);
            this->SetRightIconBrush(this->RightIconStyle.DisabledBrush);
        }
    }
    return;
}

void Jafg::WTextButton::OnSelectedStateChanged()
{
    SuperButton::OnSelectedStateChanged();
    if (this->bUpdateBrushOnStateChange)
    {
        if (this->bSelected)
        {
            this->SetTextBrush(this->TextStyle.SelectedBrush);
            this->SetLeftIconBrush(this->LeftIconStyle.SelectedBrush);
            this->SetRightIconBrush(this->RightIconStyle.SelectedBrush);
        }
        else
        {
            this->SetTextBrush(this->TextStyle.NormalBrush);
            this->SetLeftIconBrush(this->LeftIconStyle.NormalBrush);
            this->SetRightIconBrush(this->RightIconStyle.NormalBrush);
        }
    }
    return;
}

LVec2F Jafg::WTextButton::GetLeftIconTopLeft(LVec2F Translation) const noexcept
{
    check(this->LeftIcon.get())
    f32 TotalHeight{(this->GetAnchoredSize_v2().y - this->GetPadding().GetDesiredSizeInSpt(this->GetViewport()).y)};
    return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
        + this->GetPadding().GetTopLeftOffsetInSpt(this->GetViewport())
        + LVec2F{0.0f, (TotalHeight - static_cast<f32>(this->LeftIcon->GetExtent().Height * this->LeftIconBrush.Scale)) * 0.5f};
}

LVec2F Jafg::WTextButton::GetRightIconTopLeft(LVec2F Translation) const noexcept
{
    check(this->RightIcon.get())
    f32 TotalHeight{(this->GetAnchoredSize_v2().y - this->GetPadding().GetDesiredSizeInSpt(this->GetViewport()).y)};
    return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
        + LVec2F{this->GetAnchoredSize_v2().x, 0.0f}
        - LVec2F{this->GetPadding().GetRightOffsetInSpt(this->GetViewport()), this->GetPadding().GetTopOffsetInSpt(this->GetViewport())}
        - LVec2F{static_cast<f32>(this->RightIcon->GetExtent().Width * this->RightIconBrush.Scale), 0.0f}
        + LVec2F{0.0f, (TotalHeight - static_cast<f32>(this->RightIcon->GetExtent().Height * this->RightIconBrush.Scale)) * 0.5f};
}
