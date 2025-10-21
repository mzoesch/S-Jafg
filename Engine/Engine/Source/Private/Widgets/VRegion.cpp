// Copyright mzoesch. All rights reserved.

#include "Widgets/VRegion.h"

void Jafg::WVRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize; check( DesiredSize.X == 0.0f && DesiredSize.Y == 0.0f )
    for (LWidgetSlot const* ChildSlot : this->GetChildren())
    {
        DesiredSize.X  = Maths::Max(DesiredSize.X, ChildSlot->Content->GetDesiredSize_v2().X);
        DesiredSize.Y += ChildSlot->Content->GetDesiredSize_v2().Y;

        continue;
    }

    DesiredSize.Y += this->VSpace * (this->GetChildren().size() - 1);

    DesiredSize += this->GetPadding().GetDesiredSizeInSpt();

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

    f32 TotalDesiredSize { 0.0f };

    /* In percent. */
    f32 TotalFreeUsage   { 0.0f };

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        checkSlow( ChildSlot->Content )

        TotalDesiredSize += ChildSlot->Content->GetDesiredSize_v2().Y;
        TotalFreeUsage   += ChildSlot->Content->GetAnchor().MaxY;

        continue;
    }

    const f32 FreeSpace
    {
        (this->GetAnchoredSize_v2().Y - this->GetPadding().GetDesiredSizeYInSpt())
        - TotalDesiredSize
        - this->GetVSpace() * (this->GetChildren().size() - 1)
    };

    const f32 InverseFreeUsage { 1.0f / TotalFreeUsage };

    InDirectChild->SetAnchoredSize
    ({
        Maths::Max
        (
            InDirectChild->GetDesiredSize_v2().X,
            InDirectChild->GetAnchor().MaxX * (this->GetAnchoredSize_v2().X - this->GetPadding().GetDesiredSizeXInSpt())
        ),
        InDirectChild->GetDesiredSize_v2().Y
        + InDirectChild->GetAnchor().MaxY * InverseFreeUsage * FreeSpace
    });

    return;
}

LVector2 Jafg::WVRegion::GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const
{
    check( InDirectChild )

    f32 Offset { 0.0f };
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content == InDirectChild)
        {
            break;
        }

        Offset += ChildSlot->Content->GetAnchoredSize_v2().Y;
        Offset += this->GetVSpace();

        continue;
    }

    LVector2 Out
    {
        this->GetPadding().GetLeftOffsetInSpt()
        + InDirectChild->GetAnchor().MinX *
        (
            this->GetAnchoredSize_v2().X
            - this->GetPadding().GetDesiredSizeXInSpt()
            - InDirectChild->GetAnchoredSize_v2().X
        )
        + InDirectChild->GetLostAnchoredSize_v2().X * 0.5f,
        this->GetPadding().GetTopOffsetInSpt()
        + Offset
        + InDirectChild->GetLostAnchoredSize_v2().Y * 0.5f
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
