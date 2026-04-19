// Copyright mzoesch. All rights reserved.

#include "Nodes/DismissibleFloatingWidget.h"
#include "Platform/Surface.h"
#include "User/Input/InputTypes.h"

Jafg::LCursorReply Jafg::WDismissibleFloatingWidget::SweepMouse(LNodeSweepData const& Data, LVec2F const& Location)
{
    if (this->IsHitTestable() == false || this->GetWindow().IsInBounds(Data, Location))
    {
        return Super::SweepMouse(Data, Location);
    }
    if (this->GetViewport().GetSurface().HasConsumableKeyState(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton), ERawInputStateBits::Press))
    {
        if (this->OnDismissEvent.IsValid())
        {
            this->OnDismissEvent(*this);
        }
        Tasks::Make(ENamedThreads::Master, ETaskTime::Late, [this]{ this->MarkAsGarbage_v2(); });
    }
    return LCursorReply::Unhandled();
}
