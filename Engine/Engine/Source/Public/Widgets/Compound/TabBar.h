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
class WSwitcher;
class WTabBarPanel;
class WTabBarButton;

MAKE_DELEGATE_SIGNATURE(LOnTabBarLoaded, void, WTabBar* TabBar, WNode* Button, WTabBarPanel* Panel)

struct LTabBarTabDescriptor final
{
    LTabBarTabDescriptor() { }
    PROHIBIT_COPY(LTabBarTabDescriptor)
    DEFAULT_MOVE(LTabBarTabDescriptor)
    ~LTabBarTabDescriptor() = default;

    LString Identifier;
    LString DisplayName;

    LOnTabBarLoaded Callback;

    LPadding Padding { 0.0f };

    //#
    //# The panel widget to use. Required.
    //#
    TSubclassOf<WTabBarPanel> PanelWidgetClass;

    //#
    //# The button to use. Leave as nullptr to use the default button that comes with the tab bar.
    //# ButtonWidgetClass and OnButtonReleased are mutually exclusive.
    //#
    TSubclassOf<WTabBarButton> ButtonWidgetClass;
    //#
    //# Optional delegate that gets called when the button is released.
    //# ButtonWidgetClass and OnButtonReleased are mutually exclusive.
    //#
    LOnTabBarButtonRelease OnButtonRelease;

    //#
    //# An optional value that may contain the identifier of the tab that this tab should be added after (in close
    //# proximity to it).
    //#
    LString AddAfter;
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
    FORCEINLINE void SetWrapperClass(const TSubclassOf<WParentBase>& InWrapperClass) { this->WrapperClass = InWrapperClass; }
    FORCEINLINE void ResetDefaultAlignmentPreference() { this->bIsVertical.Reset(); }
    FORCEINLINE void SetVerticalPreference() { this->bIsVertical = true; }
    FORCEINLINE void SetHorizontalPreference() { this->bIsVertical = false; }

    FORCEINLINE bool IsButtonContainerValid() const { return this->ButtonsContainer != nullptr; }
    FORCEINLINE auto GetButtonsContainer() -> WParentBase* { return this->ButtonsContainer; }
    FORCEINLINE auto GetButtonsContainer() const -> const WParentBase* { return this->ButtonsContainer; }
    FORCEINLINE bool IsSwitcherValid() const { return this->Switcher != nullptr; }
    FORCEINLINE auto GetSwitcher() -> WSwitcher* { return this->Switcher; }
    FORCEINLINE auto GetSwitcher() const -> const WSwitcher* { return this->Switcher; }

    void RegisterTab(LTabBarTabDescriptor&& InTabDescriptor);
    void AppendVSpace(const f32 InVSpace);
    void AppendHSpace(const f32 InHSpace);
    void AppendStretch(const LAnchor& InStretch);
    bool UnregisterTab(const LString& Identifier);
    bool UnregisterTabChecked(const LString& Identifier);

    template <typename TNode>
    FORCEINLINE void SetButtonsContainerClass() { this->SetButtonsContainerClass(TNode::StaticClass()); }
    FORCEINLINE void SetButtonsContainerClass(const TSubclassOf<WParentBase>& InButtonsContainerClass) { this->ButtonsContainerClass = InButtonsContainerClass; }
    FORCEINLINE auto GetCurrentButtonsContainerClass() const -> const TSubclassOf<WParentBase>& { return this->ButtonsContainerClass; }

    template <typename TNode>
    FORCEINLINE void SetSwitcherClass() { this->SetSwitcherClass(TNode::StaticClass()); }
    FORCEINLINE void SetSwitcherClass(const TSubclassOf<WSwitcher>& InSwitcherClass) { this->SwitcherClass = InSwitcherClass; }
    FORCEINLINE auto GetCurrentSwitcherClass() const -> const TSubclassOf<WSwitcher>& { return this->SwitcherClass; }

    void OnTabBarButtonReleased(const LString& Identifier);
    void OnOuterVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility);

private:

    void LoadTab(const LTabBarTabDescriptor& Descriptor, const i32 InIndex);

protected:

    struct LAddedTabBarTab;

    //#
    //# An optional class to set that wraps both the buttons section and the switcher.
    //#
    TSubclassOf<WParentBase> WrapperClass;
    //#
    //# The preference for alignment of buttons and switcher if the wrapper class is not set.
    //# If this is not set, then there will not occur any wrapping.
    //#
    TOptional<bool> bIsVertical { false };

    //#
    //# The container where the buttons are stored.
    //#
    WParentBase* ButtonsContainer { nullptr };
    TSubclassOf<WParentBase> ButtonsContainerClass;
    TSubclassOf<WNode> DefaultButtonClass;

    //#
    //# The switcher where the panels are stored.
    //#
    WSwitcher* Switcher { nullptr };
    TSubclassOf<WSwitcher> SwitcherClass;

    const void* CurrentlyFocusedTab { nullptr };
                const LAddedTabBarTab* GetCurrentlyFocusedTab() const;
    FORCEINLINE const LAddedTabBarTab* GetCurrentlyFocusedTabChecked() const;
    FORCEINLINE const LAddedTabBarTab* GetCurrentlyFocusedTabPanicked() const;

    struct LAddedTabBarTab final
    {
        LString Identifier;
        WNode* Button = nullptr;
        WNode* Panel = nullptr;
        i8 SwitcherIndex = INDEX_NONE;
    };
    TArray<LAddedTabBarTab> TabsInOrder;
    TArray<LTabBarTabDescriptor> DeferredTabs;
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
