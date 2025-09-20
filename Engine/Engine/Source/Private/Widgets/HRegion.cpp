// Copyright mzoesch. All rights reserved.

#include "Widgets/HRegion.h"

void Jafg::WHRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        DesiredSize.X += ChildSlot->Content->GetDesiredSize().X;
        DesiredSize.Y  = Maths::Max(DesiredSize.Y, ChildSlot->Content->GetDesiredSize().Y);

        continue;
    }

    DesiredSize.X += this->HSpace * (this->GetChildren().GetSize() - 1);

    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

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

        TotalDesiredSize += ChildSlot->Content->GetDesiredSize().X;
        TotalFreeUsage   += ChildSlot->Content->GetAnchor().MaxX;

        continue;
    }

    const f32 FreeSpace
    {
        (this->GetAnchoredSize().X - this->GetPadding().GetDesiredSizeX())
        - TotalDesiredSize
        - this->GetHSpace() * (this->GetChildren().GetSize() - 1)
    };

    const f32 InverseFreeUsage { 1.0f / TotalFreeUsage };

    InDirectChild->SetAnchoredSize
    ({
        InDirectChild->GetDesiredSize().X
        + InDirectChild->GetAnchor().MaxX * InverseFreeUsage * FreeSpace,
        Maths::Max
        (
            InDirectChild->GetDesiredSize().Y,
            InDirectChild->GetAnchor().MaxY * (this->GetAnchoredSize().Y - this->GetPadding().GetDesiredSizeY())
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

        Offset += ChildSlot->Content->GetAnchoredSize().X;
        Offset += this->GetHSpace();

        continue;
    }

    LVector2 Out
    {
        this->GetPadding().GetLeftOffset()
        + Offset
        + InDirectChild->GetLostAnchoredSize().X,
        this->GetPadding().GetTopOffset()
        + InDirectChild->GetAnchor().MinY *
        (
            this->GetAnchoredSize().Y
            - this->GetPadding().GetDesiredSizeY()
            - InDirectChild->GetAnchoredSize().Y
        )
        + InDirectChild->GetLostAnchoredSize().Y
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
