// Copyright mzoesch. All rights reserved.

#include "Widgets/Editor.h"
#include "Nodes/HDragRegion.h"

void Jafg::WEditor::Construct()
{
    Super::Construct();

    BeginStyling(*this).Root<WHDragRegion>()
        // .SetInitialState(LInitialHDragRegionState{100_pt,{},100_pt})
    [
        NewStaticNode(WRegion)
            .Tint(Colors::Red)
            .Visibility(ENodeVisibility::Visible)
            .MinDesiredSize({25_pt, 0.0f})
        +
        NewStaticNode(WRegion)
            .Tint(Colors::Green)
            .Visibility(ENodeVisibility::Visible)
            .MinDesiredSize({25_pt, 0.0f})
        +
        NewStaticNode(WRegion)
            .Tint(Colors::Blue)
            .Visibility(ENodeVisibility::Visible)
            .MinDesiredSize({25_pt, 0.0f})

    ];

    // if ( Surface->GetOwnedController())
    // {
    //     LOG_VERBOSE(LogJgcLifetime, "Local ego already possesses a persona controller. Skipping default jgc frontend world creation and persona controller login.")
    // }
    // else
    // {
    //     auto StartupWorld{GEngine->SummonWorld("JgcStartUp")};
    //     GEngine->Browse(StartupWorld, TargetLevel, {.OnWorldPostInit = [Surface = &*Surface](Jafg::LWorld& World)
    //     {
    //         if (Surface->iscon())
    //         {
    //             LOG_WARNING(LogJgcLifetime, "Surface [{}] already posses a persona controller through [{}@{}]. Skipping login",
    //                 Surface->GetHumanReadableName(),
    //                 Surface->GetOwnedControllerChecked()->GetWorld().GetHumanReadableName(),
    //                 Surface->GetOwnedControllerChecked()->GetWorld().GetUnderlyingLevelName()
    //                 )
    //         }
    //         else
    //         {
    //             LOG_VERBOSE(LogJgcLifetime, "Logging in surface [{}] for [{}@{}].",
    //                 Surface->GetHumanReadableName(),
    //                 World.GetHumanReadableName(),
    //                 World.GetUnderlyingLevelName()
    //                 )
    //             if (LString OutError; World.Login({Jafg::EIncomingConnectionRequest::Local, Surface}, &OutError) == nullptr)
    //             {
    //                 LOG_FATAL(LogJgcLifetime, "Failed to login: {}", OutError)
    //             }
    //         }
    //     }});
    // }

    return;
}
