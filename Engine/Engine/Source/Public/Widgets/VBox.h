// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetRegion.h"
#include "VBox.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API WVBox : public WWidgetRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WVBox)

public:

    virtual auto GetRelativeTopLeftFromMostOuter(const WWidgetNode* WhoAsked) const -> LVector2 override;
    virtual auto UpdateDesiredSize() const -> void override;
};

} /* ~Namespace Jafg. */
