// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetParentBase.h"
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
class RHI_API WUserWidget : public WWidgetParentBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WUserWidget)

public:

    void ViewportDrawEntry(LViewport* Context) const;

    // WWidgetNode implementation
    virtual void Draw(LViewport* Context) const override;
    virtual auto GetViewportSize() const -> LIntVector2 override;
    // ~WWidgetNode implementation

    // WWidgetParentBase implementation
    virtual auto RemoveFromParent(const bool bDestroy = true) -> void override;
    // ~WWidgetParentBase implementation

    /** Add this widget to the main viewport of the current active local player. */
    void AddToViewport(LViewport* InViewport);

    void ReplaceRoot(WWidgetParent* InRoot);
    FORCEINLINE auto HasRoot() const -> bool { return this->Root != nullptr; }
    FORCEINLINE auto GetRoot() const -> WWidgetParent* { return this->Root;  }

private:

    /** The absolute root of this widget. Attach everything to this widget. */
    WWidgetParent* Root             = nullptr;
    /** Where this widget resides in. Can be null if attached to another widget. So do not use without checking. */
    LViewport*     AttachedViewport = nullptr;
};

template <typename TWidget>
FORCEINLINE auto ConstructUserWidget(Private::LObjectContext* InContext) -> TWidget*
{
    return NewObject<TWidget>(InContext);
}

} /* ~Namespace Jafg */
