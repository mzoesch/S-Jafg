// Copyright mzoesch. All rights reserved.

#include "Widgets/UserWidget.h"
#include "Widgets/Viewport.h"
#include "Widgets/Parent.h"

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
