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
        this->SetDesiredSizeInSpt(HorizontalControlFlow.UpdateDesiredSize(*this, this->Space));
        return;
    }
    virtual LVec2F GetAnchoredSizeForChild(WNode const& DirectChild) const override
    {
        return HorizontalControlFlow.GetAnchoredSizeForChild(*this, DirectChild, this->Space);
    }
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(WNode const& DirectChild) const override
    {
        return HorizontalControlFlow.GetAnchoredTopLeftFromMostOuterForChild(*this, DirectChild, this->Space);
    }

    //# Horizontal space between children.
    LNodeSize1 Space;
};

struct LFactoryHRegion : NODE_FACTORY_PARENT(WHRegion)
{
    NODE_FACTORY_BODY(WHRegion)

    FORCEINLINE decltype(auto) Space(this auto&& Self, LNodeSize1 Space) noexcept
    {
        NODE_FACTORY_SELF().Space = Space;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
