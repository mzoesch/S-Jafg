// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Region.h"
#include "Nodes/DynRegion.h"
#include "DynRegion.generated.h"

namespace Jafg
{

struct LFactoryDynRegion;

//#
//# A region that can dynamically control the layout of its children.
//# The orchestration of the control flow is restricted to the deriving class.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryDynRegion, EJxxClassBits::Abstract)
class ENGINE_API WDynRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WDynRegion)

public:

    virtual void UpdateDesiredSize() const override
    {
        Super::UpdateDesiredSize();
        this->SetDesiredSizeInSpt(DynamicControlFlowOrchestration.UpdateDesiredSize(this->Cf, *this, this->Space));
        return;
    }
    virtual LVec2F GetAnchoredSizeForChild(WNode const& DirectChild) const override
    {
        check(DirectChild.TransformsWidgetLayout())
        if (!DirectChild.Anchor.IsStretchedHorizontal())
        {
            return Super::GetAnchoredSizeForChild(DirectChild);
        }
        return DynamicControlFlowOrchestration.GetAnchoredSizeForChild(this->Cf, *this, DirectChild, this->Space);
    }
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(WNode const& DirectChild) const override
    {
        return DynamicControlFlowOrchestration.GetAnchoredTopLeftFromMostOuterForChild(this->Cf, *this, DirectChild, this->Space);
    }

    //# Might get ignored in some control flows. This is the desired space inbetween children.
    LNodeSize1 Space;

protected:

    ENodePrimitiveControlFlow Cf{ ENodePrimitiveControlFlow::Horizontal };
};

struct LFactoryDynRegion : NODE_FACTORY_PARENT(WDynRegion)
{
    NODE_FACTORY_BODY(WDynRegion)

    FORCEINLINE decltype(auto) Space(this auto&& Self, LNodeSize1 Space) noexcept
    {
        NODE_FACTORY_SELF().Space = Space;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
