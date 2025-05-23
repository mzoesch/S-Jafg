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

struct LPreferencesPanelData : public LWidgetNodeData
{
    LPreference* Preference = nullptr;
};

#if PLATFORM_WASM
template <> NODISCARD inline auto FormatArgLegacy<TSubclassOf<WPreferencesPanel>>(TSubclassOf<WPreferencesPanel> Arg) { return Arg->GetSpacedClassName().ToC(); }
#endif /* PLATFORM_WASM */

DECLARE_JAFG_WIDGET()
class ENGINE_API WPreferencesPanel : public WCommonMenuTabBarPanel
{
    GENERATED_CLASS_BODY()

    friend JCorePreferencesSubsystem;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WPreferencesPanel)

    virtual void Construct() override;
    virtual bool AddData(const LWidgetNodeData* InData) override;
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
