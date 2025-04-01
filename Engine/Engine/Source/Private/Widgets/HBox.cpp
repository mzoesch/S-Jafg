// Copyright mzoesch. All rights reserved.

#include "Widgets/HBox.h"

void Jafg::WHBox::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        DesiredSize.X += ChildSlot->Content->GetDesiredSize().X;
        DesiredSize.Y  = Maths::Max(DesiredSize.Y, ChildSlot->Content->GetDesiredSize().Y);

        continue;
    }

    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

    return;
}

Jafg::LVector2 Jafg::WHBox::GetRelativeTopLeftForChild(const WWidgetNode* InDirectChild) const
{
    check( InDirectChild )

    LVector2 Offset = Super::GetRelativeTopLeftForChild(InDirectChild);
    i32 Idx = this->GetChildren().FindByPredicate([InDirectChild] (const LWidgetSlot* const InSlot) -> bool
    {
        return InSlot->Content == InDirectChild;
    });
    check( Idx != INDEX_NONE )

    while (--Idx > INDEX_NONE)
    {
        Offset.X += this->GetChildren()[Idx]->Content->GetDesiredSize().X;
    }

    return Offset;
}

void Jafg::WHBox::UpdateAnchoredSizeForChild(const LViewport& Context, const WWidgetNode* InDirectChild) const
{
    Super::UpdateAnchoredSizeForChild(Context, InDirectChild);

    if (InDirectChild->GetAnchor().IsStretchedHorizontal() == false)
    {
        return;
    }

    float HFromOthers = 0.0f;
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content == InDirectChild)
        {
            break;
        }

        HFromOthers += ChildSlot->Content->GetAnchoredSize().X;
    }

    InDirectChild->SetAnchoredSize({InDirectChild->GetAnchoredSize().X - HFromOthers, InDirectChild->GetAnchoredSize().Y});

    return;
}

Jafg::LVector2 Jafg::WHBox::GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WWidgetNode* InDirectChild) const
{
    check( InDirectChild )

    LVector2 Out = this->GetAnchoredTopLeftFromMostOuter(Context);
    LVector2 Offset = this->GetRelativeTopLeftForChild(InDirectChild);
    Out += LVector2(0.0f, InDirectChild->GetAnchor().MinY)
        * (this->GetAnchoredSize() - InDirectChild->GetAnchoredSize() - Offset);
    Out += Offset * (LVector2::OneVector - LVector2(0.0f, InDirectChild->GetAnchor().MinY));

    return Out;
}
