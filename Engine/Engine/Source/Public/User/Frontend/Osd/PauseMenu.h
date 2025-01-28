// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "PauseMenu.generated.h"

namespace Jafg
{

class WTabBar;
class WPauseMenu;

MAKE_MULTICAST_SIGNATURE(LOnPauseMenuConstruct, WPauseMenu* PauseMenu)

DECLARE_JAFG_WIDGET()
class ENGINE_API WPauseMenu final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WPauseMenu)

public:

    //#
    //# Bind to this delegate to get notified when any pause menu is being constructed.
    //#
    static LOnPauseMenuConstruct OnPauseMenuConstruct;

    virtual void Construct() override;

private:

    WTabBar* PauseTabBar = nullptr;
};

} /* ~Namespace Jafg */
