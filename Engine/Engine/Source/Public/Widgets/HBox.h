// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetRegion.h"
#include "HBox.generated.h"

namespace Jafg
{

//# Horizontal region widget. TODO: Rename this to region to not confuse with the widget box.
DECLARE_JAFG_WIDGET()
class ENGINE_API WHBox : public WWidgetRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WHBox)

public:

    virtual void UpdateDesiredSize() const override;
    virtual auto GetRelativeTopLeftForChild(const WWidgetNode* InDirectChild) const -> LVector2 override;
};

} /* ~Namespace Jafg */
