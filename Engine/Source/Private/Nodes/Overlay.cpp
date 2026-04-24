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
            Child->SetDesiredSize({});
        }
    }

    LVec2F DesiredSize{maths::zero_vector<LVec2F>};
    for (auto& Child : this->GetChildren())
    {
        LVec2F ChildDesiredSize{Child->GetDesiredSize_v2()};
        DesiredSize.x = maths::max(DesiredSize.x, ChildDesiredSize.x);
        DesiredSize.y = maths::max(DesiredSize.y, ChildDesiredSize.y);
    }
    DesiredSize += this->Padding.GetDesiredSizeInSpt(*this);

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

void Jafg::WOverlay::UpdateAnchoredSize() const
{
    Super::UpdateAnchoredSize();

    for (auto& Child : this->GetChildren())
    {
        if (Child->TransformsWidgetLayout())
        {
            Child->UpdateAnchoredSize();
        }
        else
        {
            Child->SetAnchoredSize(maths::zero_vector<LVec2F>);
        }
    }

    return;
}

LVec2F Jafg::WOverlay::GetAnchoredSizeForChild(WNode const* DirectChild) const
{
    check(DirectChild)
    check(DirectChild->TransformsWidgetLayout())

    return {
        maths::max(
            DirectChild->GetDesiredSize_v2().x,
            DirectChild->Anchor.MaxX * (this->GetAnchoredSize_v2().x - this->Padding.GetDesiredSizeXInSpt(this->GetViewport()))
            ),
        maths::max(
            DirectChild->GetDesiredSize_v2().y,
            DirectChild->Anchor.MaxY * (this->GetAnchoredSize_v2().y - this->Padding.GetDesiredSizeYInSpt(this->GetViewport()))
            )
    };
}

LVec2F Jafg::WOverlay::GetAnchoredTopLeftFromMostOuterForChild(WNode const* DirectChild) const
{
    check(DirectChild)

    LVec2F Out
    {
        this->Padding.GetLeftOffsetInSpt(this->GetViewport())
        + DirectChild->Anchor.MinX *
        (
            this->GetAnchoredSize_v2().x
            - this->Padding.GetDesiredSizeXInSpt(this->GetViewport())
            - DirectChild->GetAnchoredSize_v2().x
        )
        + DirectChild->GetLostAnchoredSize_v2().x * 0.5f,
        this->Padding.GetTopOffsetInSpt(this->GetViewport())
        + DirectChild->Anchor.MinY *
        (
            this->GetAnchoredSize_v2().y
            - this->Padding.GetDesiredSizeYInSpt(this->GetViewport())
            - DirectChild->GetAnchoredSize_v2().y
        )
        + DirectChild->GetLostAnchoredSize_v2().y * 0.5f,
    };

    Out += this->GetAnchoredTopLeftFromMostOuter();

    return Out;
}
