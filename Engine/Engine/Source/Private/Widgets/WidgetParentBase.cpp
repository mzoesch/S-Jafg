// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetParentBase.h"

Jafg::LCursorReply Jafg::WWidgetParentBase::SweepMouse(LViewport& Context, const LVector2& InLocation)
{
    if (this->CanChildrenBeHitTestable() == false)
    {
        return Super::SweepMouse(Context, InLocation);
    }

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        if (ChildSlot->Content->ShouldCheckForInputs())
        {
            const LCursorReply Reply = ChildSlot->Content->SweepMouse(Context, InLocation);
            if (Reply.IsHandled())
            {
                return Reply;
            }
        }

        continue;
    }

    return Super::SweepMouse(Context, InLocation);
}

void Jafg::WWidgetParentBase::UpdateDesiredSize() const
{
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        ChildSlot->Content->UpdateDesiredSize();
    }

    Super::UpdateDesiredSize();

    return;
}

void Jafg::WWidgetParentBase::UpdateAnchoredSize(const LViewport& Context) const
{
    Super::UpdateAnchoredSize(Context);

    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        ChildSlot->Content->UpdateAnchoredSize(Context);
    }

    return;
}
