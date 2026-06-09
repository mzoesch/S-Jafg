// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Region.h"
#include "Nodes/Controlflow.h"
#include "VRegion.generated.h"

namespace Jafg
{

struct LFactoryVRegion;

//# Vertical region node. Children are stacked underneath each other. From top to bottom.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryVRegion)
class ENGINE_API WVRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WVRegion)

public:

    virtual void UpdateDesiredSize() const override
    {
        Super::UpdateDesiredSize();
        this->SetDesiredSizeInSpt(VerticalControlFlow.UpdateDesiredSize(*this, this->Space));
        return;
    }
    virtual LVec2F GetAnchoredSizeForChild(WNode const& DirectChild) const override
    {
        return VerticalControlFlow.GetAnchoredSizeForChild(*this, DirectChild, this->Space);
    }
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(WNode const& DirectChild) const override
    {
        return VerticalControlFlow.GetAnchoredTopLeftFromMostOuterForChild(*this, DirectChild, this->Space);
    }

    //# Vertical space between children.
    LNodeSize1 Space;
};

struct LFactoryVRegion : NODE_FACTORY_PARENT(WVRegion)
{
    NODE_FACTORY_BODY(WVRegion)

    decltype(auto) Space(this auto&& Self, LNodeSize1 Space) noexcept
    {
        NODE_FACTORY_SELF().Space = Space;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
