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

    DEFAULT_OBJECT_CONSTRUCTORS(JPreferencesPanelData)

public:

    LPreference* Preference{ nullptr };
};

DECLARE_JAFG_WIDGET()
class WPreferencesPanel : public WCommonMenuTabBarPanel
{
    GENERATED_CLASS_BODY()

    friend JCorePreferencesSubsystem;

protected:

    DEFAULT_NODE_CONSTRUCTORS(WPreferencesPanel)

    virtual void Construct() override;
    virtual bool AddData(JNodeData& Data) override;
};

DECLARE_JAFG_WIDGET(ECxxClassFlags::Config)
class WPreferencesScreen : public WCommonMenuTabBar
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WPreferencesScreen)
    {
        this->SetHorizontalPreference();
        this->SetAnchor(EAnchor::Fill);
    }

public:

    virtual void Construct() override;

    CLASS_FIELD(Config)
    TSubclassOf<WPreferencesPanel> PanelClass{ DefaultInit };
};

} /* ~Namespace Jafg */
