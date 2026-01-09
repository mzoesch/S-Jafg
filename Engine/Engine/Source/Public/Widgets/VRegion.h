// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "Widgets/VRegionFactory.h"
#include "VRegion.generated.h"

namespace Jafg
{

//#
//# Vertical region widget. Children are stacked underneath each other. From top to bottom.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryVRegion)
class ENGINE_API WVRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WVRegion)

public:

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const override;
    virtual LVector2D GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const override;

    FORCEINLINE void SetVSpace(const LWidgetSize1 InVSpace) { this->VSpace = InVSpace; }
    FORCEINLINE LWidgetSize1 GetVSpace() const { return this->VSpace; }

private:

    //# Vertical space between children.
    LWidgetSize1 VSpace;
};

} /* ~Namespace Jafg */
