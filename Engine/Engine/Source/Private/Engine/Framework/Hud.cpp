// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Framework/Hud.h"
#include "Core/Application.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "Subsystems/HudSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Widgets/WidgetNode.h"
#include "Widgets/Viewport.h"
#include "Engine/Engine.h"

void Jafg::LHud::Initialize(Private::LObjectContext* InOuter)
{
    this->Outer = InOuter;
    GCurrentWidgetContextState = this->Outer;

    checkSlow( this->Collection == nullptr )
    this->Collection = new LSubsystemCollection(this->Outer);
    this->Collection->LocateAllSubsystemsOfClass(JHudSubsystem::StaticClass());
    this->Collection->InitializeSubsystems();

    check( this->GetMainViewport() )

    return;
}

void Jafg::LHud::Tick(LSurface& Surface, const bool bCheckInput)
{
    if (bCheckInput && Surface.IsMouseLocationMeaningful())
    {
        this->GetMainViewport()->DispatchInputs(Surface, Surface.GetMouseLocation());
    }
    else
    {
        this->GetMainViewport()->OnMouseLeftViewport(Surface, bCheckInput == false);
    }

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

    this->GetMainViewport()->Tick();

    return;
}

void Jafg::LHud::TearDown()
{
    GCurrentWidgetContextState = nullptr;

    this->Collection->TearDownSubsystems();
    delete this->Collection;
    this->Collection = nullptr;

    return;
}

Jafg::LViewport* Jafg::LHud::GetMainViewport() const
{
    return GEngine->GetPanickedLocalEgo()->GetPrimarySurface()->GetViewport();
}

void Jafg::LHud::AddWidget(WUserWidget* Widget) const
{
    this->GetMainViewport()->AddWidget(Widget);
}

void Jafg::LHud::RemoveWidget(WUserWidget* Widget) const
{
    this->GetMainViewport()->RemoveWidget(Widget);
}

Jafg::WWidgetNode* Jafg::LHud::GetTopLevelWidgetByClass(const LObjectClass* WidgetClass) const
{
    return this->GetMainViewport()->GetTopLevelWidgetByClass(WidgetClass);
}

Jafg::WWidgetNode* Jafg::LHud::GetCheckedTopLevelWidgetByClass(const LObjectClass* WidgetClass) const
{
    return this->GetMainViewport()->GetCheckedTopLevelWidgetByClass(WidgetClass);
}

bool Jafg::LHud::ChangeWidgetVisibility(const LObjectClass* WidgetClass, const EWidgetVisibility::Type InVisibility, const bool bAllowNotFound /* = false */) const
{
    WWidgetNode* Widget = this->GetTopLevelWidgetByClass(WidgetClass);
    if (Widget == nullptr)
    {
        if (bAllowNotFound == false)
        {
            panicMsgf( "Could not find widget of class [{}] to change visibility.", WidgetClass->GetSpacedClassName() )
        }

        return false;
    }

    if (Widget->GetVisibility() == InVisibility)
    {
        return false;
    }

    Widget->SetVisibility(InVisibility);

    return true;
}
