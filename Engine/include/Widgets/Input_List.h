// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextBox.h"
#include "Input_List.generated.h"

namespace Jafg
{

struct LFactoryInput_List;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryInput_List)
class ENGINE_API WInput_List : public WTextBoxIconizedRight
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WInput_List)
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual void Construct() override;
    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    TArray<LString> Items;

    //# Called if the selected item changed.
    EVENT_DECL(OnValueChanged, void(LString const& Selected))
};

struct LFactoryInput_List: NODE_FACTORY_PARENT(WInput_List)
{
    NODE_FACTORY_BODY(WInput_List)

    decltype(auto) Selected(this auto&& Self, LString Selected) noexcept
    {
        return std::forward<decltype(Self)>(Self).Content(std::move(Selected));
    }
    decltype(auto) Items(this auto&& Self, TArray<LString> Items) noexcept
    {
        NODE_FACTORY_SELF().Items = std::move(Items);
        return NODE_FACTORY_RESULT();
    }

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnValueChanged, OnValueChanged)
};

} /* ~Namespace Jafg */
