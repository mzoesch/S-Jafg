// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Compound/TabBarBase.h"
#include "Widgets/Compound/TabBarButton.h"
#include "Widgets/Compound/TabBarPanel.h"
#include "TabBar.generated.h"

namespace Jafg
{

class WTabBarBase;
class WTabBar;
class WWidgetSwitcher;
class WTabBarPanel;
class WTabBarButton;

struct LTabBarTabDescriptor final
{
    LTabBarTabDescriptor() { }
    DEFAULT_REALLOC_OF_ANY_FORM(LTabBarTabDescriptor)
    ~LTabBarTabDescriptor() = default;

    LSimpleString Identifier;
    LString DisplayName;

    LPadding Padding = LPadding(0.0f);

    //#
    //# The panel widget to use. Required.
    //#
    TSubclassOf<WTabBarBase> PanelWidgetClass = nullptr;

    //#
    //# The button to use. Leave as nullptr to use the default button that comes with the tab bar.
    //#
    TSubclassOf<WTabBarButton> ButtonWidgetClass = nullptr;

    //#
    //# An optional value that may contain the identifier of the tab that this tab should be added after (in close
    //# proximity to it).
    //#
    LSimpleString AddAfter;
};

struct LTabBarTabData : public LWidgetNodeData
{
    LTabBarTabDescriptor* Descriptor;
};

//#
//# A tab bar that uses the selection and focusable logic that is commonly used in Jafg.
//#
//# The tab bar consists of three main parts:
//#   - The tab bar button: The button that represents the tab in a given collection.
//#   - The tab bar panel:  The panel that represents the content of the tab.
//#   - The tab bar:        The collection of the buttons and panels.
//# Tab bars can be nested within each other with the superclass WTabBarBase.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WTabBar : public WTabBarBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTabBar)

public:

    virtual void Construct() override;

    virtual void UpdateDesiredSize() const override;

    FORCEINLINE bool IsButtonContainerValid() const { return this->ButtonsContainer != nullptr; }
    FORCEINLINE auto GetButtonsContainer() -> WWidgetParentBase* { return this->ButtonsContainer; }
    FORCEINLINE auto GetButtonsContainer() const -> const WWidgetParentBase* { return this->ButtonsContainer; }
    FORCEINLINE bool IsSwitcherValid() const { return this->Switcher != nullptr; }
    FORCEINLINE auto GetSwitcher() -> WWidgetSwitcher* { return this->Switcher; }
    FORCEINLINE auto GetSwitcher() const -> const WWidgetSwitcher* { return this->Switcher; }

    void RegisterTab(LTabBarTabDescriptor&& InTabDescriptor);
    bool UnregisterTab(const LSimpleString& Identifier);
    bool UnregisterTabChecked(const LSimpleString& Identifier);

    template <typename TNode>
    FORCEINLINE void SetButtonsContainerClass() { this->SetButtonsContainerClass(TNode::StaticClass()); }
    FORCEINLINE void SetButtonsContainerClass(const TSubclassOf<WWidgetParentBase>& InButtonsContainerClass) { this->ButtonsContainerClass = InButtonsContainerClass; }
    FORCEINLINE auto GetCurrentButtonsContainerClass() const -> const TSubclassOf<WWidgetParentBase>& { return this->ButtonsContainerClass; }

    template <typename TNode>
    FORCEINLINE void SetSwitcherClass() { this->SetSwitcherClass(TNode::StaticClass()); }
    FORCEINLINE void SetSwitcherClass(const TSubclassOf<WWidgetSwitcher>& InSwitcherClass) { this->SwitcherClass = InSwitcherClass; }
    FORCEINLINE auto GetCurrentSwitcherClass() const -> const TSubclassOf<WWidgetSwitcher>& { return this->SwitcherClass; }

    FORCEINLINE auto GetTabsInOrder() const -> const TdhArray<LTabBarTabDescriptor>& { return this->TabsInOrder; }

private:

    void LoadTab(int32 InIndex);

protected:

    //#
    //# The container where the buttons are stored.
    //#
    WWidgetParentBase* ButtonsContainer = nullptr;
    TSubclassOf<WWidgetParentBase> ButtonsContainerClass = nullptr;
    TSubclassOf<WWidgetNode> DefaultButtonClass = nullptr;

    //#
    //# The switcher where the panels are stored.
    //#
    WWidgetSwitcher* Switcher = nullptr;
    TSubclassOf<WWidgetSwitcher> SwitcherClass = nullptr;

    TdhArray<LTabBarTabDescriptor> TabsInOrder = { };
};

} /* ~Namespace Jafg */
