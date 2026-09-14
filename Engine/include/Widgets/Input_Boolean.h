// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/HRegion.h"
#include "Input_Boolean.generated.h"

namespace Jafg
{

class WCheckmarkButton;
struct LFactoryInput_Boolean;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryInput_Boolean)
class ENGINE_API WInput_Boolean : public WHRegion
{
    GENERATED_CLASS_BODY()

    /* For initial setup. */
    friend LFactoryInput_Boolean;

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WInput_Boolean)
    {
        this->Brush.Tint = Colors::Transparent;
        this->Brush.OutlineTint = Colors::White;
        this->Brush.OutlineThickness = 1;
    }

public:

    virtual void Construct() override;

    NODISCARD LString GetFormattedValueString() const noexcept
    {
        LString Result;
        if (this->GetValue())
        {
            Result = "true";
        }
        else
        {
            Result = "false";
        }
        return Result;
    }
    NODISCARD ENGINE_API bool GetValue() const noexcept;

    //#
    //# Called if the value should be toggled.
    //# If the result is different from the #Current, #OnValueChanged will be called.
    //#
    //# If not bound the default implementation will be used with: value = !value.
    //#
    TFunction2<bool(bool Current)> OnToggle;

    //# Called iff the value actually changed from on toggle.
    EVENT_DECL(OnValueChanged, void(f64 Value))

private:

    std::optional<bool> bInitialValue;
    WCheckmarkButton* CheckmarkButton{};
};

struct LFactoryInput_Boolean: NODE_FACTORY_PARENT(WInput_Boolean)
{
    NODE_FACTORY_BODY(WInput_Boolean)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnToggle, OnToggle)
    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnValueChanged, OnValueChanged)

    decltype(auto) Value(this auto&& Self, bool Value) noexcept
    {
        NODE_FACTORY_SELF().bInitialValue = Value;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
