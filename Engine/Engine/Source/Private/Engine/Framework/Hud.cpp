// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Framework/Hud.h"
#include "Core/Application.h"
#include "Subsystems/HudSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Widgets/WidgetNode.h"
#include "Widgets/Viewport.h"

#include "Widgets/WidgetRegion.h"

void Jafg::LHud::Initialize(Private::LObjectContext* InOuter)
{
    this->Outer                = InOuter;
    GCurrentWidgetContextState = this->Outer;

    this->MainViewport = new LViewport();
    this->MainViewport->Initialize();

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

    this->MainViewport->Tick();

    return;
}

void Jafg::LHud::Draw()
{
}

void Jafg::LHud::TearDown()
{
    this->Collection->TearDownSubsystems();
    delete this->Collection;
    this->Collection = nullptr;

    this->MainViewport->TearDown();
    delete this->MainViewport;
    this->MainViewport = nullptr;

    return;
}

void Jafg::LHud::AddWidget(WUserWidget* Widget) const
{
    this->MainViewport->AddWidget(Widget);
}

void Jafg::LHud::RemoveWidget(WUserWidget* Widget) const
{
    this->MainViewport->RemoveWidget(Widget);
}
