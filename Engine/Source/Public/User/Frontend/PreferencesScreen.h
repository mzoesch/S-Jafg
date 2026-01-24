// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Blueprint/CommonMenuTabBar.h"
#include "PreferencesScreen.generated.h"

namespace Jafg
{

class LPreference;
class WPreferencesPanel;
class WPreferencesScreen;
class JCorePreferencesSubsystem;

DECLARE_JAFG_CLASS()
class JPreferencesPanelData : public JNodeData
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JPreferencesPanelData)

public:

    LPreference* Preference{ nullptr };
};

DECLARE_JAFG_WIDGET()
class WPreferencesPanel : public WCommonMenuTabBarPanel
{
    GENERATED_CLASS_BODY()

    friend JCorePreferencesSubsystem;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WPreferencesPanel)

    virtual void Construct() override;
    virtual bool AddData(JNodeData& Data) override;
};

DECLARE_JAFG_WIDGET(ECxxClassFlags::Config)
class WPreferencesScreen : public WCommonMenuTabBar
{
    GENERATED_CLASS_BODY()

protected:

    explicit WPreferencesScreen(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WPreferencesScreen)

public:

    virtual void Construct() override;

    CLASS_FIELD(Config)
    TSubclassOf<WPreferencesPanel> PanelClass{ DefaultInit };
};

} /* ~Namespace Jafg */
