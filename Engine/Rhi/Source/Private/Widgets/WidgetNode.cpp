// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetNode.h"
#include "Widgets/WidgetParent.h"

namespace Jafg
{

RHI_API Private::LObjectContext* GCurrentWidgetContextState = nullptr;

struct LWidgetConstructor
{

};

} /* ~Namespace Jafg */

void Jafg::WWidgetNode::Construct()
{
}

void Jafg::WWidgetNode::Tick()
{

}

void Jafg::WWidgetNode::Destruct()
{
}

void Jafg::WWidgetNode::Draw(LSurface* Context) const
{
}

void Jafg::WWidgetNode::RemoveFromParent(const bool bDestroy)
{
    if (this->Parent)
    {
        this->Parent->RemoveChild(this);
        this->Parent = nullptr;
    }

    if (bDestroy)
    {
        this->MarkAsGarbage();
    }

    return;
}
