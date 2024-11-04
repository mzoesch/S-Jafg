// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Widgets/WidgetNode.h"

namespace Jafg
{
class LViewport;
}

namespace Jafg
{

class WUserWidget;
struct LSubsystemCollection;

/**
 * The hud is owned by the local player and shares its lifetime.
 * The hud is the main hub for all user interface elements. Create hud subsystems to automatically
 * add multiple widgets of a type to the viewport.
 */
class ENGINE_API LHud final
{
public:

    LHud() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LHud)
    ~LHud() = default;

    void Initialize(Private::LObjectContext* InOuter);
    void Tick();
    void TearDown();

    auto GetMainViewport() const -> LViewport* ;

    void AddWidget(WUserWidget* Widget) const;
    void RemoveWidget(WUserWidget* Widget) const;

private:

    Private::LObjectContext* Outer;
    LSubsystemCollection*    Collection;
};

} /* ~Namespace Jafg. */
