// Copyright mzoesch. All rights reserved.

#include "Widgets/Editor.h"
#include "Nodes/TabOverlay.h"
#include "Nodes/VRegion.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "User/UserPreferences.h"
#include "Nodes/DropDown.h"
#include "Core/Application.h"
#include "Widgets/TagInspector.h"
#include "Widgets/ColorInspector.h"
#include "Widgets/ClassInspector.h"

void Jafg::WEditor::Construct()
{
    Super::Construct();
    auto& Prefs{GetSingleton<JUserPreferences>()};

    BeginStyling(*this).StaticRoot<WVRegion>()
        .Anchor(EAnchor::Fill)
    [
        NewStaticNode(WDropDown)
            .MinDesiredSize({ENodeSize::StaticPoints, 0, 10.0f})
            .DropDownRoot({.Nodes={
                LDropDownNodeSubMenu{
                    .DisplayName = "File",
                    .Children = {
                        LDropDownNodeOption{
                            .Selector = {
                                .DisplayName = "Exit",
                                },
                            .OnAction = []{ Application::RequestEngineExit("Invoked by editor."); return algo::reply::handled(); }
                            },
                        },
                    },
                LDropDownNodeSubMenu{
                    .DisplayName = "Edit",
                    },
                LDropDownNodeSubMenu{
                    .DisplayName = "View",
                    .Children = {
                        LDropDownNodeOption{
                            .Selector = WTagInspector::TabSelectorCreateInfo(),
                            .OnAction = [this]{ this->AddWindow<WTagInspector>(true); return algo::reply::unhandled(); },
                            },
                        LDropDownNodeOption{
                            .Selector = WColorInspector::TabSelectorCreateInfo(),
                            .OnAction = [this]{ this->AddWindow<WColorInspector>(true); return algo::reply::unhandled(); },
                            },
                        LDropDownNodeOption{
                            .Selector = WClassInspector::TabSelectorCreateInfo(),
                            .OnAction = [this]{ this->AddWindow<WClassInspector>(true); return algo::reply::unhandled(); },
                            },
                        },
                    },
                LDropDownNodeSubMenu{
                    .DisplayName = "Build",
                    },
                LDropDownNodeSubMenu{
                    .DisplayName = "Help",
                    },
                },})
        +
        NewStaticNode(WTabOverlayHParent)
            // .SetInitialState(LInitialHDragRegionState{100_pt,{},100_pt})
            .Possibilities(*this)
            .Padding({ENodeSize::StaticPoints, 3.0f, 0.0f, 3.0f, 3.0f})
            .Tint(*Prefs.BackgroundColor)
        // [
        //     // Make a delegate to autoamtically find and create new TabOverlays in case none exist.
        //       NewEditorOverlay()[WTagInspector::TabCreateInfo()]
        //     + NewEditorOverlay()[WColorInspector::TabCreateInfo()]
        // ]
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
