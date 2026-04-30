// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "Spacer.generated.h"

namespace Jafg
{

struct LFactorySpacer;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactorySpacer)
class ENGINE_API WSpacer final : public WNode
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WSpacer)

public:

    virtual void UpdateDesiredSize() const override { this->SetDesiredSize(this->Size); }

    LNodeSize2 Size;
};

struct LFactorySpacer : NODE_FACTORY_PARENT(WSpacer)
{
    NODE_FACTORY_BODY(WSpacer)

    decltype(auto) Size(this auto&& Self, LNodeSize2 const& Size) noexcept
    {
        NODE_FACTORY_SELF().Size = Size;
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Height(this auto&& Self, LNodeSize1 Height) noexcept
    {
        check(DETAIL_JAFG_NODE_FACTORY_SELF().Size.X == 0.0f)
        NODE_FACTORY_SELF().Size = LNodeSize2{0.0f, Height};
        return NODE_FACTORY_RESULT();
    }
    decltype(auto) Width(this auto&& Self, LNodeSize1 Width) noexcept
    {
        check(DETAIL_JAFG_NODE_FACTORY_SELF().Size.Y == 0.0f)
        NODE_FACTORY_SELF().Size = LNodeSize2{Width, 0.0f};
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
