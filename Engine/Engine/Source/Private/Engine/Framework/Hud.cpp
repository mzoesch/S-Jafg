// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/Hud.h"
#include "Widgets/WidgetNode.h"
#include "Widgets/WidgetRegion.h"

void Jafg::LHud::BeginLife()
{
    // WWidgetRegion* Region = new WWidgetRegion();


    return;
}

void Jafg::LHud::Tick()
{
    for (WWidgetNode* Widget : this->TopLevelWidgets)
    {
        if (Widget->ShouldNowTick())
        {
            Widget->Tick();
        }

        continue;
    }

    return;
}

void Jafg::LHud::Draw()
{
}

void Jafg::LHud::EndLife()
{
    for (WWidgetNode* Widget : this->TopLevelWidgets)
    {
        Widget->Destruct();
        delete Widget;
    }

    this->TopLevelWidgets.Empty();

    return;
}
