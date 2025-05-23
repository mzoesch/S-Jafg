// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreFrontendWidgetsSubsystem.h"
#include "Framework/Frontend.h"
#include "User/LocalEgo.h"
#include "User/Frontend/Osd/FrontendScreen.h"

bool Jafg::JCoreFrontendWidgetsSubsystem::ShouldCreateSubsystem(const LObjectContext* InOuter) const
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

    return Super::IsOuterFrontend(InOuter);
}

void Jafg::JCoreFrontendWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    check( this->GetLocalEgo() )
    LFrontend* Frontend = this->GetLocalEgo()->GetFrontend();

    this->FrontendScreen = ConstructDeferredWidgetNode<WFrontendScreen>(this->GetOuter());
    this->FrontendScreen->AddToViewport(&Frontend->GetFocusedSurfaceChecked()->GetViewport());
    MakeDeferredWidgetNodeFinal(this->FrontendScreen);

    return;
}

void Jafg::JCoreFrontendWidgetsSubsystem::TearDown()
{
    Super::TearDown();

    if (ensure(this->FrontendScreen))
    {
        this->FrontendScreen->RemoveFromParent();
        this->FrontendScreen = nullptr;
    }

    return;
}
