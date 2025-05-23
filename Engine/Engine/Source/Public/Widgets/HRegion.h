// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Region.h"
#include "HRegion.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryHRegion : public TWidgetFactoryRegion<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryRegion)

    FORCEINLINE TFactoryRetTy& HSpace(const f32 InHSpace) { this->This()->SetHSpace(InHSpace); return this->Self(); }
};

//#
//# Horizontal region widget. Children are stacked next to each other. From left to right.
//#
DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryHRegion)
class ENGINE_API WHRegion : public WRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WHRegion)

public:

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const override;
    virtual auto GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const -> LVector2 override;

    FORCEINLINE void SetHSpace(const f32 InHSpace) { this->HSpace = InHSpace; }
    FORCEINLINE f32  GetHSpace() const { return this->HSpace; }

private:

    //# Vertical space between children.
    f32 HSpace { 0.0f };
};

} /* ~Namespace Jafg */
