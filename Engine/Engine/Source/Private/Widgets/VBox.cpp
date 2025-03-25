// Copyright mzoesch. All rights reserved.

#include "Widgets/VBox.h"

void Jafg::WVBox::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        DesiredSize.X  = Maths::Max(DesiredSize.X, ChildSlot->Content->GetDesiredSize().X);
        DesiredSize.Y += ChildSlot->Content->GetDesiredSize().Y;

        continue;
    }

    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

    return;
}

Jafg::LVector2 Jafg::WVBox::GetRelativeTopLeftForChild(const WWidgetNode* InDirectChild) const
{
    check( InDirectChild )

    LVector2 Offset = Super::GetRelativeTopLeftForChild(InDirectChild);
    i32 Idx = this->GetChildren().FindIndexByPredicate([InDirectChild] (const LWidgetSlot* const InSlot) -> bool
    {
        return InSlot->Content == InDirectChild;
    });
    check( Idx != INDEX_NONE )

    while (--Idx > INDEX_NONE)
    {
        Offset.Y += this->GetChildren()[Idx]->Content->GetDesiredSize().Y;
    }

    return Offset;
}

void Jafg::WVBox::UpdateAnchoredSizeForChild(const LViewport& Context, const WWidgetNode* InDirectChild) const
{
    Super::UpdateAnchoredSizeForChild(Context, InDirectChild);

    if (InDirectChild->GetAnchor().IsStretchedVertical() == false)
    {
        return;
    }

    float VFromOthers = 0.0f;
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content == InDirectChild)
        {
            break;
        }

        VFromOthers += ChildSlot->Content->GetAnchoredSize().Y;
    }

    InDirectChild->SetAnchoredSize({InDirectChild->GetAnchoredSize().X, InDirectChild->GetAnchoredSize().Y - VFromOthers});

    return;
}

Jafg::LVector2 Jafg::WVBox::GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WWidgetNode* InDirectChild) const
{
    check( InDirectChild )

    LVector2 Out = this->GetAnchoredTopLeftFromMostOuter(Context);
    LVector2 Offset = this->GetRelativeTopLeftForChild(InDirectChild);
    Out += LVector2(InDirectChild->GetAnchor().MinX, 0.0f)
        * (this->GetAnchoredSize() - InDirectChild->GetAnchoredSize() - Offset);
    Out += Offset * (LVector2::OneVector - LVector2(InDirectChild->GetAnchor().MinX, 0.0f));

    return Out;
}
