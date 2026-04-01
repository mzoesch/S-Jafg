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
    DesiredSize += this->GetPadding().GetDesiredSizeInSpt(*this);

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

LVec2F Jafg::WHRegion::GetAnchoredSizeForChild(LViewport const& Viewport, WNode const* InDirectChild) const
{
    check(InDirectChild)
    checkSlow(InDirectChild->TransformsWidgetLayout())

    if (InDirectChild->GetAnchor().IsStretchedHorizontal() == false)
    {
        return Super::GetAnchoredSizeForChild(Viewport, InDirectChild);
    }

    f32 TotalDesiredSize { 0.0 };

    /* In percent. */
    f32 TotalFreeUsage   { 0.0 };

    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        TotalDesiredSize += Child->GetDesiredSize_v2().x;
        TotalFreeUsage   += Child->GetAnchor().MaxX;
    }

    const f32 FreeSpace
    {
        (this->GetAnchoredSize_v2().x - this->GetPadding().GetDesiredSizeXInSpt(Viewport))
        - TotalDesiredSize
        - InSpt(this->GetViewport(), this->GetHSpace()) * (this->GetChildren().size() - 1)
    };

    const f32 InverseFreeUsage { 1.0f / TotalFreeUsage };

    return {
          InDirectChild->GetDesiredSize_v2().x
        + InDirectChild->GetAnchor().MaxX * InverseFreeUsage * FreeSpace
        , maths::max(
            InDirectChild->GetDesiredSize_v2().y,
            InDirectChild->GetAnchor().MaxY * (this->GetAnchoredSize_v2().y - this->GetPadding().GetDesiredSizeYInSpt(Viewport))
            )
        };
}

LVec2F Jafg::WHRegion::GetAnchoredTopLeftFromMostOuterForChild(LViewport const& Context, WNode const* InDirectChild) const
{
    check( InDirectChild )

    f32 Offset { 0.0f };
    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        if (&*Child == InDirectChild)
        {
            break;
        }

        Offset += Child->GetAnchoredSize_v2().x;
        Offset += InSpt(this->GetViewport(), this->GetHSpace());

        continue;
    }

    LVec2F Out
    {
        this->GetPadding().GetLeftOffsetInSpt(Context)
        + Offset
        + InDirectChild->GetLostAnchoredSize_v2().x,
        this->GetPadding().GetTopOffsetInSpt(Context)
        + InDirectChild->GetAnchor().MinY *
        (
            this->GetAnchoredSize_v2().y
            - this->GetPadding().GetDesiredSizeYInSpt(Context)
            - InDirectChild->GetAnchoredSize_v2().y
        )
        + InDirectChild->GetLostAnchoredSize_v2().y
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
