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

void Jafg::WWidgetNode::MarkAsGarbage()
{
    Super::MarkAsGarbage();

    this->Destruct();

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

LIntVector2 Jafg::WWidgetNode::GetViewportSize() const
{
    if (this->Parent)
    {
        return this->Parent->GetViewportSize();
    }

    panic( "Failed to find window dimensions." )

    return LIntVector2::ZeroVector;
}
