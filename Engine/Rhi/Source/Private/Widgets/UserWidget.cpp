// Copyright mzoesch. All rights reserved.

#include "Widgets/UserWidget.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetParent.h"

void Jafg::WUserWidget::RemoveFromParent(const bool bDestroy /* = true */)
{
    Super::RemoveFromParent();

    if (this->AttachedViewport)
    {
        this->AttachedViewport->RemoveWidget(this);
    }

    if (bDestroy)
    {
        this->KillYourSelfNow(true);
    }

    return;
}

void Jafg::WUserWidget::AddToViewport(LViewport* InViewport)
{
    this->AttachedViewport = InViewport;
    this->AttachedViewport->AddWidget(this);

    return;
}

void Jafg::WUserWidget::ReplaceRoot(WWidgetParent* InRoot)
{
    if (this->HasRoot())
    {
        this->Root->RemoveFromParent();
    }

    this->Root = InRoot;

    return;
}
