// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"
#include "Nodes/UserWidget.h"
#include "JgcPauseScreen.generated.h"

namespace Jgc
{

DECLARE_JAFG_WIDGET()
class WPauseScreen : public Jafg::WUserWidget
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WIDGET_CONSTRUCTORS(WPauseScreen)

public:

    void Construct() override;
};

} /* ~Namespace Jgc */
