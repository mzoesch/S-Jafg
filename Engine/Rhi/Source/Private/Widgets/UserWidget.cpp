// Copyright mzoesch. All rights reserved.

#include "Widgets/UserWidget.h"
#include "Widgets/Viewport.h"
#include "Widgets/WidgetParent.h"

void Jafg::WUserWidget::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    if (this->Root)
    {
        checkSlow( this->Root->Parent == this )
        this->Root->Content->Draw(Context);
    }

    return;
}

Jafg::LIntVector2 Jafg::WUserWidget::GetViewportSize() const
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

void Jafg::WUserWidget::RemoveChild(WWidgetNode* InChild)
{
    check( this->SingleRootChild.GetSize() == 1 )

    if (this->SingleRootChild[0]->Content == InChild)
    {
        this->RemoveChild(this->SingleRootChild[0]);
        return;
    }

    panic( "The in child is not the root of this widget." )

    return;
}

void Jafg::WUserWidget::RemoveChild(LWidgetSlot* InSlot)
{
    check( this->SingleRootChild.GetSize() == 1 )

    if (this->SingleRootChild[0] == InSlot)
    {
        this->SingleRootChild.Empty();
        this->Root = nullptr;
        delete InSlot;
    }
    else
    {
        panic( "The in slot is not the root of this widget." )
    }

    return;
}

Jafg::WWidgetParent* Jafg::WUserWidget::ReplaceRootImpl(WWidgetParent& InRoot)
{
    check( this->Slot == nullptr )

    if (this->HasRoot())
    {
        check( this->Root )
        check( this->Root == this->SingleRootChild[0] )
        check( this->Root->Parent == this )
        check( this->Root->Content )
        this->Root->Content->RemoveFromParent();
        check( this->Root == nullptr )
        check( this->SingleRootChild.IsEmpty() )
    }

    check( InRoot.Slot == nullptr )
    check( this->Root == nullptr )
    check( this->SingleRootChild.IsEmpty() )

    this->SingleRootChild.Emplace(new LWidgetSlot(this, &InRoot));
    // this->Root = new LWidgetSlot(this, &InRoot);
    this->Root = this->SingleRootChild[0];

    this->Root->Content->Slot = this->Root;
    this->Root->Margin = this->GetPaddingPtr();

    check( this->Root == this->SingleRootChild[0] )

    return &InRoot;
}
