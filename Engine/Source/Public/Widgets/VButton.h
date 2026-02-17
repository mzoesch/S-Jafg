// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/VRegion.h"
#include "Widgets/ButtonForward.h"
#include "VButton.generated.h"

namespace Jafg
{

class WVButton;
typedef TFunction<void(WVButton* Self, LKeyEvent const& InKeyEvent)> LOnVButtonKeyEvent;
typedef TFactoryButtonBase<WVButton> LFactoryVButton;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryVButton)
class ENGINE_API WVButton : public WVRegion, public LButtonBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WVButton) noexcept
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual void Construct() override;
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    virtual LReply OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;
    virtual LReply OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent) override;

    LOnVButtonKeyEvent OnPrimaryPressDelegate;
    LOnVButtonKeyEvent OnPrimaryReleaseDelegate;
    LOnVButtonKeyEvent OnSecondaryPressDelegate;
    LOnVButtonKeyEvent OnSecondaryReleaseDelegate;

    void SetEnabled(const bool bInEnabled) override;
};

} /* ~Namespace Jafg */
