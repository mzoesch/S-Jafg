// Copyright mzoesch. All rights reserved.

#include "Nodes/VRegion.h"

void Jafg::WVRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVec2F DesiredSize{ maths::zero_vector<LVec2F> };
    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        DesiredSize.x  = maths::max(DesiredSize.x, Child->GetDesiredSize_v2().x);
        DesiredSize.y += Child->GetDesiredSize_v2().y;
    }
    DesiredSize.y += InSpt(this->GetViewport(), this->VSpace) * (this->GetChildren().size() - 1);
    DesiredSize += this->Padding.GetDesiredSizeInSpt(*this);

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

LVec2F Jafg::WVRegion::GetAnchoredSizeForChild(WNode const* DirectChild) const
{
    check( DirectChild )
    checkSlow( DirectChild->TransformsWidgetLayout() )

    if (DirectChild->Anchor.IsStretchedVertical() == false)
    {
        return Super::GetAnchoredSizeForChild(DirectChild);
    }

    f32 TotalDesiredSize { 0.0 };

    /* In percent. */
    f32 TotalFreeUsage   { 0.0 };

    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        TotalDesiredSize += Child->GetDesiredSize_v2().y;
        TotalFreeUsage   += Child->Anchor.MaxY;
    }

    const f32 FreeSpace
    {
        (this->GetAnchoredSize_v2().y - this->Padding.GetDesiredSizeYInSpt(this->GetViewport()))
        - TotalDesiredSize
        - InSpt(this->GetViewport(), this->VSpace) * (this->GetChildren().size() - 1)
    };

    const f32 InverseFreeUsage { 1.0f / TotalFreeUsage };

    return {
        maths::max(
            DirectChild->GetDesiredSize_v2().x,
            DirectChild->Anchor.MaxX * (this->GetAnchoredSize_v2().x - this->Padding.GetDesiredSizeXInSpt(this->GetViewport()))
            )
        , DirectChild->GetDesiredSize_v2().y
        + DirectChild->Anchor.MaxY * InverseFreeUsage * FreeSpace
    };
}

LVec2F Jafg::WVRegion::GetAnchoredTopLeftFromMostOuterForChild(WNode const* DirectChild) const
{
    check(DirectChild)

    f32 Offset{0.0};
    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        if (&*Child == DirectChild)
        {
            break;
        }

        Offset += Child->GetAnchoredSize_v2().y;
        Offset += InSpt(this->GetViewport(), this->VSpace);

        continue;
    }

    LVec2F Out
    {
        this->Padding.GetLeftOffsetInSpt(this->GetViewport())
        + DirectChild->Anchor.MinX *
        (
            this->GetAnchoredSize_v2().x
            - this->Padding.GetDesiredSizeXInSpt(this->GetViewport())
            - DirectChild->GetAnchoredSize_v2().x
        )
        + DirectChild->GetLostAnchoredSize_v2().x * 0.5,
        this->Padding.GetTopOffsetInSpt(this->GetViewport())
        + Offset
        + DirectChild->GetLostAnchoredSize_v2().y * 0.5
    };

    Out += this->GetAnchoredTopLeftFromMostOuter();

    return Out;
}
