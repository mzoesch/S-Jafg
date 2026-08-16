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
    check(!(Node._GetNodeState() & Jafg::Detail::NodeStateSwept))

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
    if (Node._GetNodeState() & Jafg::Detail::NodeStateSwept)
    {
        Jafg::WParent const* Parent{Node.GetParent()};
        while (Parent)
        {
            check(Parent->_GetNodeState() & Jafg::Detail::ENodeStateBits::Fallthrough)
            Parent = Parent->GetParent();
        }
    }
    else
    {
        AssertInvariantImpl(Node);
    }

    return;
}
#endif /* JAFG_DO_CHECKS */

void RemoveFallthroughTransitively(Jafg::WNode& Node) noexcept
{
    if (auto* Parent{Node.As<Jafg::WParent>()})
    {
        for (auto& Child : Parent->GetChildren())
        {
            check(Child.get())
            RemoveFallthroughTransitively(*Child);
        }
    }

    Node._RemoveDispatchedState();
    Node._RemoveFallthroughState();
    return;
}

} /* ~Namespace <Anonymous> */

Jafg::LNodeReply Jafg::WNode::Sweep(LNodeSweepInfo const& Info, std::optional<LVec2F> const& Location)
{
    if (Location.has_value())
    {
        if (this->IsHitTestable())
        {
            if (this->AabbTest(Info, *Location))
            {
                if (this->NodeState & Detail::ENodeStateBits::Dispatched)
                {
                    checkCode(::AssertInvariant(*this))
                    return this->OnCursorMoved(*Location);
                }

                this->NodeState |= Detail::ENodeStateBits::Fallthrough;
                WParent* Parent{this->Parent};
                while (Parent)
                {
                    if (Parent->_GetNodeState() & Detail::ENodeStateBits::Fallthrough)
                    {
                        break;
                    }
                    Parent->NodeState |= Detail::ENodeStateBits::Fallthrough;
                    Parent = Parent->GetParent();
                    continue;
                }
                this->NodeState |= Detail::ENodeStateBits::Dispatched;

                checkCode(::AssertInvariant(*this))
                return this->OnCursorEnter();
            }
        }
    }

    this->_RemoveDispatchedState();
    ::RemoveFallthroughTransitively(*this);
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

    if (this->TransformsWidgetLayout())
    {
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
    }
    else
    {
        check(this->DesiredSize_v2 == maths::zero_vector<LVec2F>)
    }

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

    if (this->TransformsWidgetLayout())
    {
        LVec2F Out;
        Out.x = maths::max(this->Anchor.MaxX * static_cast<f32>(this->AttachedViewport.GetExtent().width), this->DesiredSize_v2.x);
        Out.y = maths::max(this->Anchor.MaxY * static_cast<f32>(this->AttachedViewport.GetExtent().height), this->DesiredSize_v2.y);
        this->SetAnchoredSize(Out);
    }
    else
    {
        this->SetAnchoredSize(maths::zero_vector<LVec2F>);
    }

    return;
}

void Jafg::WNode::SetAnchoredSize(LVec2F const& InSize) const noexcept
{
    this->AnchoredSize_v2 = InSize;
    this->LostAnchoredSize_v2 = maths::zero_vector<LVec2F>;

    if (this->TransformsWidgetLayout())
    {
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
    }
    else
    {
        check(this->AnchoredSize_v2 == maths::zero_vector<LVec2F>)
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

LVec2F Jafg::WNode::GetTranslationFromMostOuter() const noexcept
{
    LVec2F Result;
    if (this->Parent)
    {
        Result = this->Parent->GetTranslationForChildFromMostOuter(*this);
    }
    else
    {
        Result = maths::zero_vector<LVec2F>;
    }
    return Result;
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
    check(Detail::GMutableEngine && "Absence of GMutableEngine if undefined behavior.")
    return Detail::GMutableEngine->GetLocalEgo().GetFrontend();
}

#if JAFG_DO_CHECKS
void Jafg::WNode::_check_PrintIndentedTickTrace()
{
    auto Indent{0uz};
    WParent* Parent{this->Parent};
    while (Parent)
    {
        ++Indent;
        Parent = Parent->GetParent();
    }

    LOG_VERBOSE(LogWidgetFramework, "{}[{}]", std::string(Indent * 2, ' '), this->GetNameAsString())
}

void Jafg::WNode::_check_StateInvariant()
{
    AssertInvariant(*this);
}

void Jafg::WNode::_check_Destruct()
{
    if (this->Parent)
    {
        jassert(!algo::contains(this->Parent->GetChildren(), this, algo::unique_raw))
    }
}
#endif /* JAFG_DO_CHECKS */

TJxxUnique<Jafg::WNode> Jafg::WNode::RemoveFromTreeImpl() noexcept
{
    return this->GetParentChecked()->RemoveChildImpl(*this);
}
