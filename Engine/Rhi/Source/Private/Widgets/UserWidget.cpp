// Copyright mzoesch. All rights reserved.

#include "Widgets/UserWidget.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetParent.h"

void Jafg::WUserWidget::ViewportDrawEntry(LViewport* Context) const
{
    Super::Draw(Context);

    if (this->Root)
    {
        this->Root->Content->Draw(Context);
    }

    return;
}

void Jafg::WUserWidget::Draw(LViewport* Context) const
{
    Super::Draw(Context);

    if (this->Root)
    {
        checkSlow( this->Root->Parent == this )
        this->Root->Content->Draw(Context);
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


Jafg::WWidgetParent* Jafg::WUserWidget::ReplaceRoot(WWidgetParent* InRoot)
{
    check( this->Slot == nullptr )

    if (this->HasRoot())
    {
        check( this->Root )
        check( this->Root->Parent == this )
        check( this->Root->Content )
        this->Root->Content->RemoveFromParent();
        check( this->Root == nullptr )
    }

    check( InRoot->Slot == nullptr )
    check( this->Root == nullptr )

    this->Root = new LWidgetSlot(this, InRoot);
    this->Root->Content->Slot = this->Root;
    this->Root->Margin = this->GetPaddingPtr();

    return InRoot;
}

Jafg::WWidgetParent* Jafg::WUserWidget::ReplaceRoot(WWidgetParent& InRoot)
{
    return this->ReplaceRoot(&InRoot);
}
