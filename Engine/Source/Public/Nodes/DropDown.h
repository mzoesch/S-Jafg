// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Nodes/DropDownForward.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "DropDown.generated.h"

namespace Jafg
{

struct LFactoryDropDown;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryDropDown)
class ENGINE_API WDropDown final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WDropDown)
    {
        this->Anchor = EAnchor::HFill;
    }

public:

    virtual void Construct() override;

    inline void SetDropDownRoot(LDropDownRoot Root) noexcept
    {
        check(this->_HasBegunLife() == false)
        this->DropDownRoot = std::move(Root);
    }

private:

    void OnDismiss(WFloatingWidget& FloatingWidget);

    LCursorReply OnMouseEnterInRoot(WNode& Node, LDropDownNodeSubMenu const& Submenu);
    LCursorReply OnMouseLeaveInRoot(WNode& Node, LDropDownNodeSubMenu const& Submenu);
    LCursorReply OnMouseLeaveFloatingWidget(WNode& Node);

    LDropDownRoot DropDownRoot;
    WParent* RootSubmenuContainer{};
    std::unordered_map<LDropDownNodeSubMenu const*, TJxxUnique<WFloatingWidget>> OpenSubmenus;
};

struct LFactoryDropDown : NODE_FACTORY_PARENT(WDropDown)
{
    NODE_FACTORY_BODY(WDropDown)

    constexpr decltype(auto) DropDownRoot(this auto&& Self, LDropDownRoot Root) noexcept
    {
        NODE_FACTORY_SELF().SetDropDownRoot(std::move(Root));
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
