// Copyright mzoesch. All rights reserved.

#include "Nodes/Node.h"
#include "Engine/Engine.h"
#include "Nodes/Viewport.h"
#include "Nodes/Parent.h"

namespace
{

#if JAFG_DO_CHECKS
void AssertInvariantImpl(Jafg::WNode const& Node)
{
    check(!(Node.GetNodeState() & Jafg::ENodeStateBits::Hovered))

    if (Jafg::WParent const* Parent{Node.As<Jafg::WParent>()})
    {
        for (auto& Child : Parent->GetChildren())
        {
            check(Child.get())
            AssertInvariantImpl(*Child);
        }
    }

    return;
}
void AssertInvariant(Jafg::WNode const& Node)
{
    if (!(Node.GetNodeState() & Jafg::ENodeStateBits::Hovered))
    {
        AssertInvariantImpl(Node);
    }
    else
    {
        Jafg::WParent const* Parent{Node.GetParent()};
        while (Parent)
        {
            check(Parent->GetNodeState() & Jafg::ENodeStateBits::Hovered)
            Parent = Parent->GetParent();
        }
    }

    return;
}
#endif /* JAFG_DO_CHECKS */

} /* ~Namespace <Anonymous> */

Jafg::LNodeReply Jafg::WNode::Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location)
{
    if (Location.has_value())
    {
        if (this->IsHitTestable())
        {
            if (this->AabbTest(Info, *Location))
            {
                if (this->NodeState & ENodeStateBits::HoveredDispatched)
                {
                    return this->OnCursorMoved(*Location);
                }
                if (this->NodeState & ENodeStateBits::Hovered)
                {
                    return {};
                }
                checkCode(::AssertInvariant(*this))
                this->NodeState |= ENodeStateBits::Hovered | ENodeStateBits::HoveredDispatched;
                WParent* Parent{this->Parent};
                while (Parent)
                {
                    if (Parent->GetNodeState() & ENodeStateBits::Hovered)
                    {
                        break;
                    }
                    Parent->NodeState |= ENodeStateBits::Hovered;
                    Parent = Parent->GetParent();
                    continue;
                }
                checkCode(::AssertInvariant(*this))
                return this->OnCursorEnter();
            }
        }
    }

    this->_RemoveHoverState();
    checkCode(::AssertInvariant(*this))
    return {};
}

void Jafg::WNode::SetVisibility(const ENodeVisibility InVisibility)
{
    check(!this->_IsGarbage())
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
    check(!this->_IsGarbage())

    if (this->Parent)
    {
        this->Parent->RemoveChild(*this);
    }
    else
    {
        this->MarkAsGarbage_v2();
    }

    return;
}

Jafg::WNode& Jafg::WNode::GetMostOuterParent() noexcept
{
    if (this->Parent)
    {
        return this->Parent->GetMostOuterParent();
    }
    return *this;
}

Jafg::WNode const& Jafg::WNode::GetMostOuterParent() const noexcept
{
    if (this->Parent)
    {
        return this->Parent->GetMostOuterParent();
    }
    return *this;
}

void Jafg::WNode::SetDesiredSizeInSpt(LVec2F Size) const noexcept
{
    this->DesiredSize_v2 = Size;

    LVec2F SptMinSize{this->MinDesiredSize.InStaticPoints(this->GetViewport())};
    LVec2F SptMaxSize{this->MaxDesiredSize.InStaticPoints(this->GetViewport())};

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

void Jafg::WNode::UpdateAnchoredSize() const
{
    check(this->TransformsWidgetLayout())
    check(this->Anchor.IsNormalized())

    if (this->Parent)
    {
        this->SetAnchoredSize(this->Parent->GetAnchoredSizeForChild(*this));
        return;
    }

    LVec2F Out;
    Out.x = maths::max(this->Anchor.MaxX * static_cast<f32>(this->AttachedViewport.GetExtent().width), this->DesiredSize_v2.x);
    Out.y = maths::max(this->Anchor.MaxY * static_cast<f32>(this->AttachedViewport.GetExtent().height), this->DesiredSize_v2.y);
    this->SetAnchoredSize(Out);

    return;
}

void Jafg::WNode::SetAnchoredSize(LVec2F const& InSize) const noexcept
{
    this->AnchoredSize_v2 = InSize;
    this->LostAnchoredSize_v2 = maths::zero_vector<LVec2F>;

    if (this->MaxDesiredSize.Size.x > 0.0)
    {
        this->LostAnchoredSize_v2.x = maths::max(this->AnchoredSize_v2.x - this->MaxDesiredSize.InStaticPoints(this->GetViewport()).x, 0.0f);
        this->AnchoredSize_v2.x = maths::min(this->AnchoredSize_v2.x, this->MaxDesiredSize.InStaticPoints(this->GetViewport()).x);
    }
    if (this->MaxDesiredSize.Size.y > 0.0)
    {
        this->LostAnchoredSize_v2.y = maths::max(this->AnchoredSize_v2.y - this->MaxDesiredSize.InStaticPoints(this->GetViewport()).y, 0.0f);
        this->AnchoredSize_v2.y = maths::min(this->AnchoredSize_v2.y, this->MaxDesiredSize.InStaticPoints(this->GetViewport()).y);
    }

    return;
}

LVec2F Jafg::WNode::GetAnchoredTopLeftFromMostOuter() const
{
    check(this->TransformsWidgetLayout())
    check(this->Anchor.IsNormalized())

    if (this->Parent)
    {
        return this->Parent->GetAnchoredTopLeftFromMostOuterForChild(*this);
    }

    return {
        this->Anchor.MinX * static_cast<f32>(this->AttachedViewport.GetExtent().width),
        this->Anchor.MinY * static_cast<f32>(this->AttachedViewport.GetExtent().height)
        };
}

LVec2F Jafg::WNode::GetAnchoredAndTranslatedTopLeftFromMostOuter(LVec2F const& Translation) const
{
    return this->GetAnchoredTopLeftFromMostOuter() + Translation;
}

std::optional<Jafg::LMargin> Jafg::WNode::GetMargin() const noexcept
{
    if (this->Parent)
    {
        return this->Parent->Padding;
    }
    return {};
}

Jafg::LFrontend const& Jafg::WNode::GetFrontend() const noexcept
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
    return GEngine->GetLocalEgo().GetFrontend();
}

Jafg::LFrontend& Jafg::WNode::GetMutableFrontend() const noexcept
{
    check(GMutableEngine && "Absence of GMutableEngine if undefined behavior.")
    return GMutableEngine->GetLocalEgo().GetFrontend();
}

#if JAFG_DO_CHECKS
void Jafg::WNode::_check_StateInvariant()
{
    AssertInvariant(*this);
}

void Jafg::WNode::_check_Destruct()
{
    if (this->Parent)
    {
        jassert(!algo::contains(this->Parent->GetChildren(), this, algo::unique_raw{}))
    }
}

#endif /* JAFG_DO_CHECKS */

TJxxUnique<Jafg::WNode> Jafg::WNode::RemoveFromTreeImpl() noexcept
{
    return this->GetParentChecked()->RemoveChildImpl(*this);
}
