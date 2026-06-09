// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Parent.h"
#include "Nodes/Controlflow.h"
#include "HParent.generated.h"

namespace Jafg
{

struct LFactoryHParent;

//# Horizontal parent node. Children are stacked next to each other. From left to right.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryHParent)
class ENGINE_API WHParent : public WParent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WHParent)

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
    LNodeSize1 Space{};
};

struct LFactoryHParent : NODE_FACTORY_PARENT(WHParent)
{
    NODE_FACTORY_BODY(WHParent)

    FORCEINLINE decltype(auto) Space(this auto&& Self, LNodeSize1 Space) noexcept
    {
        NODE_FACTORY_SELF().Space = Space;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
