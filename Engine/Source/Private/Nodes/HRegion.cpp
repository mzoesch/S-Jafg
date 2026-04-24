// Copyright mzoesch. All rights reserved.

#include "Nodes/HRegion.h"

void Jafg::WHRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVec2F DesiredSize{maths::zero_vector<LVec2F>};
    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        DesiredSize.x += Child->GetDesiredSize_v2().x;
        DesiredSize.y  = maths::max(DesiredSize.y, Child->GetDesiredSize_v2().y);
    }
    DesiredSize.x += InSpt(this->GetViewport(), this->HSpace * (this->GetChildren().size() - 1));
    DesiredSize += this->Padding.GetDesiredSizeInSpt(*this);

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

LVec2F Jafg::WHRegion::GetAnchoredSizeForChild(WNode const* DirectChild) const
{
    check(DirectChild)
    checkSlow(DirectChild->TransformsWidgetLayout())

    if (DirectChild->Anchor.IsStretchedHorizontal() == false)
    {
        return Super::GetAnchoredSizeForChild(DirectChild);
    }

    f32 TotalDesiredSize { 0.0 };

    /* In percent. */
    f32 TotalFreeUsage   { 0.0 };

    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        TotalDesiredSize += Child->GetDesiredSize_v2().x;
        TotalFreeUsage   += Child->Anchor.MaxX;
    }

    const f32 FreeSpace
    {
        (this->GetAnchoredSize_v2().x - this->Padding.GetDesiredSizeXInSpt(this->GetViewport()))
        - TotalDesiredSize
        - InSpt(this->GetViewport(), this->HSpace) * (this->GetChildren().size() - 1)
    };

    const f32 InverseFreeUsage { 1.0f / TotalFreeUsage };

    return {
          DirectChild->GetDesiredSize_v2().x
        + DirectChild->Anchor.MaxX * InverseFreeUsage * FreeSpace
        , maths::max(
            DirectChild->GetDesiredSize_v2().y,
            DirectChild->Anchor.MaxY * (this->GetAnchoredSize_v2().y - this->Padding.GetDesiredSizeYInSpt(this->GetViewport()))
            )
        };
}

LVec2F Jafg::WHRegion::GetAnchoredTopLeftFromMostOuterForChild(WNode const* DirectChild) const
{
    check( DirectChild )

    f32 Offset { 0.0f };
    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        if (&*Child == DirectChild)
        {
            break;
        }

        Offset += Child->GetAnchoredSize_v2().x;
        Offset += InSpt(this->GetViewport(), this->HSpace);

        continue;
    }

    LVec2F Out
    {
        this->Padding.GetLeftOffsetInSpt(this->GetViewport())
        + Offset
        + DirectChild->GetLostAnchoredSize_v2().x,
        this->Padding.GetTopOffsetInSpt(this->GetViewport())
        + DirectChild->Anchor.MinY *
        (
            this->GetAnchoredSize_v2().y
            - this->Padding.GetDesiredSizeYInSpt(this->GetViewport())
            - DirectChild->GetAnchoredSize_v2().y
        )
        + DirectChild->GetLostAnchoredSize_v2().y
    };

    Out += this->GetAnchoredTopLeftFromMostOuter();

    return Out;
}
