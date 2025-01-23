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

    virtual auto GetRelativeTopLeftFromOuter(const WWidgetNode* WhoAsked) const -> LVector2 override;
    virtual auto GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const -> LVector2 override;
    // virtual auto GetAnchoredTopLeftFromMostOuter(const LViewport& Context, const WWidgetNode* WhoAsked) const -> LVector2 override;
    virtual auto UpdateDesiredSize() const -> void override;
    // virtual auto UpdateAnchoredSize(const LViewport& Context) const -> void override;
};

} /* ~Namespace Jafg */
