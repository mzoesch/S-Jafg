// Copyright mzoesch. All rights reserved.

#pragma once

#include "WidgetNode.h"
#include "UserWidget.generated.h"

namespace Jafg
{

class LViewport;
class WWidgetParent;

/**
 * A user widget is a widget node that can be added to the local player widget viewport.
 * A user widget can consist of multiple widget nodes and can be used to create complex
 * user interfaces.
 */
DECLARE_JAFG_CLASS()
class RHI_API WUserWidget : public WWidgetNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WUserWidget)

public:

    virtual void RemoveFromParent(const bool bDestroy = true) override;

    /** Add this widget to the main viewport of the current active local player. */
    void AddToViewport(LViewport* InViewport);

    void ReplaceRoot(WWidgetParent* InRoot);
    FORCEINLINE auto HasRoot() const -> bool { return this->Root != nullptr; }
    FORCEINLINE auto GetRoot() const -> WWidgetParent* { return this->Root;  }

private:

    /** The absolute root of this widget. Attach everything to this widget. */
    WWidgetParent* Root             = nullptr;
    /** Where this widget resides in. */
    LViewport*     AttachedViewport = nullptr;
};

template <typename TWidget>
FORCEINLINE auto ConstructUserWidget(Private::LObjectContext* InContext) -> TWidget*
{
    return NewObject<TWidget>(InContext);
}

} /* ~Namespace Jafg */
