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
class WNode;
class WTabBarButton;
struct LFactoryTabBar;

typedef TFunction<void(WTabBar* TabBar, WNode* Button, WNode* Panel)> LOnTabBarLoaded;

struct LTabBarTabDescriptor final
{
    LTabBarTabDescriptor() { }
    PROHIBIT_COPY(LTabBarTabDescriptor)
    DEFAULT_MOVE(LTabBarTabDescriptor)
    ~LTabBarTabDescriptor() = default;

    LString IdentifierField;
    LString DisplayNameField;

    LOnTabBarLoaded CallbackField;

    LPadding PaddingField { 0.0f };

    //#
    //# The panel widget to use. Required.
    //#
    TSubclassOf<WNode> PanelWidgetClassField;

    //#
    //# The button to use. Leave as nullptr to use the default button that comes with the tab bar.
    //# ButtonWidgetClass and OnButtonReleased are mutually exclusive.
    //#
    TSubclassOf<WTabBarButton> ButtonWidgetClassField;
    //#
    //# Optional delegate that gets called when the button is released.
    //# ButtonWidgetClass and OnButtonReleased are mutually exclusive.
    //#
    LOnTabBarButtonRelease OnButtonReleaseField;

    //#
    //# An optional value that may contain the identifier of the tab that this tab should be added after (in close
    //# proximity to it).
    //#
    LString AddAfterField;

    FORCEINLINE LTabBarTabDescriptor&& Identifier(const LString& InString) { this->IdentifierField = InString; return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& Identifier(LString&& InString) { this->IdentifierField = std::move(InString); return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& DisplayName(const LString& InString) { this->DisplayNameField = InString; return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& DisplayName(LString&& InString) { this->DisplayNameField = std::move(InString); return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& Callback(LOnTabBarLoaded&& InCallback) { this->CallbackField = std::move(InCallback); return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& Padding(const LPadding& InPadding) { this->PaddingField = InPadding; return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& PanelWidgetClass(const TSubclassOf<WNode>& InClass) { this->PanelWidgetClassField = InClass; return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& ButtonWidgetClass(const TSubclassOf<WTabBarButton>& InClass) { this->ButtonWidgetClassField = InClass; return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& OnButtonRelease(LOnTabBarButtonRelease&& InCallback) { this->OnButtonReleaseField = std::move(InCallback); return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& AddAfter(const LString& InString) { this->AddAfterField = InString; return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& AddAfter(LString&& InString) { this->AddAfterField = std::move(InString); return std::move(*this); }

    FORCEINLINE LTabBarTabDescriptor&& Sibling(LTabBarTabDescriptor&& InSibling) { this->Siblings.emplace_back(std::move(InSibling)); return std::move(*this); }
    FORCEINLINE LTabBarTabDescriptor&& operator+(LTabBarTabDescriptor&& InSibling) { return this->Sibling(std::move(InSibling)); }

    TArray<LTabBarTabDescriptor> Siblings;
};

DECLARE_JAFG_CLASS()
class JTabBarData : public JNodeData
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTORS(JTabBarData)

public:

    WTabBar* TabBar{};
    LTabBarTabDescriptor const* Descriptor{};
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
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTabBar)
class WTabBar : public WTabBarPanel
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WTabBar)
    {
        this->SetAnchor(EAnchor::TopLeft);
    }

public:

    virtual void Construct() override;

    FORCEINLINE void ResetWrapperClass() { this->WrapperClass = nullptr; }
    FORCEINLINE void SetWrapperClass(const TSubclassOf<WParent>& InWrapperClass) { this->WrapperClass = InWrapperClass; }
    FORCEINLINE void ResetDefaultAlignmentPreference() { this->bIsVertical.reset(); }
    FORCEINLINE void SetVerticalPreference() { this->bIsVertical = true; }
    FORCEINLINE void SetHorizontalPreference() { this->bIsVertical = false; }

    FORCEINLINE bool IsButtonContainerValid() const { return this->ButtonsContainer != nullptr; }
    FORCEINLINE auto GetButtonsContainer() -> WParent* { return this->ButtonsContainer; }
    FORCEINLINE auto GetButtonsContainer() const -> const WParent* { return this->ButtonsContainer; }
    FORCEINLINE bool IsSwitcherValid() const { return this->Switcher != nullptr; }
    FORCEINLINE auto GetSwitcher() -> WSwitcher* { return this->Switcher; }
    FORCEINLINE auto GetSwitcher() const -> const WSwitcher* { return this->Switcher; }

    void RegisterTab(LTabBarTabDescriptor&& InTabDescriptor);
    void AppendVSpace_v2(LWidgetSize1 VSpace);
    void AppendHSpace_v2(LWidgetSize1 HSpace);
    void AppendStretch(const LAnchor& InStretch);
    bool UnregisterTab(const LString& Identifier);
    bool UnregisterTabChecked(const LString& Identifier) { const bool bOut = this->UnregisterTab(Identifier); check( bOut ); return bOut; }

    template<typename TNode>
    FORCEINLINE void SetButtonsContainerClass() { this->SetButtonsContainerClass(TNode::StaticClass()); }
    FORCEINLINE void SetButtonsContainerClass(const TSubclassOf<WParent>& InButtonsContainerClass) { this->ButtonsContainerClass = InButtonsContainerClass; }
    FORCEINLINE auto GetCurrentButtonsContainerClass() const -> const TSubclassOf<WParent>& { return this->ButtonsContainerClass; }

    template<typename TNode>
    FORCEINLINE void SetSwitcherClass() { this->SetSwitcherClass(TNode::StaticClass()); }
    FORCEINLINE void SetSwitcherClass(const TSubclassOf<WSwitcher>& InSwitcherClass) { this->SwitcherClass = InSwitcherClass; }
    FORCEINLINE auto GetCurrentSwitcherClass() const -> const TSubclassOf<WSwitcher>& { return this->SwitcherClass; }

    FORCEINLINE i32  GetDefaultIndex() const { return this->DefaultIndex; }
    FORCEINLINE void SetDefaultIndex(const i32 InIndex) { this->DefaultIndex = InIndex; }

    FORCEINLINE bool IsAllowNone() const { return this->bAllowNone; }
    FORCEINLINE void SetAllowNone(const bool InAllowNone) { this->bAllowNone = InAllowNone; }

    void ResetToDefault();
    void ActivateTab(const LString& Identifier);
    void OnTabBarButtonReleased(const LString& Identifier);

private:

    void LoadTab(LTabBarTabDescriptor&& InTabDescriptor, const i32 InIndex);

protected:

    struct LAddedTabBarTab;

    //# An optional class to set that wraps both the buttons section and the switcher.
    TSubclassOf<WParent> WrapperClass;
    //#
    //# The preference for alignment of buttons and switcher if the wrapper class is not set.
    //# If this is not set, then there will not occur any wrapping.
    //#
    TOptional<bool> bIsVertical{ false };

    //# The container where the buttons are stored.
    WParent* ButtonsContainer{};
    TSubclassOf<WParent> ButtonsContainerClass;
    TSubclassOf<WNode> DefaultButtonClass;

    //# The switcher where the panels are stored.
    WSwitcher* Switcher{};
    TSubclassOf<WSwitcher> SwitcherClass;

    void const* CurrentlyFocusedTab{};
                const LAddedTabBarTab* GetCurrentlyFocusedTab() const;
    FORCEINLINE const LAddedTabBarTab* GetCurrentlyFocusedTabChecked() const;
    FORCEINLINE const LAddedTabBarTab* GetCurrentlyFocusedTabPanicked() const;

    struct LAddedTabBarTab final
    {
        LString Identifier;
        WNode* Button{ nullptr };
        WNode* Panel{ nullptr };
        i8 SwitcherIndex{ INDEX_NONE };
    };
    TArray<LAddedTabBarTab> TabsInOrder;
    TArray<LTabBarTabDescriptor> DeferredTabs;

    //#
    //# The default index to switch to when the tab bar is being made visible, or #ResetToDefault is called
    //# INDEX_NONE for no tab to be present.
    //#
    i32 DefaultIndex{ INDEX_NONE };

    //#
    //# Whether to allow none to be selected.
    //#
    bool bAllowNone{ true };
};

struct LFactoryTabBar : NODE_FACTORY_PARENT(WTabBar)
{
    NODE_FACTORY_BODY(WTabBar)

    decltype(auto) AlignHorizontal(this auto&& Self)
    {
        NODE_FACTORY_SELF().SetHorizontalPreference();
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) AlignVertical(this auto&& Self)
    {
        NODE_FACTORY_SELF().SetVerticalPreference();
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) AddTab(this auto&& Self, LTabBarTabDescriptor&& InDescriptor)
    {
        NODE_FACTORY_SELF().RegisterTab(std::move(InDescriptor));
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) AllowNone(this auto&& Self)
    {
        NODE_FACTORY_SELF().SetAllowNone(true);
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) DisallowNone(this auto&& Self)
    {
        NODE_FACTORY_SELF().SetAllowNone(false);
        return NODE_FACTORY_RESULT();
    }

    decltype(auto) DefaultIndex(this auto&& Self, const i32 InIndex)
    {
        NODE_FACTORY_SELF().SetDefaultIndex(InIndex);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */

Jafg::WTabBar::LAddedTabBarTab const* Jafg::WTabBar::GetCurrentlyFocusedTabChecked() const
{
    LAddedTabBarTab const* Out{this->GetCurrentlyFocusedTab()};
    check(Out)
    return Out;
}

Jafg::WTabBar::LAddedTabBarTab const* Jafg::WTabBar::GetCurrentlyFocusedTabPanicked() const
{
    if (LAddedTabBarTab const* Out{this->GetCurrentlyFocusedTab()}; Out)
    {
        return Out;
    }

    panic("No currently focused tab found.")
}
