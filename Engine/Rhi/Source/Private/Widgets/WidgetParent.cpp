// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetParent.h"

Jafg::WWidgetParent* Jafg::WWidgetParent::AddChild(WWidgetNode* Child)
{
    this->Children.Add(Child);
    Child->Parent = this;

    return this;
}
