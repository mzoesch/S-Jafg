// Copyright mzoesch. All rights reserved.

#include "Widgets/Editor.h"
#include "Nodes/TabOverlay.h"
#include "Nodes/VParent.h"
#include "Nodes/DropDownTabBar.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "User/UserPreferences.h"
#include "Core/App.h"
#include "Widgets/TagInspector.h"
#include "Widgets/ColorInspector.h"
#include "Widgets/ClassInspector.h"
#include "Widgets/WorldViewer.h"

Jafg::WParent& Jafg::WEditor::GetOverlayRoot() noexcept
{
    check(this->GetChildren().size() == 1)
    check(this->GetChildren().front()->AsStatic<WVParent>().GetChildren().size() == 1)
    check(this->GetChildren().front()->AsStatic<WVParent>().GetChildren().front()->IsA<WDropDownTabBar>())
    return this->GetChildren().front()->AsStatic<WParent>();
}

void Jafg::WEditor::Construct()
{
    Super::Construct();
    auto& Prefs{GetSingleton<JUserPreferences>()};

    BeginStyling(*this).StaticRoot<WVParent>()
        .Anchor(EAnchor::Fill)
    [
        NewStaticNode(WDropDownTabBar)
            .MinDesiredSize({ENodeSize::StaticPoints, 0, 10.0f})
            .Tabs({.SubMenus={
                LDropDownNodeSubmenu{
                    .Selector = {.DisplayName="File"},
                    .Children = {
                        LDropDownNodeOption{
                            .Selector = {
                                .DisplayName = "Exit",
                                .Icon = "Icons/Jafg.Leave"
                                },
                            .OnAction = [](auto&&...){ App::RequestEngineExit("Invoked by editor."); return algo::reply::handled(); }
                            },
                        },
                    },
                LDropDownNodeSubmenu{
                    .Selector = { .DisplayName="Edit" },
                    },
                LDropDownNodeSubmenu{
                    .Selector = { .DisplayName="View" },
                    .Children = {
                        LDropDownNodeOption{
                            .Selector = WWorldViewer::TabSelectorCreateInfo(),
                            .OnAction = [this](auto&&...){ this->AddWindow<WWorldViewer>(true); return algo::reply::unhandled(); },
                            },
                        LDropDownNodeOption{
                            .Selector = WTagInspector::TabSelectorCreateInfo(),
                            .OnAction = [this](auto&&...){ this->AddWindow<WTagInspector>(true); return algo::reply::unhandled(); },
                            },
                        LDropDownNodeOption{
                            .Selector = WColorInspector::TabSelectorCreateInfo(),
                            .OnAction = [this](auto&&...){ this->AddWindow<WColorInspector>(true); return algo::reply::unhandled(); },
                            },
                        LDropDownNodeOption{
                            .Selector = WClassInspector::TabSelectorCreateInfo(),
                            .OnAction = [this](auto&&...){ this->AddWindow<WClassInspector>(true); return algo::reply::unhandled(); },
                            },
                        },
                    },
                LDropDownNodeSubmenu{
                    .Selector = { .DisplayName="Build" },
                    },
                LDropDownNodeSubmenu{
                    .Selector = { .DisplayName="Help" },
                    },
                },})
    ];

    // {
    //     auto& Overlay{this->FindNewOverlay()};
    //     Overlay.RegisterTab(WClassInspector::TabCreateInfo());
    // }
    {
        auto& Overlay{this->FindNewOverlay()};
        Overlay.RegisterTab(WWorldViewer::TabCreateInfo());
    }
    // {
    //     auto& Overlay{this->FindNewOverlay()};
    //     Overlay.RegisterTab(WTagInspector::TabCreateInfo());
    //     Overlay.RegisterTab(WColorInspector::TabCreateInfo());
    // }

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
