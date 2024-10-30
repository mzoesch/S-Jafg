// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "DebugScreen.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API WDebugScreen final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WDebugScreen)

public:

    // WUserWidget implementation
    virtual void Tick() override;
    // ~WUserWidget implementation
};

} /* ~Namespace Jafg */
