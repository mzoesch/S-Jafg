// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParentBase.h"
#include "WidgetParent.generated.h"

namespace Jafg
{

/**
 * The base class for all nodes that can possess children.
 */
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class RHI_API WWidgetParent : public WWidgetParentBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WWidgetParent)

public:

    // WWidgetNode implementation
    virtual void MarkAsGarbage() override;
    virtual void Construct() override;
    virtual void Draw(LViewport* Context) const override;
    virtual void Destruct() override;
    // ~WWidgetNode implementation

    FORCEINLINE auto GetChildren() const -> const TdhArray<WWidgetNode*>& override { return this->Children; }
    FORCEINLINE auto RemoveChild(WWidgetNode* Child) -> void override { this->Children.RemoveOnceChecked(Child); }
    virtual auto AddChild(WWidgetNode* Child) -> WWidgetParentBase* override;

private:

    TdhArray<WWidgetNode*> Children;
};

} /* ~Namespace Jafg. */
