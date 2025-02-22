// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetRegion.h"
#include "VBox.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET()
class ENGINE_API WVBox : public WWidgetRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WVBox)

public:

    virtual auto GetRelativeTopLeftForChild(const WWidgetNode* InDirectChild) const -> LVector2 override;
    virtual void UpdateDesiredSize() const override;
    virtual auto GetAnchoredTopLeftFromMostOuterForChild(const LViewport& Context, const WWidgetNode* InDirectChild) const -> LVector2 override;
};

} /* ~Namespace Jafg */
