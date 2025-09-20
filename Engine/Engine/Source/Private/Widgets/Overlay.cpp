// Copyright mzoesch. All rights reserved.

#include "Widgets/Overlay.h"

void Jafg::WOverlay::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->TransformsWidgetLayout())
        {
            ChildSlot->Content->UpdateDesiredSize();
        }
        else
        {
            ChildSlot->Content->SetDesiredSize(LVector2::Zero());
        }

        continue;
    }

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        const LVector2 ChildDesiredSize = ChildSlot->Content->GetDesiredSize();
        DesiredSize.X = Maths::Max(DesiredSize.X, ChildDesiredSize.X);
        DesiredSize.Y = Maths::Max(DesiredSize.Y, ChildDesiredSize.Y);
        continue;
    }

    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

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
            ChildSlot->Content->SetAnchoredSize(LVector2::Zero());
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
            InDirectChild->GetDesiredSize().X,
            InDirectChild->GetAnchor().MaxX * (this->GetAnchoredSize().X - this->GetPadding().GetDesiredSizeX())
        ),
        Maths::Max
        (
            InDirectChild->GetDesiredSize().Y,
            InDirectChild->GetAnchor().MaxY * (this->GetAnchoredSize().Y - this->GetPadding().GetDesiredSizeY())
        )
    });

    return;
}

LVector2 Jafg::WOverlay::GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const
{
    check( InDirectChild )

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
        + InDirectChild->GetAnchor().MinY *
        (
            this->GetAnchoredSize().Y
            - this->GetPadding().GetDesiredSizeY()
            - InDirectChild->GetAnchoredSize().Y
        )
        + InDirectChild->GetLostAnchoredSize().Y * 0.5f,
    };

    Out += this->GetAnchoredTopLeftFromMostOuter(Context);

    return Out;
}
