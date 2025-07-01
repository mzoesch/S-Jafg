// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "DebugMenu.generated.h"

namespace Jafg
{

class WTextBlock;

DECLARE_JAFG_WIDGET()
class WDebugMenu final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WDebugMenu)

public:

    virtual void Construct() override;
    virtual void Tick() override;
    virtual void OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility) override;
};

} /* ~Namespace Jafg */
