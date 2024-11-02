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

void Jafg::WWidgetNode::RemoveFromParent(const bool bDestroy /* = true */)
{
    if (this->Slot)
    {
        check( this->Slot->Parent )
        this->Slot->Parent->RemoveChild(this);
        this->Slot = nullptr;
    }
    if (bDestroy)
    {
        this->MarkAsGarbage();
    }

    return;
}

auto Jafg::WWidgetNode::GetParent() const -> WWidgetParentBase*
{
    if (this->Slot)
    {
        return this->Slot->Parent;
    }

    return nullptr;
}

LIntVector2 Jafg::WWidgetNode::GetViewportSize() const
{
    if (this->Slot)
    {
        check( this->Slot->Parent )
        return this->Slot->Parent->GetViewportSize();
    }

    panic( "Failed to find window dimensions." )

    return LIntVector2::ZeroVector;
}

LVector2 Jafg::WWidgetNode::GetRelativeTopLeft() const
{
    if (this->Slot)
    {
        return this->Slot->Margin->GetTopLeftOffset();
    }

    return LVector2::Zero();
}

LVector2 Jafg::WWidgetNode::GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const
{
    if (this->Slot)
    {
        return this->Slot->Parent->GetRelativeTopLeftFromMostOuter(this) + this->GetRelativeTopLeft();
    }

    return this->GetRelativeTopLeft();
}
