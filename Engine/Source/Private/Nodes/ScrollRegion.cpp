// Copyright mzoesch. All rights reserved.

#include "Nodes/ScrollRegion.h"
#include "Engine/Engine.h"
#include "Rhi/NodeRenderInfo.h"
#include "User/UserPreferences.h"

void Jafg::WScrollRegion::Draw(LNodeRenderInfo const& Info) const
{
    WNode::Draw(Info);
    this->DrawRegionBrush(Info);

    check(this->ScrollPosition.x >= 0.0f && this->ScrollPosition.x <= 1.0f)
    check(this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f)

    const LVec2F AnchoredTopLeftFromMostOuter{this->GetAnchoredTopLeftFromMostOuter(Info.Viewport)};
    const LVec2F MostOuterTopLeftContentArea {AnchoredTopLeftFromMostOuter + this->GetPadding().GetTopLeftOffsetInSpt(Info.Viewport)};
    const LVec2F MaxContentAreaSize {this->GetAnchoredSize_v2() - this->GetPadding().GetDesiredSizeInSpt(Info.Viewport)};

    const f64 MaxScrollY{maths::max(static_cast<f64>(this->DesiredSizeOfChildren.y) - static_cast<f64>(this->GetAnchoredSize_v2().y), 0.0)};
    const f64 ScrollOffsetY{ this->ScrollPosition.y * MaxScrollY};
    const f64 ScrollOffsetYPercent{ ScrollOffsetY / static_cast<f64>(this->DesiredSizeOfChildren.y)};
    const f64 VisibleY{maths::clamp(this->GetAnchoredSize_v2().y / this->DesiredSizeOfChildren.y, 0.0f, 1.0f)};

    const f64 MaxScrollX{maths::max(static_cast<f64>(this->DesiredSizeOfChildren.x) - static_cast<f64>(this->GetAnchoredSize_v2().x), 0.0)};
    const f64 ScrollOffsetX{ this->ScrollPosition.x * MaxScrollX};
    const f64 ScrollOffsetXPercent{ ScrollOffsetX / static_cast<f64>(this->DesiredSizeOfChildren.x)};
    const f64 VisibleX{maths::clamp(this->GetAnchoredSize_v2().x / this->DesiredSizeOfChildren.x, 0.0f, 1.0f)};

    this->LastVisible = {VisibleX, VisibleY};

    // Info.CommandBuffer.setScissor(0, vk::Rect2D{
    //     .offset = vk::Offset2D{static_cast<i32>(MostOuterTopLeftContentArea.x), static_cast<i32>(MostOuterTopLeftContentArea.y)},
    //     .extent = vk::Extent2D{static_cast<u32>(MaxContentAreaSize.x), static_cast<u32>(MaxContentAreaSize.y)},
    //     });

    if (this->bCullNonVisible)
    {
        // ?
    }

    if (this->GetChildren().empty() == false)
    {
        vk::Rect2D Rect{Info.Batches[Info.Batches.size() - 1].first};
        u64 Instances{Info.VisualInstances.size()};
        Info.BeginNewBatch(vk::Rect2D{
            .offset = vk::Offset2D{
                static_cast<decltype(vk::Offset2D::x)>(AnchoredTopLeftFromMostOuter.x),
                static_cast<decltype(vk::Offset2D::y)>(AnchoredTopLeftFromMostOuter.y)
                },
            .extent = vk::Extent2D{
                static_cast<decltype(vk::Extent2D::width)>(this->GetAnchoredSize_v2().x),
                static_cast<decltype(vk::Extent2D::height)>(this->GetAnchoredSize_v2().y)
                },
            });
        for (auto& Child : this->GetChildren())
        {
            if (Child->ShouldNowDraw())
            {
                check(Child.get())
                LVec2D Translation{ -ScrollOffsetX, -ScrollOffsetY };
                if (Child->GetAnchor().IsPushedHorizontal())
                {
                    if (f32 Diff{Child->GetAnchoredSize_v2().x - this->GetAnchoredSize_v2().x}; Diff > 0.0f)
                    {
                        Translation.x += Diff;
                    }
                }
                if (Child->GetAnchor().IsPushedVertical())
                {
                    if (f32 Diff{Child->GetAnchoredSize_v2().y - this->GetAnchoredSize_v2().y}; Diff > 0.0f)
                    {
                        Translation.y += Diff;
                    }
                }
                Child->Draw(LNodeRenderInfo{static_cast<LRenderInfo const&>(Info), Info.Viewport,
                    Info.TextureSubsystem,
                    Info.MaterialSubsystem,
                    Info.FontSubsystem,
                    Translation, LRect2D{
                        .Offset = AnchoredTopLeftFromMostOuter,
                        .Extent = this->GetAnchoredSize_v2(),
                        },
                    Info.Batches, Info.VisualInstances,
                    });
            }
            continue;
        }
        if (Instances == Info.VisualInstances.size())
        {
            Info.Batches.pop_back();
        }
        else
        {
            Info.BeginNewBatch(Rect);
        }
    }

    if (this->GetAnchoredSize_v2().y > 0.0f && this->ShouldDrawVScrollbar(VisibleY))
    {
        if (this->BarBrush.VScrollBarBackgroundWidth > 0.0f)
        {
            Info.AddInstance(LVisualInstance{
                .Rect = {AnchoredTopLeftFromMostOuter + this->GetVBackgroundScrollPositionFromOuter(), this->GetVBackgroundScrollSize()},
                .Tint = this->BarBrush.VBackgroundTint.Bits,
                .BackgroundTint = Colors::Black.Bits,
                .Radii = maths::zero_vector<LVec4F>,
                .OutlineTint = Colors::Black.Bits,
                .TexCoordRect = maths::zero_vector<LVec4F>,
                .OutlineThickness = 0.0f,
                .TextureIndex = UBO::BindlessTextureArray::IdentityMulIdx,
                .SamplerIndex = UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx,
                .MsdfPixelRange = 0.0f,
                });
        }
        if (this->BarBrush.VScrollBarWidth > 0.0f)
        {
            Info.AddInstance(LVisualInstance{
                .Rect = {AnchoredTopLeftFromMostOuter + this->GetVForegroundScrollPositionFromOuter(ScrollOffsetYPercent), this->GetVForegroundScrollSize()},
                .Tint = this->BarBrush.VTint.Bits,
                .BackgroundTint = Colors::Black.Bits,
                .Radii = maths::zero_vector<LVec4F>,
                .OutlineTint = Colors::Black.Bits,
                .TexCoordRect = maths::zero_vector<LVec4F>,
                .OutlineThickness = 0.0f,
                .TextureIndex = UBO::BindlessTextureArray::IdentityMulIdx,
                .SamplerIndex = UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx,
                .MsdfPixelRange = 0.0f,
                });
        }
    }

    if (this->GetAnchoredSize_v2().x > 0.0f && this->ShouldDrawHScrollbar(VisibleX))
    {
        if (this->BarBrush.HScrollBarBackgroundHeight > 0.0f)
        {
            Info.AddInstance(LVisualInstance{
                .Rect = {AnchoredTopLeftFromMostOuter + this->GetHBackgroundScrollPositionFromOuter(), this->GetHBackgroundScrollSize()},
                .Tint = this->BarBrush.HBackgroundTint.Bits,
                .BackgroundTint = Colors::Black.Bits,
                .Radii = maths::zero_vector<LVec4F>,
                .OutlineTint = Colors::Black.Bits,
                .TexCoordRect = maths::zero_vector<LVec4F>,
                .OutlineThickness = 0.0f,
                .TextureIndex = UBO::BindlessTextureArray::IdentityMulIdx,
                .SamplerIndex = UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx,
                .MsdfPixelRange = 0.0f,
                });
        }
        if (this->BarBrush.HScrollBarHeight > 0.0f)
        {
            Info.AddInstance(LVisualInstance{
                .Rect = {AnchoredTopLeftFromMostOuter + this->GetHForegroundScrollPositionFromOuter(ScrollOffsetXPercent), this->GetHForegroundScrollSize()},
                .Tint = this->BarBrush.HTint.Bits,
                .BackgroundTint = Colors::Black.Bits,
                .Radii = maths::zero_vector<LVec4F>,
                .OutlineTint = Colors::Black.Bits,
                .TexCoordRect = maths::zero_vector<LVec4F>,
                .OutlineThickness = 0.0f,
                .TextureIndex = UBO::BindlessTextureArray::IdentityMulIdx,
                .SamplerIndex = UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx,
                .MsdfPixelRange = 0.0f,
                });
        }
    }

    return;
}

Jafg::LCursorReply Jafg::WScrollRegion::SweepMouse(LNodeSweepData const& Data, LVec2F const& Location)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return WNode::SweepMouse(Data, Location);
    }

    if (this->IsInBounds(Data, Location))
    {
        check(this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f)
        const f32 maxScrollY = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f);
        const f32 ScrollOffsetY = this->ScrollPosition.y * maxScrollY;
        const f32 maxScrollX = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f);
        const f32 ScrollOffsetX = this->ScrollPosition.x * maxScrollX;

        for (auto& Child : this->GetChildren())
        {
            if (Child->ShouldCheckForInputs())
            {
                if (LCursorReply Reply = Child->SweepMouse(
                      {.Translation={Data.Translation.x + (-ScrollOffsetX), Data.Translation.y + (-ScrollOffsetY)}}
                    , Location); Reply.IsHandled())
                {
                    return Reply;
                }
            }
            continue;
        }
    }
    return WNode::SweepMouse(Data, Location);
}

Jafg::LReply Jafg::WScrollRegion::SweepFocusTest(LNodeSweepData const& Data, LVec2F const& Location)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return WNode::SweepFocusTest(Data, Location);
    }

    if (this->IsInBounds(Data, Location))
    {
        check(this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f)
        const f32 maxScrollY = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f);
        const f32 ScrollOffsetY = this->ScrollPosition.y * maxScrollY;
        const f32 maxScrollX = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f);
        const f32 ScrollOffsetX = this->ScrollPosition.x * maxScrollX;

        for (auto& Child : this->GetChildren())
        {
            if (Child->ShouldCheckForInputs())
            {
                if (const LReply Reply = Child->SweepFocusTest(
                      {.Translation={Data.Translation.x + (-ScrollOffsetX), Data.Translation.y + (-ScrollOffsetY)}}
                    , Location); Reply.IsHandled())
                {
                    return Reply;
                }
            }
            continue;
        }
    }
    return WNode::SweepFocusTest(Data, Location);
}

bool Jafg::WScrollRegion::UserInterfaceTick()
{
    if (this->GetViewport().GetSurface().GetMouseLocation().has_value() == false)
    {
        return {};
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
                - (maths::eq_zero_e(this->MbOffset.y) ? (VForegroundScrollHeight * 0.5f) : 0.0f) + this->MbOffset.y
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
                - (maths::eq_zero_e(this->MbOffset.x) ? (HForegroundScrollWidth * 0.5f) : 0.0f) + this->MbOffset.x
            )
            /
            (this->GetHBackgroundScrollSize().x - HForegroundScrollWidth),
            0.0f,
            1.0f
        );
    }

    return {};
}

Jafg::LReply Jafg::WScrollRegion::OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event)
{
    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseWheelUp))
    {
        this->HandleMouseWheelUp(Event.Value);
        return LReply::Handled();
    }
    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseWheelDown))
    {
        this->HandleMouseWheelDown(Event.Value);
        return LReply::Handled();
    }

    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
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
    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
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
            if (Child->IsInBounds({.Translation={-ScrollOffsetX, -ScrollOffsetY}}, Data.Surface.GetMouseLocationValue()) == false)
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

    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseWheelUp))
    {
        this->HandleMouseWheelUp(Event.Value);
        return LReply::Handled();
    }
    if (Event.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseWheelDown))
    {
        this->HandleMouseWheelDown(Event.Value);
        return LReply::Handled();
    }

    /* Not super. */
    return WNode::OnKeyDownNoFocus(Data, Event);
}

Jafg::LReply Jafg::WScrollRegion::OnKeyUpNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& InKeyEvent)
{
    check(this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f)

    {
        const f32 maxScrollY = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f);
        const f32 ScrollOffsetY = this->ScrollPosition.y * maxScrollY;

        const f32 maxScrollX = maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f);
        const f32 ScrollOffsetX = this->ScrollPosition.x * maxScrollX;

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
            if (Child->IsInBounds({.Translation={-ScrollOffsetX, -ScrollOffsetY}}, Data.Surface.GetMouseLocationValue()) == false)
            {
                continue;
            }
            if (LReply Reply{Child->OnKeyUpNoFocus(Data, InKeyEvent)}; Reply.IsHandled())
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
    check(this->UserInterfaceTickDelegateHandle.IsValid() == false)

    if (InViewport.GetSurface().GetMouseLocation().has_value() == false)
    {
        return false;
    }

    auto MouseLocation{InViewport.GetSurface().GetMouseLocationValue()};
    LVec2F TopLeftMostOuter{this->GetAnchoredTopLeftFromMostOuter(InViewport)};

    if (this->LastVisible.y < 1.0f && LViewport::IsInBounds(
        TopLeftMostOuter + this->GetVInteractiveAreaScrollPositionFromOuter(),
        this->GetVInteractiveAreaScrollSize(),
        MouseLocation
        ))
    {
        if (LVec2D TopLeftForeground{TopLeftMostOuter + this->GetVForegroundScrollPositionFromOuter()};
            LViewport::IsInBounds(TopLeftForeground, this->GetVForegroundScrollSize(), MouseLocation))
        {
            this->MbOffset.y = (MouseLocation.y - TopLeftForeground.y) * -1.0f;
        }
        this->bUiTickV = true;
        this->UserInterfaceTickDelegateHandle = InViewport.OnLateTick.Emplace(this, &WScrollRegion::UserInterfaceTick);
        return true;
    }

    if (this->LastVisible.x < 1.0f && LViewport::IsInBounds(
        TopLeftMostOuter + this->GetHInteractiveAreaScrollPositionFromOuter(),
        this->GetHInteractiveAreaScrollSize(),
        MouseLocation
        ))
    {
        if (LVec2D TopLeftForeground{TopLeftMostOuter + this->GetHForegroundScrollPositionFromOuter()};
            LViewport::IsInBounds(TopLeftForeground, this->GetHForegroundScrollSize(), MouseLocation))
        {
            this->MbOffset.x = (MouseLocation.x - TopLeftForeground.x) * -1.0f;
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
        this->MbOffset = maths::zero_vector<LVec2F>;
        InViewport.OnLateTick.Remove(&this->UserInterfaceTickDelegateHandle);
        return true;
    }

    return false;
}

LVec2F Jafg::WScrollRegion::GetVInteractiveAreaScrollSize() const
{
    return {
        maths::max(this->BarBrush.VScrollBarBackgroundWidth, this->BarBrush.VScrollBarWidth),
        this->GetAnchoredSize_v2().y - this->BarBrush.VScrollBarPadding.y * 2
        };
}

LVec2F Jafg::WScrollRegion::GetVInteractiveAreaScrollPositionFromOuter() const
{
    return {
        this->GetAnchoredSize_v2().x - maths::max(this->BarBrush.VScrollBarBackgroundWidth, this->BarBrush.VScrollBarWidth) - this->BarBrush.VScrollBarPadding.x,
        this->BarBrush.VScrollBarPadding.y
        };
}

LVec2F Jafg::WScrollRegion::GetHInteractiveAreaScrollSize() const
{
    return {
        this->GetAnchoredSize_v2().x - this->BarBrush.HScrollBarPadding.x * 2,
        maths::max(this->BarBrush.HScrollBarBackgroundHeight, this->BarBrush.HScrollBarHeight)
        };
}

LVec2F Jafg::WScrollRegion::GetHInteractiveAreaScrollPositionFromOuter() const
{
    return {
        this->BarBrush.HScrollBarPadding.x,
        this->GetAnchoredSize_v2().y - maths::max(this->BarBrush.HScrollBarBackgroundHeight, this->BarBrush.HScrollBarHeight) - this->BarBrush.HScrollBarPadding.y
        };
}

LVec2F Jafg::WScrollRegion::GetVBackgroundScrollSize() const
{
    return {
        this->BarBrush.VScrollBarBackgroundWidth,
        this->GetAnchoredSize_v2().y - this->BarBrush.VScrollBarPadding.y * 2
        };
}

LVec2F Jafg::WScrollRegion::GetVBackgroundScrollPositionFromOuter() const
{
    return {
        this->GetAnchoredSize_v2().x - this->BarBrush.VScrollBarBackgroundWidth - this->BarBrush.VScrollBarPadding.x,
        this->BarBrush.VScrollBarPadding.y
        };
}

LVec2F Jafg::WScrollRegion::GetVForegroundScrollSize() const
{
    return this->GetVForegroundScrollSize(maths::clamp(this->GetAnchoredSize_v2().y / this->DesiredSizeOfChildren.y, 0.0f, 1.0f));
}

LVec2F Jafg::WScrollRegion::GetVForegroundScrollSize(f32 InVisibleY) const
{
    return {
        this->BarBrush.VScrollBarWidth,
        (this->GetAnchoredSize_v2().y - this->BarBrush.VScrollBarPadding.y * 2) * InVisibleY
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
    check(InScrollOffsetYPercent >= 0.0 && InScrollOffsetYPercent <= 1.0)
    return {
        this->GetAnchoredSize_v2().x - this->BarBrush.VScrollBarWidth - this->BarBrush.VScrollBarPadding.x
        - maths::max(this->BarBrush.VScrollBarBackgroundWidth - this->BarBrush.VScrollBarWidth, 0.0f) / 2.0,
        this->BarBrush.VScrollBarPadding.y + InScrollOffsetYPercent * (this->GetAnchoredSize_v2().y - this->BarBrush.VScrollBarPadding.y * 2)
        };
}

LVec2F Jafg::WScrollRegion::GetHBackgroundScrollSize() const
{
    return {
        this->GetAnchoredSize_v2().x - this->BarBrush.HScrollBarPadding.x * 2,
        this->BarBrush.HScrollBarBackgroundHeight
        };
}

LVec2F Jafg::WScrollRegion::GetHBackgroundScrollPositionFromOuter() const
{
    return {
        this->BarBrush.HScrollBarPadding.x,
        this->GetAnchoredSize_v2().y - this->BarBrush.HScrollBarBackgroundHeight - this->BarBrush.HScrollBarPadding.y
        };
}

LVec2F Jafg::WScrollRegion::GetHForegroundScrollSize() const
{
    return this->GetHForegroundScrollSize(maths::clamp(this->GetAnchoredSize_v2().x / this->DesiredSizeOfChildren.x, 0.0f, 1.0f));
}

LVec2F Jafg::WScrollRegion::GetHForegroundScrollSize(const f32 InVisibleX) const
{
    return {
        (this->GetAnchoredSize_v2().x - this->BarBrush.HScrollBarPadding.x * 2) * InVisibleX,
        this->BarBrush.HScrollBarHeight,
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
    check(InScrollOffsetXPercent >= 0.0 && InScrollOffsetXPercent <= 1.0)
    return {
        this->BarBrush.HScrollBarPadding.x + InScrollOffsetXPercent * (this->GetAnchoredSize_v2().x - this->BarBrush.HScrollBarPadding.x * 2),
        this->GetAnchoredSize_v2().y - this->BarBrush.HScrollBarHeight - this->BarBrush.HScrollBarPadding.y
        - maths::max(this->BarBrush.HScrollBarBackgroundHeight - this->BarBrush.HScrollBarHeight, 0.0f) / 2.0,
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
}

void Jafg::WScrollRegion::HandleMouseWheelDown(f32 Value)
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
}
