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

    LVector2D DesiredSize;
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        LVector2D ChildDesiredSize = ChildSlot->Content->GetDesiredSize_v2();
        DesiredSize.X = Maths::Max(DesiredSize.X, ChildDesiredSize.X);
        DesiredSize.Y = Maths::Max(DesiredSize.Y, ChildDesiredSize.Y);
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
            ChildSlot->Content->SetAnchoredSize(LVector2D::ZeroVector);
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
        Maths::Max
        (
            InDirectChild->GetDesiredSize_v2().X,
            InDirectChild->GetAnchor().MaxX * (this->GetAnchoredSize_v2().X - this->GetPadding().GetDesiredSizeXInSpt(Context))
        ),
        Maths::Max
        (
            InDirectChild->GetDesiredSize_v2().Y,
            InDirectChild->GetAnchor().MaxY * (this->GetAnchoredSize_v2().Y - this->GetPadding().GetDesiredSizeYInSpt(Context))
        )
    });

    return;
}

LVector2D Jafg::WOverlay::GetAnchoredTopLeftFromMostOuterForChild(LViewport const& Context, WNode const* InDirectChild) const
{
    check( InDirectChild )

    LVector2D Out
    {
        this->GetPadding().GetLeftOffsetInSpt(Context)
        + InDirectChild->GetAnchor().MinX *
        (
            this->GetAnchoredSize_v2().X
            - this->GetPadding().GetDesiredSizeXInSpt(Context)
            - InDirectChild->GetAnchoredSize_v2().X
        )
        + InDirectChild->GetLostAnchoredSize_v2().X * 0.5f,
        this->GetPadding().GetTopOffsetInSpt(Context)
        + InDirectChild->GetAnchor().MinY *
        (
            this->GetAnchoredSize_v2().Y
            - this->GetPadding().GetDesiredSizeYInSpt(Context)
            - InDirectChild->GetAnchoredSize_v2().Y
        )
        + InDirectChild->GetLostAnchoredSize_v2().Y * 0.5f,
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
