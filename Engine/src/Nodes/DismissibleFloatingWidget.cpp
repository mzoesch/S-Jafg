// Copyright mzoesch. All rights reserved.

#include "Nodes/DismissibleFloatingWidget.h"
#include "Framework/Surface.h"

Jafg::LNodeReply Jafg::WDismissibleFloatingWidget::SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location)
{
    if (this->IsHitTestable())
    {
        if (this->AabbTest(Info, Location))
        {
            if (!this->GetWindow().AabbTest(Info, Location))
            {
                /* We do not consume the key. That is correct. */
                if (this->OnDismissEvent.IsValid())
                {
                    this->OnDismissEvent(*this);
                }
                Tasks::Make(ENamedThreads::Master, ETaskTime::Late, [this]{ this->MarkAsGarbage_v2(); });
                return LNodeReply::Unhandled();
            }
        }
    }

    return Super::SweepFocus(Info, Location);
}
