// Copyright mzoesch. All rights reserved.

#include "Nodes/Overlay.h"

void Jafg::WOverlay::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    for (auto& Child : this->GetChildren())
    {
        if (Child->TransformsWidgetLayout())
        {
            Child->UpdateDesiredSize();
        }
        else
        {
            Child->SetDesiredSize(LWidgetSize2::Zero());
        }
    }

    LVec2F DesiredSize{maths::zero_vector<LVec2F>};
    for (auto& Child : this->GetChildren())
    {
        LVec2F ChildDesiredSize{Child->GetDesiredSize_v2()};
        DesiredSize.x = maths::max(DesiredSize.x, ChildDesiredSize.x);
        DesiredSize.y = maths::max(DesiredSize.y, ChildDesiredSize.y);
    }
    DesiredSize += this->GetPadding().GetDesiredSizeInSpt(*this);

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

void Jafg::WOverlay::UpdateAnchoredSize(LViewport const& Viewport) const
{
    Super::UpdateAnchoredSize(Viewport);

    for (auto& Child : this->GetChildren())
    {
        if (Child->TransformsWidgetLayout())
        {
            Child->UpdateAnchoredSize(Viewport);
        }
        else
        {
            Child->SetAnchoredSize(maths::zero_vector<LVec2F>);
        }
    }

    return;
}

LVec2F Jafg::WOverlay::GetAnchoredSizeForChild(LViewport const& Viewport, WNode const* InDirectChild) const
{
    check(InDirectChild)
    check(InDirectChild->TransformsWidgetLayout())

    return {
        maths::max(
            InDirectChild->GetDesiredSize_v2().x,
            InDirectChild->GetAnchor().MaxX * (this->GetAnchoredSize_v2().x - this->GetPadding().GetDesiredSizeXInSpt(Viewport))
            ),
        maths::max(
            InDirectChild->GetDesiredSize_v2().y,
            InDirectChild->GetAnchor().MaxY * (this->GetAnchoredSize_v2().y - this->GetPadding().GetDesiredSizeYInSpt(Viewport))
            )
    };
}

LVec2F Jafg::WOverlay::GetAnchoredTopLeftFromMostOuterForChild(LViewport const& Viewport, WNode const* InDirectChild) const
{
    check(InDirectChild)

    LVec2F Out
    {
        this->GetPadding().GetLeftOffsetInSpt(Viewport)
        + InDirectChild->GetAnchor().MinX *
        (
            this->GetAnchoredSize_v2().x
            - this->GetPadding().GetDesiredSizeXInSpt(Viewport)
            - InDirectChild->GetAnchoredSize_v2().x
        )
        + InDirectChild->GetLostAnchoredSize_v2().x * 0.5f,
        this->GetPadding().GetTopOffsetInSpt(Viewport)
        + InDirectChild->GetAnchor().MinY *
        (
            this->GetAnchoredSize_v2().y
            - this->GetPadding().GetDesiredSizeYInSpt(Viewport)
            - InDirectChild->GetAnchoredSize_v2().y
        )
        + InDirectChild->GetLostAnchoredSize_v2().y * 0.5f,
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Viewport);

    return Out;
}
