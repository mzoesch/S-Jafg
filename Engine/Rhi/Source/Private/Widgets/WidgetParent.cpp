// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetParent.h"

void Jafg::WWidgetParent::MarkAsGarbage()
{
    for (WWidgetNode* Child : this->Children)
    {
        Child->MarkAsGarbage();
    }

    Super::MarkAsGarbage();

    return;
}

void Jafg::WWidgetParent::Construct()
{
    Super::Construct();

    for (WWidgetNode* Child : this->Children)
    {
        Child->BeginLife();
    }

    return;
}

void Jafg::WWidgetParent::Draw(LViewport* Context) const
{
    Super::Draw(Context);

    for (const WWidgetNode* Child : this->Children)
    {
        Child->Draw(Context);
    }

    return;
}

void Jafg::WWidgetParent::Destruct()
{
    Super::Destruct();

    for (WWidgetNode* Child : this->Children)
    {
        Child->EndLife();
    }

    return;
}

Jafg::WWidgetParentBase* Jafg::WWidgetParent::AddChild(WWidgetNode* Child)
{
    this->Children.Add(Child);
    Child->Parent = this;

    return this;
}
