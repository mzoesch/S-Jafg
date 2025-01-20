// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Compound/TabBarBase.h"
#include "TabBar.generated.h"

namespace Jafg
{

class WWidgetSwitcher;

//#
//# A tab bar that uses the selection and focusable logic that is commonly used in Jafg.
//#
//# The tab bar consists of three main parts:
//#   - The tab bar button: The button that represents the tab in a given collection.
//#   - The tab bar panel:  The panel that represents the content of the tab.
//#   - The tab bar:        The collection of the buttons and panels.
//# Tab bars can be nested within each other with the superclass WTabBarBase.
//#
DECLARE_JAFG_CLASS(EClassFlags::Abstract)
class ENGINE_API WTabBar : public WTabBarBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTabBar)

    virtual void Construct() override;

public:

    FORCEINLINE auto GetButtonsContainer() -> WWidgetParentBase* { return this->ButtonsContainer; }
    FORCEINLINE auto GetButtonsContainer() const -> const WWidgetParentBase* { return this->ButtonsContainer; }
    FORCEINLINE auto GetSwitcher() -> WWidgetSwitcher* { return this->Switcher; }
    FORCEINLINE auto GetSwitcher() const -> const WWidgetSwitcher* { return this->Switcher; }

protected:

    //#
    //# The container where the buttons are stored.
    //#
    WWidgetParentBase* ButtonsContainer = nullptr;

    //#
    //# The switcher where the panels are stored.
    //#
    WWidgetSwitcher* Switcher = nullptr;
};

} /* ~Namespace Jafg */
