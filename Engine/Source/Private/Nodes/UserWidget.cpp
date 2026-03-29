// Copyright mzoesch. All rights reserved.

#include "Nodes/UserWidget.h"
#include "Nodes/Viewport.h"
#include "Nodes/Parent.h"

void Jafg::WUserWidget::Destruct()
{
    if (this->IsTopLevel())
    {
        this->GetViewport()._RemoveWidget(this);
        this->bIsTopLevel = false;
    }

    Super::Destruct();
    check(this->GetChildren().size() == 0)
    return;
}
