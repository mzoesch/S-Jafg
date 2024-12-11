// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "PauseMenu.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API WPauseMenu final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WPauseMenu)

public:

    virtual void Construct() override;
};

} /* ~Namespace Jafg */
