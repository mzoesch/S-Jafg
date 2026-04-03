// Copyright mzoesch. All rights reserved.

#include "Nodes/Node.h"
#include "Engine/Engine.h"
#include "Nodes/Viewport.h"
#include "Nodes/Parent.h"
#include "Nodes/Region.h"
#include "User/UserPreferences.h"

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
        return this->OnCursorEnter();
        // if (LCursorReply Reply{this->OnCursorEnter()}; Reply.IsHandled())
        // {
        //     return Reply;
        // }
        // return LCursorReply::Handled();
    }

    return this->OnCursorMoved(InLocation);
}

Jafg::LReply Jafg::WNode::SweepFocusTest(const LViewport& Context, const LVec2F& InLocation)
{
    if (this->IsInBounds(Context, InLocation) == false || this->IsHitTestable() == false)
    {
        return LReply::Unhandled();
    }

    return {this};
}

Jafg::LReply Jafg::WNode::OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& Event)
{
    if (this->OnKeyDownEvent.IsValid())
    {
        if (auto Reply{this->OnKeyDownEvent.Invoke(*this, Data, Event)}; Reply.IsHandled())
        {
            return Reply;
        }
    }

    if (this->Parent)
    {
        return this->Parent->OnKeyDown(Data, Event);
    }

    return LReply::Unhandled();
}

Jafg::LReply Jafg::WNode::OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& Event)
{
    if (this->OnKeyUpEvent.IsValid())
    {
        if (auto Reply{this->OnKeyUpEvent.Invoke(*this, Data, Event)}; Reply.IsHandled())
        {
            return Reply;
        }
    }

    if (this->Parent)
    {
        return this->Parent->OnKeyUp(Data, Event);
    }

    return LReply::Unhandled();
}

Jafg::LReply Jafg::WNode::OnKeyDownNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event)
{
    check(this->IsInBounds(Data.Viewport, Data.Surface.GetMouseLocationValue()))
    return LReply::Unhandled();
}

Jafg::LReply Jafg::WNode::OnKeyUpNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& Event)
{
    check(this->IsInBounds(Data.Viewport, Data.Surface.GetMouseLocationValue()))
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

void Jafg::WNode::RemoveFromParent2()
{
    check(this->_IsGarbage() == false)

    if (this->Parent)
    {
        this->Parent->RemoveChild(this);
    }
    else
    {
        this->MarkAsGarbage_v2();
    }

    return;
}

Jafg::WNode* Jafg::WNode::GetMostOuterParent() noexcept
{
    if (this->Parent)
    {
        return this->Parent->GetMostOuterParent();
    }
    return this;
}

Jafg::WNode const* Jafg::WNode::GetMostOuterParent() const noexcept
{
    if (this->Parent)
    {
        return this->Parent->GetMostOuterParent();
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

    if (this->Parent)
    {
        this->SetAnchoredSize(this->Parent->GetAnchoredSizeForChild(Context, this));
        return;
    }

    LVec2F Out;
    Out.x = maths::max(this->Anchor.MaxX * static_cast<f32>(Context.GetDimensions().x), this->DesiredSize_v2.x);
    Out.y = maths::max(this->Anchor.MaxY * static_cast<f32>(Context.GetDimensions().y), this->DesiredSize_v2.y);
    this->SetAnchoredSize(Out);

    return;
}

void Jafg::WNode::SetAnchoredSize(LVec2F const& InSize) const noexcept
{
    this->AnchoredSize_v2 = InSize;
    this->LostAnchoredSize_v2 = maths::zero_vector<LVec2F>;

    if (this->MaxDesiredSize.X > 0.0)
    {
        this->LostAnchoredSize_v2.x = maths::max(this->AnchoredSize_v2.x - InSpt(this->GetViewport(), this->MaxDesiredSize).x, 0.0f);
        this->AnchoredSize_v2.x = maths::min(this->AnchoredSize_v2.x, InSpt(this->GetViewport(), this->MaxDesiredSize).x);
    }
    if (this->MaxDesiredSize.Y > 0.0)
    {
        this->LostAnchoredSize_v2.y = maths::max(this->AnchoredSize_v2.y - InSpt(this->GetViewport(), this->MaxDesiredSize).y, 0.0f);
        this->AnchoredSize_v2.y = maths::min(this->AnchoredSize_v2.y, InSpt(this->GetViewport(), this->MaxDesiredSize).y);
    }

    return;
}

LVec2F Jafg::WNode::GetAnchoredTopLeftFromMostOuter(LViewport const& Viewport) const
{
    check(this->TransformsWidgetLayout())
    check(this->Anchor.IsNormalized())

    if (this->Parent)
    {
        return this->Parent->GetAnchoredTopLeftFromMostOuterForChild(Viewport, this);
    }

    return {
        this->Anchor.MinX * static_cast<f32>(Viewport.GetDimensions().x),
        this->Anchor.MinY * static_cast<f32>(Viewport.GetDimensions().y)
        };
}

LVec2F Jafg::WNode::GetAnchoredAndTranslatedTopLeftFromMostOuter(const LViewport& Viewport) const
{
    return this->GetAnchoredTopLeftFromMostOuter(Viewport) + Viewport.GetFrameTranslation();
}

TOptional<Jafg::LMargin> Jafg::WNode::GetMargin() const noexcept
{
    if (this->Parent)
    {
        return this->Parent->GetPadding();
    }
    return {};
}

Jafg::LFrontend& Jafg::WNode::GetFrontend() const noexcept
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
    return GEngine->GetLocalEgo().GetFrontend();
}

#if JAFG_DO_CHECKS
void Jafg::WNode::_check_Destruct()
{
    if (this->Parent)
    {
        jassert(algo::contains(this->Parent->GetChildren(), this, [](auto const& E){return &*E;}) == false)
    }
}
#endif /* JAFG_DO_CHECKS */
