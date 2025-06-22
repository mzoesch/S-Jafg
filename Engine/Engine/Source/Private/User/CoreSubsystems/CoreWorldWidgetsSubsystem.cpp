// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreWorldWidgetsSubsystem.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "Widgets/UserWidget.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "User/Frontend/Hud/Crosshair.h"
#include "User/Frontend/Osd/ConsoleScreen.h"
#include "User/Frontend/Osd/PauseScreen.h"

bool Jafg::JCoreWorldWidgetsSubsystem::ShouldCreateSubsystem(const LObjectContext* InOuter) const
{
    if (Super::ShouldCreateSubsystem(InOuter) == false)
    {
        return false;
    }

    if (this->GetLocalEgo() == nullptr)
    {
        LOG_WARNING(LogWidgets, "No local ego found. Widgets subsystem will not be initialized.")
        return false;
    }

    return Super::IsOuterWorld(InOuter);
}

void Jafg::JCoreWorldWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    check( this->GetLocalEgo() )
    LFrontend* Frontend = this->GetLocalEgo()->GetFrontend();

    this->DebugScreen = ConstructDeferredWidgetNode<WDebugScreen>(this->GetOuter());
    this->DebugScreen->AddToViewport(&Frontend->GetFocusedSurfaceChecked()->GetViewport());
    this->DebugScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->DebugScreen);

    this->Crosshair = ConstructDeferredWidgetNode<WCrosshair>(this->GetOuter());
    this->Crosshair->AddToViewport(&Frontend->GetFocusedSurfaceChecked()->GetViewport());
    this->Crosshair->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
    MakeDeferredWidgetNodeFinal(this->Crosshair);

    this->ConsoleScreen = ConstructDeferredWidgetNode<WConsoleScreen>(this->GetOuter());
    this->ConsoleScreen->AddToViewport(&Frontend->GetFocusedSurfaceChecked()->GetViewport());
    this->ConsoleScreen->SetConsoleFrontendState(EConsoleScreenState::Hide);
    MakeDeferredWidgetNodeFinal(this->ConsoleScreen);

    this->PauseScreen = ConstructDeferredWidgetNode<WPauseScreen>(this->GetOuter());
    this->PauseScreen->AddToViewport(&Frontend->GetFocusedSurfaceChecked()->GetViewport());
    this->PauseScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->PauseScreen);

    return;
}

void Jafg::JCoreWorldWidgetsSubsystem::TearDown()
{
    Super::TearDown();

    if (ensure(this->DebugScreen))
    {
        this->DebugScreen->RemoveFromParent();
        this->DebugScreen = nullptr;
    }

    if (ensure(this->Crosshair))
    {
        this->Crosshair->RemoveFromParent();
        this->Crosshair = nullptr;
    }

    if (ensure(this->ConsoleScreen))
    {
        this->ConsoleScreen->RemoveFromParent();
        this->ConsoleScreen = nullptr;
    }

    if (ensure(this->PauseScreen))
    {
        this->PauseScreen->RemoveFromParent();
        this->PauseScreen = nullptr;
    }

    return;
}
