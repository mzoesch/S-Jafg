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

LVec2F Jafg::WVRegion::GetAnchoredSizeForChild(LViewport const& Viewport, WNode const* InDirectChild) const
{
    check( InDirectChild )
    checkSlow( InDirectChild->TransformsWidgetLayout() )

    if (InDirectChild->Anchor.IsStretchedVertical() == false)
    {
        return Super::GetAnchoredSizeForChild(Viewport, InDirectChild);
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
        (this->GetAnchoredSize_v2().y - this->Padding.GetDesiredSizeYInSpt(Viewport))
        - TotalDesiredSize
        - InSpt(this->GetViewport(), this->VSpace) * (this->GetChildren().size() - 1)
    };

    const f32 InverseFreeUsage { 1.0f / TotalFreeUsage };

    return {
        maths::max(
            InDirectChild->GetDesiredSize_v2().x,
            InDirectChild->Anchor.MaxX * (this->GetAnchoredSize_v2().x - this->Padding.GetDesiredSizeXInSpt(Viewport))
            )
        , InDirectChild->GetDesiredSize_v2().y
        + InDirectChild->Anchor.MaxY * InverseFreeUsage * FreeSpace
    };
}

LVec2F Jafg::WVRegion::GetAnchoredTopLeftFromMostOuterForChild(LViewport const& Context, WNode const* InDirectChild) const
{
    check( InDirectChild )

    f32 Offset{0.0};
    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        if (&*Child == InDirectChild)
        {
            break;
        }

        Offset += Child->GetAnchoredSize_v2().y;
        Offset += InSpt(this->GetViewport(), this->VSpace);

        continue;
    }

    LVec2F Out
    {
        this->Padding.GetLeftOffsetInSpt(Context)
        + InDirectChild->Anchor.MinX *
        (
            this->GetAnchoredSize_v2().x
            - this->Padding.GetDesiredSizeXInSpt(Context)
            - InDirectChild->GetAnchoredSize_v2().x
        )
        + InDirectChild->GetLostAnchoredSize_v2().x * 0.5,
        this->Padding.GetTopOffsetInSpt(Context)
        + Offset
        + InDirectChild->GetLostAnchoredSize_v2().y * 0.5
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
