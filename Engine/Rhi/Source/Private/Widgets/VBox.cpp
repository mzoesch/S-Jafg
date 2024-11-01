// Copyright mzoesch. All rights reserved.

#include "Widgets/VBox.h"

LVector2 Jafg::WVBox::GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const
{
    LVector2 Offset = Super::GetRelativeTopLeftFromMostOuter(WhoAsked);

    int32 Idx = this->GetChildren().FindIndexByPredicate([WhoAsked] (const LWidgetSlot* const InSlot) -> bool
    {
        return InSlot->Content == WhoAsked;
    });
    jassert( Idx != INDEX_NONE )

    while (--Idx > INDEX_NONE)
    {
        Offset.Y += this->GetChildren()[Idx]->Content->GetDesiredSize().Y;
    }

    return Offset;
}
