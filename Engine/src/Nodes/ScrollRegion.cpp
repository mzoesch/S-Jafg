// Copyright mzoesch. All rights reserved.

#include "Nodes/ScrollRegion.h"
#include "Engine/Engine.h"
#include "Rhi/NodeRenderInfo.h"
#include "User/UserPreferences.h"

void Jafg::WScrollRegion::Draw(LNodeRenderInfo const& Info) const
{
    WNode::Draw(Info);
    this->Brush.Draw(Info, {
        .offset = this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
        .extent = this->GetAnchoredSize_v2(),
        });

    check(this->ScrollPosition.x >= 0.0f && this->ScrollPosition.x <= 1.0f)
    check(this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f)

    const LVec2F AnchoredTopLeftFromMostOuter{this->GetAnchoredTopLeftFromMostOuter()};
    // const LVec2F MostOuterTopLeftContentArea {AnchoredTopLeftFromMostOuter + this->Padding.GetTopLeftOffset().InStaticPoints(Info.Viewport)};
    // const LVec2F MaxContentAreaSize {this->GetAnchoredSize_v2() - this->Padding.GetDesiredSize().InStaticPoints(Info.Viewport)};

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
        u64 Instances{Info.VisualInstances->size()};
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
                if (Child->Anchor.IsPushedHorizontal())
                {
                    if (f32 Diff{Child->GetAnchoredSize_v2().x - this->GetAnchoredSize_v2().x}; Diff > 0.0f)
                    {
                        Translation.x += Diff;
                    }
                }
                if (Child->Anchor.IsPushedVertical())
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
                    Translation, LRect2F{
                        .offset = AnchoredTopLeftFromMostOuter,
                        .extent = this->GetAnchoredSize_v2(),
                        },
                    Info.Batches, Info.VisualInstances,
                    });
            }
            continue;
        }
        if (Instances == Info.VisualInstances->size())
        {
            Info.Batches.pop_back();
        }
        else
        {
            Info.BeginNewBatch(Rect);
        }
    }

    if (this->GetAnchoredSize_v2().y > 0.0f && this->ShouldDrawVScrollbar(static_cast<f32>(VisibleY)))
    {
        if (this->BarBrush.VScrollBarBackgroundWidth > 0.0f)
        {
            Info.AddInstance({
                .Rect = {AnchoredTopLeftFromMostOuter + this->GetVBackgroundScrollPositionFromOuter(), this->GetVBackgroundScrollSize()},
                .Tint = this->BarBrush.VBackgroundTint,
                });
        }
        if (this->BarBrush.VScrollBarWidth > 0.0f)
        {
            Info.AddInstance({
                .Rect = {AnchoredTopLeftFromMostOuter + this->GetVForegroundScrollPositionFromOuter(static_cast<f32>(ScrollOffsetYPercent)), this->GetVForegroundScrollSize()},
                .Tint = this->BarBrush.VTint,
                });
        }
    }

    if (this->GetAnchoredSize_v2().x > 0.0f && this->ShouldDrawHScrollbar(static_cast<f32>(VisibleX)))
    {
        if (this->BarBrush.HScrollBarBackgroundHeight > 0.0f)
        {
            Info.AddInstance({
                .Rect = {AnchoredTopLeftFromMostOuter + this->GetHBackgroundScrollPositionFromOuter(), this->GetHBackgroundScrollSize()},
                .Tint = this->BarBrush.HBackgroundTint,
                });
        }
        if (this->BarBrush.HScrollBarHeight > 0.0f)
        {
            Info.AddInstance({
                .Rect = {AnchoredTopLeftFromMostOuter + this->GetHForegroundScrollPositionFromOuter(static_cast<f32>(ScrollOffsetXPercent)), this->GetHForegroundScrollSize()},
                .Tint = this->BarBrush.HTint,
                });
        }
    }

    return;
}

Jafg::LNodeReply Jafg::WScrollRegion::SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location)
{
    if (this->CanChildrenBeHitTestable() && this->AabbTest(Info, Location))
    {
        check(this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f)
        const f32 maxScrollY{maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f)};
        const f32 ScrollOffsetY{this->ScrollPosition.y * maxScrollY};
        const f32 maxScrollX{maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f)};
        const f32 ScrollOffsetX{this->ScrollPosition.x * maxScrollX};
        return Super::SweepFocus({
            .Translation=Info.Translation,
            .ChildTranslationHint=LVec2F{-ScrollOffsetX, -ScrollOffsetY} + Info.ChildTranslationHint,
            }, Location);
    }
    return Super::SweepFocus(Info, Location);
}

Jafg::LNodeReply Jafg::WScrollRegion::Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location)
{
    if (Location.has_value() && this->CanChildrenBeHitTestable() && this->AabbTest(Info, *Location))
    {
        check(this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f)
        const f32 maxScrollY{maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f)};
        const f32 ScrollOffsetY{this->ScrollPosition.y * maxScrollY};
        const f32 maxScrollX{maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f)};
        const f32 ScrollOffsetX{this->ScrollPosition.x * maxScrollX};
        return Super::Sweep({
            .Translation=Info.Translation,
            .ChildTranslationHint=LVec2F{-ScrollOffsetX, -ScrollOffsetY} + Info.ChildTranslationHint,
            }, Location);
    }
    return Super::Sweep(Info, Location);
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
                - (this->GetAnchoredTopLeftFromMostOuter() + this->GetVBackgroundScrollPositionFromOuter()).y
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
                - (this->GetAnchoredTopLeftFromMostOuter() + this->GetHBackgroundScrollPositionFromOuter()).x
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

Jafg::LNodeReply Jafg::WScrollRegion::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (Info.CursorLocation.has_value())
    {
        if (this->AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
        {
            if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::MouseWheelUp)))
            {
                this->HandleMouseWheelUp(Event.Value);
                return LNodeReply::Handled();
            }
            if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::MouseWheelDown)))
            {
                this->HandleMouseWheelDown(Event.Value);
                return LNodeReply::Handled();
            }
            if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
            {
                if (this->MBDownOnScrollbar(Info.CursorLocation))
                {
                    return LNodeReply::Handled();
                }
            }
        }
    }

    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
    {
        if (this->MBUpOnScrollbar())
        {
            return LNodeReply::Handled();
        }
    }

    return Super::OnKeyEventFocused(Info, Event);
}

Jafg::LNodeReply Jafg::WScrollRegion::OnKeyEventUnfocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    check(this->ScrollPosition.y >= 0.0f && this->ScrollPosition.y <= 1.0f)

    const f32 maxScrollY{maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f)};
    const f32 ScrollOffsetY{this->ScrollPosition.y * maxScrollY};
    const f32 maxScrollX{maths::max(static_cast<f32>(this->DesiredSizeOfChildren.x) - static_cast<f32>(this->GetAnchoredSize_v2().x), 0.0f)};
    const f32 ScrollOffsetX{this->ScrollPosition.x * maxScrollX};
    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        if (&*Child == Info.Viewport.GetFocusedWidget())
        {
            continue;
        }
        if (!Child->ShouldCheckForInputs())
        {
            continue;
        }
        if (!Child->AabbTest({.Translation={-ScrollOffsetX, -ScrollOffsetY}}, *Info.CursorLocation))
        {
            continue;
        }

        if (auto Reply{Child->OnKeyEventUnfocused(Info, Event)}; Reply.IsHandled())
        {
            return Reply;
        }
        continue;
    }

    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::MouseWheelUp)))
    {
        this->HandleMouseWheelUp(Event.Value);
        return LNodeReply::Handled();
    }
    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::MouseWheelDown)))
    {
        this->HandleMouseWheelDown(Event.Value);
        return LNodeReply::Handled();
    }

    /* Not super. */
    return WNode::OnKeyEventUnfocused(Info, Event);
}

void Jafg::WScrollRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    this->DesiredSizeOfChildren = this->GetDesiredSize_v2();
    this->SetDesiredSizeInSpt(
          this->ScrollRegionSize.InStaticPoints(this->GetViewport())
        + this->Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
        + (this->bUseChildrenDesiredSize ? this->DesiredSizeOfChildren : maths::zero_vector<LVec2F>)
        );
    return;
}

bool Jafg::WScrollRegion::MBDownOnScrollbar(std::optional<LVec2F> const& CursorLocation)
{
    check(!this->UserInterfaceTickDelegateHandle.IsValid())

    if (!CursorLocation)
    {
        return false;
    }

    auto MouseLocation{*CursorLocation};
    LVec2F TopLeftMostOuter{this->GetAnchoredTopLeftFromMostOuter()};

    if (this->LastVisible.y < 1.0f && maths::aabb_point({
        .offset = TopLeftMostOuter + this->GetVInteractiveAreaScrollPositionFromOuter(),
        .extent = this->GetVInteractiveAreaScrollSize()}, MouseLocation))
    {
        if (LVec2D TopLeftForeground{TopLeftMostOuter + this->GetVForegroundScrollPositionFromOuter()};
            maths::aabb_point({TopLeftForeground, this->GetVForegroundScrollSize()}, MouseLocation))
        {
            this->MbOffset.y = static_cast<f32>((MouseLocation.y - TopLeftForeground.y) * -1.0);
        }
        this->bUiTickV = true;
        this->UserInterfaceTickDelegateHandle = this->GetViewport().OnLateTick.Emplace(this, &WScrollRegion::UserInterfaceTick);
        return true;
    }

    if (this->LastVisible.x < 1.0f && maths::aabb_point({
        .offset = TopLeftMostOuter + this->GetHInteractiveAreaScrollPositionFromOuter(),
        .extent = this->GetHInteractiveAreaScrollSize(),
        }, MouseLocation))
    {
        if (LVec2D TopLeftForeground{TopLeftMostOuter + this->GetHForegroundScrollPositionFromOuter()};
            maths::aabb_point({.offset = TopLeftForeground, .extent = this->GetHForegroundScrollSize()}, MouseLocation))
        {
            this->MbOffset.x = static_cast<f32>((MouseLocation.x - TopLeftForeground.x) * -1.0);
        }
        this->bUiTickH = true;
        this->UserInterfaceTickDelegateHandle = this->GetViewport().OnLateTick.Emplace(this, &WScrollRegion::UserInterfaceTick);
        return true;
    }

    return false;
}

bool Jafg::WScrollRegion::MBUpOnScrollbar()
{
    if (this->UserInterfaceTickDelegateHandle.IsValid())
    {
        this->bUiTickV = false;
        this->bUiTickH = false;
        this->MbOffset = maths::zero_vector<LVec2F>;
        this->GetViewport().OnLateTick.Remove(&this->UserInterfaceTickDelegateHandle);
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
        + Value
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
        + Value
        *   (*Prefs.MouseWheelScrollSpeed /
            maths::max(static_cast<f32>(this->DesiredSizeOfChildren.y) - static_cast<f32>(this->GetAnchoredSize_v2().y), 0.0f))
        * (Prefs.bInvertVerticalScrollWheel ? -1.0f : 1.0f),
        0.0f,
        1.0f
        );
}
