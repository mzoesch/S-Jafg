// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/HRegion.h"
#include "Widgets/ButtonForward.h"
#include "HButton.generated.h"

namespace Jafg
{

class WHButton;
typedef TFunction<void(WHButton* Self, LKeyEvent const& InKeyEvent)> LOnHButtonKeyEvent;
typedef TFactoryButtonBase<WHButton> LFactoryHButton;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryHButton)
class ENGINE_API WHButton : public WHRegion, public LButtonBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WHButton) noexcept
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual LCursorReply SweepMouse(LViewport& Context, const LVec2F& InLocation) override;

    virtual void Construct() override;
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    LOnHButtonKeyEvent OnPrimaryPressDelegate;
    LOnHButtonKeyEvent OnPrimaryReleaseDelegate;
    LOnHButtonKeyEvent OnSecondaryPressDelegate;
    LOnHButtonKeyEvent OnSecondaryReleaseDelegate;

    void SetEnabled(const bool bInEnabled) override;
};

} /* ~Namespace Jafg */
