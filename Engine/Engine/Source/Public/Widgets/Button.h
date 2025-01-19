// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/WidgetRegion.h"
#include "Button.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(EClassFlags::Abstract,)
class ENGINE_API WButton : public WWidgetRegion
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WButton)
};

} /* ~Namespace Jafg */
