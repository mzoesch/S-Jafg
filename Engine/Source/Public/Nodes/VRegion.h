// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Region.h"
#include "VRegion.generated.h"

namespace Jafg
{

struct LFactoryVRegion;

//# Vertical region widget. Children are stacked underneath each other. From top to bottom.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryVRegion)
class ENGINE_API WVRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WVRegion)

public:

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const override;
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const override;

    constexpr void SetVSpace(const LWidgetSize1 InVSpace) { this->VSpace = InVSpace; }
    constexpr LWidgetSize1 GetVSpace() const { return this->VSpace; }

private:

    //# Vertical space between children.
    LWidgetSize1 VSpace;
};

struct LFactoryVRegion : NODE_FACTORY_PARENT(WVRegion)
{
    NODE_FACTORY_BODY(WVRegion)

    FORCEINLINE decltype(auto) VSpace(this auto&& Self, LWidgetSize1 InHSpace) noexcept
    {
        NODE_FACTORY_SELF().SetVSpace(InHSpace);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
