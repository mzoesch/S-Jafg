// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "VRegion.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryVRegion : public TWidgetFactoryRegion<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryRegion)

    FORCEINLINE TFactoryRetTy& VSpace(const f32 InVSpace) { this->This()->SetVSpace(InVSpace); return this->Self(); }
};

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
    virtual auto GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const -> LVector2 override;

    FORCEINLINE void SetVSpace(const f32 InVSpace) { this->VSpace = InVSpace; }
    FORCEINLINE f32  GetVSpace() const { return this->VSpace; }

private:

    //# Vertical space between children.
    f32 VSpace { 0.0f };
};

} /* ~Namespace Jafg */
