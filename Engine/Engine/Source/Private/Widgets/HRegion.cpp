// Copyright mzoesch. All rights reserved.

#include "Widgets/HRegion.h"

void Jafg::WHRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        DesiredSize.X += ChildSlot->Content->GetDesiredSize_v2().X;
        DesiredSize.Y  = Maths::Max(DesiredSize.Y, ChildSlot->Content->GetDesiredSize_v2().Y);

        continue;
    }

    DesiredSize.X += this->HSpace * (this->GetChildren().size() - 1);

    DesiredSize += this->GetPadding().GetDesiredSizeInSpt();

    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

void Jafg::WHRegion::UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const
{
    check( InDirectChild )
    checkSlow( InDirectChild->TransformsWidgetLayout() )

    if (InDirectChild->GetAnchor().IsStretchedHorizontal() == false)
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

        TotalDesiredSize += ChildSlot->Content->GetDesiredSize_v2().X;
        TotalFreeUsage   += ChildSlot->Content->GetAnchor().MaxX;

        continue;
    }

    const f32 FreeSpace
    {
        (this->GetAnchoredSize_v2().X - this->GetPadding().GetDesiredSizeXInSpt())
        - TotalDesiredSize
        - this->GetHSpace() * (this->GetChildren().size() - 1)
    };

    const f32 InverseFreeUsage { 1.0f / TotalFreeUsage };

    InDirectChild->SetAnchoredSize
    ({
        InDirectChild->GetDesiredSize_v2().X
        + InDirectChild->GetAnchor().MaxX * InverseFreeUsage * FreeSpace,
        Maths::Max
        (
            InDirectChild->GetDesiredSize_v2().Y,
            InDirectChild->GetAnchor().MaxY * (this->GetAnchoredSize_v2().Y - this->GetPadding().GetDesiredSizeYInSpt())
        )
    });

    return;
}

LVector2 Jafg::WHRegion::GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const
{
    check( InDirectChild )

    f32 Offset { 0.0f };
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content == InDirectChild)
        {
            break;
        }

        Offset += ChildSlot->Content->GetAnchoredSize_v2().X;
        Offset += this->GetHSpace();

        continue;
    }

    LVector2 Out
    {
        this->GetPadding().GetLeftOffsetInSpt()
        + Offset
        + InDirectChild->GetLostAnchoredSize_v2().X,
        this->GetPadding().GetTopOffsetInSpt()
        + InDirectChild->GetAnchor().MinY *
        (
            this->GetAnchoredSize_v2().Y
            - this->GetPadding().GetDesiredSizeYInSpt()
            - InDirectChild->GetAnchoredSize_v2().Y
        )
        + InDirectChild->GetLostAnchoredSize_v2().Y
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
