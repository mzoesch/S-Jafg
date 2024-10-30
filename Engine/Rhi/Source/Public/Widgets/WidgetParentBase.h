// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetNode.h"
#include "WidgetParentBase.generated.h"

namespace Jafg
{

/**
 * Pure virtual abstraction of a widget parent.
 * To let other widgets implement their own data structure for children.
 * TODO: Please think of a better name for this class.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class RHI_API WWidgetParentBase : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetParentBase)

public:

    virtual auto GetChildren()             const -> const TdhArray<WWidgetNode*>& PURE_VIRTUAL(return TdhArray<WWidgetNode*>())
    virtual auto RemoveChild(WWidgetNode* Child) -> void PURE_VIRTUAL()
    virtual auto AddChild(WWidgetNode* Child)    -> WWidgetParentBase* PURE_VIRTUAL(return nullptr)
};

} /* ~Namespace Jafg. */
