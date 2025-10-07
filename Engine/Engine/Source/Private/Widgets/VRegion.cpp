// Copyright mzoesch. All rights reserved.

#include "Widgets/VRegion.h"

void Jafg::WVRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        DesiredSize.X  = Maths::Max(DesiredSize.X, ChildSlot->Content->GetDesiredSize().X);
        DesiredSize.Y += ChildSlot->Content->GetDesiredSize().Y;

        continue;
    }

    DesiredSize.Y += this->VSpace * (this->GetChildren().size() - 1);

    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

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

        TotalDesiredSize += ChildSlot->Content->GetDesiredSize().Y;
        TotalFreeUsage   += ChildSlot->Content->GetAnchor().MaxY;

        continue;
    }

    const f32 FreeSpace
    {
        (this->GetAnchoredSize().Y - this->GetPadding().GetDesiredSizeY())
        - TotalDesiredSize
        - this->GetVSpace() * (this->GetChildren().size() - 1)
    };

    const f32 InverseFreeUsage { 1.0f / TotalFreeUsage };

    InDirectChild->SetAnchoredSize
    ({
        Maths::Max
        (
            InDirectChild->GetDesiredSize().X,
            InDirectChild->GetAnchor().MaxX * (this->GetAnchoredSize().X - this->GetPadding().GetDesiredSizeX())
        ),
        InDirectChild->GetDesiredSize().Y
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

        Offset += ChildSlot->Content->GetAnchoredSize().Y;
        Offset += this->GetVSpace();

        continue;
    }

    LVector2 Out
    {
        this->GetPadding().GetLeftOffset()
        + InDirectChild->GetAnchor().MinX *
        (
            this->GetAnchoredSize().X
            - this->GetPadding().GetDesiredSizeX()
            - InDirectChild->GetAnchoredSize().X
        )
        + InDirectChild->GetLostAnchoredSize().X * 0.5f,
        this->GetPadding().GetTopOffset()
        + Offset
        + InDirectChild->GetLostAnchoredSize().Y * 0.5f
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
