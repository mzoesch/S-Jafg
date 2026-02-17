// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "HRegion.generated.h"

namespace Jafg
{

struct LFactoryHRegion;

//#
//# Horizontal region widget. Children are stacked next to each other. From left to right.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryHRegion)
class ENGINE_API WHRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WHRegion)

public:

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const override;
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const override;

    FORCEINLINE void SetHSpace(const f32 InHSpace) { this->HSpace = InHSpace; }
    FORCEINLINE f32  GetHSpace() const { return this->HSpace; }

private:

    //# Horizontal space between children.
    f32 HSpace{};
};

struct LFactoryHRegion : NODE_FACTORY_PARENT(WHRegion)
{
    NODE_FACTORY_BODY(WHRegion)

    FORCEINLINE decltype(auto) HSpace(this auto&& Self, const f32 InHSpace) noexcept
    {
        NODE_FACTORY_SELF().SetHSpace(InHSpace);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
