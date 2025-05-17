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

    DEFAULT_OBJECT_CONSTRUCTOR(WOverlay)

public:

    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSize(const LViewport& Context) const override;
    virtual void UpdateAnchoredSizeForChild(const LViewport& Context, const WNode* InDirectChild) const override;
    virtual auto GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WNode* InDirectChild) const -> LVector2 override;
};

} /* ~Namespace Jafg */
