// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "Nodes/Font.h"
#include "Text.generated.h"

namespace Jafg
{

struct LFactoryText;

struct LTextBrush : public LTextBrushBase
{
    LColor Tint{ Colors::White };
    LColor OutlineTint{ Colors::White };
    f32 OutlineThickness{};
    bool bSkipBrushDraw{};
    LPadding Padding;
};

//# A node that can display text.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryText)
class ENGINE_API WText : public WNode, public TMutableTextContainer<WText>
{
    GENERATED_CLASS_BODY()

protected:

    explicit WText(LNodeDynamicInit const& Init) noexcept
        : Super{Init}, TMutableTextContainer{*this}
    {
    }
    template<typename TCxxClass>
    explicit WText(TNodeStaticInit<TCxxClass> const& Init) noexcept
        : Super{Init}, TMutableTextContainer{*this}
    {
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    LTextBrush TextBrush;
    inline constexpr LVec2F GetRelativeTextTopLeft() const noexcept
    {
        return this->TextBrush.Padding.GetTopLeftOffset().InStaticPoints(this->GetViewport())
            + TextLocation.GetRelativeTopLeft(
                this->GetAnchoredSize_v2() - this->TextBrush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
                , *this
                );
    }
};

struct LFactoryText : public TFactoryMutableTextContainer<WText, &WText::TextBrush>
{
    NODE_FACTORY_BODY(WText)

    constexpr decltype(auto) TextBrush(this auto&& Self, LTextBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().TextBrush = Brush;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.Tint = Tint;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextOutlineTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.OutlineTint = Tint;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextOutlineThickness(this auto&& Self, f32 Thickness) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.OutlineThickness = Thickness;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) SkipTextBrushDraw(this auto&& Self, bool bSkip) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.bSkipBrushDraw = bSkip;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Padding(this auto&& Self, LPadding const& Padding)
    {
        NODE_FACTORY_SELF().TextBrush.Padding = Padding;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
