// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "Crosshair.generated.h"

namespace Jafg
{

DECLARE_JAFG_WIDGET()
class ENGINE_API WCrosshair : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WCrosshair)

public:

    // WUserWidget implementation
    virtual void Construct() override;
    // ~WUserWidget implementation
};

} /* ~Namespace Jafg */
