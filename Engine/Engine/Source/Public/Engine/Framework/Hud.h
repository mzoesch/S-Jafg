// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

class WWidgetNode;

MAKE_EXTERNAL_TEMPLATE_DHARRAY(ENGINE, WWidgetNode*)

class ENGINE_API LHud final
{
public:

    LHud() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LHud)
    ~LHud() = default;

    void BeginLife();
    void Tick();
    void Draw();
    void EndLife();

private:

    TdhArray<WWidgetNode*> TopLevelWidgets;
};

} /* ~Namespace Jafg. */
