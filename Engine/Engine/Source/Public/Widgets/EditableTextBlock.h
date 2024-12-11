// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetRegion.h"
#include "EditableTextBlock.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API WEditableTextBlock : public WWidgetRegion
{
    GENERATED_CLASS_BODY()

protected:

    explicit WEditableTextBlock(const LObjectInitializer& ObjectInitializer);

public:

    // WWidgetNode implementation
    virtual LCursorReply OnCursorEnter() override;
    virtual LCursorReply OnCursorLeave() override;
    // ~WWidgetNode implementation

    WIDGET_REGION_SUBCLASS_WSDSML_DECLARATIONS(WEditableTextBlock)
};

} /* ~Namespace Jafg */
