// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "PauseScreen.generated.h"

namespace Jafg
{

class WTabBar;
class WPauseScreen;

MAKE_MULTICAST_SIGNATURE(LOnPauseScreenConstruct, WPauseScreen* PauseScreen)

DECLARE_JAFG_WIDGET()
class ENGINE_API WPauseScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WPauseScreen)

public:

    //#
    //# Bind to this delegate to get notified when any pause menu is being constructed.
    //#
    static LOnPauseScreenConstruct OnPauseScreenConstruct;

    virtual void Construct() override;

    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;

private:

    WTabBar* PauseTabBar = nullptr;
};

} /* ~Namespace Jafg */
