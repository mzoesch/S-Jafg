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

MAKE_DELEGATE_SIGNATURE(LOnTabBarLoaded, void, WTabBar* TabBar, WNode* Button, WNode* Panel)

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

    FORCEINLINE TFactoryRetTy& AddTab(LTabBarTabDescriptor&& InDescriptor) { this->This()->RegisterTab(std::move(InDescriptor)); return this->Self(); }
    FORCEINLINE TFactoryRetTy& AllowNone() { this->This()->SetAllowNone(true); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DisallowNone() { this->This()->SetAllowNone(false); return this->Self(); }
    FORCEINLINE TFactoryRetTy& DefaultIndex(const i32 InIndex) { this->This()->SetDefaultIndex(InIndex); return this->Self(); }

    FORCEINLINE TFactoryRetTy& operator[](LTabBarTabDescriptor&& InDescriptor) { return this->AddTab(std::move(InDescriptor)); }
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
class WTabBar : public WTabBarPanel
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTabBar(LCxxObjectInitializer const& CxxObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(WTabBar)

public:

    virtual void Construct() override;

    FORCEINLINE void ResetWrapperClass() { this->WrapperClass = nullptr; }
    FORCEINLINE void SetWrapperClass(const TSubclassOf<WParentBase>& InWrapperClass) { this->WrapperClass = InWrapperClass; }
    FORCEINLINE void ResetDefaultAlignmentPreference() { this->bIsVertical.reset(); }
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
    bool UnregisterTabChecked(const LString& Identifier) { const bool bOut = this->UnregisterTab(Identifier); check( bOut ); return bOut; }

    template <typename TNode>
    FORCEINLINE void SetButtonsContainerClass() { this->SetButtonsContainerClass(TNode::StaticClass()); }
    FORCEINLINE void SetButtonsContainerClass(const TSubclassOf<WParentBase>& InButtonsContainerClass) { this->ButtonsContainerClass = InButtonsContainerClass; }
    FORCEINLINE auto GetCurrentButtonsContainerClass() const -> const TSubclassOf<WParentBase>& { return this->ButtonsContainerClass; }

    template <typename TNode>
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

    //#
    //# An optional class to set that wraps both the buttons section and the switcher.
    //#
    TSubclassOf<WParentBase> WrapperClass;
    //#
    //# The preference for alignment of buttons and switcher if the wrapper class is not set.
    //# If this is not set, then there will not occur any wrapping.
    //#
    TOptional<bool> bIsVertical{ false };

    //#
    //# The container where the buttons are stored.
    //#
    WParentBase* ButtonsContainer{ nullptr };
    TSubclassOf<WParentBase> ButtonsContainerClass;
    TSubclassOf<WNode> DefaultButtonClass;

    //#
    //# The switcher where the panels are stored.
    //#
    WSwitcher* Switcher{ nullptr };
    TSubclassOf<WSwitcher> SwitcherClass;

    CDR_NULL_PTR(void const*) CurrentlyFocusedTab{ nullptr };
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
    TCdrEmptyArray<TArray<LAddedTabBarTab>> TabsInOrder;
    TCdrEmptyArray<TArray<LTabBarTabDescriptor>> DeferredTabs;

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
