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
        this->SetDesiredSizeInSpt(VerticalControlFlow.UpdateDesiredSize(*this, this->VSpace));
        return;
    }
    virtual LVec2F GetAnchoredSizeForChild(WNode const& DirectChild) const override
    {
        return VerticalControlFlow.GetAnchoredSizeForChild(*this, DirectChild, this->VSpace);
    }
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(WNode const& DirectChild) const override
    {
        return VerticalControlFlow.GetAnchoredTopLeftFromMostOuterForChild(*this, DirectChild, this->VSpace);
    }

    //# Vertical space between children.
    LNodeSize1 VSpace;
};

struct LFactoryVRegion : NODE_FACTORY_PARENT(WVRegion)
{
    NODE_FACTORY_BODY(WVRegion)

    decltype(auto) VSpace(this auto&& Self, LNodeSize1 VSpace) noexcept
    {
        NODE_FACTORY_SELF().VSpace = VSpace;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
