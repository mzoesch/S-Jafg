// Copyright mzoesch. All rights reserved.

#include "Widgets/Editor.h"
#include "Nodes/HDragRegion.h"
#include "Nodes/TabOverlay.h"
#include "Nodes/VRegion.h"
#include "Nodes/HRegion.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "User/UserPreferences.h"
#include "Widgets/TagInspector.h"
#include "Nodes/DropDown.h"
#include "Widgets/ColorInspector.h"

void Jafg::WEditor::Construct()
{
    Super::Construct();
    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto NewEditorOverlay{[this]{
        return NewStaticNode(WTabOverlay)
            .Delegate([this](auto& F){ this->Overlays.emplace_back(*StaticCast<WTabOverlay>(&F.GetRawNode())); })
            .Visibility(ENodeVisibility::Visible)
            .MinDesiredSize({25_pt, 0.0f})
            .WrapperInjection(WVRegion::StaticSubclass(), [](LFactoryVRegion& Factory){ Factory.SkipBrushDraw(true); })
            .SelectorsInjection(WHRegion::StaticSubclass());
        }};

    BeginStyling(*this).StaticRoot<WVRegion>()
        .Anchor(EAnchor::Fill)
    [
        NewStaticNode(WDropDown)
            .MinDesiredSize({EWidgetSize::StaticPoints, 0, 10.0f})
            .DropDownRoot({.Nodes={
                LDropDownNodeSubMenu{
                    .DisplayName = "File",
                    },
                LDropDownNodeSubMenu{
                    .DisplayName = "Edit",
                    },
                LDropDownNodeSubMenu{
                    .DisplayName = "View",
                    .Children = {
                        LDropDownNodeOption{
                            .Selector = WTagInspector::TabSelectorCreateInfo(),
                            .OnAction = [this](LDropDownNodeOption const&) { this->AddWindow<WTagInspector>(); },
                            },
                        LDropDownNodeOption{
                            .Selector = WColorInspector::TabSelectorCreateInfo(),
                            .OnAction = [this](LDropDownNodeOption const&) { this->AddWindow<WColorInspector>(); },
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
        NewStaticNode(WHDragRegion)
            // .SetInitialState(LInitialHDragRegionState{100_pt,{},100_pt})
            .Padding({EWidgetSize::StaticPoints, 3.0f, 0.0f, 3.0f, 3.0f})
            .Tint(*Prefs.BackgroundColor)
        [
            NewEditorOverlay()
            [
                  WTagInspector::TabCreateInfo() + WColorInspector::TabCreateInfo()
                // + WColorInspector::CreateTabDescriptor(this->GetViewport())
                // + LTabOverlayElement{
                //     .Selector = LTabOverlayElement::CreateInfo{
                //         .DisplayName = "Tab 2",
                //         .Icon = this->GetFrontend().GetSubsystemChecked<JTextureSubsystem>()->FromTextureViewIdentifier("Icons/Jafg.Preferences"),
                //         },
                //     .Panel = NewStaticNode(WVRegion)
                //         .Anchor(EAnchor::Fill)
                //         .Tint(*Prefs.ForegroundColor)
                //     [
                //           NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::Header)
                //         + NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::SubHeader)
                //         + NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::Body)
                //         + NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::Compact)
                //         + NewStaticNode(WTextBox).SkipBrushDraw(true).Content("Just Some Text").TextScale(ETextScale::Small)
                //             // .SamplerAddressMode(UBO::BindlessTextureArray::NearestClampToEdgeSamplerIdx)
                //     ]
                //     }
            ]
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

    check(!this->Overlays.empty())

    return;
}

void Jafg::WEditor::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);
}

Jafg::WUserWidget& Jafg::WEditor::AddWindow(LTabCreateInfo Info)
{
    if (this->Selected)
    {
        if (auto* Widget{this->Selected->FindWidgetSlow(Info.Panel.GetClassOrDefault())})
        {
            return *Widget;
        }
    }
    for (auto& Overlay: this->Overlays)
    {
        if (auto* Widget{Overlay->FindWidgetSlow(Info.Panel.GetClassOrDefault())})
        {
            return *Widget;
        }
    }

    WTabOverlay* Overlay{this->Selected};
    if (!Overlay)
    {
        check(!this->Overlays.empty())
        Overlay = &this->Overlays.front();
    }

    auto* Result{Overlay->RegisterTab(std::move(Info)).second};
    check(Result)
    return *Result;
}
