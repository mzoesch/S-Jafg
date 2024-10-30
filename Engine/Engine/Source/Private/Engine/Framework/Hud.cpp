// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/Hud.h"
#include "Core/Application.h"
#include "Subsystems/HudSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Widgets/UserWidget.h"
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
        if (JHudSubsystem* HudSubsystem = DynamicCast<JHudSubsystem>(Subsystem); HudSubsystem)
        {
            if (HudSubsystem->ShouldTick())
            {
                HudSubsystem->Tick(Application::GetDeltaTimeAsFloat());
            }

            return;
        }

        panicMsgf( "Could not cast predicated hud subsystem [{}] to JHudSubsystem.", Subsystem->GetFullName() )

        return;
    });

    for (WUserWidget* Widget : this->TopLevelWidgets)
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

void Jafg::LHud::AddWidget(WUserWidget* Widget)
{
    check( Widget )
    this->TopLevelWidgets.Add(Widget);

    return;
}

void Jafg::LHud::RemoveWidget(WUserWidget* Widget)
{
    this->TopLevelWidgets.RemoveOnceChecked(Widget);
}
