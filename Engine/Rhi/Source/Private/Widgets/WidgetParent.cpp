// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetParent.h"

void Jafg::WWidgetParent::MarkAsGarbage()
{
    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        ChildSlot->Content->Slot = nullptr;
        ChildSlot->Content->MarkAsGarbage();
        delete ChildSlot;
    }

    this->Children.Empty();

    Super::MarkAsGarbage();

    return;
}

void Jafg::WWidgetParent::Construct()
{
    Super::Construct();

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        ChildSlot->Content->BeginLife();
    }

    return;
}

void Jafg::WWidgetParent::Draw(LViewport* Context) const
{
    Super::Draw(Context);

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        checkSlow( ChildSlot->Content )
        ChildSlot->Content->Draw(Context);

        continue;
    }

    return;
}

void Jafg::WWidgetParent::Destruct()
{
    Super::Destruct();

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        ChildSlot->Content->Slot = nullptr;
        ChildSlot->Content->MarkAsGarbage();
        delete ChildSlot;
    }

    this->Children.Empty();

    return;
}

void Jafg::WWidgetParent::RemoveChild(WWidgetNode* Child)
{
    for (LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content == Child)
        {
            ChildSlot->Content->Slot = nullptr;
            ChildSlot->Content->MarkAsGarbage();
            this->Children.RemoveOnceChecked(ChildSlot);
            delete ChildSlot;

            check( this->Children.FindByPredicate([Child] (const LWidgetSlot* Slot)
            {
                return Slot->Content == Child;
            }) == nullptr )

            return;
        }

        continue;
    }

    panic( "The in child is not a child of this widget." )

    return;
}

void Jafg::WWidgetParent::RemoveChild(LWidgetSlot* Child)
{
    Child->Content->Slot = nullptr;
    this->Children.RemoveOnceChecked(Child);
    delete Child;

    return;
}

Jafg::LWidgetSlot* Jafg::WWidgetParent::AddChild(WWidgetNode* Child)
{
    LWidgetSlot* NewChildSlot = new LWidgetSlot(this, Child);
    this->Children.Add(NewChildSlot);
    NewChildSlot->Content->Slot = NewChildSlot;
    NewChildSlot->Margin = this->GetPaddingPtr();

    return NewChildSlot;
}
