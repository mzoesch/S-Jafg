// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Compound/TabBar.h"
#include "Widgets/TextBlock.h"
#include "CommonMenuTabBar.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET()
class ENGINE_API WCommonMenuTabBarButton : public WTabBarButton
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WCommonMenuTabBarButton)

public:

    virtual void Construct() override;
};

//#
//# Development only. Do not use.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WDevelopmentTabBarPanelPlaceholder : public WTabBarPanel
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WDevelopmentTabBarPanelPlaceholder)

    virtual void Construct() override;

    WWidgetNode* Panel = nullptr;
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WCommonMenuTabBar : public WTabBar
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WCommonMenuTabBar)

public:

    virtual void Construct() override;
};

} /* ~Namespace Jafg */
