// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/HRegion.h"
#include "Input_Slider.generated.h"

namespace Jafg
{

class WSliderRegion;
class WEditableTextButton;
struct LFactoryInput_Slider;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryInput_Slider)
class ENGINE_API WInput_Slider : public WHRegion
{
    GENERATED_CLASS_BODY()

    /* For initial setup. */
    friend LFactoryInput_Slider;

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WInput_Slider)
    {
        this->Brush.Tint = Colors::Transparent;
        this->Brush.OutlineTint = Colors::White;
        this->Brush.OutlineThickness = 1;
    }

public:

    virtual void Construct() override;

    NODISCARD LString GetFormattedValueString() const noexcept;

    NODISCARD constexpr f64 GetValue() const noexcept { return this->Value; }
    NODISCARD constexpr f64 GetStep() const noexcept { return this->Step; }
    NODISCARD constexpr f64 GetMinimum() const noexcept { return this->Minimum; }
    NODISCARD constexpr f64 GetMaximum() const noexcept { return this->Maximum; }

    EVENT_DECL(OnValueChanged, void(f64 Value))

private:

    f64 Value{};
    f64 Step{};
    f64 Minimum{};
    f64 Maximum{};

    WSliderRegion* SliderRegion{};
    WEditableTextButton* EditableTextButton{};
};

struct LFactoryInput_Slider: NODE_FACTORY_PARENT(WInput_Slider)
{
    NODE_FACTORY_BODY(WInput_Slider)

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnValueChanged, OnValueChanged)

    decltype(auto) Value(this auto&& Self, f64 InValue) noexcept
    {
        NODE_FACTORY_SELF().Value = InValue;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Step(this auto&& Self, f64 InStep) noexcept
    {
        NODE_FACTORY_SELF().Step = InStep;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Minimum(this auto&& Self, f64 InMinimum) noexcept
    {
        NODE_FACTORY_SELF().Minimum = InMinimum;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Maximum(this auto&& Self, f64 InMaximum) noexcept
    {
        NODE_FACTORY_SELF().Maximum = InMaximum;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
