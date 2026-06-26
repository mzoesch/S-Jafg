// Copyright mzoesch. All rights reserved.

#include "Nodes/TextButton.h"
#include "Engine/Engine.h"
#include "Framework/Frontend.h"
#include "Rhi/NodeRenderInfo.h"
#include "Framework/TextureSubsystem.h"

void Jafg::WTextButton::OnBrushChanged(EStyleBits Bit) noexcept
{
    TButtonBase::OnBrushChanged(Bit);
    ApplyStyleBit(this->TextStyle, this->TextBrush, Bit);
}

void Jafg::WTextButtonIconizedDouble::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (this->LeftIcon.get() && static_cast<f32>(this->LeftIconBrush.Scale) > 0.0f)
    {
        if (!this->LeftIcon->IsBindless())
        {
            this->GetMutableFrontend().GetSubsystemChecked<JTextureSubsystem>()->AddTextureToGlobalBindlessArray(&*this->LeftIcon);
            check(this->LeftIcon->IsBindless())
        }
        Info.AddInstance({
            .Rect = {maths::round(this->GetLeftIconTopLeft(Info.Translation)),
                     this->LeftIcon->GetExtentAsVec2F() * static_cast<f32>(this->LeftIconBrush.Scale)},
            .Tint = this->LeftIconBrush.Tint,
            .TextureIndex = this->LeftIcon->GetBindlessIndex(),
            });
    }
    if (this->RightIcon.get() && static_cast<f32>(this->RightIconBrush.Scale) > 0.0f)
    {
        if (!this->RightIcon->IsBindless())
        {
            this->GetMutableFrontend().GetSubsystemChecked<JTextureSubsystem>()->AddTextureToGlobalBindlessArray(&*this->RightIcon);
            check(this->RightIcon->IsBindless())
        }
        Info.AddInstance({
            .Rect = {maths::round(this->GetRightIconTopLeft(Info.Translation)),
                     this->RightIcon->GetExtentAsVec2F() * static_cast<f32>(this->RightIconBrush.Scale)},
            .Tint = this->RightIconBrush.Tint,
            .TextureIndex = this->RightIcon->GetBindlessIndex(),
            });
    }
}

void Jafg::WTextButtonIconizedDouble::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    auto GetSize{[this](LTexture2Ref const& Ref, LIconBrush const& Brush)
    {
        if (Ref.get() && static_cast<f32>(Brush.Scale) > 0.0f)
        {
            f32 Width{maths::max(
                static_cast<f32>(Ref->GetExtent().width * Brush.Scale)
                , Brush.MinIconSize.InStaticPoints(this->GetViewport())
                )};
            return LVec2F{Width + Brush.InwardsPadding.InStaticPoints(this->GetViewport()), 0.0f};
        }
        if (Brush.bAlwaysPad)
        {
            return LVec2F{
                Brush.MinIconSize.InStaticPoints(this->GetViewport()) + Brush.InwardsPadding.InStaticPoints(this->GetViewport())
                , 0.0f
                };
        }
        return maths::zero_vector<LVec2F>;
    }};

    {
        LVec2F Size{GetSize(this->LeftIcon, this->LeftIconBrush)};
        this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
        this->TextDrawOffset = Size;
    }
    {
        LVec2F Size{GetSize(this->RightIcon, this->RightIconBrush)};
        this->SetDesiredSizeInSpt(this->GetDesiredSize_v2() + Size);
    }
}

Jafg::LNodeReply Jafg::WTextButtonIconizedDouble::OnCursorMoved(LVec2F const& InLocation)
{
    if (this->bUpdateBrushOnStateChange && this->bEnabled && this->TransformsWidgetLayout())
    {
        if (this->DecoupledLeftKeyEvent.IsValid() && maths::aabb_point({
            // TODO: Fix translation.
            .Offset = this->GetLeftIconTopLeft(maths::zero_vector<LVec2F>),
            .Extent = {
                static_cast<f32>(this->LeftIcon->GetExtent().width * this->LeftIconBrush.Scale),
                static_cast<f32>(this->LeftIcon->GetExtent().height * this->LeftIconBrush.Scale)
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
        if (this->DecoupledRightKeyEvent.IsValid() && maths::aabb_point({
            // TODO: Fix translation.
            .Offset = this->GetRightIconTopLeft(maths::zero_vector<LVec2F>),
            .Extent = {
                static_cast<f32>(this->RightIcon->GetExtent().width * this->RightIconBrush.Scale),
                static_cast<f32>(this->RightIcon->GetExtent().height * this->RightIconBrush.Scale)
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

Jafg::LNodeReply Jafg::WTextButtonIconizedDouble::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (this->bEnabled)
    {
        if (Info.CursorLocation)
        {
            if (this->DecoupledLeftKeyEvent.IsValid() && maths::aabb_point({
                .Offset = this->GetLeftIconTopLeft(Info.Translation),
                .Extent = {
                    static_cast<f32>(this->LeftIcon->GetExtent().width * this->LeftIconBrush.Scale),
                    static_cast<f32>(this->LeftIcon->GetExtent().height * this->LeftIconBrush.Scale)
                    },
                }, *Info.CursorLocation))
            {
                if (auto Reply{this->DecoupledLeftKeyEvent(*this, Info, Event)}; Reply.IsHandled())
                {
                    return Reply;
                }
            }
            else if (this->DecoupledRightKeyEvent.IsValid() && maths::aabb_point({
                .Offset = this->GetRightIconTopLeft(Info.Translation),
                .Extent = {
                    static_cast<f32>(this->RightIcon->GetExtent().width * this->RightIconBrush.Scale),
                    static_cast<f32>(this->RightIcon->GetExtent().height * this->RightIconBrush.Scale)
                    },
                }, *Info.CursorLocation))
            {
                if (auto Reply{this->DecoupledRightKeyEvent(*this, Info, Event)}; Reply.IsHandled())
                {
                    return Reply;
                }
            }
        }

        if (!this->bSelected && this->bUpdateBrushOnStateChange)
        {
            if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)
                , LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
            {
                this->LeftIconBrush = this->LeftIconStyle.PressBrush;
                this->RightIconBrush = this->RightIconStyle.PressBrush;
            }
            if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)
                , LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
            {
                this->LeftIconBrush = this->LeftIconStyle.HoverBrush;
                this->RightIconBrush = this->RightIconStyle.HoverBrush;
            }
        }
    }

    return Super::OnKeyEventFocused(Info, Event);
}

void Jafg::WTextButtonIconizedDouble::OnBrushChanged(EStyleBits Bit) noexcept
{
    Super::OnBrushChanged(Bit);

    ApplyStyleBit(this->LeftIconStyle, this->LeftIconBrush, Bit);
    ApplyStyleBit(this->RightIconStyle, this->RightIconBrush, Bit);
}

LVec2F Jafg::WTextButtonIconizedDouble::GetLeftIconTopLeft(LVec2F Translation) const noexcept
{
    check(this->LeftIcon.get())
    f32 TotalHeight{(this->GetAnchoredSize_v2().y - this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport()).y)};

    f32 Offset{};
    if (this->LeftIconBrush.Scale > 0)
    {
        f32 IconSize{static_cast<f32>(this->LeftIcon->GetExtent().width * this->LeftIconBrush.Scale)};
        f32 Playroom{maths::max(this->LeftIconBrush.MinIconSize.InStaticPoints(this->GetViewport()) - IconSize, 0.0f)};
        switch (this->LeftIconBrush.Alignment)
        {
        case LIconBrush::Align::Left: { break; }
        case LIconBrush::Align::Center: { Offset = maths::floor(Playroom * 0.5f); break; }
        case LIconBrush::Align::Right: { Offset = maths::floor(Playroom); break; }
        }
    }

    return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
        + this->Brush.Padding.GetTopLeftOffset().InStaticPoints(this->GetViewport())
        + LVec2F{Offset, (TotalHeight - static_cast<f32>(this->LeftIcon->GetExtent().height * this->LeftIconBrush.Scale)) * 0.5f};
}

LVec2F Jafg::WTextButtonIconizedDouble::GetRightIconTopLeft(LVec2F Translation) const noexcept
{
    check(this->RightIcon.get())
    f32 TotalHeight{(this->GetAnchoredSize_v2().y - this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport()).y)};

    f32 Offset{};
    if (this->RightIconBrush.Scale > 0)
    {
        f32 IconSize{static_cast<f32>(this->RightIcon->GetExtent().width * this->RightIconBrush.Scale)};
        f32 Playroom{maths::max(this->RightIconBrush.MinIconSize.InStaticPoints(this->GetViewport()) - IconSize, 0.0f)};
        switch (this->RightIconBrush.Alignment)
        {
        case LIconBrush::Align::Left: { break; }
        case LIconBrush::Align::Center: { Offset = maths::floor(Playroom * 0.5f); break; }
        case LIconBrush::Align::Right: { Offset = maths::floor(Playroom); break; }
        }
    }

    return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
        + LVec2F{this->GetAnchoredSize_v2().x, 0.0f}
        - LVec2F{this->Brush.Padding.GetRightOffset().InStaticPoints(this->GetViewport()), this->Brush.Padding.GetTopOffset().InStaticPoints(this->GetViewport())}
        - LVec2F{maths::max(
              static_cast<f32>(this->RightIcon->GetExtent().width * this->RightIconBrush.Scale)
            , this->RightIconBrush.MinIconSize.InStaticPoints(this->GetViewport())
            ), 0.0f}
        + LVec2F{Offset, (TotalHeight - static_cast<f32>(this->RightIcon->GetExtent().height * this->RightIconBrush.Scale)) * 0.5f};
}
