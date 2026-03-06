// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Parent.h"
#include "Overlay.generated.h"

namespace Jafg
{

//#
//# An overlay is a parent node that has simple but efficient (in terms of CPU cycles) drawing capabilities.
//# It has many children that can be individually anchored with the #EAnchor enum.
//# Children are, as the name suggests, overlaid on top of each other and are independent of each of their siblings.
//#
DECLARE_JAFG_WIDGET()
class ENGINE_API WOverlay : public WParent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WOverlay)

public:

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSize(const LViewport& Viewport) const override;
    virtual void UpdateAnchoredSizeForChild(const LViewport& Viewport, const WNode* InDirectChild) const override;
    virtual LVec2F GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Viewport, const WNode* InDirectChild) const override;
};

} /* ~Namespace Jafg */
