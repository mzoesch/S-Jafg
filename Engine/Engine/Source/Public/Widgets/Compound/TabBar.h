// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Compound/TabBarPanel.h"
#include "Widgets/Compound/TabBarButton.h"
#include "Widgets/Compound/TabBarPanel.h"
#include "TabBar.generated.h"

namespace Jafg
{

class WTabBarPanel;
class WTabBar;
class WWidgetSwitcher;
class WTabBarPanel;
class WTabBarButton;

MAKE_DELEGATE_SIGNATURE(LOnTabBarLoaded, void, WTabBar* TabBar, WWidgetNode* Button, WTabBarPanel* Panel)

struct LTabBarTabDescriptor final
{
    LTabBarTabDescriptor() { }
    PROHIBIT_COPY(LTabBarTabDescriptor)
    DEFAULT_MOVE(LTabBarTabDescriptor)
    ~LTabBarTabDescriptor() = default;

    LSimpleString Identifier;
    LString DisplayName;

    LOnTabBarLoaded Callback;

    LPadding Padding = LPadding(0.0f);

    //#
    //# The panel widget to use. Required.
    //#
    TSubclassOf<WTabBarPanel> PanelWidgetClass = nullptr;

    //#
    //# The button to use. Leave as nullptr to use the default button that comes with the tab bar.
    //# ButtonWidgetClass and OnButtonPressed are mutually exclusive.
    //#
    TSubclassOf<WTabBarButton> ButtonWidgetClass = nullptr;
    //#
    //# Optional delegate that gets called when the button is pressed.
    //# ButtonWidgetClass and OnButtonPressed are mutually exclusive.
    //#
    LOnTabBarButtonPressed OnButtonPressed = nullptr;

    //#
    //# An optional value that may contain the identifier of the tab that this tab should be added after (in close
    //# proximity to it).
    //#
    LSimpleString AddAfter;
};

struct LTabBarTabData : public LWidgetNodeData
{
    WTabBar* Context;
    const LTabBarTabDescriptor* Descriptor;
};

template <typename TNode>
class TWidgetFactoryTabBar : public TWidgetFactoryParentBase<TNode>
{
public:

    using Super         = TWidgetFactoryParentBase<TNode>;
    using TFactoryRetTy = typename Super::TFactoryRetTy;

    FORCEINLINE TFactoryRetTy& AlignHorizontal() { this->This()->SetHorizontalPreference(); return this->Self(); }
    FORCEINLINE TFactoryRetTy& AlignVertical() { this->This()->SetVerticalPreference(); return this->Self(); }
};

//#
//# A tab bar that uses the selection and focusable logic that is commonly used in Jafg.
//#
//# The tab bar consists of three main parts:
//#   - The tab bar button: The button that represents the tab in a given collection.
//#   - The tab bar panel:  The panel that represents the content of the tab.
//#   - The tab bar:        The collection of the buttons and panels.
//# Tab bars can be nested within each other.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryTabBar)
class ENGINE_API WTabBar : public WTabBarPanel
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTabBar(const LObjectInitializer& ObjectInitializer);

public:

    virtual void Construct() override;

    FORCEINLINE void ResetWrapperClass() { this->WrapperClass = nullptr; }
    FORCEINLINE void SetWrapperClass(const TSubclassOf<WWidgetParentBase>& InWrapperClass) { this->WrapperClass = InWrapperClass; }
    FORCEINLINE void ResetDefaultAlignmentPreference() { this->bIsVertical.Reset(); }
    FORCEINLINE void SetVerticalPreference() { this->bIsVertical = true; }
    FORCEINLINE void SetHorizontalPreference() { this->bIsVertical = false; }

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

    void OnTabBarButtonPressed(const LSimpleString& Identifier);
    void OnOuterVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility);

private:

    void LoadTab(const LTabBarTabDescriptor& Descriptor, const int32 InIndex);

protected:

    struct LAddedTabBarTab;

    //#
    //# An optional class to set that wraps both the buttons section and the switcher.
    //#
    TSubclassOf<WWidgetParentBase> WrapperClass = nullptr;
    //#
    //# The preference for alignment of buttons and switcher if the wrapper class is not set.
    //# If this is not set, then there will not occur any wrapping.
    //#
    TOptional<bool> bIsVertical = false;

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

    const void* CurrentlyFocusedTab = nullptr;
    const LAddedTabBarTab* GetCurrentlyFocusedTab() const;
    FORCEINLINE const LAddedTabBarTab* GetCurrentlyFocusedTabChecked() const;
    FORCEINLINE const LAddedTabBarTab* GetCurrentlyFocusedTabPanicked() const;

    struct LAddedTabBarTab final
    {
        LSimpleString Identifier;
        WWidgetNode* Button = nullptr;
        WWidgetNode* Panel = nullptr;
        int8 SwitcherIndex = INDEX_NONE;
    };
    TdhArray<LAddedTabBarTab> TabsInOrder = { };
    TdhArray<LTabBarTabDescriptor> DeferredTabs = { };
};

} /* ~Namespace Jafg */

const Jafg::WTabBar::LAddedTabBarTab* Jafg::WTabBar::GetCurrentlyFocusedTabChecked() const
{
    const LAddedTabBarTab* Out = this->GetCurrentlyFocusedTab();
    check( Out )
    return Out;
}

const Jafg::WTabBar::LAddedTabBarTab* Jafg::WTabBar::GetCurrentlyFocusedTabPanicked() const
{
    if (const LAddedTabBarTab* Out = this->GetCurrentlyFocusedTab(); Out)
    {
        return Out;
    }
    panic( "No currently focused tab found." )
    return nullptr;
}
