// Copyright mzoesch. All rights reserved.

#include "Widgets/Editor.h"

#include <Framework/FrontendVk.h>
#include <Framework/TextureSubsystem.h>

#include "Nodes/HDragRegion.h"
#include "Nodes/TabOverlay.h"
#include "Nodes/VRegion.h"
#include "Nodes/HRegion.h"
#include "User/UserPreferences.h"


#include "Nodes/TextBox.h"

void Jafg::WEditor::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};

    BeginStyling(*this).StaticRoot<WHDragRegion>()
        // .SetInitialState(LInitialHDragRegionState{100_pt,{},100_pt})
        .Padding(3_spt)
        .Tint(*Prefs.BackgroundColor)
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
        NewStaticNode(WTabOverlay)
            .Visibility(ENodeVisibility::Visible)
            .MinDesiredSize({25_pt, 0.0f})
            .WrapperClass(WVRegion::StaticSubclass(), [](LFactoryVRegion& Factory){ Factory.SkipBrushDraw(true); })
            .SelectorsClass(WHRegion::StaticSubclass())
        [
            LTabOverlayElement{
                .Selector = LTabOverlayElement::Info{
                    .DisplayName = "Tab_1",
                    .Icon = this->GetFrontend().GetSubsystemChecked<JTextureSubsystem>()->FromTextureViewIdentifier("Icons/Jafg.Preferences"),
                    },
                .Panel = NewStaticNode(WVRegion)
                    .Anchor(EAnchor::Fill)
                    .Tint(*Prefs.ForegroundColor)
                [
                      NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::Header)
                    + NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::SubHeader)
                    + NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::Body)
                    + NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::Compact)
                    + NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::Small)
                        // .SamplerAddressMode(UBO::BindlessTextureArray::NearestClampToEdgeSamplerIdx)
                ]
                }
            +
            LTabOverlayElement{
                .Selector = LTabOverlayElement::Info{.DisplayName = "Tab_2"},
                .Panel = NewStaticNode(WVRegion)
                    .Anchor(EAnchor::Fill)
                    .Tint(*Prefs.ForegroundColor)
                [
                    NewStaticNode(WTextBox)
                        .SkipBrushDraw(true)
                        .Content("Hello, World 2!")
                ]
                }
        ]
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
