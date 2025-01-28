// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetParent.h"

void Jafg::WWidgetParent::OnGarbage()
{
    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        ChildSlot->Content->Slot = nullptr;
        ChildSlot->Content->MarkAsGarbage();
        delete ChildSlot;
    }

    this->Children.Empty();

    Super::OnGarbage();

    return;
}

void Jafg::WWidgetParent::Construct()
{
    Super::Construct();

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        MakeDeferredWidgetNodeFinal(ChildSlot->Content);
    }

    return;
}

void Jafg::WWidgetParent::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    for (const LWidgetSlot* ChildSlot : this->Children)
    {
        if (ChildSlot->Content->ShouldNowDraw())
        {
            checkSlow( ChildSlot->Content )
            ChildSlot->Content->Draw(Context);
        }

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
    this->RemoveChild(Child->Content);
}

Jafg::LWidgetSlot* Jafg::WWidgetParent::AddChild(WWidgetNode* InChild)
{
    check( InChild )
    LWidgetSlot* NewChildSlot = new LWidgetSlot(this, InChild);
    this->Children.Add(NewChildSlot);
    NewChildSlot->Content->Slot = NewChildSlot;
    NewChildSlot->Margin = this->GetPaddingPtr();

    return NewChildSlot;
}

Jafg::LWidgetSlot* Jafg::WWidgetParent::AddChildAt(const int32 InIndex, WWidgetNode* InChild)
{
    check( InChild )
    LWidgetSlot* NewChildSlot = new LWidgetSlot(this, InChild);
    this->Children.AddAt(InIndex, NewChildSlot);
    NewChildSlot->Content->Slot = NewChildSlot;
    NewChildSlot->Margin = this->GetPaddingPtr();

    return NewChildSlot;
}
