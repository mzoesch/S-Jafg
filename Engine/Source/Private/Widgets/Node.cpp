// Copyright mzoesch. All rights reserved.

#include "Widgets/Node.h"
#include "Engine/Engine.h"
#include "Widgets/Viewport.h"
#include "Widgets/Parent.h"
#include "Widgets/Region.h"
#include "User/UserPreferences.h"

namespace Jafg
{

ENGINE_API const LAnchor LAnchor::VTop    { 0.0, 0.0, 0.0, 0.0 };
ENGINE_API const LAnchor LAnchor::VCenter { 0.0, 0.5, 0.0, 0.0 };
ENGINE_API const LAnchor LAnchor::VBottom { 0.0, 1.0, 0.0, 0.0 };
ENGINE_API const LAnchor LAnchor::HLeft   { 0.0, 0.0, 0.0, 0.0 };
ENGINE_API const LAnchor LAnchor::HCenter { 0.5, 0.0, 0.0, 0.0 };
ENGINE_API const LAnchor LAnchor::HRight  { 1.0, 0.0, 0.0, 0.0 };
ENGINE_API const LAnchor LAnchor::VFill   { 0.0, 0.0, 0.0, 1.0 };
ENGINE_API const LAnchor LAnchor::HFill   { 0.0, 0.0, 1.0, 0.0 };

bool LAnchor::IsNormalized() const noexcept
{
    return
           this->Anchors.x >= 0.0f && this->Anchors.y >= 0.0f && this->Anchors.z >= 0.0f && this->Anchors.w >= 0.0f
        && this->Anchors.x <= 1.0f && this->Anchors.y <= 1.0f && this->Anchors.z <= 1.0f && this->Anchors.w <= 1.0f
        && this->MinX + this->MaxX <= 1.0f
        && this->MinY + this->MaxY <= 1.0f;
}

void LAnchor::Normalize() noexcept
{
#if LAL_DO_CHECKS
    LAnchor Old = *this;
#endif /* LAL_DO_CHECKS */

    this->Anchors.x = maths::clamp(this->Anchors.x, 0.0f, 1.0f);
    this->Anchors.y = maths::clamp(this->Anchors.y, 0.0f, 1.0f);
    this->Anchors.z = maths::clamp(this->Anchors.z, 0.0f, 1.0f);
    this->Anchors.w = maths::clamp(this->Anchors.w, 0.0f, 1.0f);

    this->MaxX = maths::min(this->MaxX, 1.0f - this->MinX);
    this->MaxY = maths::min(this->MaxY, 1.0f - this->MinY);

#if LAL_DO_CHECKS
    if (Old != *this)
    {
        LOG_WARNING(LogWidgetFramework, "Anchor was not normalized correctly: {} -> {}.", Old.ToString(), this->ToString())
    }
#endif /* LAL_DO_CHECKS */

    return;
}

LString LexToString(ENodeVisibility Visibility)
{
    switch (Visibility)
    {
    case ENodeVisibility::Visible: { return "Visible"; }
    case ENodeVisibility::Hidden: { return "Hidden"; }
    case ENodeVisibility::Collapsed: { return "Collapsed"; }
    case ENodeVisibility::DerivedHitTestInvisible: { return "DerivedHitTestInvisible"; }
    case ENodeVisibility::TransitiveHitTestInvisible: { return "TransitiveHitTestInvisible"; }
    case ENodeVisibility::IntransitiveHitTestInvisible: { return "IntransitiveHitTestInvisible"; }
    default: checkNoEntry() return {"<unknown>"};
    }
}

} /* ~Namespace Jafg */

f32 Jafg::InSpt(LViewport const& Viewport, LWidgetSize1 Size) noexcept
{
    if (Size.Type == EWidgetSize::StaticPoints)
    {
        return Size.Size;
    }

    return InSptFromRelative(Viewport, Size.Size);
}

LVec2F Jafg::InSpt(LViewport const& Viewport, LWidgetSize2 Size) noexcept
{
    if (Size.Type == EWidgetSize::StaticPoints)
    {
        return Size.Size;
    }

    return InSptFromRelative(Viewport, Size.Size);
}

f32 Jafg::InSptFromRelative(LViewport const& Viewport, f32 Relative) noexcept
{
    const EApplicationScale Scale{Viewport.GetMaxAllowApplicationScale()};
    check(Scale != EApplicationScale::Auto)
    return Relative * LexToDouble(Scale);
}

LVec2F Jafg::InSptFromRelative(LViewport const& Viewport, LVec2F Relative) noexcept
{
    const EApplicationScale Scale{Viewport.GetMaxAllowApplicationScale()};
    check(Scale != EApplicationScale::Auto)
    return Relative * LexToFloat(Scale);
}

bool Jafg::WNode::IsInBounds(const LViewport& Context, const LVec2F& InLocation) const
{
    if (this->TransformsWidgetLayout() == false)
    {
        return false;
    }

    const LVec2D TopLeftMostOuter{this->GetAnchoredTopLeftFromMostOuter(Context) + Context.GetSweepTranslation()};
    return
            TopLeftMostOuter.x <= InLocation.x
         && InLocation.x       <= TopLeftMostOuter.x + this->GetAnchoredSize_v2().x
         && TopLeftMostOuter.y <= InLocation.y
         && InLocation.y       <= TopLeftMostOuter.y + this->GetAnchoredSize_v2().y
         ;
}

Jafg::LCursorReply Jafg::WNode::SweepMouse(LViewport& Context, const LVec2F& InLocation)
{
    if (this->IsHitTestable() == false)
    {
        return LCursorReply::Unhandled();
    }

    if (this->IsInBounds(Context, InLocation) == false)
    {
        return LCursorReply::Unhandled();
    }

    if (Context.AddHoveredWidgetForFrame(this))
    {
        if (LCursorReply Reply{ this->OnCursorEnter() }; Reply.IsHandled())
        {
            return Reply;
        }

        return LCursorReply::Handled();
    }

    return this->OnCursorMoved(InLocation);
}

Jafg::LReply Jafg::WNode::SweepFocusTest(const LViewport& Context, const LVec2F& InLocation)
{
    if (this->IsInBounds(Context, InLocation) == false || this->IsHitTestable() == false)
    {
        return LReply::Unhandled();
    }

    return { this };
}

Jafg::LReply Jafg::WNode::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (this->OnKeyDownEvent.IsValid())
    {
        return this->OnKeyDownEvent.Invoke(*this, InViewport, InKeyEvent);
    }

    if (this->Slot.Parent)
    {
        return this->Slot.Parent->OnKeyDown(InViewport, InKeyEvent);
    }

    return LReply::Unhandled();
}

Jafg::LReply Jafg::WNode::OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (this->OnKeyUpEvent.IsValid())
    {
        return this->OnKeyUpEvent.Invoke(*this, InViewport, InKeyEvent);
    }

    if (this->Slot.Parent)
    {
        return this->Slot.Parent->OnKeyUp(InViewport, InKeyEvent);
    }

    return LReply::Unhandled();
}

Jafg::LReply Jafg::WNode::OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    check( this->IsInBounds(InViewport, *InViewport.GetCachedCursorLocationChecked() ) )
    return LReply::Unhandled();
}

Jafg::LReply Jafg::WNode::OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    check( this->IsInBounds(InViewport, *InViewport.GetCachedCursorLocationChecked() ) )
    return LReply::Unhandled();
}

bool Jafg::WNode::IsFocusWidget() const
{
    return this->IsFocusWidget(&this->GetViewport());
}

bool Jafg::WNode::IsFocusWidget(const LViewport* InViewport) const
{
    if (InViewport)
    {
        return InViewport->GetFocusedWidget() == this;
    }

    return false;
}

bool Jafg::WNode::IsFocusWidgetTransitive() const
{
    return this->IsFocusWidgetTransitive(&this->GetViewport());
}

bool Jafg::WNode::IsFocusWidgetTransitive(const LViewport* InViewport) const
{
    if (InViewport)
    {
        return InViewport->GetFocusedWidget() == this;
    }

    return false;
}

void Jafg::WNode::SetVisibility(const ENodeVisibility InVisibility)
{
    if (this->Visibility == InVisibility)
    {
        return;
    }

    const ENodeVisibility OldVisibility = this->Visibility;
    this->Visibility = InVisibility;
    this->OnVisibilityChanged(OldVisibility, InVisibility);

    return;
}

void Jafg::WNode::RemoveFromParent(const bool bDestroy /* = true */)
{
    if (this->Slot.Parent)
    {
        check(this->Slot.Content == this)
        this->Slot.Parent->RemoveChild(this);
        check(this->Slot.Parent == nullptr && this->Slot.Content == nullptr && this->Slot.Margin == nullptr)
    }

    if (bDestroy)
    {
        this->MarkAsGarbage_v2();
    }

    return;
}

Jafg::WNode* Jafg::WNode::GetMostOuterParent() noexcept
{
    if (this->Slot.Parent)
    {
        return this->Slot.Parent->GetMostOuterParent();
    }
    return this;
}

Jafg::WNode const* Jafg::WNode::GetMostOuterParent() const noexcept
{
    if (this->Slot.Parent)
    {
        return this->Slot.Parent->GetMostOuterParent();
    }
    return this;
}

bool Jafg::WNode::FindNodeInVisiblePath(const WNode* InNode) const
{
    return this == InNode && this->ShouldNowDraw();
}

LVec2u32 Jafg::WNode::GetViewportSize() const
{
    return this->GetViewport().GetDimensions();
}

void Jafg::WNode::SetDesiredSizeInSpt(LVec2F Size) const noexcept
{
    this->DesiredSize_v2 = Size;

    LVec2F SptMinSize{InSpt(this->GetViewport(), this->MinDesiredSize)};
    LVec2F SptMaxSize{InSpt(this->GetViewport(), this->MaxDesiredSize)};

    this->DesiredSize_v2.x = maths::max(this->DesiredSize_v2.x, SptMinSize.x);
    this->DesiredSize_v2.y = maths::max(this->DesiredSize_v2.y, SptMinSize.y);

    if (SptMaxSize.x > 0.0f)
    {
        this->DesiredSize_v2.x = maths::min(this->DesiredSize_v2.x, SptMaxSize.x);
    }
    if (SptMaxSize.y > 0.0f)
    {
        this->DesiredSize_v2.y = maths::min(this->DesiredSize_v2.y, SptMaxSize.y);
    }

    check(this->DesiredSize_v2.x >= 0.0f && this->DesiredSize_v2.y >= 0.0f)

    return;
}

void Jafg::WNode::UpdateAnchoredSize(LViewport const& Context) const
{
    check(this->TransformsWidgetLayout())
    check(this->Anchor.IsNormalized())

    if (this->Slot.Parent)
    {
        check(this->Slot.Content == this)
        this->Slot.Parent->UpdateAnchoredSizeForChild(Context, this);
        return;
    }

    LVec2F Out;
    Out.x = maths::max(this->Anchor.MaxX * static_cast<f32>(Context.GetDimensions().x), this->DesiredSize_v2.x);
    Out.y = maths::max(this->Anchor.MaxY * static_cast<f32>(Context.GetDimensions().y), this->DesiredSize_v2.y);
    this->SetAnchoredSize(Out);

    return;
}

void Jafg::WNode::SetAnchoredSize(LVec2F&& InSize) const noexcept
{
    this->LostAnchoredSize_v2 = maths::zero_vector<LVec2F>;
    this->AnchoredSize_v2 = std::move(InSize);

    if (this->MaxDesiredSize.X > 0.0)
    {
        this->LostAnchoredSize_v2.x = maths::max(this->AnchoredSize_v2.x - this->MaxDesiredSize.X, 0.0f);
        this->AnchoredSize_v2.x = maths::min(this->AnchoredSize_v2.x, this->MaxDesiredSize.X);
    }
    if (this->MaxDesiredSize.Y > 0.0)
    {
        this->LostAnchoredSize_v2.y = maths::max(this->AnchoredSize_v2.y - this->MaxDesiredSize.Y, 0.0f);
        this->AnchoredSize_v2.y = maths::min(this->AnchoredSize_v2.y, this->MaxDesiredSize.Y);
    }

    return;
}

LVec2F Jafg::WNode::GetAnchoredTopLeftFromMostOuter(const LViewport& Context) const
{
    check(this->TransformsWidgetLayout())
    check(this->Anchor.IsNormalized())

    if (this->Slot.Parent)
    {
        check(this->Slot.Content == this)
        return this->Slot.Parent->GetAnchoredTopLeftFromMostOuterForChild(Context, this);
    }

    return {
        this->Anchor.MinX * static_cast<f32>(Context.GetDimensions().x),
        this->Anchor.MinY * static_cast<f32>(Context.GetDimensions().y)
        };
}

LVec2F Jafg::WNode::GetAnchoredAndTranslatedTopLeftFromMostOuter(const LViewport& Context) const
{
    return this->GetAnchoredTopLeftFromMostOuter(Context) + Context.GetFrameTranslation();
}

bool Jafg::WNode::SetMargin(const LMargin& InMargin) noexcept
{
    if (this->Slot.Margin)
    {
        check(this->Slot.Parent)
        *this->Slot.Margin = InMargin;
        return true;
    }

    return false;
}
