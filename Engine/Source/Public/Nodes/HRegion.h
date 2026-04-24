// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Region.h"
#include "HRegion.generated.h"

namespace Jafg
{

struct LFactoryHRegion;

//# Horizontal region widget. Children are stacked next to each other. From left to right.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryHRegion)
class ENGINE_API WHRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WHRegion)

public:

    virtual void UpdateDesiredSize() const override;
    virtual LVec2F GetAnchoredSizeForChild(WNode const* DirectChild) const override;
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(WNode const* DirectChild) const override;

    //# Horizontal space between children.
    LWidgetSize1 HSpace{};
};

struct LFactoryHRegion : NODE_FACTORY_PARENT(WHRegion)
{
    NODE_FACTORY_BODY(WHRegion)

    FORCEINLINE decltype(auto) HSpace(this auto&& Self, LWidgetSize1 HSpace) noexcept
    {
        NODE_FACTORY_SELF().HSpace = HSpace;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
