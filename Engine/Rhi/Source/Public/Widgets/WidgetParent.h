// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"
#include "WidgetParent.generated.h"

namespace Jafg
{

/**
 * The base class for all nodes that can possess children.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class RHI_API WWidgetParent : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetParent)

public:

    FORCEINLINE auto GetChildren() const -> const TdhArray<WWidgetNode*>& { return this->Children; }
    FORCEINLINE auto RemoveChild(WWidgetNode* Child) -> void { this->Children.RemoveOnceChecked(Child); }

    WWidgetParent* AddChild(WWidgetNode* Child);

private:

    TdhArray<WWidgetNode*> Children;
};

} /* ~Namespace Jafg. */
