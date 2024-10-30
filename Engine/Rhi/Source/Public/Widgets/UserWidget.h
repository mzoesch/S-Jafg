// Copyright mzoesch. All rights reserved.

#pragma once

#include "WidgetNode.h"
#include "UserWidget.generated.h"

namespace Jafg
{

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

    /**
     * Removes this widget from its parent widget.
     * @param bDestroy If true, the widget will be destroyed automatically.
     */
    void RemoveFromParent(const bool bDestroy = true);

    /**
     * Add this widget to the main viewport of the current active local player.
     */
    void AddToMainViewport();
};

template <typename TWidget>
TWidget* ConstructUserWidget(Private::LObjectContext* InContext)
{
    return NewObject<TWidget>(InContext);
}

} /* ~Namespace Jafg */
