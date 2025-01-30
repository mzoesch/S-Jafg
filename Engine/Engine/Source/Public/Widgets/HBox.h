// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetRegion.h"
#include "HBox.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET()
class ENGINE_API WHBox : public WWidgetRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WHBox)

public:

    // WWidgetNode implementation
    virtual auto GetRelativeTopLeftFromOuter(const WWidgetNode* WhoAsked) const -> LVector2 override;
    virtual auto GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const -> LVector2 override;
    virtual void UpdateDesiredSize() const override;
    virtual void UpdateAnchoredSize(const LViewport& Context) const override;
    // ~WWidgetNode implementation
};

} /* ~Namespace Jafg */
