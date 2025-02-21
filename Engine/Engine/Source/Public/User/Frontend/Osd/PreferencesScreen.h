// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "PreferencesScreen.generated.h"

namespace Jafg
{

class LPreference;

struct LPreferencesPanelData : public LWidgetNodeData
{
    const LPreference* Preference = nullptr;
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WPreferencesPanel : public WCommonMenuTabBarPanel
{
    GENERATED_WIDGET_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WPreferencesPanel)

    virtual void Construct() override;
    virtual bool AddData(LWidgetNodeData* InData) override;
};

DECLARE_JAFG_WIDGET(EClassFlags::Config)
class ENGINE_API WPreferencesScreen : public WCommonMenuTabBar
{
    GENERATED_CLASS_BODY()

protected:

    explicit WPreferencesScreen(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;

    CLASS_FIELD(Config)
    TSubclassOf<WPreferencesPanel> PanelClass = LazyInit;
};

} /* ~Namespace Jafg */
