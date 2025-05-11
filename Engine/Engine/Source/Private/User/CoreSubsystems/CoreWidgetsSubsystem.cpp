    // Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreWidgetsSubsystem.h"
#include "Framework/Frontend.h"
#include "Widgets/UserWidget.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "User/Frontend/Hud/Crosshair.h"
#include "User/Frontend/Osd/ConsoleScreen.h"
#include "User/Frontend/Osd/PauseScreen.h"
#include "Widgets/Region.h"
#include "Widgets/VRegion.h"

    void Jafg::JCoreWidgetsSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    this->DebugScreen = ConstructDeferredWidgetNode<WDebugScreen>(this->GetOuter());
    this->DebugScreen->AddToViewport(&this->GetFrontend()->GetFocusedSurfaceChecked()->GetViewport());
    this->DebugScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->DebugScreen);

    this->Crosshair = ConstructDeferredWidgetNode<WCrosshair>(this->GetOuter());
    this->Crosshair->AddToViewport(&this->GetFrontend()->GetFocusedSurfaceChecked()->GetViewport());
    // this->Crosshair->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
    this->Crosshair->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->Crosshair);

    this->ConsoleScreen = ConstructDeferredWidgetNode<WConsoleScreen>(this->GetOuter());
    this->ConsoleScreen->AddToViewport(&this->GetFrontend()->GetFocusedSurfaceChecked()->GetViewport());
    this->ConsoleScreen->SetConsoleFrontendState(EConsoleScreenState::Hide);
    MakeDeferredWidgetNodeFinal(this->ConsoleScreen);

    this->PauseScreen = ConstructDeferredWidgetNode<WPauseScreen>(this->GetOuter());
    this->PauseScreen->AddToViewport(&this->GetFrontend()->GetFocusedSurfaceChecked()->GetViewport());
    this->PauseScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->PauseScreen);

    // WMyTest* T = ConstructDeferredWidgetNode<WMyTest>(this->GetOuter());
    // T->AddToViewport(&this->GetFrontend()->GetFocusedSurfaceChecked()->GetViewport());
    // T->SetVisibility(EWidgetVisibility::Visible);
    // MakeDeferredWidgetNodeFinal(T);

    return;
}

void Jafg::JCoreWidgetsSubsystem::TearDown()
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

    return;
}

void Jafg::WMyTest::Construct()
{
    Super::Construct();

    this->SetPadding(50);
    MakeRootNode(WVRegion)
        .Anchor(EAnchor::VFill | EAnchor::HRight)
        .Tint({255,255,255,64})
        .Padding(10)
        .VSpace(5)
    [
        NewNode(WRegion)
            .Tint({255,0,0,64})
            .MinDesiredSize({190,80})
            .Padding(5)
        [
            NewNode(WRegion)
                .Anchor(EAnchor::BottomRight)
                .Tint({0,255,0,64})
                .MinDesiredSize(20)
        ]
        +
        NewNode(WRegion)
            .Anchor(EAnchor::Fill)
            .Tint({255,0,0,64})
            .MinDesiredSize(40)
            .Padding(5)
        +
        NewNode(WRegion)
            .Anchor(EAnchor::VFill | EAnchor::HCenter)
            .Tint({255,0,0,64})
            .MinDesiredSize(20)
            .Padding(5)
        +
        NewNode(WRegion)
            .Anchor(EAnchor::VFill | EAnchor::HRight)
            .Tint({0,255,0,64})
            .MinDesiredSize(40)
            .Padding(5)
        +
        NewNode(WRegion)
            .Anchor(EAnchor::HFill)
            .Tint({0,255,0,64})
            .MinDesiredSize(40)
            .Padding(5)
    ]
    FinishWidgetStyling()

    return;
}
