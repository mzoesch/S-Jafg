// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/Hud.h"
#include "Subsystems/HudSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Widgets/WidgetNode.h"
#include "Widgets/WidgetRegion.h"

void Jafg::LHud::Initialize(Private::LObjectContext* InOuter)
{
    this->Outer = InOuter;

    checkSlow( this->Collection == nullptr )

    this->Collection = new LSubsystemCollection(this->Outer);
    this->Collection->LocateAllSubsystemsOfClass(JHudSubsystem::StaticClass());
    this->Collection->InitializeSubsystems();

    return;
}

void Jafg::LHud::Tick()
{
    this->Collection->ForEachSubsystem( [] (JSubsystem* Subsystem)
    {
        return;
    });

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

void Jafg::LHud::TearDown()
{
    this->Collection->TearDownSubsystems();

    for (WWidgetNode* Widget : this->TopLevelWidgets)
    {
        Widget->MarkAsGarbage();
    }

    this->TopLevelWidgets.Empty();

    return;
}
