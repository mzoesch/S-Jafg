// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetNode.h"

namespace Jafg
{

struct LWidgetConstructor
{

};


} /* ~Namespace Jafg */

void Jafg::WWidgetNode::MarkAsGarbage()
{
    this->Destruct();
    Super::MarkAsGarbage();
    return;
}

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
