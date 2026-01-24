// Copyright mzoesch. All rights reserved.

#include "Widgets/Overlay.h"

void Jafg::WOverlay::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    for (LWidgetSlot const* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->TransformsWidgetLayout())
        {
            ChildSlot->Content->UpdateDesiredSize();
        }
        else
        {
            ChildSlot->Content->SetDesiredSize(LWidgetSize2::Zero());
        }

        continue;
    }

    LVec2F DesiredSize{ maths::zero_vector<LVec2F> };
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        LVec2F ChildDesiredSize = ChildSlot->Content->GetDesiredSize_v2();
        DesiredSize.x = maths::max(DesiredSize.x, ChildDesiredSize.x);
        DesiredSize.y = maths::max(DesiredSize.y, ChildDesiredSize.y);
        continue;
    }

    DesiredSize += this->GetPadding().GetDesiredSizeInSpt(*this);

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

void Jafg::WOverlay::UpdateAnchoredSize(const LViewport& Context) const
{
    Super::UpdateAnchoredSize(Context);

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->TransformsWidgetLayout())
        {
            ChildSlot->Content->UpdateAnchoredSize(Context);
        }
        else
        {
            ChildSlot->Content->SetAnchoredSize(maths::zero_vector<LVec2F>);
        }

        continue;
    }

    return;
}

void Jafg::WOverlay::UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const
{
    check( InDirectChild )
    checkSlow( InDirectChild->TransformsWidgetLayout() )

    InDirectChild->SetAnchoredSize
    ({
        maths::max
        (
            InDirectChild->GetDesiredSize_v2().x,
            InDirectChild->GetAnchor().MaxX * (this->GetAnchoredSize_v2().x - this->GetPadding().GetDesiredSizeXInSpt(Context))
        ),
        maths::max
        (
            InDirectChild->GetDesiredSize_v2().y,
            InDirectChild->GetAnchor().MaxY * (this->GetAnchoredSize_v2().y - this->GetPadding().GetDesiredSizeYInSpt(Context))
        )
    });

    return;
}

LVec2F Jafg::WOverlay::GetAnchoredTopLeftFromMostOuterForChild(LViewport const& Context, WNode const* InDirectChild) const
{
    check( InDirectChild )

    LVec2F Out
    {
        this->GetPadding().GetLeftOffsetInSpt(Context)
        + InDirectChild->GetAnchor().MinX *
        (
            this->GetAnchoredSize_v2().x
            - this->GetPadding().GetDesiredSizeXInSpt(Context)
            - InDirectChild->GetAnchoredSize_v2().x
        )
        + InDirectChild->GetLostAnchoredSize_v2().x * 0.5f,
        this->GetPadding().GetTopOffsetInSpt(Context)
        + InDirectChild->GetAnchor().MinY *
        (
            this->GetAnchoredSize_v2().y
            - this->GetPadding().GetDesiredSizeYInSpt(Context)
            - InDirectChild->GetAnchoredSize_v2().y
        )
        + InDirectChild->GetLostAnchoredSize_v2().y * 0.5f,
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
