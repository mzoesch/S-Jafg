// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "Separator.generated.h"

namespace Jafg
{

struct LFactoryHSeparator;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryHSeparator)
class ENGINE_API WHSeparator final : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WHSeparator)
    {
        this->Anchor = EAnchor::HFill;
        this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override
    {
        Super::UpdateDesiredSize();
        auto Size{this->Padding.GetDesiredSize().InStaticPoints(this->GetViewport())};
        Size.y += static_cast<f32>(this->Thickness);
        this->SetDesiredSizeInSpt(Size);
    }

    LPadding Padding{ 2.0_spt, 5.0f };
    LColor SeparatorTint{ Colors::Gray };
    //# Size in spt.
    u32 Thickness{ 1 };
};

struct LFactoryHSeparator : NODE_FACTORY_PARENT(WHSeparator)
{
    NODE_FACTORY_BODY(WHSeparator)

    decltype(auto) Padding(this auto&& Self, LPadding const& Padding)
    {
        NODE_FACTORY_SELF().Padding = Padding;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) SeparatorTint(this auto&& Self, LColor Tint) noexcept
    {
        NODE_FACTORY_SELF().SeparatorTint = Tint;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) Thickness(this auto&& Self, u32 Thickness) noexcept
    {
        NODE_FACTORY_SELF().Thickness = Thickness;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
