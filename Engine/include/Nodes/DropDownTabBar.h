// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/DropDown.h"
#include "Nodes/FloatingWidget.h"
#include "DropDownTabBar.generated.h"

namespace Jafg
{

class WTextBox;
struct LFactoryDropDownTabbar;

struct LTabbarCreateInfo
{
    TArray<LDropDownNodeParent> SubMenus;
};

//# A drop-down tab-bar is a collection widget for drop-downs.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryDropDownTabbar)
class ENGINE_API WDropDownTabBar : public WUserWidget
{
    GENERATED_CLASS_BODY()

    /* For initial construction. */
    friend LFactoryDropDownTabbar;

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WDropDownTabBar)
    {
        this->Anchor = EAnchor::HFill;
    }

public:

    virtual void Construct() override;

private:

    void Select(WTextBox* Target = nullptr);
    void OnDismiss(WFloatingWidget& FloatingWidget);
    LNodeReply OnMouseEnterInRoot(WNode& Node, LDropDownNodeParent const& Submenu);

    LTabbarCreateInfo Tabs;
    WParent* RootSubmenuContainer{};
    std::unordered_map<LDropDownNodeParent const*, TJxxUnique<WFloatingWidget>> OpenSubmenus;
};

struct LFactoryDropDownTabbar : NODE_FACTORY_PARENT(WDropDownTabBar)
{
    NODE_FACTORY_BODY(WDropDownTabBar)

    constexpr decltype(auto) Tabs(this auto&& Self, LTabbarCreateInfo Tabs) noexcept
    {
        NODE_FACTORY_SELF().Tabs = std::move(Tabs);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
