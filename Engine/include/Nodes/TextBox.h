// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Box.h"
#include "Nodes/Font.h"
#include "Nodes/Iconized.h"
#include "TextBox.generated.h"

namespace Jafg
{

class JFontSubsystem;
class WTextBoxIconizedLeft;
class WTextBoxIconizedRight;
struct LFactoryTextBox;
typedef TFactoryIconized<WTextBoxIconizedLeft> LFactoryTextBoxIconizedLeft;
typedef TFactoryIconized<WTextBoxIconizedRight> LFactoryTextBoxIconizedRight;

struct LTextBoxBrush : public LTextBrushBase
{
    LColor Tint{ Colors::White };
    LColor OutlineTint{ Colors::White };
    f32 OutlineThickness{};
    bool bSkipBrushDraw{};
};

//# A box that can display text.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextBox)
class ENGINE_API WTextBox : public WBox, public TMutableTextContainer<WTextBox>
{
    GENERATED_CLASS_BODY()

    // TODO:
    //      Multiple fonts (not in one box but in general - for multiple fonts in one textbox, make that an extra class).
    //      New lines.
    //      Text wrapping (cutoff or with ...)
    //      Spacing between glyphs and lines.

protected:

    explicit WTextBox(LNodeDynamicInit const& Init) noexcept
        : Super{Init}, TMutableTextContainer{*this}
    {
        this->Brush.Tint = Colors::Black;
        this->Brush.Padding = {4_spt, 0};
    }
    template<typename TCxxClass>
    explicit WTextBox(TNodeStaticInit<TCxxClass> const& Init) noexcept
        : Super{Init}, TMutableTextContainer{*this}
    {
        this->Brush.Tint = Colors::Black;
        this->Brush.Padding = {4_spt, 0};
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    LTextBoxBrush TextBrush;
    inline constexpr LVec2F GetRelativeTextTopLeft() const noexcept
    {
        return this->Brush.Padding.GetTopLeftOffset().InStaticPoints(this->GetViewport())
            + TextLocation.GetRelativeTopLeft(
                  this->GetAnchoredSize_v2() - this->Brush.Padding.GetDesiredSize().InStaticPoints(this->GetViewport())
                , *this
                );
    }
};

//#
//# A text box that also has the ability to display an icon on the left hand side.
//# Unlike textures. An icon only has a width and does, therefore, not affect the height of this node.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextBoxIconizedLeft)
class ENGINE_API WTextBoxIconizedLeft : public WTextBox, public LIconized
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTextBoxIconizedLeft)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

private:

    LVec2F GetIconTopLeft(LVec2F Translation) const noexcept
    {
        return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
            + IconLocation.GetRelativeLeftIconTopLeft
                <&WTextBoxIconizedLeft::Icon, &WTextBoxIconizedLeft::IconBrush>(*this, this->Brush.Padding);
    }
};

//#
//# A text box that also has the ability to display an icon on the right hand side.
//# Unlike textures. An icon only has a width and does, therefore, not affect the height of this node.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextBoxIconizedRight)
class ENGINE_API WTextBoxIconizedRight : public WTextBox, public LIconized
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTextBoxIconizedRight)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

private:

    LVec2F GetIconTopLeft(LVec2F Translation) const noexcept
    {
        return this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)
            + IconLocation.GetRelativeRightIconTopLeft
                <&WTextBoxIconizedRight::Icon, &WTextBoxIconizedRight::IconBrush>(*this, this->Brush.Padding);
    }
};

struct LFactoryTextBox : public TFactoryMutableTextContainer<WTextBox, &WTextBox::TextBrush>
{
    NODE_FACTORY_BODY(WTextBox)

    constexpr decltype(auto) TextBrush(this auto&& Self, LTextBoxBrush const& Brush) noexcept
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
};

} /* ~Namespace Jafg */
