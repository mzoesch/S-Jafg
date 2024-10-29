// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Widgets/WidgetNode.h"

namespace Jafg
{

class WWidgetNode;
struct LSubsystemCollection;

MAKE_EXTERNAL_TEMPLATE_DHARRAY(ENGINE, WWidgetNode*)

class ENGINE_API LHud final
{
public:

    LHud() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LHud)
    ~LHud() = default;

    void Initialize(Private::LObjectContext* InOuter);
    void Tick();
    void Draw();
    void TearDown();

private:

    Private::LObjectContext* Outer;
    LSubsystemCollection*    Collection;
    TdhArray<WWidgetNode*>   TopLevelWidgets;
};

} /* ~Namespace Jafg. */
