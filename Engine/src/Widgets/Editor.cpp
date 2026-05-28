// Copyright mzoesch. All rights reserved.

#include "Widgets/Editor.h"
#include "Nodes/TabOverlay.h"
#include "Nodes/VParent.h"
#include "Nodes/DropDownTabBar.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "User/UserPreferences.h"
#include "Core/App.h"
#include "Engine/Engine.h"
#include "Nodes/HParent.h"
#include "Widgets/TagInspector.h"
#include "Widgets/ColorInspector.h"
#include "Widgets/ClassInspector.h"
#include "Widgets/WorldViewer.h"

Jafg::WParent& Jafg::WEditor::GetOverlayRoot() noexcept
{
    check(this->OverlayRoot)
    return *this->OverlayRoot;
}

void Jafg::WEditor::Construct()
{
    Super::Construct();
    auto& Prefs{GetSingleton<JUserPreferences>()};

    BeginStyling(*this).StaticRoot<WVParent>().SaveTo(&this->OverlayRoot)
        .Anchor(EAnchor::Fill)
    [
        NewStaticNode(WHParent).SaveTo(&this->Bar)
            .Anchor(EAnchor::HFill)
        [
            NewStaticNode(WDropDownTabBar)
                .MinDesiredSize({ENodeSize::StaticPoints, 0, 10.0f})
                .Tabs({.SubMenus={
                    LDropDownNodeSubmenu{
                        .Selector = {.DisplayName="File",},
                        .Children = {
                            LDropDownNodeOption{
                                .Selector = {
                                    .DisplayName = "Open Engine in Explorer",
                                    .Icon = "Icons/Jafg.Directory",
                                    },
                                .OnAction = [this](auto&&...){ this->GetFrontend().OpenDirectory(Finder::GetCwd()); return algo::reply::unhandled(); }
                                },
                            LDropDownNodeOption{
                                .Selector = {
                                    .DisplayName = "Open Engine in Terminal",
                                    .Icon = "Icons/Jafg.Terminal",
                                    },
                                .OnAction = [this](auto&&...){ this->GetFrontend().OpenTerminal(Finder::GetCwd()); return algo::reply::unhandled(); }
                                },
                            LDropDownNodeSeparator{.DisplayName="EXIT",},
                            LDropDownNodeOption{
                                .Selector = {
                                    .DisplayName = "Exit",
                                    .Icon = "Icons/Jafg.Leave",
                                    },
                                .OnAction = [](auto&&...){ App::RequestEngineExit("Invoked by editor."); return algo::reply::handled(); }
                                },
                            },
                        },
                    LDropDownNodeSubmenu{
                        .Selector = {.DisplayName="Edit",},
                        },
                    LDropDownNodeSubmenu{
                        .Selector = {.DisplayName="View",},
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
                        .Selector = {.DisplayName="Build",},
                        },
                    LDropDownNodeDeferredSubMenu{
                        .Selector = {.DisplayName="Help",},
                        .OnChildren = [this]
                        {
                            TArray<LDropDownNode> Result;

                            Result.emplace_back(LDropDownNodeSeparator{"ABOUT"});
                            Result.emplace_back(LDropDownNodeInformation{"Jafg @mzoesch"});
                            Result.emplace_back(LDropDownNodeInformation{algo::sprintf("v{} at [{} - {}] from [{} @ {}]"
                                , App::EngineVersion(), App::BuildTime(), App::BuildDate(), App::BuildVcsBranch(), App::BuildVcsRevision())});
                            Result.emplace_back(LDropDownNodeInformation{algo::sprintf("Platform: {}", App::GetTargetPlatformCompound())});
                            Result.emplace_back(LDropDownNodeInformation{algo::sprintf("Target: {}", App::GetTargetCompound())});
                            Result.emplace_back(LDropDownNodeSeparator{"RUNTIME"});
                            Result.emplace_back(LDropDownNodeInformation{algo::sprintf("Uptime: {}s in [{}]", static_cast<u64>(App::GetElapsedTime()), this->GetEngine().FrameCount)});
                            Result.emplace_back(LDropDownNodeInformation{algo::sprintf("Rate: {} ticks/s"
                                , static_cast<u64>(this->GetEngine().PreviousStat.FrameCount / algo::time_diff(this->GetEngine().PreviousStat.Start, this->GetEngine().CurrentStat.Start)))});
                            Result.emplace_back(LDropDownNodeInformation{algo::sprintf("Rate h[{:.6f}] l[{:.6f}]", this->GetEngine().PreviousStat.High, this->GetEngine().PreviousStat.Low)});
                            Result.emplace_back(LDropDownNodeSeparator{"RESOURCES"});
                            Result.emplace_back(LDropDownNodeOption{
                                .Selector = {
                                    .DisplayName = "Visit Homepage",
                                    .Icon = "Icons/Jafg.Open",
                                    },
                                .OnAction = [this](auto&&...){ this->GetFrontend().OpenUrl(App::Homepage()); return algo::reply::unhandled(); },
                                });

                            return Result;
                        },},
                    },})
        +
        NewStaticNode(WTextBox)
            .Anchor(EAnchor::VFill)
            .Tint(*Prefs.BackgroundColor)
            .TextTint(Colors::Gray)
            .TextScale(ETextScale::Small)
            .Content(algo::sprintf("v {}", App::EngineVersion()))
        ]
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

    return;
}

void Jafg::WEditor::Tick()
{
    if (auto &Prefs{GetSingleton<JUserPreferences>()}; *Prefs.EditorShowRate)
    {
        if (!this->Rate)
        {
            check(this->Bar)
            this->Bar->AddChildAt(std::min(1uz, this->Bar->GetChildren().size()),
                NewNode(this->GetViewport()).Class<WTextBox>().SaveTo(&this->Rate)
                .Anchor(EAnchor::VFill)
                .Tint(*Prefs.BackgroundColor)
                .TextTint(Colors::Gray)
                .TextScale(ETextScale::Small)
                .Unique());
            check(this->Rate)
        }

        if (auto& Engine{this->GetEngine()}; this->LastRatePoint != Engine.PreviousStat.Start)
        {
            this->LastRatePoint = Engine.PreviousStat.Start;
            auto Diff{algo::time_diff(Engine.PreviousStat.Start, Engine.CurrentStat.Start)};
            this->Rate->SetContent(algo::sprintf("{:.3f}ms {}ts"
                , (Diff / static_cast<f64>(Engine.PreviousStat.FrameCount)) * maths::s2ms_f
                , static_cast<u64>(static_cast<f64>(Engine.PreviousStat.FrameCount) / Diff)));
        }
    }
    else
    {
        if (this->Rate)
        {
            this->Rate->RemoveFromParent2();
        }
    }

    Super::Tick();

    return;
}
