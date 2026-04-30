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
    virtual LNodeReply SweepFocus(LNodeSweepInfo const& Info, LVec2F const& Location) override;

    virtual LNodeReply OnCursorEnter() override
    {
        check(!this->_check_MutableMouseEntered())
        checkCode(this->_check_MutableMouseEntered() = true)
        if (this->OnCursorEnterEvent)
        {
            return this->OnCursorEnterEvent(*this);
        }
        return LNodeReply::Unhandled();
    }
    virtual LNodeReply OnCursorMoved(LVec2F const& Location) override
    {
        check(this->_check_MutableMouseEntered())
        if (this->OnCursorMovedEvent)
        {
            return this->OnCursorMovedEvent(*this);
        }
        return LNodeReply::Unhandled();
    }
};

struct LFactoryDismissibleFloatingWidget : NODE_FACTORY_PARENT(WDismissibleFloatingWidget)
{
    NODE_FACTORY_BODY(WDismissibleFloatingWidget)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnDismiss, OnDismissEvent)
};

} /* ~Namespace Jafg */
