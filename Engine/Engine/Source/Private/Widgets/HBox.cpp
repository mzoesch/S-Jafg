// Copyright mzoesch. All rights reserved.

#include "Widgets/HBox.h"

Jafg::LVector2 Jafg::WHBox::GetRelativeTopLeftFromOuter(const WWidgetNode* WhoAsked) const
{
    if (WhoAsked == nullptr || this == WhoAsked)
    {
        return Super::GetRelativeTopLeftFromOuter(WhoAsked);
    }

    LVector2 Offset = Super::GetRelativeTopLeftFromOuter(WhoAsked);
    int32 Idx = this->GetChildren().FindIndexByPredicate([WhoAsked] (const LWidgetSlot* const InSlot) -> bool
    {
        return InSlot->Content == WhoAsked;
    });
    check( Idx != INDEX_NONE )

    while (--Idx > INDEX_NONE)
    {
        Offset.X += this->GetChildren()[Idx]->Content->GetDesiredSize().X;
    }

    return Offset;
}

Jafg::LVector2 Jafg::WHBox::GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const
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
        Offset.X += this->GetChildren()[Idx]->Content->GetDesiredSize().X;
    }

    return Offset;
}

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

void Jafg::WHBox::UpdateAnchoredSize(const LViewport& Context) const
{
    // DO NOT CALL THE SUPER METHOD OF THE PARENT AND PARENT BASE CLASS!!!
    WWidgetNode::UpdateAnchoredSize(Context);

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        ChildSlot->Content->UpdateAnchoredSize(Context);
    }

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->GetAnchor().IsStretchedHorizontal() == false)
        {
            continue;
        }

        float SizeOfOtherNodeX = 0.0f;
        for (const LWidgetSlot* ChildSlot2 : this->GetChildren())
        {
            if (ChildSlot2->Content != ChildSlot->Content)
            {
                SizeOfOtherNodeX += ChildSlot2->Content->GetAnchoredSize().X;
            }
        }

        ChildSlot->Content->SetAnchoredSize({
            ChildSlot->Content->GetAnchoredSize().X - SizeOfOtherNodeX,
            ChildSlot->Content->GetAnchoredSize().Y,
        });

        continue;
    }

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        ChildSlot->Content->UpdateAnchoredSizeOfChildren(Context);
    }

    return;
}
