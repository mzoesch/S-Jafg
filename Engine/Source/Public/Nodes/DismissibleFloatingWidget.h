// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/FloatingWidget.h"
#include "DismissibleFloatingWidget.generated.h"

namespace Jafg
{

struct LFactoryDismissibleFloatingWidget;

//# A floating widget that can be dismissed by clicking on the non-covered background.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryDismissibleFloatingWidget)
class ENGINE_API WDismissibleFloatingWidget : public WFloatingWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WDismissibleFloatingWidget)
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->Anchor = EAnchor::Fill;
    }

public:

    EVENT_DECL(OnDismissEvent, void(WDismissibleFloatingWidget& Self))
    virtual LCursorReply SweepMouse(LNodeSweepData const& Data, LVec2F const& Location) override;
};

struct LFactoryDismissibleFloatingWidget : NODE_FACTORY_PARENT(WDismissibleFloatingWidget)
{
    NODE_FACTORY_BODY(WDismissibleFloatingWidget)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnDismiss, OnDismissEvent)
};

} /* ~Namespace Jafg */
