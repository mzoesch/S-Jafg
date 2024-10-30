// Copyright mzoesch. All rights reserved.

#include "Widgets/UserWidget.h"

void Jafg::WUserWidget::RemoveFromParent(const bool bDestroy)
{
    if (this->GetParent())
    {
        this->GetParent()->RemoveChild(this);
    }
    else
    {
        // Todo remove from local player.
    }

    if (bDestroy)
    {
        this->KillYourSelfNow();
    }

    return;
}

void Jafg::WUserWidget::AddToMainViewport()
{
}
