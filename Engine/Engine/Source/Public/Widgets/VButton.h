// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/VRegion.h"
#include "Widgets/ButtonForward.h"
#include "VButton.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryVButton)
class WVButton : public WVRegion, public LButtonBase
{
    GENERATED_CLASS_BODY()

protected:

    explicit WVButton(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WVButton)

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
