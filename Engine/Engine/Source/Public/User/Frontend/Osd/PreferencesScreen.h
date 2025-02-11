// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "PreferencesScreen.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET()
class ENGINE_API WPreferencesPanel : public WTabBarPanel
{
    GENERATED_WIDGET_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WPreferencesPanel)
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WPreferencesScreen : public WCommonMenuTabBar
{
    GENERATED_CLASS_BODY()

protected:

    explicit WPreferencesScreen(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;
};

} /* ~Namespace Jafg */
