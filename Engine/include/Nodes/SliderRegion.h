// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Region.h"
#include "SliderRegion.generated.h"

namespace Jafg
{

struct LFactorySliderRegion;

struct LSliderBrush
{
    LColor Tint{ Colors::White };
    bool bSkipBrushDraw{};

    ENGINE_API void Draw(LNodeRenderInfo const& Info, LRect2F const& Rect, f64 Value) const noexcept;
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactorySliderRegion)
class WSliderRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WSliderRegion)
    {
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual LNodeReply OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event) override;

    //# @return True, if the value is acceptable, false otherwise.
    EVENT_DECL(OnValueChanged, bool(f64 Value))

    LSliderBrush SliderBrush;

    //# The value of this slider. Has to be [0.0, 1.0].
    f64 Value{};
};

struct LFactorySliderRegion: NODE_FACTORY_PARENT(WSliderRegion)
{
    NODE_FACTORY_BODY(WSliderRegion)

    decltype(auto) SliderBrush(this auto&& Self, LSliderBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().SliderBrush = Brush;
        return Self;
    }
    decltype(auto) SliderTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().SliderBrush.Tint = Tint;
        return Self;
    }
    decltype(auto) SkipSliderTintBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SliderBrush.bSkipBrushDraw = bInSkip;
        return Self;
    }

    decltype(auto) Value(this auto&& Self, f64 Value) noexcept
    {
        check(Value >= 0.0 && Value <= 1.0)
        NODE_FACTORY_SELF().Value = Value;
        return Self;
    }

    JAFG_NODE_FACTORY_DELEGATE_BINDINGS(OnValueChanged, OnValueChanged)
};

} /* ~Namespace Jafg */
