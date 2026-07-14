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
#include "Nodes/Text.h"
#include "Nodes/Button.h"
#include "Widgets/TagInspector.h"
#include "Widgets/ColorInspector.h"
#include "Widgets/ClassInspector.h"
#include "Widgets/WorldViewer.h"
#include "Widgets/EditorFinder.h"
#include "Widgets/Input_Vector3.h"
#include "Widgets/AssetInspectors.h"
#include "Nodes/EditableTextButton.h"
#include "Widgets/EditorFactory.h"
#include "Components/SceneComponent.h"
#include "Framework/LackeyForward.h"
#include "Framework/PersonaController.h"
#include "Platform/Surface.h"
#include "User/Input/InputTypes.h"
#include "User/Input/InputActionValue.h"
#include "Framework/Actor.h"
#include "Framework/MaterialSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "User/LocalEgo.h"
#include "Rhi/ViewProj.h"

#if JAFG_WITH_EDITOR

template<>
Jafg::Detail::LNodeFactoryBase Jafg::GetEditorNode<LWorldTrans>(TEditorNodeCreateInfo<LWorldTrans> const& Info) noexcept
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    struct RowPair final
    {
        std::size_t FrameCount{};
        EStyleBits Bit{EStyleBits::Identity};
        WTextButton* Label;
        WButton* Content;
        WInput_Vector3* Vector;
        WButton* Reset;
    };

    auto Translation{std::make_shared<RowPair>()};
    auto Rotation{std::make_shared<RowPair>()};
    auto Scale{std::make_shared<RowPair>()};

    auto MakeLabel{[&Info, &Prefs](std::shared_ptr<RowPair> Pointer, LString Content)
    {
        return NewNode(Info.Viewport).Class<WTextButton>().SaveTo(&Pointer->Label)
            .MinDesiredSize({128_spt, 0})
            .Anchor(EAnchor::VFill)
            .InBrushChained<EStyleBits::ActiveCombi, &LBoxBrush::Tint, &LBoxBrush::Padding>(*Prefs.ForegroundColorVariant, {20_spt, 0, 0, 0})
            .InBrushChained<EStyleBits::InactiveCombi, &LBoxBrush::Tint, &LBoxBrush::Padding>(*Prefs.ForegroundColor, {20_spt, 0, 0, 0})
            .InAllTextBrushes<&LTextBoxBrush::TextVAlign>(ETextVAlign::Center)
            .Content(std::move(Content))
            .OnBrushChanged([Pointer](auto& Self, EStyleBits Bit)
            {
                check(Pointer.get() && Pointer->Label && Pointer->Content && Pointer->Vector && Pointer->Reset)
                if (Pointer->FrameCount < GEngine->FrameCount || Bit != EStyleBits::Normal)
                {
                    Pointer->FrameCount = GEngine->FrameCount;
                    Pointer->Bit = Bit;
                    ApplyStyleBit(Pointer->Content->Style, Pointer->Content->Brush, Bit);
                }
                else
                {
                    ApplyStyleBit(Pointer->Label->Style, Pointer->Label->Brush, Pointer->Bit);
                }

                return;
            });
    }};

    auto MakeContent{[&Info, &Prefs](std::shared_ptr<RowPair> Pointer, Detail::LNodeFactoryBase&& Factory)
    {
        return NewNode(Info.Viewport).Class<WButton>().SaveTo(&Pointer->Content)
            .Anchor(EAnchor::HFill)
            .Visibility(ENodeVisibility::Visible)
            .Padding({12_spt, 3})
            .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::Tint>(*Prefs.ForegroundColorVariant)
            .InBrush<EStyleBits::InactiveCombi, &LRegionBrush::Tint>(*Prefs.ForegroundColor)
            .OnBrushChanged([Pointer](auto& Self, EStyleBits Bit)
            {
                check(Pointer.get() && Pointer->Label && Pointer->Content && Pointer->Vector && Pointer->Reset)
                if (Pointer->FrameCount < GEngine->FrameCount || Bit != EStyleBits::Normal)
                {
                    Pointer->FrameCount = GEngine->FrameCount;
                    Pointer->Bit = Bit;
                    ApplyStyleBit(Pointer->Label->Style, Pointer->Label->Brush, Pointer->Bit);
                }
                else
                {
                    ApplyStyleBit(Pointer->Content->Style, Pointer->Content->Brush, Bit);
                }

                return;
            })
        [
            std::move(Factory)
        ];
    }};

    auto MakeReset([&Info, &Prefs](std::shared_ptr<RowPair> Pointer, LVec3D const& Identity, bool bEnabled)
    {
        check(Pointer.get() && !Pointer->Reset)
        return NewNode(Info.Viewport).Class<WButton>().SaveTo(&Pointer->Reset)
            .Anchor(EAnchor::VFill)
            .MinDesiredSize({24_spt, 20})
            .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::BorderTint>(*Prefs.ForegroundColorVariant)
            .InBrush<EStyleBits::InactiveCombi, &LRegionBrush::BorderTint>(*Prefs.ForegroundColor)
            .InBrush<EStyleBits::ActiveCombi|EStyleBits::Normal, &LRegionBrush::Tint>(Colors::White)
            .InBrush<EStyleBits::Disabled, &LRegionBrush::Tint>(Colors::Gray)
            .InAllBrushes<&LRegionBrush::Background>(LRegionBrush::Icon("Icons/Jafg.Reset"))
            .Enabled(bEnabled)
            .OnKeyEventFocused([Pointer, Identity](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                {
                    check(Pointer.get() && Pointer->Label && Pointer->Content && Pointer->Vector && Pointer->Reset)
                    Pointer->Vector->Set(Identity);
                    return LNodeReply::Handled();
                }
                return LNodeReply::Unhandled();
            })
            ;
    });

    return NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill) // TODO: 1_spt vpadding?
        .Space(1_spt)
    [
        MakeLabel(Translation, "Translation")
        + MakeContent(Translation, NewNode(Info.Viewport).Class<WInput_Vector3>(LVec3D{Info.Field.t}).SaveTo(&Translation->Vector)
            .OnVectorChanged([Pointer=Translation, Field=&Info.Field](WInput_Vector3& Self)
            {
                check(Pointer.get() && Pointer->Label && Pointer->Content && Pointer->Vector && Pointer->Reset)
                Field->t = Self.Get<f64>();
                Pointer->Reset->SetEnabled(Field->t != maths::zero_vector<LVec3F>);
            }))
        + MakeReset(Translation, maths::zero_vector<LVec3D>, Info.Field.t != maths::zero_vector<LVec3F>)
    ]
    + NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
    [
        MakeLabel(Rotation, "Rotation")
        + MakeContent(Rotation, NewNode(Info.Viewport).Class<WInput_Vector3>(LVec3D{maths::euler_angles_deg(Info.Field.r)}).SaveTo(&Rotation->Vector)
            .OnVectorChanged([Pointer=Rotation, Field=&Info.Field](WInput_Vector3& Self)
            {
                check(Pointer.get() && Pointer->Label && Pointer->Content && Pointer->Vector && Pointer->Reset)
                Field->r = maths::rotator_deg(Self.Get<f64>());
                Pointer->Reset->SetEnabled(Field->r != maths::identity<LWorldQuat>);
            }))
        + MakeReset(Rotation, maths::zero_vector<LVec3D>, Info.Field.r != maths::identity<LWorldQuat>)
    ]
    + NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
    [
        MakeLabel(Scale, "Scale")
        + MakeContent(Scale, NewNode(Info.Viewport).Class<WInput_Vector3>(LVec3D{Info.Field.s}).SaveTo(&Scale->Vector)
            .OnVectorChanged([Pointer=Scale, Field=&Info.Field](WInput_Vector3& Self)
            {
                check(Pointer.get() && Pointer->Label && Pointer->Content && Pointer->Vector && Pointer->Reset)
                Field->s = Self.Get<f64>();
                Pointer->Reset->SetEnabled(Field->t != maths::zero_vector<LVec3F>);
            }))
        + MakeReset(Scale, maths::one_vector<LVec3D>, Info.Field.s != maths::one_vector<LVec3F>)
    ];
}

template<>
Jafg::Detail::LNodeFactoryBase Jafg::GetEditorNode<LString>(TEditorNodeCreateInfo<LString> const& Info) noexcept
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    struct MyState : public LEditorRowState
    {
        WEditableTextButton* EditableTextButton{};
        WButton* ResetButton{};
        std::optional<LString> Default;
    };
    auto SharedState{std::make_shared<MyState>()};
    SharedState->Default = Info.Default;

    return NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
    [
        EditorComponentLabel(Info.Viewport, Info.What, SharedState)
        + EditorComponentContentWrapper(Info.Viewport, SharedState)
        [
            NewNode(Info.Viewport).Class<WEditableTextButton>().SaveTo(&SharedState->EditableTextButton)
                .Anchor(EAnchor::HFill)
                .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
                .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
                .Content(Info.Field)
                .OnContentChanged([SharedState](WEditableTextButton& Self, LString const& Content)
                {
                    check(SharedState.get())
                    SharedState->ResetButton->SetEnabled(Content != *SharedState->Default);
                })
                .OnContentCommitted([SharedState, Field=&Info.Field](WEditableTextButton& Self, LString const& Content, ETextCommit Commit)
                {
                    check(SharedState.get())
                    if (Commit != ETextCommit::OnCleared)
                    {
                        *Field = Content;
                        if (SharedState->Default)
                        {
                            SharedState->ResetButton->SetEnabled(*Field != *SharedState->Default);
                        }
                        else
                        {
                            check(!SharedState->ResetButton->IsEnabled())
                        }
                    }
                    else
                    {
                        Self.SetContent(*Field);
                    }
                })
        ]
        + EditorResetButton(Info.Viewport, Info.Default && Info.Field != Info.Default, [SharedState, Field=&Info.Field](WButton& Self)
        {
            check(SharedState.get())
            check(SharedState->Default)
            *Field = *SharedState->Default;
            check(SharedState->EditableTextButton && SharedState->ResetButton)
            SharedState->EditableTextButton->SetContent(*Field);
            SharedState->ResetButton->SetEnabled(*Field != *SharedState->Default);
        }, MyState::Lambda<WButton>(SharedState)).SaveTo(&SharedState->ResetButton)
    ];
}

#endif /* JAFG_WITH_EDITOR */

void Jafg::ToggleEditorNodesTransitively(WNode& Node, bool bEnabled)
{
    if (auto* Parent{Node.As<WParent>()})
    {
        check(!Node.IsA<WUserWidget>())

        for (auto& Child: Parent->GetChildren())
        {
            ToggleEditorNodesTransitively(*Child, bEnabled);
        }
    }

    if (auto* Button{Node.As<WButton>()})
    {
        Button->SetEnabled(bEnabled);
    }
    else if (auto* TextButton{Node.As<WTextButton>()})
    {
        TextButton->SetEnabled(bEnabled);
    }
}

Jafg::LNodeReply Jafg::WEditorCategorySeparator::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
    {
        this->SetSelected(!this->IsSelected());

        if (this->IsSelected())
        {
            this->LeftIcon = LTexture2::FromAsset("Icons/Jafg.ExtendDown");
        }
        else
        {
            this->LeftIcon = LTexture2::FromAsset("Icons/Jafg.ExtendRight");
        }

        for (auto const& [Visibility, Node]: this->Nodes)
        {
            if (this->IsSelected())
            {
                Node->SetVisibility(Visibility);
            }
            else
            {
                Node->SetVisibility(ENodeVisibility::Collapsed);
            }
        }
    }

    return Super::OnKeyEventFocused(Info, Event);
}

void Jafg::WEditorCategorySeparator::_ctor_Logic()
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    this->Anchor = EAnchor::HFill;
    this->Style.SetEverywhere<&LBoxBrush::Tint>(*Prefs.ForegroundColorVariant);
    this->LeftIcon = LTexture2::FromAsset("Icons/Jafg.ExtendDown");
    this->SetSelectable(true);
    this->SetSelected(true);
}

void Jafg::WEditor::BeginClassLife(LBeginClassLifeInfo const& Info)
{
    Super::BeginClassLife(Info);
    finder::create_directories(WEditor::GetUserLayoutsPath());

    if (!Detail::GMutableEngine->RegisterLevel({
        .Identifier = "LevelEditor",
        .SupremePoliciesClass = AEditorSupremePolicies::StaticClass(),
        }))
    {
        LOG_WARNING(LogEditor, "Level [LevelEditor] failed to registered.")
    }
}

Jafg::WParent& Jafg::WEditor::GetOverlayRoot() noexcept
{
    check(this->OverlayRoot)
    return *this->OverlayRoot;
}

void Jafg::WEditor::Construct()
{
    {
        auto& Prefs{GetMutableSingleton<JUserPreferences>()};
        check(!Prefs.Editor)
        Prefs.Editor = this;
    }

    Super::Construct();

    this->DiscoverLayouts();
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
                                .OnAction = [this](auto&&...){ this->GetFrontend().OpenDirectory(finder::current_path()); return algo::reply::unhandled(); }
                                },
                            LDropDownNodeOption{
                                .Selector = {
                                    .DisplayName = "Open Engine in Terminal",
                                    .Icon = "Icons/Jafg.Terminal",
                                    },
                                .OnAction = [this](auto&&...){ this->GetFrontend().OpenTerminal(finder::current_path()); return algo::reply::unhandled(); }
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
                            LDropDownNodeSeparator{.DisplayName="APPEARANCE"},
                            LDropDownNodeOption{
                                .Selector = {
                                    .DisplayName = "Toggle Fullscreen",
                                    .Icon = "Icons/Jafg.Fullscreen",
                                    },
                                .OnAction = [this](auto&&...)
                                {
                                    this->GetViewport().GetSurface().SetFullscreen(!this->GetViewport().GetSurface().IsFullscreen());
                                    return algo::reply::unhandled();
                                }
                                },
                            LDropDownNodeSeparator{.DisplayName="WIDGETS"},
                            LDropDownNodeOption{
                                .Selector = WFinder::TabSelectorCreateInfo(),
                                .OnAction = [this](auto&&...){ this->AddWindow<WFinder>(true); return algo::reply::unhandled(); },
                                },
                            LDropDownNodeOption{
                                .Selector = WWorldViewer::TabSelectorCreateInfo(),
                                .OnAction = [this](auto&&...){ this->AddWindow<WWorldViewer>(true); return algo::reply::unhandled(); },
                                },
                            LDropDownNodeOption{
                                .Selector = WWorldViewerHierarchy::TabSelectorCreateInfo(),
                                .OnAction = [this](auto&&...){ this->AddWindow<WWorldViewerHierarchy>(true); return algo::reply::unhandled(); },
                                },
                            LDropDownNodeOption{
                                .Selector = WWorldViewerInspector::TabSelectorCreateInfo(),
                                .OnAction = [this](auto&&...){ this->AddWindow<WWorldViewerInspector>(true); return algo::reply::unhandled(); },
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
                            Result.emplace_back(LDropDownNodeInformation{algo::sprintf("Platform: {}-{}", App::GetTargetPlatform(), App::GetTargetArchitecture())});
                            Result.emplace_back(LDropDownNodeInformation{algo::sprintf("Target: {}-{}", App::GetTargetType(), App::GetTargetConfiguration())});
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

    this->ApplyEditorLayout(this->LoadEditorLayout(*Prefs.EditorLastLayout));
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
}

TArray<LPath> const& Jafg::WEditor::DiscoverLayouts()
{
    LOG_VERBOSE(LogEditor, "Discovering editor layouts.")

    this->DiscoveredLayouts.clear();

    this->DiscoveredLayouts.emplace_back(finder::default_config_dir()/"DefaultEditorLayout.json");
    this->DiscoveredLayouts.append_range(finder::retrieve_files<finder::recursive_directory_iterator>(WEditor::GetUserLayoutsPath(), ".*\\.json"));

    LOG_VERBOSE(LogEditor, "Discovered [{}] layouts at:", this->DiscoveredLayouts.size())
    for (auto const& Layout : this->DiscoveredLayouts)
    {
        LOG_VERBOSE(LogEditor, " - {}", Layout)
    }

    return this->GetDiscoveredLayouts();
}

Jafg::LEditorLayout Jafg::WEditor::LoadEditorLayout(LPath Path)
{
    LOG_VERBOSE(LogEditor, "[{}]: Loading.", Path)

    LEditorLayout Result = json::parse(finder::read_file(Path), nullptr, false).get<LEditorLayout>();
    Result.Path = std::move(Path);

    if (Result.Surfaces.empty())
    {
        LOG_FATAL(LogEditor, "[{}]: Layout must contain at least one surface. Failed to load layout.", Result.Path)
    }

    auto ValidateFlow{[](this auto&& Self, LPath const& P, LEditorLayout::LFlow const& F) -> void
    {
        if (F.Controlflow != ENodePrimitiveControlflow::Horizontal && F.Controlflow != ENodePrimitiveControlflow::Vertical)
        {
            LOG_FATAL(LogEditor, "[{}]: Flow is not valid.", P)
        }
        if (F.Children.empty())
        {
            LOG_FATAL(LogEditor, "[{}]: Flow must contain at least one child. Failed to load layout.", P)
        }

        for (auto const& Child : F.Children)
        {
            if (std::holds_alternative<LEditorLayout::LFlow>(Child))
            {
                Self(P, std::get<LEditorLayout::LFlow>(Child));
            }
            else if (std::holds_alternative<LEditorLayout::LNodes>(Child))
            {
                if (std::get<LEditorLayout::LNodes>(Child).Dist < 0.0f || std::get<LEditorLayout::LNodes>(Child).Dist > 1.0f)
                {
                    LOG_FATAL(LogEditor, "[{}]: Invalid node distribution [{} <= {} <= {}].",
                        P, 0.0f, std::get<LEditorLayout::LNodes>(Child).Dist, 1.0f)
                }
                if (std::get<LEditorLayout::LNodes>(Child).Children.empty())
                {
                    LOG_FATAL(LogEditor, "[{}]: Nodes must contain at least one child. Failed to load layout.", P)
                }
            }
            else
            {
                std::unreachable();
            }
        }

        return;
    }};

    for (auto const& Surface : Result.Surfaces)
    {
        ValidateFlow(Result.Path, Surface.Layout);
    }

    return Result;
}

void Jafg::WEditor::ApplyEditorLayout(LEditorLayout const& Layout)
{
    LOG_VERBOSE(LogEditor, "[{}]: Applying layout.", Layout.Path)

    auto& Surface{this->GetViewport().GetSurface()};
    check(!Layout.Surfaces.empty())

    auto& S{Layout.Surfaces[0]};
    check(!S.bFullscreen && "WiP -- Not currently supported.")
    check(!S.bBorderless && "WiP -- Not currently supported.")
    auto& L{S.Layout};

    auto LoadChildrenToOverlay{[](WTabOverlay& Overlay, TArray<LString> Nodes)
    {
        for (auto& Node : Nodes)
        {
            TSubclassOf<WUserWidget> Class{Detail::GetGlobalCxxRecordRegistry().GetClassByNameAsserted(Node)->StaticClass};
            if (!Class.IsValidType())
            {
                LOG_FATAL(LogEditor, "[{}]: Class [{}] is not a valid {}. Failed to load layout."
                    , Node, Class.GetClass()->GetFullyQualifiedName(), WUserWidget::StaticClass().GetFullyQualifiedName())
            }
            Overlay.RegisterTab({.Panel = Class,});
        }
    }};

    check(L.Controlflow == ENodePrimitiveControlflow::Horizontal)
    for (auto& Child : L.Children)
    {
        if (std::holds_alternative<LEditorLayout::LNodes>(Child))
        {
            auto& Nodes{std::get<LEditorLayout::LNodes>(Child)};
            WTabOverlay& Overlay{this->FindNewOverlay(Nodes.Dist)};
            LoadChildrenToOverlay(Overlay, Nodes.Children);
        }
        else
        {
            std::unreachable();
        }
    }

    Surface.SetWindowSize(S.Dimensions);

    for (auto It{Layout.Surfaces.begin() + 1}; It != Layout.Surfaces.end(); /*++It*/)
    {
        LOG_FATAL(LogEditor, "Currently only one surface is allowed.")
    }
}

void Jafg::AEditorSupremePolicies::OnWorldPreInit()
{
    Super::OnWorldPreInit();

    auto& Frontend{this->GetMutableLocalEgo().GetFrontend()};
    auto MaterialInstance{Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.MetallicGold")};

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/XYZ.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{0,0,-10});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Plane.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{0,0,-4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cube.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{2,0,-4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Sphere.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{4,0,-4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Icosphere.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{6,0,-4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cylinder.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{8,0,-4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cone.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{10,0,-4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Torus.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{12,0,-4});
    });
}

void Jafg::AEditorSupremePolicies::OnPersonaControllerCreated(APersonaController& Pc)
{
    Super::OnPersonaControllerCreated(Pc);
    Pc.EmplaceComponent<AEditorPersonaControllerComponent>();
}

TJxxUnique<Jafg::APawn> Jafg::AEditorSupremePolicies::GetPawnForPersonaController(APersonaController const& Pc)
{
    auto Pawn{Super::GetPawnForPersonaController(Pc)};
    if (Pawn.get() == nullptr)
    {
        return nullptr;
    }

    Pawn->GetComponentChecked<ASceneComponent>().SetTranslation({0.f, 2.0f, 0.f});
    if (Pc.IsLocallyPossessed())
    {
        Pawn->EmplaceComponent<AEditorCameraComponent>();
    }

    return Pawn;
}

void Jafg::AEditorPersonaControllerComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);

    LOG_TRACE(LogRhi, "Allocating debug editor buffers.")
    auto& Frontend{this->GetMutableLocalEgo().GetFrontend()};
    check(Frontend.Vk_GetNumberOfFramesInFlight() != 0)

    this->RayInstance = Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.DebugLine");
    check(this->RayInstance->Material->Template.my_shader.get_descriptor_slot(ShaderSpace, DebugLinesIndex).CxxName == SSBO::Ray::name())
    check(this->RayInstance->Material->Template.my_shader.get_descriptor_slot(ShaderSpace, ViewProjIndex).CxxName == UBO::ViewProj::name())

    this->RayBuffers = Frontend.Vk_CreateFrequentMappedBuffer(SSBO::Ray::buffer_create_info(MaxLineCount));
    this->RayViewBuffers = Frontend.Vk_CreateFrequentMappedBuffer(UBO::ViewProj::buffer_create_info());
}

void Jafg::AEditorPersonaControllerComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    auto Reduce{[Dt](auto&& Range)
    {
        for (auto It{Range.begin()}; It != Range.end();)
        {
            It->Duration -= Dt;
            if (It->Duration <= 0.0f)
            {
                It = Range.erase(It);
            }
            else
            {
                ++It;
            }
        }
    }};

    Reduce(this->Rays);
    Reduce(this->Aabbs);

    this->Rays.append_range(this->NextRays);
    this->NextRays.clear();
    this->Aabbs.append_range(this->NextAabbs);
    this->NextAabbs.clear();
}

void Jafg::AEditorPersonaControllerComponent::Render(LActorRenderInfo const& Info) noexcept
{
    Super::Render(Info);
    auto& Prefs{GetSingleton<JUserPreferences>()};
    check(this->RayInstance.get())

    for (auto& E: this->GetWorld().GetEmployees() | algo::views::filter([](auto& E){ return E->template IsA<AActor>(); }))
    {
        auto& A{*StaticCastChecked<AActor>(&*E)};
        for (auto& Comp: A.GetComponents() | algo::views::filter([](auto& C){ return C->template IsA<ASceneComponent>(); }))
        {
            auto& Sc{*StaticCastChecked<ASceneComponent>(&*Comp)};
            if (Sc.ShouldRender())
            {
                this->AddAabb({
                    .Tint = *Prefs.EditorAabbVisualizationTint,
                    .Value = Sc.GetAabb().apply(Sc.GetTransform())
                    });
            }
        }
    }

    if (this->Rays.empty() && this->Aabbs.empty())
    {
        return;
    }

    auto& Set{this->RayInstance->Vk_GetUniqueDescriptorSet(ShaderSpace, Info.Frame)};

    rhi::object_range<SSBO::Ray> DeviceRays{this->Rays.size() + this->Aabbs.size() * 12, MaxLineCount};
    algo::for_each(this->Rays, [&DeviceRays](auto const& Ray)
    {
        DeviceRays->emplace_back(Ray->origin, Ray->direction * Ray->magnitude, Ray.Tint);
    });
    algo::for_each(this->Aabbs, [&DeviceRays](auto const& Aabb)
    {
        for (auto& edge: Aabb->edges())
        {
            DeviceRays->emplace_back(edge.origin, edge.delta, Aabb.Tint);
        }
    });

    DeviceRays.upload_and_update(Info.Frontend.Vk_GetDevice(), Set, DebugLinesIndex, this->RayBuffers[Info.Frame]);
    UBO::ViewProj{Info.WorldData.proj * Info.WorldData.view}.upload_and_update(Info.Frontend.Vk_GetDevice(), Set, ViewProjIndex, this->RayViewBuffers[Info.Frame]);

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *this->RayInstance->Material->Pipeline);
    Info.CommandBuffer.bindDescriptorSets2({
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .layout = *this->RayInstance->Material->Pipeline.pipeline_layout,
        .firstSet = 0,
        .descriptorSetCount = 1,
        .pDescriptorSets = &*Set,
        .dynamicOffsetCount = 0,
        .pDynamicOffsets = nullptr
        });
    Info.CommandBuffer.draw(2, static_cast<u32>(DeviceRays->size()), 0, 0);
}

bool Jafg::AEditorCameraComponent::ActivateUserInputContext() const noexcept
{
    if (auto* Ctrl{this->GetOwningPawn().GetOwningController()})
    {
        if (auto* Lackey{Ctrl->TryGetOwningLackey<ELackey::Local>()})
        {
            bool b1{Lackey->GetUserInput().ActivateContext(LUserInputTag::AsTagChecked("RhiDebug"))};
            bool b2{Lackey->GetUserInput().ActivateContext(LUserInputTag::AsTagChecked("EditorCameraCapturer"))};
            Lackey->GetUserInput().SetConsumeMouse(false);
            return b1 && b2;
        }
    }
    return false;
}

void Jafg::AEditorCameraComponent::OnTrace(rhi::extent2 Extent, LVec2F Location)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& Pawn{this->GetOwningPawn()};
    auto Eye{Pawn.GetEye()};

    LVec2F NormalLocation{
        (2.0f * Location.x) / static_cast<f32>(Extent.width) - 1.0f,
        1.0f - (2.0f * Location.y) / static_cast<f32>(Extent.height),
        };

    LMat4F InvProj{inverse(glm::perspectiveRH_ZO(Eye.VertFov,
        static_cast<f32>(Extent.width) / static_cast<f32>(Extent.height),
        Eye.NearFrustum, Eye.FarFrustum
        ))};
    LMat4F InvView{inverse(glm::lookAtRH(Eye.Translation, Eye.Translation + Eye.Front, Eye.Up))};

    LVec4F NearClip{NormalLocation.x, NormalLocation.y, 0.0f, 1.0f};
    LVec4F FarClip{NormalLocation.x, NormalLocation.y, 1.0f, 1.0f};

    LVec4F NearView{InvProj * NearClip}; NearView /= NearView.w;
    LVec4F FarView{InvProj * FarClip};  FarView  /= FarView.w;

    LVec3F NearWorld{LVec3F(InvView * NearView)};
    LVec3F FarWorld{LVec3F(InvView * FarView)};

    LWorldMagRay3 Ray{.origin = NearWorld, .direction = normalize(FarWorld - NearWorld), .magnitude = *Prefs.EditorTraceLength,};
    check(maths::normalized(Ray.direction))

    if (auto* PcComp{Pawn.GetOwningController()->GetComponent<AEditorPersonaControllerComponent>()})
    {
        PcComp->AddRay({.Tint = *Prefs.EditorTraceVisualizationTint, .Duration = *Prefs.EditorTraceVisualizationDuration, .Value = Ray,});
    }

    for (auto& Hit: Pawn.GetWorld().LineTraceNonPhysical(Ray, {.bSingleHit=true}))
    {
        if (auto* Comp{this->GetOwningActor().AsStatic<APawn>().GetOwningControllerChecked()->GetComponent<AEditorPersonaControllerComponent>()})
        {
            constexpr auto offset{0.01f};
            Comp->AddAabb({
                .Tint = *Prefs.EditorHitVisualizationTint,
                .Duration = *Prefs.EditorTraceVisualizationDuration,
                .Value = {
                    .min = Hit.GlobalWorldLocation - LVec3F{offset},
                    .max = Hit.GlobalWorldLocation + LVec3F{offset},
                    }
                });
        }
    }
}

void Jafg::AEditorCameraComponent::OnMove(LInputActionValue const& Value)
{
    if (auto* Sc{this->GetOwningActor().GetComponent<ASceneComponent>()})
    {
        LWorldVec3 Front{Sc->GetRotator() * maths::forward_vector<LVec3F>};

        auto Value3D{Value.GetAxis3DValue() * this->VelocityMultiplier};

        Value3D *= this->GetWorld().GetDeltaTime();

        LWorldVec3 Delta{maths::zero_vector<LWorldVec3>};
        Delta += Front * Value3D.x;
        Delta += glm::normalize(glm::cross(Front, maths::up_vector<LWorldVec3>)) * Value3D.y;
        Delta += maths::up_vector<LWorldVec3> * Value3D.z;
        Sc->AddTranslation(Delta);
    }
    else
    {
        LOG_WARNING(LogEcs, "Component [{}] is attached to [{}] which does not possess a [{}].",
            this->GetNameAsString(),
            this->GetOwningActor().GetNameAsString(),
            algo::type_name<ASceneComponent>()
            )
    }
}

void Jafg::AEditorCameraComponent::OnRotate(LInputActionValue const& Value)
{
    if (auto* Sc{this->GetOwningActor().GetComponent<ASceneComponent>()})
    {
        auto Value2D{Value.GetAxis2DValue() * this->Sensitivity};

        /* Yaw */
        Sc->AddRotator(maths::angle_axis(-glm::radians(Value2D.x), maths::up_vector<LWorldVec3>), ESceneSweep::Teleport);

        /* Pitch */
        LWorldReal Pitch{maths::clamp(this->CachedPitch + glm::radians(Value2D.y), glm::radians(-89.9f), glm::radians(89.9f))};
        Sc->AddRotator(maths::angle_axis(-(this->CachedPitch - Pitch), Sc->GetRotator() * maths::right_vector<LWorldVec3>), ESceneSweep::Teleport);

        Sc->SetRotator(maths::normalize(Sc->GetRotator()), ESceneSweep::Teleport);
    }
    else
    {
        LOG_WARNING(LogEcs, "Component [{}] is attached to [{}] which does not possess a [{}].",
            this->GetNameAsString(),
            this->GetOwningActor().GetNameAsString(),
            algo::type_name<ASceneComponent>()
            )
    }
}

void Jafg::AEditorCameraComponent::OnVelocityMultiplierChange(LInputActionValue const& Value)
{
    this->VelocityMultiplier = maths::clamp(this->VelocityMultiplier + Value.GetAxis1DValue(), MinVelocityMultiplier, MaxVelocityMultiplier);
}
