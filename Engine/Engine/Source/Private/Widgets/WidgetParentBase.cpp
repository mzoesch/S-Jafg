// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetParentBase.h"

void Jafg::WWidgetParentBase::UpdateDesiredSize() const
{
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        ChildSlot->Content->UpdateDesiredSize();
    }

    Super::UpdateDesiredSize();

    return;
}
