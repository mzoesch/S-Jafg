// Copyright mzoesch. All rights reserved.

#include "Widgets/UserWidget.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetParent.h"

void Jafg::WUserWidget::ViewportDrawEntry(LViewport* Context) const
{
    Super::Draw(Context);

    if (this->Root)
    {
        this->Root->Draw(Context);
    }

    return;
}

void Jafg::WUserWidget::Draw(LViewport* Context) const
{
    Super::Draw(Context);

    if (this->Root)
    {
        this->Root->Draw(Context);
    }

    return;
}

LIntVector2 Jafg::WUserWidget::GetViewportSize() const
{
    if (this->AttachedViewport)
    {
        return this->AttachedViewport->GetDimensions();
    }

    return Super::GetViewportSize();
}

void Jafg::WUserWidget::AddToViewport(LViewport* InViewport)
{
    this->AttachedViewport = InViewport;
    this->AttachedViewport->AddWidget(this);

    return;
}

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


void Jafg::WUserWidget::ReplaceRoot(WWidgetParent* InRoot)
{
    if (this->HasRoot())
    {
        this->Root->RemoveFromParent();
    }

    this->Root = InRoot;
    InRoot->Parent = this;

    return;
}
