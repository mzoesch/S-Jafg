// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/TextIconized.h"
#include "TextIconizedSeparator.generated.h"

namespace Jafg
{

struct LFactoryTextIconizedLeftSeparator;
struct LFactoryTextIconizedRightSeparator;

//#
//# A text node, that may have separator line centered to the text height.
//# Depending on the text align the separator line may be on the left, right, or on both sides of the text.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextIconizedLeftSeparator)
class ENGINE_API WTextIconizedLeftSeparator final : public WTextIconizedLeft
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WTextIconizedLeftSeparator)
    {
        this->Anchor = EAnchor::HFill;
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;

    LColor SeparatorTint{ Colors::Gray };
    u32 TextOffset{ 5 };
    u32 Thickness{ 1 };
};

//#
//# A text node, that may have separator line centered to the text height.
//# Depending on the text align the separator line may be on the left, right, or on both sides of the text.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextIconizedRightSeparator)
class ENGINE_API WTextIconizedRightSeparator final : public WTextIconizedRight
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WTextIconizedRightSeparator)
    {
        this->Anchor = EAnchor::HFill;
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;

    LColor SeparatorTint{ Colors::Gray };
    u32 TextOffset{ 5 };
    u32 Thickness{ 1 };
};

struct LFactoryTextIconizedLeftSeparator : NODE_FACTORY_PARENT(WTextIconizedLeftSeparator)
{
    NODE_FACTORY_BODY(WTextIconizedLeftSeparator)

    constexpr decltype(auto) SeparatorTint(this auto&& Self, LColor Tint) noexcept
    {
        NODE_FACTORY_SELF().SeparatorTint = Tint;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextOffset(this auto&& Self, u32 Offset) noexcept
    {
        NODE_FACTORY_SELF().TextOffset = Offset;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) Thickness(this auto&& Self, u32 Thickness) noexcept
    {
        NODE_FACTORY_SELF().Thickness = Thickness;
        return NODE_FACTORY_RESULT();
    }
};

struct LFactoryTextIconizedRightSeparator : NODE_FACTORY_PARENT(WTextIconizedRightSeparator)
{
    NODE_FACTORY_BODY(WTextIconizedRightSeparator)

    constexpr decltype(auto) SeparatorTint(this auto&& Self, LColor Tint) noexcept
    {
        NODE_FACTORY_SELF().SeparatorTint = Tint;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextOffset(this auto&& Self, u32 Offset) noexcept
    {
        NODE_FACTORY_SELF().TextOffset = Offset;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) Thickness(this auto&& Self, u32 Thickness) noexcept
    {
        NODE_FACTORY_SELF().Thickness = Thickness;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
