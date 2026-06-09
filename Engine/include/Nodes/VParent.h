// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Parent.h"
#include "Nodes/Controlflow.h"
#include "VParent.generated.h"

namespace Jafg
{

struct LFactoryVParent;

//# Vertical parent node. Children are stacked underneath to each other. From top to bottom.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryVParent)
class ENGINE_API WVParent : public WParent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WVParent)

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
    LNodeSize1 Space{};
};

struct LFactoryVParent : NODE_FACTORY_PARENT(WVParent)
{
    NODE_FACTORY_BODY(WVParent)

    FORCEINLINE decltype(auto) Space(this auto&& Self, LNodeSize1 Space) noexcept
    {
        NODE_FACTORY_SELF().Space = Space;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
