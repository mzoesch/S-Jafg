// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Region.h"
#include "Nodes/Controlflow.h"
#include "HRegion.generated.h"

namespace Jafg
{

struct LFactoryHRegion;

//# Horizontal region node. Children are stacked next to each other. From left to right.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryHRegion)
class ENGINE_API WHRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WHRegion)

public:

    virtual void UpdateDesiredSize() const override
    {
        Super::UpdateDesiredSize();
        this->SetDesiredSizeInSpt(HorizontalControlFlow.UpdateDesiredSize(*this, this->HSpace));
        return;
    }
    virtual LVec2F GetAnchoredSizeForChild(WNode const& DirectChild) const override
    {
        return HorizontalControlFlow.GetAnchoredSizeForChild(*this, DirectChild, this->HSpace);
    }
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(WNode const& DirectChild) const override
    {
        return HorizontalControlFlow.GetAnchoredTopLeftFromMostOuterForChild(*this, DirectChild, this->HSpace);
    }

    //# Horizontal space between children.
    LNodeSize1 HSpace{};
};

struct LFactoryHRegion : NODE_FACTORY_PARENT(WHRegion)
{
    NODE_FACTORY_BODY(WHRegion)

    FORCEINLINE decltype(auto) HSpace(this auto&& Self, LNodeSize1 HSpace) noexcept
    {
        NODE_FACTORY_SELF().HSpace = HSpace;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
