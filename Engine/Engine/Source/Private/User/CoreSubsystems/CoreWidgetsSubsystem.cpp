    // Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CoreWidgetsSubsystem.h"
#include "Framework/Frontend.h"
#include "Widgets/UserWidget.h"
#include "User/Frontend/Osd/DebugScreen.h"
#include "User/Frontend/Hud/Crosshair.h"
#include "User/Frontend/Osd/ConsoleScreen.h"
#include "User/Frontend/Osd/PauseScreen.h"
#include "Widgets/ScrollRegion.h"
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
    this->Crosshair->SetVisibility(EWidgetVisibility::TransitiveHitTestInvisible);
    // this->Crosshair->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->Crosshair);

    this->ConsoleScreen = ConstructDeferredWidgetNode<WConsoleScreen>(this->GetOuter());
    this->ConsoleScreen->AddToViewport(&this->GetFrontend()->GetFocusedSurfaceChecked()->GetViewport());
    this->ConsoleScreen->SetConsoleFrontendState(EConsoleScreenState::Hide);
    MakeDeferredWidgetNodeFinal(this->ConsoleScreen);

    this->PauseScreen = ConstructDeferredWidgetNode<WPauseScreen>(this->GetOuter());
    this->PauseScreen->AddToViewport(&this->GetFrontend()->GetFocusedSurfaceChecked()->GetViewport());
    this->PauseScreen->SetVisibility(EWidgetVisibility::Collapsed);
    MakeDeferredWidgetNodeFinal(this->PauseScreen);

    // WUserWidget* Test = ConstructDeferredWidgetNode<WUserWidget>(this->GetOuter());
    // Test->AddToViewport(&this->GetFrontend()->GetFocusedSurfaceChecked()->GetViewport());
    // WScrollRegion* R;
    // NewNodeCtx(Test, WScrollRegion).SaveTo(&R)
    //     .Anchor(EAnchor::HRight | EAnchor::VFill)
    //     .MinDesiredSize({600,0})
    //     .Tint({255,0,0,32})
    //     .Padding({25})
    // [
    //     NewNodeCtx(Test, WVRegion)
    //         .Anchor(EAnchor::Fill)
    //         .Tint({0,255,0,64})
    //     [
    //         NewNodeCtx(Test, WRegion)
    //            .MinDesiredSize({1600,400})
    //            .Tint({255,0,255,64})
    //         +
    //         NewNodeCtx(Test, WRegion)
    //             .MinDesiredSize({2000,400})
    //             .Tint({0,0,255,128})
    //         +
    //         NewNodeCtx(Test, WRegion)
    //             .MinDesiredSize({1800,400})
    //             .Tint({255,0,255,64})
    //         +
    //         NewNodeCtx(Test, WRegion)
    //             .MinDesiredSize({400})
    //             .Tint({0,0,255,64})
    //         +
    //         NewNodeCtx(Test, WRegion)
    //             .MinDesiredSize({400})
    //             .Tint({255,0,255,64})
    //     ]
    // ];
    // Test->AddChild(R);
    // MakeDeferredObjectFinal(Test);

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
