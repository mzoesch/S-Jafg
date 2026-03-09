// Copyright mzoesch. All rights reserved.

#include "Widgets/VRegion.h"

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
    DesiredSize += this->GetPadding().GetDesiredSizeInSpt(*this);

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

void Jafg::WVRegion::UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const
{
    check( InDirectChild )
    checkSlow( InDirectChild->TransformsWidgetLayout() )

    if (InDirectChild->GetAnchor().IsStretchedVertical() == false)
    {
        Super::UpdateAnchoredSizeForChild(Context, InDirectChild);
        return;
    }

    f32 TotalDesiredSize { 0.0 };

    /* In percent. */
    f32 TotalFreeUsage   { 0.0 };

    for (auto& Child : this->GetChildren())
    {
        check(Child.get())
        TotalDesiredSize += Child->GetDesiredSize_v2().y;
        TotalFreeUsage   += Child->GetAnchor().MaxY;
    }

    const f32 FreeSpace
    {
        (this->GetAnchoredSize_v2().y - this->GetPadding().GetDesiredSizeYInSpt(Context))
        - TotalDesiredSize
        - InSpt(this->GetViewport(), this->GetVSpace()) * (this->GetChildren().size() - 1)
    };

    const f32 InverseFreeUsage { 1.0f / TotalFreeUsage };

    InDirectChild->SetAnchoredSize
    ({
        maths::max
        (
            InDirectChild->GetDesiredSize_v2().x,
            InDirectChild->GetAnchor().MaxX * (this->GetAnchoredSize_v2().x - this->GetPadding().GetDesiredSizeXInSpt(Context))
        ),
        InDirectChild->GetDesiredSize_v2().y
        + InDirectChild->GetAnchor().MaxY * InverseFreeUsage * FreeSpace
    });

    return;
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
        Offset += InSpt(this->GetViewport(), this->GetVSpace());

        continue;
    }

    LVec2F Out
    {
        this->GetPadding().GetLeftOffsetInSpt(Context)
        + InDirectChild->GetAnchor().MinX *
        (
            this->GetAnchoredSize_v2().x
            - this->GetPadding().GetDesiredSizeXInSpt(Context)
            - InDirectChild->GetAnchoredSize_v2().x
        )
        + InDirectChild->GetLostAnchoredSize_v2().x * 0.5,
        this->GetPadding().GetTopOffsetInSpt(Context)
        + Offset
        + InDirectChild->GetLostAnchoredSize_v2().y * 0.5
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
