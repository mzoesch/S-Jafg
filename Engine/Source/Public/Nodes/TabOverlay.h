// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Overlay.h"
#include "Nodes/Switcher.h"
#include "Nodes/TextButton.h"
#include "Nodes/GenericTabInfos.h"
// #include "Nodes/UserWidget.h"
#include "TabOverlay.generated.h"

namespace Jafg
{

class WTabOverlaySelector;
struct LFactoryTabOverlay;
struct LTabCreateInfo;

//#
//# A tab overlay is an overlay widget with a switcher and a selector container.
//# It automatically provides common standard tab logic to its children.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTabOverlay)
class ENGINE_API WTabOverlay : public WOverlay
{
    GENERATED_CLASS_BODY()

    //# For subclass access.
    friend LFactoryTabOverlay;

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTabOverlay)

public:

    typedef std::pair<WTabOverlaySelector*, WUserWidget*> Tab;

    virtual void Construct() override;

    //# Registers a new tab. Do not call #AddChild or any of its variants.
    Tab  RegisterTab(LTabCreateInfo&& Descriptor);
    void CloseTab(WTabOverlaySelector* Selector);

    FORCEINLINE auto const& GetTabs() const noexcept { return this->Tabs; }

    WUserWidget* FindWidgetSlow(LCxxClass const& Class) noexcept;
    template<typename TWidget> requires std::is_base_of_v<WUserWidget, TWidget>
    TWidget* FindWidgetSlow() noexcept
    {
        for (auto& Panel: this->Tabs | std::views::values)
        {
            if (TWidget* Casted{DynamicCast<TWidget>(Panel)})
            {
                return Casted;
            }
        }
        return nullptr;
    }
    template<typename TWidget> requires std::is_base_of_v<WUserWidget, TWidget>
    TWidget const* FindWidgetSlow() const noexcept
    {
        for (auto const& Panel: this->Tabs | std::views::values)
        {
            if (TWidget const* Casted{DynamicCast<TWidget>(Panel)})
            {
                return Casted;
            }
        }
        return nullptr;
    }

private:

    void InitializeBoilerplate();
    void SetSelectedTab(WTabOverlaySelector& Target);

    TArray<Tab> Tabs;

    //# The class that wraps the tab buttons and the switcher.
    TNodeInjection<WParent> WrapperInjection;
    WParent* Wrapper{};
    //# The container for selectors.
    TNodeInjection<WParent> SelectorsInjection;
    WParent* Selectors{};
    //# The actual selectors.
    std::move_only_function<void(LFactoryTextButton& Factory)> DefaultSelectorDelegate;
    //# The switcher dictates where the content panels are stored.
    TNodeInjection<WSwitcher> SwitcherInjection;
    WSwitcher* Switcher{};
};

DECLARE_JAFG_WIDGET()
class ENGINE_API WTabOverlaySelector final : public WTextButton
{
    GENERATED_CLASS_BODY()

protected:

    explicit WTabOverlaySelector(LNodeDynamicInit const& Init) noexcept : Super{Init}
    {
        this->CtorLogic();
    }

    template<typename TCxxClass>
    explicit WTabOverlaySelector(TNodeStaticInit<TCxxClass> const& Init, LTabSelectorCreateInfo Info) noexcept : Super{Init}
    {
        this->CtorLogic();

        this->SetContent(std::move(Info.DisplayName));
        this->SetLeftIcon(Info.Icon.Resolve());
        return;
    }

private:

    inline void CtorLogic() noexcept
    {
        this->SetOmniPadding({3_spt, 0});
        this->SetNormalTextTint({0x90});
        this->SetNormalLeftIconTint({0x90});
        this->SetNormalRightIconTint({0x90});
        this->SetSelectable(true);
        this->LoadRightIcon();
    }

    void LoadRightIcon();
};

struct LFactoryTabOverlay : NODE_FACTORY_PARENT(WTabOverlay)
{
    NODE_FACTORY_BODY(WTabOverlay)

    JAFG_NODE_FACTORY_INJECTION(WParent, WrapperInjection, WrapperInjection)
    JAFG_NODE_FACTORY_INJECTION(WParent, SelectorsInjection, SelectorsInjection)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(DefaultSelectorDelegate, DefaultSelectorDelegate)
    JAFG_NODE_FACTORY_INJECTION(WSwitcher, SwitcherInjection, SwitcherInjection)

    decltype(auto) operator[](this auto&& Self, LTabCreateInfo Descriptor) noexcept
    {
        NODE_FACTORY_SELF().RegisterTab(std::move(Descriptor));
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
