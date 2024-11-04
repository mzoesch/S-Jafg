// Copyright mzoesch. All rights reserved.

#include "Widgets/VBox.h"

Jafg::LVector2 Jafg::WVBox::GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const
{
    if (this == WhoAsked)
    {
        return Super::GetRelativeTopLeftFromMostOuter(WhoAsked);
    }

    LVector2 Offset = Super::GetRelativeTopLeftFromMostOuter(WhoAsked);
    int32 Idx = this->GetChildren().FindIndexByPredicate([WhoAsked] (const LWidgetSlot* const InSlot) -> bool
    {
        return InSlot->Content == WhoAsked;
    });
    check( Idx != INDEX_NONE )

    while (--Idx > INDEX_NONE)
    {
        Offset.Y += this->GetChildren()[Idx]->Content->GetDesiredSize().Y;
    }

    return Offset;
}

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
