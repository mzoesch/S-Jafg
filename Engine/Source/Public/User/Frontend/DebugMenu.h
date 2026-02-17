// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/UserWidget.h"
#include "DebugMenu.generated.h"

namespace Jafg
{

class WTextBox;

DECLARE_JAFG_WIDGET()
class WDebugMenu final : public WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WDebugMenu)

public:

    virtual void Construct() override;
    virtual void OnVisibilityChanged(ENodeVisibility OldVisibility, ENodeVisibility NewVisibility) override;
};

} /* ~Namespace Jafg */
