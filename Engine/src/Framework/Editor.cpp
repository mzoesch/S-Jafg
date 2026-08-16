// Copyright mzoesch. All rights reserved.

#include "Framework/Editor.h"
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
#include "Framework/SceneComponent.h"
#include "Framework/LackeyForward.h"
#include "Framework/PersonaController.h"
#include "Framework/Surface.h"
#include "User/Input/InputTypes.h"
#include "User/Input/InputActionValue.h"
#include "Framework/Actor.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/StaticMeshComponent.h"
#include "User/LocalEgo.h"
#include "Rhi/ViewProj.h"
#include "Rhi/SolidColor.h"
#include "Widgets/WorldsInspector.h"
#include "Nodes/Viewport.h"
#include "Nodes/Region.h"
#include "Nodes/DropDown.h"
#include "Nodes/TextButton.h"
#include "Rhi/NodeRenderInfo.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "Nodes/Spacer.h"
#include "Widgets/Input_Vector2.h"
#include "Nodes/HButton.h"
#include "Engine/World.h"
#include "Nodes/CheckmarkButton.h"
#include "Nodes/ScrollRegion.h"
#include "Framework/ShaderSubsystem.h"
#include "Rhi/OutlineRendering.h"
#include "Widgets/Input_Vector1.h"
#include "Framework/PawnStart.h"
#include "Framework/RigidComponent.h"
#include "Nodes/Separator.h"
#include "../Framework/PhysicsForeignCore.h"
#include "Framework/FontSubsystem.h"

namespace
{

Jafg::LBufferObjectRegistrator<Jafg::SSBO::Ray> _;

} /* ~Namespace <Anonymous> */

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

    Info.UpdateValue = [T=Translation,R=Rotation,S=Scale,Field=&Info.Field]
    {
        auto Translation{WInput_Vector3::FloatingVector{Field->t}};
        if (auto V{T->Vector->Get<WInput_Vector3::FloatingVector::value_type>()}; V != Translation)
        {
            T->Vector->Set(Translation);
        }
        auto Rotation{WInput_Vector3::FloatingVector{maths::editor_euler_angles_deg(Field->r)}};
        if (auto V{R->Vector->Get<WInput_Vector3::FloatingVector::value_type>()}; V != Rotation)
        {
            R->Vector->Set(Rotation);
        }
        auto Scale{WInput_Vector3::FloatingVector{Field->s}};
        if (auto V{S->Vector->Get<WInput_Vector3::FloatingVector::value_type>()}; V !=Scale)
        {
            S->Vector->Set(Scale);
        }
    };

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
        // TODO: glm::mat3_cast(glm::normalize(q)); normalize before converting to editor?
        + MakeContent(Rotation, NewNode(Info.Viewport).Class<WInput_Vector3>(LVec3D{maths::editor_euler_angles_deg(Info.Field.r)}).SaveTo(&Rotation->Vector)
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
                                .Selector = WEditorWorldViewer::TabSelectorCreateInfo(),
                                .OnAction = [this](auto&&...){ this->AddWindow<WEditorWorldViewer>(true); return algo::reply::unhandled(); },
                                },
                            LDropDownNodeOption{
                                .Selector = WEditorWorldViewerHierarchy::TabSelectorCreateInfo(),
                                .OnAction = [this](auto&&...){ this->AddWindow<WEditorWorldViewerHierarchy>(true); return algo::reply::unhandled(); },
                                },
                            LDropDownNodeOption{
                                .Selector = WEditorWorldViewerInspector::TabSelectorCreateInfo(),
                                .OnAction = [this](auto&&...){ this->AddWindow<WEditorWorldViewerInspector>(true); return algo::reply::unhandled(); },
                                },
                            LDropDownNodeOption{
                                .Selector = WWorldsInspector::TabSelectorCreateInfo(),
                                .OnAction = [this](auto&&...){ this->AddWindow<WWorldsInspector>(true); return algo::reply::unhandled(); },
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

void Jafg::WSecondaryEditor::Construct()
{
    Super::Construct();
    check(this->Possibilities)
    auto& Prefs{GetSingleton<JUserPreferences>()};

    BeginStyling(*this).StaticRoot<WTabOverlayParent>()
        .Anchor(EAnchor::Fill)
        .Possibilities(*this->Possibilities)
        .Tint(*Prefs.BackgroundColor)
        .Padding({ENodeSize::StaticPoints
            , static_cast<f32>(*Prefs.PreferredDragPadding), 0.0f
            , static_cast<f32>(*Prefs.PreferredDragPadding), static_cast<f32>(*Prefs.PreferredDragPadding)}
            );
}

Jafg::LFactoryTabOverlay Jafg::WSecondaryEditor::GetNewOverlay()
{
    return NewNode(this->GetViewport()).Class<WTabOverlay>()
        .Possibilities(*this->Possibilities)
        .Visibility(ENodeVisibility::Visible)
        .MinDesiredSize({25_pt, 0.0f})
        ;
}

Jafg::WTabOverlay& Jafg::WSecondaryEditor::FindNewOverlay(f32 Dist /* = {} */)
{
    if (WTabOverlayParent* Parent{this->FindNodeInVisiblePath<WTabOverlayParent>()})
    {
        auto& Result{Parent->AddChild(this->GetNewOverlay().Unique())};
        if (Dist != 0.0f)
        {
            check(Dist > 0.0f)
            Parent->SetDistFor(Result, Dist);
        }
        return Result.AsStatic<WTabOverlay>();
    }
    LOG_FATAL(LogEditor, "Failed to find a [{}] in the visible path of {}. Cannot add new overlay."
        , WTabOverlayParent::StaticClass().GetFullyQualifiedName(), this->GetVirtualTable().GetFullyQualifiedName())
}

void Jafg::WEditorWorldViewer::Construct()
{
    Super::Construct();

    auto& Prefs{GetSingleton<JUserPreferences>()};
    this->StartType = EStartType{static_cast<u8>(*Prefs.EditorStartType)};
    this->PawnStart = EPawnSart{static_cast<u8>(*Prefs.EditorPawnStart)};

    constexpr auto MinSize{23_spt2};

    BeginStyling(*this).StaticRoot<WVParent>()
        .Anchor(EAnchor::HFill)
    [
        NewStaticNode(WHParent)
            .Anchor(EAnchor::HFill)
            .Padding(3_pt)
            .Space(3_pt)
        [
            NewStaticNode(WButton)
                .MinDesiredSize(MinSize)
                .InAllBrushesChained<&LRegionBrush::Background, &LRegionBrush::Radii>(LRegionBrush::Icon("Icons/Jafg.Menu"), LVec4F{50.0f})
                .InBrushChained<EStyleBits::ActiveCombi, &LRegionBrush::Tint, &LRegionBrush::BorderTint>(Colors::White, LColor{0x4c})
                .InBrushChained<EStyleBits::InactiveCombi, &LRegionBrush::Tint, &LRegionBrush::BorderTint>(LColor{0xA0}, LColor{0x3c})
                .InAllBrushesChained<&LRegionBrush::OutlineThickness, &LRegionBrush::OutlineTint>(1, Colors::Black)
                .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                    {
                        this->CreateMenuDropDown(Info.CursorLocation.value_or(maths::zero_vector<LVec2F>));
                        return LNodeReply::Handled();
                    }
                    return LNodeReply::Unhandled();
                })
            + NewStaticNode(WHRegion)
                .Tint(Colors::Black)
                .Space(1_spt)
                .Padding(1_spt)
                .OutlineThickness(1)
                .OutlineTint(Colors::Black)
            [
                NewStaticNode(WButton).SaveTo(&this->StartButton)
                    .MinDesiredSize(MinSize)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.Start")))
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            if (this->IsRunning())
                            {
                                auto& Ctrl{*this->RunningInstance->GetOwnedPersonaControllerChecked()};
                                Ctrl.GetWorld().SetDormantStateOfLinearWorld(!Ctrl.GetWorld().IsLinearWorldDormant());
                                this->UpdateStartStopButtons();
                            }
                            else
                            {
                                this->OnLaunchAll();
                            }
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewStaticNode(WButton).SaveTo(&this->StepButton)
                    .MinDesiredSize(MinSize)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.Step")))
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            check(this->IsRunning())
                            this->RunningInstance->GetOwnedPersonaControllerChecked()->GetWorld().AddDormantTicks();
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewStaticNode(WButton).SaveTo(&this->StopButton)
                    .MinDesiredSize(MinSize)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.Stop")))
                    .InBrush<EStyleBits::ActiveCombi|EStyleBits::Normal, &LRegionBrush::Tint>(*Prefs.DangerColor)
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        check(this->IsRunning())
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            if (this == this->RunningInstance)
                            {
                                check(!!this->EditorReconnectionData)

                                auto& Engine{this->GetEngine()};
                                if (!Engine.IsWorldValid(this->EditorReconnectionData->World))
                                {
                                    LOG_FATAL(LogFrontend, "[{}]: Return world is not valid.", this->GetNameAsString())
                                }

                                LWorld& PieWorld{this->GetOwnedPersonaControllerChecked()->GetWorld()};
                                this->RunningInstance = nullptr;
                                this->TravelTo(*this->EditorReconnectionData->World);
                                check(&this->GetOwnedPersonaControllerChecked()->GetWorld() != &PieWorld)
                                PieWorld.TearDownWithTrack();
                                this->GetOwnedPersonaControllerChecked()->GetOwnedPawnChecked()->GetRootComponent().SetLocalTransformByTeleport(this->EditorReconnectionData->EyeTrans);

                                this->EditorReconnectionData->World->Unpause();
                            }
                            else
                            {
                                check(&this->RunningInstance->GetViewport().GetSurface() != &this->GetViewport().GetSurface())
                                auto& Surface{this->RunningInstance->GetViewport().GetSurface()};
                                Surface.GetMutableFrontend().RemoveSurface(Surface);
                            }

                            this->EditorReconnectionData.reset();
                            this->UpdateStartStopButtons();
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                    + NewStaticNode(WButton).SaveTo(&this->DetachButton)
                        .MinDesiredSize(MinSize)
                        .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.Detach")))
                        .OnKeyEventFocused([](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                        {
                            if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                            {
                                return LNodeReply::Handled();
                            }
                            return LNodeReply::Unhandled();
                        })
                    + NewStaticNode(WButton).SaveTo(&this->MoreButton)
                        .MinDesiredSize(MinSize)
                        .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.VMore")))
                        .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                        {
                            if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                            {
                                this->CreateStartDropDown(Info.CursorLocation.value_or(maths::zero_vector<LVec2F>));
                                return LNodeReply::Handled();
                            }
                            return LNodeReply::Unhandled();
                        })
            ]
            + NewStaticNode(WSpacer).Anchor(EAnchor::HFill)
            + NewStaticNode(WHRegion)
                .Tint(Colors::Black)
                .Space(1_spt)
                .Padding(1_spt)
                .OutlineThickness(1)
                .OutlineTint(Colors::Black)
            [
                NewStaticNode(WButton).SaveTo(&this->SelectButton)
                    .MinDesiredSize(MinSize)
                    .Selectable(true)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.Cursor")))
                    .Selected(true)
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            this->SelectGizmo(EGizmo::Select);
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewStaticNode(WButton).SaveTo(&this->TranslateGizmoButton)
                    .MinDesiredSize(MinSize)
                    .Selectable(true)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.TranslationGizmo")))
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            this->SelectGizmo(EGizmo::Translate);
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewStaticNode(WButton).SaveTo(&this->RotateGizmoButton)
                    .MinDesiredSize(MinSize)
                    .Selectable(true)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.RotationGizmo")))
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            this->SelectGizmo(EGizmo::Rotate);
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewStaticNode(WButton).SaveTo(&this->ScaleGizmoButton)
                    .MinDesiredSize(MinSize)
                    .Selectable(true)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.ScaleGizmo")))
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            this->SelectGizmo(EGizmo::Scale);
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewStaticNode(WButton).SaveTo(&this->GizmoButton)
                    .MinDesiredSize(MinSize)
                    .Selectable(true)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.Gizmo")))
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            this->SelectGizmo(EGizmo::Gizmo);
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
            ]
            + NewStaticNode(WHRegion)
                .Tint(Colors::Black)
                .Space(1_spt)
                .Padding(1_spt)
                .OutlineThickness(1)
                .OutlineTint(Colors::Black)
            [
                NewStaticNode(WButton).SaveTo(&this->ToggleGridSpace)
                    .MinDesiredSize(MinSize)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.LocalGrid")))
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                        {
                            if (auto& Btn{Self.AsStatic<WButton>()}; this->IsGridSpaceLocal())
                            {
                                Btn.Style.SetEverywhere<&LRegionBrush::Background>(LRegionBrush::Icon("Icons/Jafg.Sphere"));
                                Btn.Brush.Background = LRegionBrush::Icon("Icons/Jafg.Sphere");
                            }
                            else
                            {
                                Btn.Style.SetEverywhere<&LRegionBrush::Background>(LRegionBrush::Icon("Icons/Jafg.LocalGrid"));
                                Btn.Brush.Background = LRegionBrush::Icon("Icons/Jafg.LocalGrid");
                            }
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
            ]
            + NewStaticNode(WHRegion)
                .Tint(Colors::Black)
                .Space(1_spt)
                .Padding(1_spt)
                .OutlineThickness(1)
                .OutlineTint(Colors::Black)
            [
                NewStaticNode(WButton).SaveTo(&this->ToggleTranslationGridSnapButton)
                    .MinDesiredSize(MinSize)
                    .Selectable(true)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.Grid")))
                    .Selected(true)
                    .OnKeyEventFocused([](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                        {
                            Self.AsStatic<WButton>().SetSelected(!Self.AsStatic<WButton>().IsSelected());
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewStaticNode(WTextButton).SaveTo(&this->TranslationGridSnapButton)
                    .MinDesiredSize(MinSize)
                    .Style(Prefs.EditorSecondaryButton<LBoxBrush,JUserPreferences::Radii|JUserPreferences::Outline>())
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign,&LTextBoxBrush::TextVAlign>(ETextHAlign::Center,ETextVAlign::Center)
                    .Content(std::to_string(this->SnapTranslation))
            ]
            + NewStaticNode(WHRegion)
                .Tint(Colors::Black)
                .Space(1_spt)
                .Padding(1_spt)
                .OutlineThickness(1)
                .OutlineTint(Colors::Black)
            [
                NewStaticNode(WButton).SaveTo(&this->ToggleRotationGridSnapButton)
                    .MinDesiredSize(MinSize)
                    .Selectable(true)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.Angle")))
                    .Selected(true)
                    .OnKeyEventFocused([](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                        {
                            Self.AsStatic<WButton>().SetSelected(!Self.AsStatic<WButton>().IsSelected());
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewStaticNode(WTextButton).SaveTo(&this->RotationGridSnapButton)
                    .MinDesiredSize(MinSize)
                    .Style(Prefs.EditorSecondaryButton<LBoxBrush,JUserPreferences::Radii|JUserPreferences::Outline>())
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign,&LTextBoxBrush::TextVAlign>(ETextHAlign::Center,ETextVAlign::Center)
                    .Content(std::to_string(this->SnapRotation))
                ]
            + NewStaticNode(WHRegion)
                .Tint(Colors::Black)
                .Space(1_spt)
                .Padding(1_spt)
                .OutlineThickness(1)
                .OutlineTint(Colors::Black)
            [
                NewStaticNode(WButton).SaveTo(&this->ToggleScaleGridSnapButton)
                    .MinDesiredSize(MinSize)
                    .Selectable(true)
                    .Style(Prefs.EditorSelectablePrimaryButton<LRegionBrush,JUserPreferences::Radii|JUserPreferences::Outline>(LTexture2::FromAsset("Icons/Jafg.ScaleGizmoLight")))
                    .Selected(true)
                    .OnKeyEventFocused([](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                        {
                            Self.AsStatic<WButton>().SetSelected(!Self.AsStatic<WButton>().IsSelected());
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewStaticNode(WTextButton).SaveTo(&this->ScaleGridSnapButton)
                    .MinDesiredSize(MinSize)
                    .Style(Prefs.EditorSecondaryButton<LBoxBrush,JUserPreferences::Radii|JUserPreferences::Outline>())
                    .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign,&LTextBoxBrush::TextVAlign>(ETextHAlign::Center,ETextVAlign::Center)
                    .Content(std::to_string(this->SnapScale))
            ]
            + NewStaticNode(WHRegion)
                    .Tint(Colors::Black)
                    .Space(1_spt)
                    .Padding(1_spt)
                    .OutlineThickness(1)
                    .OutlineTint(Colors::Black)
                [
                    NewStaticNode(WTextButtonIconizedDouble).SaveTo(&this->CameraButton)
                        .MinDesiredSize(MinSize)
                        .Style(Prefs.EditorSecondaryButton<LBoxBrush,JUserPreferences::Radii|JUserPreferences::Outline>())
                        .InAllTextBrushesChained<&LTextBoxBrush::TextHAlign,&LTextBoxBrush::TextVAlign>(ETextHAlign::Center,ETextVAlign::Center)
                        .Content(this->GetCameraSpeedString())
                        .LeftIcon(LTexture2::FromAsset("Icons/Jafg.Camera"))
                        .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                        {
                            if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                            {
                                CreateDropDownMenu(this->GetViewport(), Info.CursorLocation.value_or(maths::zero_vector<LVec2F>), {}, {
                                    LDropDownNodeScratch{.OnCreate=[this](LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)
                                    {
                                        return NewNode(Viewport).Class<WHParent>().Padding({LDropDownMenuCreateInfo::RecommendedPadding, 0.0f})
                                            .Anchor(EAnchor::HFill)
                                        [
                                            NewNode(Viewport).Class<WText>()
                                                .MinDesiredSize({100_spt, 0.0f})
                                                .Content("Speed")
                                            + NewNode(Viewport).Class<WInput_Vector1>(static_cast<WInput_Vector1::FloatingVector>(this->CameraSpeed), 1.0f)
                                                .Anchor(EAnchor::HFill)
                                                .OnVectorChanged([this](WInput_Vector1& Self)
                                                {
                                                    this->RequestedCameraSpeed = static_cast<f32>(Self.Get<f64>());
                                                })
                                        ];
                                    }},
                                    LDropDownNodeScratch{.OnCreate=[this](LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)
                                    {
                                        return NewNode(Viewport).Class<WHParent>().Padding({LDropDownMenuCreateInfo::RecommendedPadding, 0.0f})
                                            .Anchor(EAnchor::HFill)
                                        [
                                            NewNode(Viewport).Class<WText>()
                                                .MinDesiredSize({100_spt, 0.0f})
                                                .Content("Acceleration")
                                            + NewNode(Viewport).Class<WInput_Vector1>(static_cast<WInput_Vector1::FloatingVector>(this->CameraAcceleration))
                                                .Anchor(EAnchor::HFill)
                                                .OnVectorChanged([this](WInput_Vector1& Self)
                                                {
                                                    this->CameraAcceleration = static_cast<f32>(Self.Get<f64>());
                                                })
                                        ];
                                    }},
                                });

                                return LNodeReply::Handled();
                            }
                            return LNodeReply::Unhandled();
                        })
                ]
        ]
        + NewStaticNode(WSpacer).Height(16_pt)
        + NewStaticNode(WHParent).Anchor(EAnchor::HFill)
        [
            NewStaticNode(WSpacer).Anchor(EAnchor::HFill)
            + NewStaticNode(WText).SaveTo(&this->DebugLocationText)
        ]
    ];

    auto& MaterialSubsystem{*this->GetMutableFrontend().GetSubsystemChecked<JMaterialSubsystem>()};
    this->SelectionMaterialInstance = MaterialSubsystem.GetInstanceFromMaterialName("Jafg.Mesh.Outline");
    check(this->SelectionMaterialInstance.get())
    this->PostSelectionMaterialInstance = MaterialSubsystem.GetInstanceFromMaterialName("Jafg.Mesh.OutlinePost");
    check(this->PostSelectionMaterialInstance.get())

    this->UpdateStartStopButtons();

    if (*Prefs.EditorAutoLaunchLastWorld && !Prefs.EditorLastWorldName->empty())
    {
        this->GetMutableEngine().SummonWorld({
            .HumanReadableName = *Prefs.EditorLastWorldName,
            .SupremePoliciesClass = *Prefs.EditorLastWorldSupremePolicies,
            .TimeBehavior = EWorldTimeBehavior::Desist,
            .TextureCube = LTextureCube2::MakeSharedTextureCube2(LTextureCube2::CubeMap{"Content/Textures/LearnOpenGlSkybox"}),
            }, std::bind(&WEditorWorldViewer::TravelToJustSummoned, this, std::placeholders::_1));
    }
}

void Jafg::WEditorWorldViewer::Tick()
{
    Super::Tick();

    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& Surface{this->GetViewport().GetSurface()};

    check(this->DebugLocationText)
    bool bSet{};
    if (Prefs.EditorShowEyeTranslation)
    {
        if (auto* Ctrl{this->GetOwnedPersonaController()})
        {
            if (auto* Pawn{Ctrl->GetOwnedPawn()})
            {
                bSet = true;
                this->DebugLocationText->SetVisibility(ENodeVisibility::TransitiveHitTestInvisible);
                this->DebugLocationText->SetContent(maths::to_string(Pawn->GetRootComponent().GetLocalTranslation()));
            }
        }
    }
    if (!bSet)
    {
        this->DebugLocationText->SetVisibility(ENodeVisibility::Collapsed);
        this->DebugLocationText->EmptyContent();
    }

    /* Hardcoded escape sequence. To always be able to escape a potential compromised user input state. */
    if (auto It{algo::find(Surface.GetRawInputs(), Surface.GetFrontend().GetPhysicalKey(ELogicalKey::F1).value(), &LRawInput::PhysicalKey)};
        It != Surface.GetRawInputs().end())
    {
        if (It->Mods & EModBits::Shift)
        {
            if (this->GetConsumeHandle().IsValid())
            {
                LOG_TRACE(LogWidgetFramework, "[{}]: Escaping user input capture.", this->GetNameAsString())
                this->UserInput._bCurrentlyConsuming = false;
                this->GetConsumeHandle().Unbind();
                this->GetViewport().GetSurface().SetInputMode(EInputModeBits::ShowMouseCursor);
            }
        }
    }

    {
        AEditorCameraComponent* Comp{};
        if (auto* Ctrl{this->GetOwnedPersonaController()})
        {
            if (auto* Pawn{Ctrl->GetOwnedPawn()})
            {
                Comp = Pawn->GetComponent<AEditorCameraComponent>();
            }
        }

        if (Comp)
        {
            if (this->CameraSpeed != Comp->GetVelocityMultiplier() && !this->RequestedCameraSpeed)
            {
                this->RequestedCameraSpeed = Comp->GetVelocityMultiplier();
            }
        }
        if (this->RequestedCameraSpeed)
        {
            if (Comp)
            {
                this->RequestedCameraSpeed = Comp->SetVelocityMultiplier(*this->RequestedCameraSpeed);
            }
            this->CameraSpeed = *this->RequestedCameraSpeed;
            this->RequestedCameraSpeed.reset();
            this->CameraButton->SetContent(this->GetCameraSpeedString());
        }

        if (Comp)
        {
            Comp->SetVelocityMultiplierAcceleration(this->CameraAcceleration);
        }

        if (Comp && this->CurrentGizmo != EGizmo::Select && Surface.HasMouseLocationForOrtho())
        {
            if (LVec2F Location{Surface.GetMouseLocationValue() - this->GetAnchoredAndTranslatedTopLeftFromMostOuter(this->GetTranslationFromMostOuter())};
                !(Location.x < 0.0f || Location.y < 0.0f || Location.x > this->GetAnchoredSize_v2().x || Location.y > this->GetAnchoredSize_v2().y))
            {
                Comp->OnHighlightTrace(*this, this->GetWorldRenderTarget().GetExtent(), Location);
            }
        }
    }

    if (this->FocusTransition)
    {
        if (auto* Ctrl{this->GetOwnedPersonaController()})
        {
            if (auto* Pawn{Ctrl->GetOwnedPawn()})
            {
                auto Diff{algo::time_diff(this->FocusTransition->Start, this->GetEngine().FrameStartTimePoint) / this->FocusTransition->Duration};
                Pawn->GetRootComponent().SetLocalTranslationByTeleport(maths::linear_lerp(
                      this->FocusTransition->Origin
                    , this->FocusTransition->Destination
                    , static_cast<LWorldReal>(maths::clamp(Diff, 0.0, 1.0))
                    ));
                if (Diff >= 1.0)
                {
                    this->FocusTransition.reset();
                }
            }
            else
            {
                this->FocusTransition.reset();
            }
        }
        else
        {
            this->FocusTransition.reset();
        }
    }
}

void Jafg::WEditorWorldViewer::Destruct()
{
    Super::Destruct();

    if (this->HasHierarchy())
    {
        this->Hierarchy->_OnWorldViewerDestruct();
        check(!this->Hierarchy)
    }

    if (this->HasInspector())
    {
        this->Inspector->_OnWorldViewerDestruct();
        check(!this->Inspector)
    }
}

Jafg::LNodeReply Jafg::WEditorWorldViewer::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (Info.CursorLocation && Event.Is<ERawInputStateBits::Press>(ELogicalKey::LeftMouseButton))
    {
        if (this->IsOwnedPersonaControllerValid())
        {
            auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
            if (Ctrl.IsOwnedPawnValid())
            {
                auto& Pawn{*Ctrl.GetOwnedPawnChecked()};
                if (auto* Comp{Pawn.GetComponent<AEditorCameraComponent>()})
                {
                    if (LVec2F Location{*Info.CursorLocation - this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)};
                        Location.x < 0.0f || Location.y < 0.0f || Location.x > this->GetAnchoredSize_v2().x || Location.y > this->GetAnchoredSize_v2().y)
                    {
                        LOG_WARNING(LogWidgetFramework, "[{}]: Click location [{}] is outside of the world viewer size [.Offset={},.Extent={}]. Ignoring trace request."
                            , this->GetNameAsString(), maths::to_string(Location)
                            , maths::to_string(this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)), maths::to_string(this->GetAnchoredSize_v2()))
                    }
                    else
                    {
                        check(&this->GetViewport() == &Info.Viewport)
                        Comp->OnTrace(*this
                            , !!(Event.Mods & EModBits::Control)
                            , this->GetWorldRenderTarget().GetExtent()
                            , Location
                            , LEditorTraceOrigin{*this,Event}
                            );
                    }
                    return LNodeReply::Handled();
                }
            }
        }
    }

    if (Event.Is<ERawInputStateBits::Press>(ELogicalKey::RightMouseButton))
    {
        if (this->IsOwnedPersonaControllerValid())
        {
            if (!this->GetConsumeHandle().IsValid())
            {
                LOG_TRACE(LogWidgetFramework, "[{}]: Entering user input capture.", this->GetNameAsString())
                check(!this->UserInput._bCurrentlyConsuming)
                this->UserInput._bCurrentlyConsuming = true;
                this->GetViewport().GetSurface().SetInputMode(this->UserInput.IsConsumingMouse()
                    ? EInputModeBits::HideMouseCursor : EInputModeBits::ShowMouseCursor);

                if (!this->GetConsumeHandle().IsValid())
                {
                    this->BindConsumeHandle();
                }

                /* We want to also let the user input delegates run in this current tick. Where we activated it. */
                this->GetViewport().OnLateTick.Emplace([this]
                {
                    this->DispatchInputDelegates();
                    return true;
                });

                return LNodeReply::Handled(false);
            }
        }
    }

    if (Event.Is<ERawInputStateBits::Press>(this->GetViewport().GetSurface().GetFrontend().GetPhysicalKey(ELogicalKey::Q)))
    {
        this->SelectGizmo(EGizmo::Select);
        return LNodeReply::Handled();
    }
    if (Event.Is<ERawInputStateBits::Press>(this->GetViewport().GetSurface().GetFrontend().GetPhysicalKey(ELogicalKey::W)))
    {
        this->SelectGizmo(EGizmo::Translate);
        return LNodeReply::Handled();
    }
    if (Event.Is<ERawInputStateBits::Press>(this->GetViewport().GetSurface().GetFrontend().GetPhysicalKey(ELogicalKey::E)))
    {
        this->SelectGizmo(EGizmo::Rotate);
        return LNodeReply::Handled();
    }
    if (Event.Is<ERawInputStateBits::Press>(this->GetViewport().GetSurface().GetFrontend().GetPhysicalKey(ELogicalKey::R)))
    {
        this->SelectGizmo(EGizmo::Scale);
        return LNodeReply::Handled();
    }
    if (Event.Is<ERawInputStateBits::Press>(this->GetViewport().GetSurface().GetFrontend().GetPhysicalKey(ELogicalKey::T)))
    {
        this->SelectGizmo(EGizmo::Gizmo);
        return LNodeReply::Handled();
    }

    if (Event.Is<ERawInputStateBits::Press>(this->GetViewport().GetSurface().GetFrontend().GetPhysicalKey(ELogicalKey::F)))
    {
        this->FocusActors();
        return LNodeReply::Handled();
    }

    return Super::OnKeyEventFocused(Info, Event);
}

void Jafg::WEditorWorldViewer::SelectActors(TArray<std::pair<AActor*, AActorComponent*>> Actors, bool bForce /* = false */)
{
    check(algo::all_of(Actors, [](auto& Elem){ return Elem.first; }))
    checkCode
    (
        std::unordered_set<AActor*> ActorSet;
        for (auto* Actor: Actors | std::views::keys)
        {
            check(ActorSet.insert(Actor).second)
        }
    )

    if (!bForce && this->SelectedActors == Actors)
    {
        return;
    }

    auto Old{std::exchange(this->SelectedActors, std::move(Actors))};
    LOG_TRACE(LogWidgetFramework, "[{}]: Selected [{}] actors.", this->GetNameAsString(), this->SelectedActors.size())
    this->OnActorsSelected.Broadcast(Old, this->SelectedActors);

    if (auto* Ctrl{this->GetOwnedPersonaController()})
    {
        if (auto* Comp{Ctrl->GetComponent<AEditorPersonaControllerComponent>()})
        {
            if (this->SelectedActors.size() == 1)
            {
                Comp->SetSelectedActor(this, this->SelectedActors.front());
            }
            else
            {
                Comp->SetSelectedActor(nullptr, {nullptr,nullptr});
            }
        }
    }
}

void Jafg::WEditorWorldViewer::SelectGizmo(EGizmo Gizmo)
{
    check(this->SelectButton)
    check(this->TranslateGizmoButton)
    check(this->RotateGizmoButton)
    check(this->ScaleGizmoButton)
    check(this->GizmoButton)

    if (this->CurrentGizmo == Gizmo)
    {
        return;
    }

    this->CurrentGizmo = Gizmo;
    this->SelectButton->SetSelected(Gizmo == EGizmo::Select);
    this->TranslateGizmoButton->SetSelected(Gizmo == EGizmo::Translate);
    this->RotateGizmoButton->SetSelected(Gizmo == EGizmo::Rotate);
    this->ScaleGizmoButton->SetSelected(Gizmo == EGizmo::Scale);
    this->GizmoButton->SetSelected(Gizmo == EGizmo::Gizmo);

    this->UpdateClientGizmo();
}

void Jafg::WEditorWorldViewer::UpdateClientGizmo()
{
    if (auto* Ctrl{this->GetOwnedPersonaController()})
    {
        if (auto* Comp{Ctrl->GetComponent<AEditorPersonaControllerComponent>()})
        {
            (void)Comp->SetSelectedGizmo(this->CurrentGizmo);
        }
    }
}

void Jafg::WEditorWorldViewer::FocusActors()
{
    if (this->SelectedActors.empty())
    {
        return;
    }

    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto GetAabb{[](AActor& Actor) noexcept -> std::optional<LWorldAabb3>
    {
        LWorldAabb3 Aabb{Actor.GetTransformedActorAabb()};
        if (Aabb.empty())
        {
            return {};
        }
        if (Aabb.squared_diagonal_length() < static_cast<LWorldReal>(0.01))
        {
            return LWorldAabb3{
                .min=LWorldVec3{static_cast<LWorldReal>(-0.1)},
                .max=LWorldVec3{static_cast<LWorldReal>(0.1)}
                }.apply(Actor.GetRootComponent().GetLocalTransform());
        }
        return Aabb;
    }};
    auto Aabb{GetAabb(*this->SelectedActors.front().first)};
    for (auto Idx{1uz}; Idx < this->SelectedActors.size(); ++Idx)
    {
        auto Aabb2{GetAabb(*this->SelectedActors[Idx].first)};
        if (!Aabb)
        {
            Aabb = Aabb2;
        }
        else if (Aabb2)
        {
            Aabb->min = maths::min(Aabb->min, Aabb2->min);
            Aabb->max = maths::max(Aabb->max, Aabb2->max);
        }
    }
    if (!Aabb)
    {
        LOG_VERBOSE(LogEditor, "[{}]: Cannot focus actors because they have no valid aabb.", this->GetNameAsString())
        return;
    }

    auto Corners{Aabb->corners()};

    auto& Pawn{*this->GetOwnedPersonaControllerChecked()->GetOwnedPawnChecked()};
    auto Origin{Pawn.GetRootComponent().GetLocalTranslation()};
    Pawn.GetRootComponent().SetLocalTranslationByTeleport(Aabb->center());
    auto Eye{Pawn.GetEye()};
    auto EyeRight{cross(Eye.front, Eye.up)};

    auto Extent{this->GetWorldRenderTarget().GetExtent()};
    LWorldReal TanY{static_cast<LWorldReal>(maths::tan(Eye.vert_fov * 0.5f))};
    LWorldReal TanX{TanY * (static_cast<LWorldReal>(Extent.width) / static_cast<LWorldReal>(Extent.height))};

    LWorldReal BackwardsMagnitude{0.0};
    for (LWorldVec3& C: Corners)
    {
        LWorldVec3 Rel{C - Eye.translation};
        LWorldVec3 Vec{dot(Rel, EyeRight), dot(Rel, Eye.up), dot(Rel, Eye.front)};
        LWorldReal RequiredZ{
            (maths::max(maths::abs(Vec.x) / TanX, maths::abs(Vec.y) / TanY))
            * static_cast<LWorldReal>(*Prefs.EditorActorFocusProjectedForwardMultiplier)
            };
        BackwardsMagnitude = maths::max(BackwardsMagnitude, RequiredZ - Vec.z);
    }
    Pawn.GetRootComponent().AddLocalTranslationByTeleport(Eye.front * -BackwardsMagnitude);

    this->FocusTransition = LFocusTransition{
        .Origin = Origin,
        .Destination = Pawn.GetRootComponent().GetLocalTranslation(),
        .Start = GetEngine().FrameStartTimePoint,
        .Duration = *Prefs.EditorActorFocusTransitionDuration,
        };
    Pawn.GetRootComponent().SetLocalTranslationByTeleport(this->FocusTransition->Origin);
}

bool Jafg::WEditorWorldViewer::IsGridSpaceLocal() const noexcept
{
    check(this->ToggleGridSpace)
    auto& Bg{this->ToggleGridSpace->Style.NormalBrush.Background};
    return std::holds_alternative<LRegionBrush::LIcon>(Bg)
        && std::get<LRegionBrush::LIcon>(Bg).Texture->GetPath().generic_string().ends_with("Icons/Jafg.LocalGrid.png");
}

bool Jafg::WEditorWorldViewer::IsTranslationGridSnapEnabled() const noexcept
{
    check(this->ToggleTranslationGridSnapButton)
    return this->ToggleTranslationGridSnapButton->IsSelected();
}

bool Jafg::WEditorWorldViewer::IsRotationGridSnapEnabled() const noexcept
{
    check(this->ToggleRotationGridSnapButton)
    return this->ToggleRotationGridSnapButton->IsSelected();
}

bool Jafg::WEditorWorldViewer::IsScaleGridSnapEnabled() const noexcept
{
    check(this->ToggleScaleGridSnapButton)
    return this->ToggleScaleGridSnapButton->IsSelected();
}

bool Jafg::WEditorWorldViewer::OnPreDrawImpl(LRenderInfo const& Info)
{
    // TODO: Move this to after the main draw?
    //       Currently we cannot depth test against the main draw. But if we do we may have the option
    //       To discard fragments if they failed to depth test against the main draw. Currently everything is just 1.0 in the depth
    //       Then we can also remove the depth image from this auxiliary render target.
    if (!this->SelectedActors.empty())
    {
        this->GetWorldRenderTarget().RenderSelected(Info, std::bind(&WEditorWorldViewer::PreDrawSelected, this, std::placeholders::_1));
    }

    bool Result{Super::OnPreDrawImpl(Info)};

    if (physx::debug_renderer::get())
    {
        physx::debug_renderer::get()->AdvanceFrameIfUsed();
    }

    return Result;
}

void Jafg::WEditorWorldViewer::OnPostWorldDrawImpl(LRenderInfo const& Info, APersonaController& Ctrl, APawn& Pawn, LWorldEye const& Eye)
{
    Super::OnPostWorldDrawImpl(Info, Ctrl, Pawn, Eye);

    auto& Prefs{GetSingleton<JUserPreferences>()};

    if (physx::debug_renderer::get())
    {
        if (auto* Comp{Ctrl.GetComponent<AEditorPersonaControllerComponent>()})
        {
            JPH::BodyManager::DrawSettings DrawSettings{
                .mDrawGetSupportFunction = *Prefs.EditorVisualizeGetSupportFunction,
                .mDrawSupportDirection = *Prefs.EditorVisualizeSupportDirection,
                .mDrawGetSupportingFace = *Prefs.EditorVisualizeGetSupportingFace,
                .mDrawShape = *Prefs.EditorVisualizeShape,
                .mDrawShapeWireframe = *Prefs.EditorVisualizeShapeWireframe,
                .mDrawShapeColor = JPH::BodyManager::EShapeColor{std::to_underlying(*Prefs.EditorVisualizeShapeColor)},
                .mDrawBoundingBox = *Prefs.EditorVisualizeBoundingBox,
                .mDrawCenterOfMassTransform = *Prefs.EditorVisualizeCenterOfMassTransform,
                .mDrawWorldTransform = *Prefs.EditorVisualizeWorldTransform,
                .mDrawVelocity = *Prefs.EditorVisualizeVelocity,
                .mDrawMassAndInertia = *Prefs.EditorVisualizeMassAndInertia,
                .mDrawSleepStats = *Prefs.EditorVisualizeSleepStats,
                .mDrawSoftBodyVertices = *Prefs.EditorVisualizeSoftBodyVertices,
                .mDrawSoftBodyVertexVelocities = *Prefs.EditorVisualizeSoftBodyVertexVelocities,
                .mDrawSoftBodyEdgeConstraints = *Prefs.EditorVisualizeSoftBodyEdgeConstraints,
                .mDrawSoftBodyBendConstraints = *Prefs.EditorVisualizeSoftBodyBendConstraints,
                .mDrawSoftBodyVolumeConstraints = *Prefs.EditorVisualizeSoftBodyVolumeConstraints,
                .mDrawSoftBodySkinConstraints = *Prefs.EditorVisualizeSoftBodySkinConstraints,
                .mDrawSoftBodyLRAConstraints = *Prefs.EditorVisualizeSoftBodyLRAConstraints,
                .mDrawSoftBodyRods = *Prefs.EditorVisualizeSoftBodyRods,
                .mDrawSoftBodyRodStates = *Prefs.EditorVisualizeSoftBodyRodStates,
                .mDrawSoftBodyRodBendTwistConstraints = *Prefs.EditorVisualizeSoftBodyRodBendTwistConstraints,
                .mDrawSoftBodyPredictedBounds = *Prefs.EditorVisualizeSoftBodyPredictedBounds,
                .mDrawSoftBodyConstraintColor = JPH::ESoftBodyConstraintColor{std::to_underlying(*Prefs.EditorVisualizeSoftBodyConstraintColor)},
                };

            if (
                   DrawSettings.mDrawGetSupportFunction
                || DrawSettings.mDrawGetSupportingFace
                || DrawSettings.mDrawShape
                || DrawSettings.mDrawBoundingBox
                || DrawSettings.mDrawCenterOfMassTransform
                || DrawSettings.mDrawWorldTransform
                || DrawSettings.mDrawVelocity
                || DrawSettings.mDrawMassAndInertia
                || DrawSettings.mDrawSleepStats
                || DrawSettings.mDrawSoftBodyVertices
                || DrawSettings.mDrawSoftBodyVertexVelocities
                || DrawSettings.mDrawSoftBodyEdgeConstraints
                || DrawSettings.mDrawSoftBodyBendConstraints
                || DrawSettings.mDrawSoftBodyVolumeConstraints
                || DrawSettings.mDrawSoftBodySkinConstraints
                || DrawSettings.mDrawSoftBodyLRAConstraints
                || DrawSettings.mDrawSoftBodyRods
                || DrawSettings.mDrawSoftBodyRodStates
                || DrawSettings.mDrawSoftBodyRodBendTwistConstraints
                || DrawSettings.mDrawSoftBodyPredictedBounds
                )
            {
                physx::debug_renderer::get()->PrepareFrame(Eye, Comp);
                Ctrl.GetWorld().GetWorldGlobalPhysicsSystem()->DrawBodies(DrawSettings, physx::debug_renderer::get(), {});
            }
        }
    }

    if (!this->SelectedActors.empty())
    {
        check(this->PostSelectionMaterialInstance->FrequentDescriptorSets[Info.Frame].empty())
        check(this->PostSelectionMaterialInstance->InfrequentDescriptorSets.size() == 1)

        // auto& ShaderSubsystem{*this->GetFrontend().GetSubsystemChecked<JShaderSubsystem>()};

        // auto& FetchedMaterial{this->PostSelectionMaterialInstance->Material->FetchedMaterial};
        auto& Pipeline{this->PostSelectionMaterialInstance->Material->Pipeline};
        algo::for_each(this->PostSelectionMaterialInstance->InfrequentDescriptorSets, [&](auto& Set)
        {
            Info.CommandBuffer.bindDescriptorSets2({
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .layout = Pipeline.pipeline_layout,
                .firstSet = 0,
                .descriptorSetCount = 1,
                .pDescriptorSets = &**Set,
                .dynamicOffsetCount = 0,
                .pDynamicOffsets = nullptr
                });
        });

        Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *this->PostSelectionMaterialInstance->Material->Pipeline);

        PC::Outline pc{
            .InverseViewportExtent = {1.0f / static_cast<f32>(Info.VkViewport.width), 1.0f / static_cast<f32>(Info.VkViewport.height)},
            .Thickness = 2,
            .Tint = Colors::Orange,
            };
        Info.CommandBuffer.pushConstants2({
            .layout = *Pipeline.pipeline_layout,
            .stageFlags = PC::Outline::shader_stage_flags(),
            .offset = 0,
            .size = sizeof(PC::Outline),
            .pValues = &pc
            });

        Info.CommandBuffer.draw(3, 1, 0, 0);
    }
}

Jafg::LTransientPersona::Local Jafg::WEditorWorldViewer::GetTransientPersona() noexcept
{
    bool bEditor{this->RunningInstance != this || std::exchange(this->bIsNextPersonaEditorControlled, false)};

    if (bEditor)
    {
        LOG_TRACE(LogEditor, "[{}]: Transient persona requested is an editor instance.", this->GetNameAsString())
    }
    else
    {
        LOG_TRACE(LogEditor, "[{}]: Transient persona requested is defaulted.", this->GetNameAsString())
    }

    return LTransientPersona::Local{
        .Lackey=*this,
        .bEditor=bEditor,
        };
}

void Jafg::WEditorWorldViewer::OnConnect()
{
    Super::OnConnect();

    if (this->EditorReconnectionData)
    {
        if (auto* Ctrl{this->GetOwnedPersonaController()})
        {
            if (Ctrl->GetComponent<AEditorPersonaControllerComponent>())
            {
                if (auto* Pawn{Ctrl->GetOwnedPawn()})
                {
                    if (Pawn->GetComponent<AEditorCameraComponent>())
                    {
                        Pawn->GetRootComponent().SetLocalTransformByTeleport(this->EditorReconnectionData->EyeTrans);
                    }
                }
            }
            else if (this->PawnStart == EPawnSart::EditorEye)
            {
                if (auto* Pawn{Ctrl->GetOwnedPawn()})
                {
                    Pawn->GetRootComponent().SetLocalTransformByTeleport(this->EditorReconnectionData->EyeTrans);
                }
            }
        }
    }

    this->UpdateClientGizmo();

    if (this->Hierarchy)
    {
        this->Hierarchy->OnWorldViewerUpdate();
    }
    if (this->Inspector)
    {
        this->Inspector->OnWorldViewerUpdate();
    }
}

void Jafg::WEditorWorldViewer::OnDisconnect()
{
    this->SelectActors({});

    Super::OnDisconnect();

    if (this->Hierarchy)
    {
        this->Hierarchy->OnWorldViewerUpdate();
    }
    if (this->Inspector)
    {
        this->Inspector->OnWorldViewerUpdate();
    }
}

void Jafg::WEditorWorldViewer::PreDrawSelected(LRenderInfo const& Info)
{
    check(!this->SelectedActors.empty())

    TArray<AActor*> ValidActors; ValidActors.reserve(this->SelectedActors.size());
    for (auto& Actor: this->SelectedActors)
    {
        if (!Actor.first->IsA<APersonaController>())
        {
            ValidActors.push_back(Actor.first);
        }
    }

    if (!ValidActors.empty())
    {
        auto& Pawn{*this->GetOwnedPersonaControllerChecked()->GetOwnedPawnChecked()};
        Pawn.GetWorld().Draw(Info, Pawn.GetEye(), &*this->SelectionMaterialInstance, {}, ValidActors);
    }
}

void Jafg::WEditorWorldViewer::InitializeRenderTarget()
{
    Super::InitializeRenderTarget();

    auto& Frontend{this->GetFrontend()};

    {
        auto Binding{this->PostSelectionMaterialInstance->GetBinding("stencil_texture")};
        vk::DescriptorImageInfo Info{
            .imageView = this->GetWorldRenderTarget().GetSelectedImageView(),
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
            };
        check(Info.sampler == nullptr)
        auto It{algo::find(this->PostSelectionMaterialInstance->InfrequentDescriptorSets, Binding.space, &LMaterialInstance::DescriptorSetInstance::Space)};
        check(It != this->PostSelectionMaterialInstance->InfrequentDescriptorSets.end())
        std::array Writes{vk::WriteDescriptorSet{
            .dstSet = **It,
            .dstBinding = Binding.index,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eSampledImage,
            .pImageInfo = &Info,
            },};
        Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
    }
    {
        auto Binding{this->PostSelectionMaterialInstance->GetBinding("sampler")};
        vk::DescriptorImageInfo Info{
            .sampler = Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetNearestSamplerClampToBorder(),
            };
        auto It{algo::find(this->PostSelectionMaterialInstance->InfrequentDescriptorSets, Binding.space, &LMaterialInstance::DescriptorSetInstance::Space)};
        check(It != this->PostSelectionMaterialInstance->InfrequentDescriptorSets.end())
        std::array Writes{vk::WriteDescriptorSet{
            .dstSet = **It,
            .dstBinding = Binding.index,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eSampler,
            .pImageInfo = &Info,
            },};
        Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
    }
}

void Jafg::WEditorWorldViewer::CreateMenuDropDown(LVec2F Where)
{
    struct LSharedNodes
    {
        WText* DisplayText{};
        WInput_Vector2* VectorInput{};
    };
    auto SharedNodes{std::make_shared<LSharedNodes>()};
    auto Res{std::make_shared<LVec2u64>(this->GetWorldRenderTarget().GetExtent().width, this->GetWorldRenderTarget().GetExtent().height)};
    CreateDropDownMenu(this->GetViewport(), Where, {}, {
        LDropDownNodeSeparator{.DisplayName = "VIEWPORT"},
        LDropDownNodeCustom{
            .OnCreate=[this, SharedNodes, Res](LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)
            {
                return NewNode(Viewport).Class<WSpacer>().Width(2_spt)
                + NewNode(Viewport).Class<WCheckmarkButton>()
                    .Anchor(EAnchor::CenterLeft)
                    .Checked(!this->DesiredViewportExtent.has_value())
                    .OnKeyEventFocused([this, SharedNodes, Res](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                            && Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation)
                            )
                        {
                            if (this->DesiredViewportExtent)
                            {
                                this->DesiredViewportExtent.reset();
                            }
                            else
                            {
                                this->DesiredViewportExtent = WEditorWorldViewer::DefaultExtent;
                            }

                            auto& Btn{Self.AsStatic<WCheckmarkButton>()};
                            Btn.SetChecked(!this->DesiredViewportExtent.has_value());

                            check(SharedNodes->DisplayText && SharedNodes->VectorInput)
                            LVec2u64 ShownVector{this->DesiredViewportExtent.has_value()
                                ? this->DesiredViewportExtent->ToVec<u64>()
                                : this->GetLastUnstableExtent().has_value()
                                    ? this->GetLastUnstableExtent()->ToVec<u64>()
                                    : this->GetWorldRenderTargetViewport().GetExtent().ToVec<u64>()};
                            SharedNodes->VectorInput->Set(ShownVector);
                            check(*Res == ShownVector)
                            SharedNodes->VectorInput->SetInputEnabled(this->IsManual());
                            SharedNodes->DisplayText->TextBrush.Tint = this->IsManual()
                                ? Colors::White : Colors::Gray;
                            SharedNodes->DisplayText->GetParentChecked()->AsStatic<LDropDownNodeCustom::Parent>()
                                .SetEnabled(this->DesiredViewportExtent.has_value());

                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewNode(Viewport).Class<WText>()
                    .Anchor(EAnchor::HFill)
                    .Visibility(ENodeVisibility::TransitiveHitTestInvisible)
                    .Padding({6_spt, 0.0f, 0.0f, 0.0f})
                    .Content("Auto Resolve Resolution");
            },
            .OnAction=[this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                {
                    if (this->DesiredViewportExtent)
                    {
                        this->DesiredViewportExtent.reset();
                    }
                    else
                    {
                        this->DesiredViewportExtent = WEditorWorldViewer::DefaultExtent;
                    }
                    return LDropDownNodeCustom::reply::handled(true);
                }
                return LDropDownNodeCustom::reply::unhandled();
            },},
        LDropDownNodeInformation{
            .What = algo::sprintf("Min-Resolution: {}x{}", WWorldViewer::MinViewportExtent.width, WWorldViewer::MinViewportExtent.height),
            },
        LDropDownNodeCustom{
            .OnCreate=[this, SharedNodes, Res](LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)
            {
                return NewNode(Viewport).Class<WText>().SaveTo(&SharedNodes->DisplayText)
                    .Padding({LDropDownMenuCreateInfo::RecommendedPaddedTextPadding, 0.0f, 0.0f, 0.0f})
                    .MinDesiredSize({128_spt, 0})
                    .TextTint(this->IsManual() ? Colors::White : Colors::Gray)
                    .Content("Resolution")
                + NewNode(Viewport).Class<WInput_Vector2>(*Res).SaveTo(&SharedNodes->VectorInput)
                    .OnVectorChanged([Res](WInput_Vector2& Self)
                    {
                        *Res = Self.Get<u64>();
                    })
                    .Enabled(this->IsManual())
                    .OnDestruct([this, Res](auto&&...)
                    {
                        if (this->IsManual())
                        {
                            if constexpr (LogWidgets.CompilesFor<ELogVerbosity::Verbose>)
                            if (Res->x < WWorldViewer::MinViewportExtent.width || Res->y < WWorldViewer::MinViewportExtent.height)
                            {
                                LOG_VERBOSE(LogWidgets
                                    , "[{}]: Clamping resolution {} to minimum allowed resolution {}."
                                    , this->GetNameAsString(), maths::to_string(*Res)
                                    , maths::to_string(maths::max(*Res, WWorldViewer::MinViewportExtent.ToVec<u64>()))
                                    )
                            }
                            *Res = maths::max(*Res, WWorldViewer::MinViewportExtent.ToVec<u64>());
                            this->DesiredViewportExtent = rhi::extent2::from_vec(*Res);
                        }
                    });
            },
            .IsEnabled = this->IsManual(),
            .OnAction=[](auto&&...){ return LDropDownNodeCustom::reply::unhandled(); },
            },
        // TODO: Make an option for commonly used resolutions
        LDropDownNodeSeparator{.DisplayName = "WORLD"},
        LDropDownNodeDeferredSubMenu{
            .Selector = {
                .DisplayName = "Connect to",
                .Icon = "Icons/Jafg.Sphere",
                },
            .OnChildren = {[this]
            {
                TArray<LDropDownNode> Result;

                Result.emplace_back(LDropDownNodeSeparator{.DisplayName="ENGINE WORLDS"});
                if (auto& Tracks{this->GetEngine().GetTracks()}; Tracks.empty())
                {
                    Result.emplace_back(LDropDownNodeInformation{.What="There are currently no worlds running."});
                }
                else
                {
                    for (auto& Track: Tracks)
                    {
                        auto& World{Track.GetWorld()};
                        if (World.GetWorldState() == EWorldState::Running)
                        {
                            Result.emplace_back(LDropDownNodeOption{
                                .Selector = {
                                    .DisplayName = World.GetHumanReadableName(),
                                    .Icon = "Icons/Jafg.Sphere",
                                    },
                                .OnAction = [this, &World](auto&&...)
                                {
                                    this->TravelTo(World);
                                    return algo::reply::unhandled();
                                },});
                        }
                        else
                        {
                            Result.emplace_back(LDropDownNodeInformation{
                                .What = algo::sprintf("{} (Not joinable)", World.GetHumanReadableName()),
                                });
                        }
                    }
                }

                struct LNewWorldNodes
                {
                    WEditableTextButton* Name{};
                    WEditableTextButton* SupremePolicies{};
                };
                auto NewWorldNodes{std::make_shared<LNewWorldNodes>()};
                Result.emplace_back(LDropDownNodeSeparator{.DisplayName="NEW WORLD"});
                Result.emplace_back(LDropDownNodeScratch{.OnCreate=[NewWorldNodes]
                (LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)
                {
                    auto& Prefs{GetSingleton<JUserPreferences>()};
                    return NewNode(Viewport).Class<WHParent>()
                        .Anchor(EAnchor::HFill)
                        .Padding({LDropDownMenuCreateInfo::RecommendedPadding, 0})
                    [
                        NewNode(Viewport).Class<WText>()
                            .Padding({LDropDownMenuCreateInfo::RecommendedPaddedTextPadding, 0.0f, 0.0f, 0.0f})
                            .MinDesiredSize({128_spt, 0})
                            .TextTint(Colors::White)
                            .Content("World Name")
                        + NewNode(Viewport).Class<WEditableTextButton>().SaveTo(&NewWorldNodes->Name)
                            .MinDesiredSize({128_spt, 0})
                            .Padding({2_spt, 0})
                            .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
                            .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
                            .Content(*Prefs.EditorLastWorldName)
                            .PlaceholderContent("World Name")
                    ];},});
                Result.emplace_back(LDropDownNodeScratch{.OnCreate=[NewWorldNodes]
                (LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)
                {
                    auto& Prefs{GetSingleton<JUserPreferences>()};
                    return NewNode(Viewport).Class<WHParent>()
                        .Anchor(EAnchor::HFill)
                        .Padding({LDropDownMenuCreateInfo::RecommendedPadding, 0})
                    [
                        NewNode(Viewport).Class<WText>()
                            .Padding({LDropDownMenuCreateInfo::RecommendedPaddedTextPadding, 0.0f, 0.0f, 0.0f})
                            .MinDesiredSize({128_spt, 0})
                            .TextTint(Colors::White)
                            .Content("Level Name")
                        + NewNode(Viewport).Class<WEditableTextButton>().SaveTo(&NewWorldNodes->SupremePolicies)
                            .MinDesiredSize({128_spt, 0})
                            .Padding({2_spt, 0})
                            .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
                            .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
                            .Content(Prefs.EditorLastWorldSupremePolicies.IsModified()
                                ? Prefs.EditorLastWorldSupremePolicies->GetClassOrDefault().GetFullyQualifiedName()
                                : Prefs.EditorLastWorldSupremePolicies.Default.GetClassOrDefault().GetFullyQualifiedName()
                                )
                            .PlaceholderContent("Supreme policies class")
                    ];},});
                Result.emplace_back(LDropDownNodeOption{
                    .Selector = {
                        .DisplayName = "Create New World",
                        },
                    .OnAction = [this, NewWorldNodes](auto&&...)
                    {
                        check(NewWorldNodes.get() && NewWorldNodes->Name && NewWorldNodes->SupremePolicies)
                        auto& Prefs{GetMutableSingleton<JUserPreferences>()};
                        Prefs.EditorLastWorldName = NewWorldNodes->Name->GetContent();
                        Prefs.EditorLastWorldSupremePolicies =
                            Detail::GetGlobalCxxRecordRegistry().GetClassByNameAsserted(NewWorldNodes->SupremePolicies->GetContent())
                                ->StaticClass;
                        this->GetMutableEngine().SummonWorld({
                            .HumanReadableName = *Prefs.EditorLastWorldName,
                            .SupremePoliciesClass = *Prefs.EditorLastWorldSupremePolicies,
                            .TimeBehavior = EWorldTimeBehavior::Desist,
                            .TextureCube = LTextureCube2::MakeSharedTextureCube2(LTextureCube2::CubeMap{"Content/Textures/LearnOpenGlSkybox"}),
                            }, std::bind(&WEditorWorldViewer::TravelToJustSummoned, this, std::placeholders::_1));
                        return algo::reply::unhandled();
                    },});
                return Result;
            },},},
        LDropDownNodeOption{
            .Selector = {
                .DisplayName = "Disconnect",
                .Icon = "Icons/Jafg.Leave",
                },
            .IsEnabled = !!this->IsOwnedPersonaControllerValid(),
            .OnAction = [this](auto&&...)
            {
                this->Disconnect();
                return algo::reply::unhandled();
            },},
        LDropDownNodeOption{
            .Selector = {
                .DisplayName = "Shutdown",
                .Icon = "Icons/Jafg.Leave",
                },
            .IsEnabled = !!this->IsOwnedPersonaControllerValid(),
            .OnAction = [this](auto&&...)
            {
                auto& World{this->GetOwnedPersonaControllerChecked()->GetWorld()};
                this->Disconnect();
                check(!this->IsOwnedPersonaControllerValid())
                World.TearDownWithTrack();
                return algo::reply::unhandled();
            },},
        CreateDropDownCheckmark("Auto-Launch Last World", *GetSingleton<JUserPreferences>().EditorAutoLaunchLastWorld, []
        {
            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
            MutablePrefs.EditorAutoLaunchLastWorld = !*MutablePrefs.EditorAutoLaunchLastWorld;
            return *MutablePrefs.EditorAutoLaunchLastWorld;
        }),
        CreateDropDownCheckmark("Depth test", *GetSingleton<JUserPreferences>().EditorPerspectiveDepthTestHint, [this]
        {
            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
            MutablePrefs.EditorPerspectiveDepthTestHint = !*MutablePrefs.EditorPerspectiveDepthTestHint;
            this->OnPerspectiveDepthTestChanged();
            return *MutablePrefs.EditorPerspectiveDepthTestHint;
        }),
        CreateDropDownCheckmark("Show eye translation", *GetSingleton<JUserPreferences>().EditorShowEyeTranslation, []
        {
            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
            MutablePrefs.EditorShowEyeTranslation = !*MutablePrefs.EditorShowEyeTranslation;
            return *MutablePrefs.EditorShowEyeTranslation;
        }),
        LDropDownNodeSeparator{.DisplayName="Visualization"},
        CreateDropDownCheckmark("Visualize actor aabbs", *GetSingleton<JUserPreferences>().EditorVisualizeAabbs, []
        {
            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
            MutablePrefs.EditorVisualizeAabbs = !*MutablePrefs.EditorVisualizeAabbs;
            return *MutablePrefs.EditorVisualizeAabbs;
        }),
        CreateDropDownCheckmark("Visualize transitive actor aabbs", *GetSingleton<JUserPreferences>().EditorVisualizeTransitiveAabbs, []
        {
            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
            MutablePrefs.EditorVisualizeTransitiveAabbs = !*MutablePrefs.EditorVisualizeTransitiveAabbs;
            return *MutablePrefs.EditorVisualizeTransitiveAabbs;
        }),
        CreateDropDownCheckmark("Visualize traces", *GetSingleton<JUserPreferences>().EditorVisualizeTraces, []
        {
            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
            MutablePrefs.EditorVisualizeTraces = !*MutablePrefs.EditorVisualizeTraces;
            return *MutablePrefs.EditorVisualizeTraces;
        }),
        CreateDropDownCheckmark("Visualize trace hits", *GetSingleton<JUserPreferences>().EditorVisualizeTraceHits, []
        {
            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
            MutablePrefs.EditorVisualizeTraceHits = !*MutablePrefs.EditorVisualizeTraceHits;
            return *MutablePrefs.EditorVisualizeTraceHits;
        }),
        CreateDropDownCheckmark("Visualize gizmo interactions", *GetSingleton<JUserPreferences>().EditorVisualizeGizmoInteractions, []
        {
            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
            MutablePrefs.EditorVisualizeGizmoInteractions = !*MutablePrefs.EditorVisualizeGizmoInteractions;
            return *MutablePrefs.EditorVisualizeGizmoInteractions;
        }),
        LDropDownNodeDeferredSubMenu{
            .Selector = {.DisplayName = "Visualize physics",},
            .OnChildren = [] -> TArray<LDropDownNode> { return {
                CreateDropDownCheckmark("GetSupportFunction", *GetSingleton<JUserPreferences>().EditorVisualizeGetSupportFunction, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeGetSupportFunction = !*MutablePrefs.EditorVisualizeGetSupportFunction;
                    return *MutablePrefs.EditorVisualizeGetSupportFunction;
                }),
                CreateDropDownCheckmark("SupportDirection", *GetSingleton<JUserPreferences>().EditorVisualizeSupportDirection, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSupportDirection = !*MutablePrefs.EditorVisualizeSupportDirection;
                    return *MutablePrefs.EditorVisualizeSupportDirection;
                }),
                CreateDropDownCheckmark("GetSupportingFace", *GetSingleton<JUserPreferences>().EditorVisualizeGetSupportingFace, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeGetSupportingFace = !*MutablePrefs.EditorVisualizeGetSupportingFace;
                    return *MutablePrefs.EditorVisualizeGetSupportingFace;
                }),
                CreateDropDownCheckmark("Shape", *GetSingleton<JUserPreferences>().EditorVisualizeShape, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeShape = !*MutablePrefs.EditorVisualizeShape;
                    return *MutablePrefs.EditorVisualizeShape;
                }),
                CreateDropDownCheckmark("ShapeWireframe", *GetSingleton<JUserPreferences>().EditorVisualizeShapeWireframe, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeShapeWireframe = !*MutablePrefs.EditorVisualizeShapeWireframe;
                    return *MutablePrefs.EditorVisualizeShapeWireframe;
                }),
                // CreateDropDownCheckmark("ShapeColor", *GetSingleton<JUserPreferences>().EditorVisualizeShapeColor, []
                // {
                //     auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                //     MutablePrefs.EditorVisualizeShapeColor = !*MutablePrefs.EditorVisualizeShapeColor;
                //     return *MutablePrefs.EditorVisualizeShapeColor;
                // }),
                CreateDropDownCheckmark("BoundingBox", *GetSingleton<JUserPreferences>().EditorVisualizeBoundingBox, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeBoundingBox = !*MutablePrefs.EditorVisualizeBoundingBox;
                    return *MutablePrefs.EditorVisualizeBoundingBox;
                }),
                CreateDropDownCheckmark("CenterOfMassTransform", *GetSingleton<JUserPreferences>().EditorVisualizeCenterOfMassTransform, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeCenterOfMassTransform = !*MutablePrefs.EditorVisualizeCenterOfMassTransform;
                    return *MutablePrefs.EditorVisualizeCenterOfMassTransform;
                }),
                CreateDropDownCheckmark("WorldTransform", *GetSingleton<JUserPreferences>().EditorVisualizeWorldTransform, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeWorldTransform = !*MutablePrefs.EditorVisualizeWorldTransform;
                    return *MutablePrefs.EditorVisualizeWorldTransform;
                }),
                CreateDropDownCheckmark("Velocity", *GetSingleton<JUserPreferences>().EditorVisualizeVelocity, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeVelocity = !*MutablePrefs.EditorVisualizeVelocity;
                    return *MutablePrefs.EditorVisualizeVelocity;
                }),
                CreateDropDownCheckmark("MassAndInertia", *GetSingleton<JUserPreferences>().EditorVisualizeMassAndInertia, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeMassAndInertia = !*MutablePrefs.EditorVisualizeMassAndInertia;
                    return *MutablePrefs.EditorVisualizeMassAndInertia;
                }),
                CreateDropDownCheckmark("SleepStats", *GetSingleton<JUserPreferences>().EditorVisualizeSleepStats, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSleepStats = !*MutablePrefs.EditorVisualizeSleepStats;
                    return *MutablePrefs.EditorVisualizeSleepStats;
                }),
                CreateDropDownCheckmark("SoftBodyVertices", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyVertices, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyVertices = !*MutablePrefs.EditorVisualizeSoftBodyVertices;
                    return *MutablePrefs.EditorVisualizeSoftBodyVertices;
                }),
                CreateDropDownCheckmark("SoftBodyVertexVelocities", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyVertexVelocities, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyVertexVelocities = !*MutablePrefs.EditorVisualizeSoftBodyVertexVelocities;
                    return *MutablePrefs.EditorVisualizeSoftBodyVertexVelocities;
                }),
                CreateDropDownCheckmark("SoftBodyEdgeConstraints", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyEdgeConstraints, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyEdgeConstraints = !*MutablePrefs.EditorVisualizeSoftBodyEdgeConstraints;
                    return *MutablePrefs.EditorVisualizeSoftBodyEdgeConstraints;
                }),
                CreateDropDownCheckmark("SoftBodyBendConstraints", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyBendConstraints, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyBendConstraints = !*MutablePrefs.EditorVisualizeSoftBodyBendConstraints;
                    return *MutablePrefs.EditorVisualizeSoftBodyBendConstraints;
                }),
                CreateDropDownCheckmark("SoftBodyVolumeConstraints", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyVolumeConstraints, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyVolumeConstraints = !*MutablePrefs.EditorVisualizeSoftBodyVolumeConstraints;
                    return *MutablePrefs.EditorVisualizeSoftBodyVolumeConstraints;
                }),
                CreateDropDownCheckmark("SoftBodySkinConstraints", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodySkinConstraints, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodySkinConstraints = !*MutablePrefs.EditorVisualizeSoftBodySkinConstraints;
                    return *MutablePrefs.EditorVisualizeSoftBodySkinConstraints;
                }),
                CreateDropDownCheckmark("SoftBodyLRAConstraints", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyLRAConstraints, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyLRAConstraints = !*MutablePrefs.EditorVisualizeSoftBodyLRAConstraints;
                    return *MutablePrefs.EditorVisualizeSoftBodyLRAConstraints;
                }),
                CreateDropDownCheckmark("SoftBodyRods", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyRods, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyRods = !*MutablePrefs.EditorVisualizeSoftBodyRods;
                    return *MutablePrefs.EditorVisualizeSoftBodyRods;
                }),
                CreateDropDownCheckmark("SoftBodyRodStates", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyRodStates, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyRodStates = !*MutablePrefs.EditorVisualizeSoftBodyRodStates;
                    return *MutablePrefs.EditorVisualizeSoftBodyRodStates;
                }),
                CreateDropDownCheckmark("SoftBodyRodBendTwistConstraints", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyRodBendTwistConstraints, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyRodBendTwistConstraints = !*MutablePrefs.EditorVisualizeSoftBodyRodBendTwistConstraints;
                    return *MutablePrefs.EditorVisualizeSoftBodyRodBendTwistConstraints;
                }),
                CreateDropDownCheckmark("SoftBodyPredictedBounds", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyPredictedBounds, []
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorVisualizeSoftBodyPredictedBounds = !*MutablePrefs.EditorVisualizeSoftBodyPredictedBounds;
                    return *MutablePrefs.EditorVisualizeSoftBodyPredictedBounds;
                }),
                // CreateDropDownCheckmark("SoftBodyConstraintColor", *GetSingleton<JUserPreferences>().EditorVisualizeSoftBodyConstraintColor, []
                // {
                //     auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                //     MutablePrefs.EditorVisualizeSoftBodyConstraintColor = !*MutablePrefs.EditorVisualizeSoftBodyConstraintColor;
                //     return *MutablePrefs.EditorVisualizeSoftBodyConstraintColor;
                // }),
                };},
            },
        });
}

void Jafg::WEditorWorldViewer::CreateStartDropDown(LVec2F Where)
{
    CreateDropDownMenu(this->GetViewport(), Where, {}, {
        LDropDownNodeSeparator{.DisplayName="LAUNCH PREFERENCES"},
        LDropDownNodeOption{
            .Selector = {
                .DisplayName = "Current Viewer",
                .Icon = "Icons/Jafg.Start",
                },
            .OnAction = [this](auto&&...)
            {
                this->StartType = EStartType::Pie;
                this->OnLaunchAll();
                return algo::reply::unhandled();
            }},
        LDropDownNodeOption{
            .Selector = {
                .DisplayName = "Detached Viewer",
                .Icon = "Icons/Jafg.StartDetached",
                },
            .OnAction = [this](auto&&...)
            {
                this->StartType = EStartType::NewPie;
                this->OnLaunchAll();
                return algo::reply::unhandled();
            }},
        LDropDownNodeOption{
            .Selector = {
                .DisplayName = "Spectate (e.g. for Physics)",
                .Icon = "Icons/Jafg.Spectate",
                },
            .OnAction = [this](auto&&...)
            {
                this->StartType = EStartType::Spectate;
                this->OnLaunchAll();
                return algo::reply::unhandled();
            }},
        LDropDownNodeOption{
            .Selector = {
                .DisplayName = "Standalone",
                .Icon = "Icons/Jafg.Open",
                },
            .OnAction = [this](auto&&...)
            {
                this->StartType = EStartType::Standalone;
                this->OnLaunchAll();
                return algo::reply::unhandled();
            }},
        LDropDownNodeSeparator{.DisplayName="SPAWN PREFERENCES"},
        CreateDropDownCheckmark("Spawn at current eye", this->PawnStart==EPawnSart::EditorEye, [this]
        {
            if (this->PawnStart == EPawnSart::EditorEye)
            {
                this->PawnStart = EPawnSart::Default;
            }
            else
            {
                this->PawnStart = EPawnSart::EditorEye;
            }
            return this->PawnStart == EPawnSart::EditorEye;
        }),
        LDropDownNodeSeparator{.DisplayName="NET PREFERENCES"},
        LDropDownNodeInformation{.What="Clients 1"},
        LDropDownNodeInformation{.What="Net Mode Standalone"},
        LDropDownNodeInformation{.What="Jitter"},
        LDropDownNodeInformation{.What="Latency"},
        });
}

namespace
{

void LoadFirstCollection(Jafg::LWorld& World)
{
    auto MaterialInstance{World.GetMutableFrontend().GetSubsystemChecked<Jafg::JMaterialSubsystem>()
        ->GetInstanceFromMaterialName("Jafg.MetallicGold")};

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})
    ->EmplaceRootComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/XYZ.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetLocalTranslationByTeleport(LWorldVec3{0,0,10});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})
    ->EmplaceRootComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Plane_2M.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetLocalTranslationByTeleport(LWorldVec3{0,0,4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})
    ->EmplaceRootComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cube.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetLocalTranslationByTeleport(LWorldVec3{2,0,4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})
    ->EmplaceRootComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Sphere.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetLocalTranslationByTeleport(LWorldVec3{4,0,4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})
    ->EmplaceRootComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Icosphere.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetLocalTranslationByTeleport(LWorldVec3{6,0,4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})
    ->EmplaceRootComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cylinder.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetLocalTranslationByTeleport(LWorldVec3{8,0,4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})
    ->EmplaceRootComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cone.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetLocalTranslationByTeleport(LWorldVec3{10,0,4});
    });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})
    ->EmplaceRootComponent<Jafg::AStaticMeshComponent>([MaterialInstance](Jafg::AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Torus.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetLocalTranslationByTeleport(LWorldVec3{12,0,4});
    });
}

void LoadGymCollection(Jafg::LWorld& World)
{
    auto& Frontend{World.GetMutableFrontend()};

    // auto GroundMaterialInstance{Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Triplanar")};
    // GroundMaterialInstance->Vk_SetField(Frontend, GroundMaterialInstance->GetBinding("base_color_map"), "Textures/Jafg/Editor/GroundGrid_8M");
    // auto* Actor{Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})};
    // Actor->SetEditorHitTestable(false);
    // Actor->EditorName = "Ground";
    // Actor->EmplaceRootComponent<Jafg::AStaticMeshComponent>([GroundMaterialInstance](Jafg::AStaticMeshComponent& Comp)
    // {
    //     Comp.SetMesh(LITERAL_TEXT("Content/Models/Plane_8M.glb"));
    //     Comp.SetMaterialInstance(std::move(GroundMaterialInstance));
    //     Comp.SetLocalScaleByTeleport(maths::up_vector<LWorldVec3> + (maths::right_vector<LWorldVec3> + maths::forward_vector<LWorldVec3>) * static_cast<LWorldReal>(100.0));
    // });

    Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::APawnStart>{World})->GetRootComponent().SetLocalTranslationByTeleport(maths::up_vector<LWorldVec3> * static_cast<LWorldReal>(2.0));

    auto& Cube{*Jafg::SpawnObject(Jafg::TWorldStaticInit<Jafg::AActor>{World})};
    auto& Rigid{Cube.EmplaceRootComponent<Jafg::ARigidComponent>()};
    Rigid.SetShouldRender(true);
    Rigid.SetLocalTranslationByTeleport({0,0.5,8});
    // Rigid.MakeSphere(0.5f);
    Rigid.MakeBox(LWorldVec3{0.5});
    Rigid.AddToSimulation(Jafg::ESimulationAddingBehavior::Activate);
    Rigid.SetLinearVelocity({0,20,5});
    Cube.EmplaceSceneComponentTo<Jafg::AStaticMeshComponent>(Rigid, [World=&World](Jafg::AStaticMeshComponent& Comp)
    {
        auto& Frontend{World->GetMutableFrontend()};
        auto MaterialInstance{Frontend.GetSubsystemChecked<Jafg::JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.UniformTriplanar")};
        MaterialInstance->Vk_SetField(Frontend, MaterialInstance->GetBinding("base_color_map"), "Textures/Jafg/Editor/ObjectGrid_1M");
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cube_1M.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
    });
}

} /* ~Namespace <Anonymous> */

void Jafg::WEditorWorldViewer::InitializeJustSummoned(LWorld& World)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    if (*Prefs.EditorLastObjectCollection == "FirstCollection")
    {
        ::LoadFirstCollection(World);
    }
    else if (*Prefs.EditorLastObjectCollection == "Gym")
    {
        ::LoadGymCollection(World);
    }
}

void Jafg::WEditorWorldViewer::TravelToJustSummoned(LWorld& World)
{
    this->InitializeJustSummoned(World);
    this->TravelTo(World);
}

Jafg::LTexture2Ref Jafg::WEditorWorldViewer::GetStartTexture() const
{
    if (this->IsRunning())
    {
        auto& World{this->RunningInstance->GetOwnedPersonaControllerChecked()->GetWorld()};
        if (World.IsTimeLinear() && World.IsLinearWorldDormant())
        {
            return LTexture2::FromAsset("Icons/Jafg.Start");
        }
        return LTexture2::FromAsset("Icons/Jafg.Pause");
    }

    switch (this->StartType)
    {
    case EStartType::Pie: return LTexture2::FromAsset("Icons/Jafg.Start");
    case EStartType::NewPie: return LTexture2::FromAsset("Icons/Jafg.StartDetached");
    case EStartType::FollowPie: return LTexture2::FromAsset("Icons/Jafg.StartFollow");
    case EStartType::Spectate: return LTexture2::FromAsset("Icons/Jafg.Spectate");
    case EStartType::Standalone: return LTexture2::FromAsset("Icons/Jafg.Open");
    }
    std::unreachable();
}

void Jafg::WEditorWorldViewer::UpdateStartStopButtons()
{
    check(this->StartButton && this->StepButton && this->StopButton && this->DetachButton && this->MoreButton)

    auto StartIcon{LRegionBrush::Icon(this->GetStartTexture())};
    this->StartButton->Brush.Background = StartIcon;
    this->StartButton->Style.SetEverywhere<&LRegionBrush::Background>(StartIcon);

    LWorld* World{};
    if (this->IsRunning())
    {
        World = &this->RunningInstance->GetOwnedPersonaControllerChecked()->GetWorld();
    }
    if (World && World->IsTimeLinear() && World->IsLinearWorldDormant())
    {
        this->StartButton->Style.SetEverywhere<&LRegionBrush::Tint>(Colors::Green);
        this->StartButton->Brush.Tint = Colors::Green;
    }
    else
    {
        this->StartButton->Style.SetEverywhere<&LRegionBrush::Tint>(Colors::White);
        this->StartButton->Brush.Tint = Colors::White;
    }

    if (this->IsRunning())
    {

        this->StepButton->SetEnabled(true);
        this->StopButton->SetEnabled(true);
        this->DetachButton->SetEnabled(true);
        this->MoreButton->SetEnabled(false);
    }
    else
    {
        this->StepButton->SetEnabled(false);
        this->StopButton->SetEnabled(false);
        this->DetachButton->SetEnabled(false);
        this->MoreButton->SetEnabled(true);
    }
}

void Jafg::WEditorWorldViewer::OnLaunchAll()
{
    check(!this->IsRunning())

    auto& Prefs{GetMutableSingleton<JUserPreferences>()};
    Prefs.EditorStartType = std::byte{std::to_underlying(this->StartType)};
    Prefs.EditorPawnStart = std::byte{std::to_underlying(this->PawnStart)};

    switch (this->StartType)
    {
    case EStartType::Pie:
    {
        this->RunningInstance = this;
        auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
        auto& Pawn{*Ctrl.GetOwnedPawnChecked()};
        auto& Engine{Ctrl.GetMutableEngine()};
        check(!this->EditorReconnectionData)
        this->EditorReconnectionData = LEditorReconnectionData{
            .World = &Ctrl.GetWorld(),
            .EyeTrans = Pawn.GetRootComponent().GetLocalTransform(),
            };
        this->UserInput.DeactivateAllContexts();
        this->UserInput.SetConsumeMouse(false);

        Ctrl.GetWorld().Pause();

        LWorldCreateInfo CreateInfo{Ctrl.GetWorld().GetCreateInfo()};
        CreateInfo.HumanReadableName = algo::sprintf("{}-pie", Ctrl.GetWorld().GetHumanReadableName());
        CreateInfo.TimeBehavior = EWorldTimeBehavior::Linear;
        Engine.SummonWorld(std::move(CreateInfo), std::bind(&WEditorWorldViewer::TravelToJustSummoned, this, std::placeholders::_1));

        break;
    }
    case EStartType::NewPie:
    {
        auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
        LWorldCreateInfo CreateInfo{Ctrl.GetWorld().GetCreateInfo()};
        CreateInfo.HumanReadableName = algo::sprintf("{}-pie", Ctrl.GetWorld().GetHumanReadableName());
        CreateInfo.TimeBehavior = EWorldTimeBehavior::Linear;
        Tasks::Make(ENamedThreads::Master, ETaskTime::Late, [CreateInfo=std::move(CreateInfo),this]
        {
            check(Detail::GMutableEngine)

            auto& Prefs{GetSingleton<JUserPreferences>()};
            auto& Frontend{Detail::GMutableEngine->GetLocalEgo().GetFrontend()};

            auto& Surface{Frontend.AddSurface(std::make_unique<LSurface>(LSurfaceCreateInfo{
                .DesiredDimensionsPx = rhi::extent2::from_vec(*Prefs.EditorPieDimensions),
                .HumanReadableName = algo::sprintf("Jafg - @mzoesch ()", CreateInfo.HumanReadableName),
                }))};

            WWorldViewer& Viewer{*ConstructWidget(TNodeStaticInit<WWorldViewer>{Surface.GetViewport()})};
            Detail::GMutableEngine->SummonWorld(std::move(CreateInfo), [Viewer=&Viewer,this](LWorld& World)
            {
                this->InitializeJustSummoned(World);
                Viewer->TravelTo(World);
            });
        });

        break;
    }
    case EStartType::Spectate:
    {
        this->RunningInstance = this;
        check(!this->bIsNextPersonaEditorControlled)
        this->bIsNextPersonaEditorControlled = true;
        auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
        auto& Pawn{*Ctrl.GetOwnedPawnChecked()};
        auto& Engine{Ctrl.GetMutableEngine()};
        check(!this->EditorReconnectionData)
        this->EditorReconnectionData = LEditorReconnectionData{
            .World = &Ctrl.GetWorld(),
            .EyeTrans = Pawn.GetRootComponent().GetLocalTransform(),
            };
        this->UserInput.DeactivateAllContexts();
        this->UserInput.SetConsumeMouse(false);

        Ctrl.GetWorld().Pause();

        LWorldCreateInfo CreateInfo{Ctrl.GetWorld().GetCreateInfo()};
        CreateInfo.HumanReadableName = algo::sprintf("{}-simulate", Ctrl.GetWorld().GetHumanReadableName());
        CreateInfo.TimeBehavior = EWorldTimeBehavior::Linear;
        Engine.SummonWorld(std::move(CreateInfo), std::bind(&WEditorWorldViewer::TravelToJustSummoned, this, std::placeholders::_1));

        break;
    }
    default: checkNoEntry()
    }

    this->UpdateStartStopButtons();
}

void Jafg::WEditorWorldViewerHierarchy::Construct()
{
    Super::Construct();

    check(!this->WorldViewer)
    this->WorldViewer = this->FindSmart();
    check(!this->WorldViewer->HasHierarchy())
    this->WorldViewer->Hierarchy = this;
    check(!this->OnActorsSelectedHandle.IsValid())
    this->OnActorsSelectedHandle = this->WorldViewer->OnActorsSelected.Emplace(this, &WEditorWorldViewerHierarchy::OnActorsSelected);

    auto& Prefs{GetSingleton<JUserPreferences>()};

    BeginStyling(*this).StaticRoot<WVRegion>()
        .Anchor(EAnchor::Fill)
        .Tint(*Prefs.ForegroundColor)
    [
        NewStaticNode(WHParent)
            .Anchor(EAnchor::HFill)
            .Padding({5_spt})
            .Space(5_spt)
        [
            NewStaticNode(WTextButtonIconizedDouble)
                .InAllLeftIconBrushesChained<&LIconBrush::InwardsPadding, &LIconBrush::MinIconSize, &LIconBrush::Alignment>
                    (0_spt, 14_spt, LIconBrush::Align::Center)
                .InAllRightIconBrushesChained<&LIconBrush::InwardsPadding, &LIconBrush::MinIconSize, &LIconBrush::Alignment>
                    (0_spt, 10_spt, LIconBrush::Align::Center)
                .LeftIcon("Icons/Jafg.Filter")
                .RightIcon("Icons/Jafg.ExtendDown")
                .InBrush<EStyleBits::Normal, &LBoxBrush::bSkipBrushDraw>(true)
                .InAllBrushesChained<&LBoxBrush::Radii, &LBoxBrush::Padding>(LVec4F{5.0f}, {5_spt, 0.0f})
            +
            NewStaticNode(WEditableTextButtonIconizedLeft)
                .Anchor(EAnchor::Fill)
                .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
                .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
                .Icon("Icons/Jafg.Search")
                .PlaceholderContent("Search...")
            +
            NewStaticNode(WText).SaveTo(&this->ConnectedText)
                .MinDesiredSize({80_spt, 0})
                .MaxDesiredSize({80_spt, 0})
                .TextAlign(ETextHAlign::Center)
                .TextAlign(ETextVAlign::Center)
                .TextTint(Colors::Gray)
        ]
        +
        NewStaticNode(WHRegion).Anchor(EAnchor::HFill)
            .Tint(*Prefs.ForegroundColorVariant)
            .Padding(ListPadding)
        [
            NewStaticNode(WText)
                .Anchor(EAnchor::HFill)
                .Content("Object Label")
            +
            NewStaticNode(WText)
                .Content("C++ Class")
                .MinDesiredSize(WEditorWorldViewerHierarchy::TypeSize)
                .MaxDesiredSize(WEditorWorldViewerHierarchy::TypeSize)
        ]
        +
        NewStaticNode(WScrollRegion)
            .Anchor(EAnchor::Fill)
            .SkipBrushDraw(true)
        [
            NewStaticNode(WVParent).SaveTo(&this->Container)
                .Anchor(EAnchor::Fill)
                .Visibility(ENodeVisibility::Visible)
                .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    if (Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
                    {
                        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                        {
                            if (this->WorldViewer)
                            {
                                this->WorldViewer->SelectActors({});
                            }
                            return LNodeReply::Handled();
                        }
                    }

                    return LNodeReply::Unhandled();
                })
        ]
        +
        NewStaticNode(WTextBox).SaveTo(&this->SelectedWorldObjectText)
            .Anchor(EAnchor::HFill)
            .Tint(*Prefs.ForegroundColorVariant)
    ];

    this->OnWorldViewerUpdate();

    return;
}

void Jafg::WEditorWorldViewerHierarchy::Destruct()
{
    Super::Destruct();

    if (this->WorldViewer)
    {
        this->DisconnectFromViewer();
    }

    return;
}

void Jafg::WEditorWorldViewerHierarchy::_OnWorldViewerDestruct()
{
    check(this->WorldViewer)
    check(this->WorldViewer->_IsGarbage())
    this->DisconnectFromViewer();
    this->OnWorldViewerUpdate();

    return;
}

void Jafg::WEditorWorldViewerHierarchy::OnWorldViewerUpdate()
{
    this->UpdateConnectedArea();
    this->UpdateWorldObjectList({}, this->WorldViewer ? this->WorldViewer->GetSelectedActors() : TArray<std::pair<AActor*,AActorComponent*>>{});
    if (auto* World{this->GetWorld()})
    {
        check(this->WorldViewer)
        this->UpdateSelectedWorldObjectText(
            World->GetEmployees().size(),
            this->Container->GetChildren().size(),
            this->WorldViewer->GetSelectedActors().size()
            );
    }
    else
    {
        this->UpdateSelectedWorldObjectText(0, 0, 0);
    }

    return;
}

Jafg::WEditorWorldViewer* Jafg::WEditorWorldViewerHierarchy::FindSmart() const noexcept
{
    auto* Result{this->FindInViewport(this->GetViewport())};
    if (!Result && &this->GetViewport() != &this->GetViewport().GetSurface().GetViewport())
    {
        Result = this->FindInViewport(this->GetViewport().GetSurface().GetViewport());
    }
    if (!Result)
    {
        for (auto& Surface: this->GetViewport().GetSurface().GetFrontend().GetSurfaces())
        {
            if (&*Surface == &this->GetViewport().GetSurface())
            {
                continue;
            }
            if (auto* Found{this->FindInViewport(Surface->GetViewport())})
            {
                Result = Found;
                break;
            }
        }
    }

    return Result;
}

Jafg::WEditorWorldViewer* Jafg::WEditorWorldViewerHierarchy::FindInViewport(LViewport const& Viewport) const noexcept
{
    for (auto& Node: Viewport.GetTopLevelWidgets())
    {
        if (auto* Result{this->FindInNode(*Node)})
        {
            return Result;
        }

        continue;
    }

    return nullptr;
}

Jafg::WEditorWorldViewer* Jafg::WEditorWorldViewerHierarchy::FindInNode(WNode& Node) const noexcept
{
    if (auto* Casted{Node.As<WEditorWorldViewer>()})
    {
        if (!Casted->HasHierarchy())
        {
            return Casted;
        }
    }

    if (auto* Parent{Node.As<WParent>()})
    {
        for (auto& Child: Parent->GetChildren())
        {
            if (auto* Result{this->FindInNode(*Child)})
            {
                return Result;
            }
        }
    }

    return nullptr;
}

void Jafg::WEditorWorldViewerHierarchy::DisconnectFromViewer()
{
    check(this->WorldViewer)
    check(this->WorldViewer->Hierarchy == this)
    this->WorldViewer->OnActorsSelected.Remove(&this->OnActorsSelectedHandle);
    this->WorldViewer->Hierarchy = nullptr;
    this->WorldViewer = nullptr;

    return;
}

bool Jafg::WEditorWorldViewerHierarchy::OnActorsSelected(TArray<std::pair<AActor*,AActorComponent*>> const& Old, TArray<std::pair<AActor*,AActorComponent*>> const& New)
{
    this->UpdateWorldObjectList(Old, New);

    if (auto* World{this->GetWorld()})
    {
        check(this->Container)
        this->UpdateSelectedWorldObjectText(
            World->GetEmployees().size(),
            this->Container->GetChildren().size(),
            New.size()
            );
    }
    else
    {
        this->UpdateSelectedWorldObjectText(0, 0, 0);
    }

    return {};
}

void Jafg::WEditorWorldViewerHierarchy::UpdateConnectedArea()
{
    check(this->ConnectedText)

    if (this->WorldViewer)
    {
        if (this->WorldViewer->IsOwnedPersonaControllerValid())
        {
            this->ConnectedText->SetContent("[connected]");
        }
        else
        {
            this->ConnectedText->SetContent("[waiting]");
        }
    }
    else
    {
        this->ConnectedText->SetContent("[disconnected]");
    }

    return;
}

void Jafg::WEditorWorldViewerHierarchy::UpdateWorldObjectList(TArray<std::pair<AActor*,AActorComponent*>> const& Old, TArray<std::pair<AActor*,AActorComponent*>> const& New)
{
    check(this->Container)
    this->Container->RemoveChildren();

    if (auto* World{this->GetWorld()})
    {
        auto& Prefs{GetSingleton<JUserPreferences>()};

        auto Counter{0uz};
        for (auto& E: World->GetEmployees())
        {
            auto* Actor{E->As<AActor>()};
            if (!Actor)
            {
                continue;
            }

            this->Container->AddChild(NewStaticNode(Detail::WEditorWorldViewerHierarchyObjectHButton).Actor(Actor)
                .Anchor(EAnchor::HFill)
                .Style(Prefs.EditorProximityBoxStyle2<LRegionBrush>(Counter++))
                .Padding(ListPadding)
                .Selectable(true)
                .Selected(algo::contains(New, Actor, algo::pair_first))
                .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    return this->OnWorldObjectListKeyEventFocus(Self.AsStatic<Detail::WEditorWorldViewerHierarchyObjectHButton>(), Info, Event);
                })
                .OnKeyEventUnfocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    return this->OnWorldObjectListKeyEventUnfocus(Self.AsStatic<Detail::WEditorWorldViewerHierarchyObjectHButton>(), Info, Event);
                })
                [
                    NewStaticNode(WText).Content(Actor->GetEditorNameOrDefault())
                        .Anchor(EAnchor::HFill)
                    +
                    NewStaticNode(WText).Content(Actor->GetNameAsString())
                        .MinDesiredSize(WEditorWorldViewerHierarchy::TypeSize)
                        .MaxDesiredSize(WEditorWorldViewerHierarchy::TypeSize)
                        .TextTint(Colors::Gray)
                ]
                .Unique());
            }
    }

    return;
}

void Jafg::WEditorWorldViewerHierarchy::UpdateSelectedWorldObjectText(std::size_t Count, std::size_t Shown, std::size_t Selected)
{
    check(this->SelectedWorldObjectText)
    this->SelectedWorldObjectText->SetContent(algo::sprintf("{}/{} objects ({} selected)", Shown, Count, Selected));
    return;
}

Jafg::LNodeReply Jafg::WEditorWorldViewerHierarchy::OnWorldObjectListKeyEventFocus(Detail::WEditorWorldViewerHierarchyObjectHButton& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    check(Self.Actor)

    if (Info.CursorLocation && this->AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
    {
        if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
        {
            check(this->WorldViewer)

            if (Event.Mods & EModBits::Shift)
            {
                TArray<AActor*> ToSelect{Self.Actor};
                auto& Parent{*Self.GetParentChecked()};
                auto Found{0uz};
                for (auto& Child: Parent.GetChildren())
                {
                    auto& CastedChild{Child->AsStatic<Detail::WEditorWorldViewerHierarchyObjectHButton>()};
                    if (Found > 0)
                    {
                        ToSelect.emplace_back(CastedChild.Actor);
                    }

                    if (&CastedChild == &Self)
                    {
                        ++Found;
                    }
                    if (CastedChild.Actor == this->LastContainerElemSelected)
                    {
                        ++Found;
                    }

                    if (Found == 2)
                    {
                        break;
                    }
                    continue;
                }

                if (Found == 2)
                {
                    auto Current{this->WorldViewer->GetSelectedActors()};
                    for (auto* Actor : ToSelect)
                    {
                        if (!algo::contains(Current, Actor, algo::pair_first))
                        {
                            Current.emplace_back(Actor, &Actor->GetRootComponent());
                        }
                    }
                    this->WorldViewer->SelectActors(std::move(Current));
                }
            }
            else if (Event.Mods & EModBits::Control)
            {
                if (algo::contains(this->WorldViewer->GetSelectedActors(), Self.Actor, algo::pair_first))
                {
                    this->WorldViewer->SelectActors(this->WorldViewer->GetSelectedActors()
                        | algo::views::filter([Actor = Self.Actor](auto& E){ return E.first != Actor; })
                        | algo::to_array_fn{this->WorldViewer->GetSelectedActors().size()}
                        );
                }
                else
                {
                    auto Current{this->WorldViewer->GetSelectedActors()};
                    Current.emplace_back(Self.Actor, &Self.Actor->GetRootComponent());
                    this->WorldViewer->SelectActors(std::move(Current));
                }
                this->LastContainerElemSelected = Self.Actor;
            }
            else
            {
                if (Self.Actor->HasRootComponent())
                {
                    this->WorldViewer->SelectActors({{Self.Actor, &Self.Actor->GetRootComponent()}});
                }
                else
                {
                    this->WorldViewer->SelectActors({{Self.Actor, nullptr}});
                }
                this->LastContainerElemSelected = Self.Actor;
            }

            return LNodeReply::Handled();
        }
    }

    return  {};
}

Jafg::LNodeReply Jafg::WEditorWorldViewerHierarchy::OnWorldObjectListKeyEventUnfocus(Detail::WEditorWorldViewerHierarchyObjectHButton& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    check(Self.Actor)

    if (Info.CursorLocation && this->AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
    {
        if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
        {
            check(this->WorldViewer)
            if (algo::contains(this->WorldViewer->GetSelectedActors(), Self.Actor, algo::pair_first))
            {
                this->WorldViewer->SelectActors(this->WorldViewer->GetSelectedActors()
                    | algo::views::filter([Actor = Self.Actor](auto& E){ return E.first != Actor; })
                    | algo::to_array_fn{this->WorldViewer->GetSelectedActors().size()}
                    );
            }
            else
            {
                auto Current{this->WorldViewer->GetSelectedActors()};
                Current.emplace_back(Self.Actor, &Self.Actor->GetRootComponent());
                this->WorldViewer->SelectActors(std::move(Current));
            }

            this->LastContainerElemSelected = Self.Actor;
            return LNodeReply::Handled();
        }
    }

    return {};
}

void Jafg::WEditorWorldViewerInspector::Construct()
{
    Super::Construct();

    check(!this->WorldViewer)
    this->WorldViewer = this->FindSmart();
    check(!this->WorldViewer->HasInspector())
    this->WorldViewer->Inspector = this;
    check(!this->OnActorsSelectedHandle.IsValid())
    this->OnActorsSelectedHandle = this->WorldViewer->OnActorsSelected.Emplace(this, &WEditorWorldViewerInspector::OnActorsSelected);

    auto& Prefs{GetSingleton<JUserPreferences>()};

    BeginStyling(*this).StaticRoot<WVRegion>()
        .Anchor(EAnchor::Fill)
        .Tint(*Prefs.ForegroundColor)
    [
        NewStaticNode(WHParent)
            .Anchor(EAnchor::HFill)
            .Padding({5_spt})
            .Space(5_spt)
        [
            NewStaticNode(WEditableTextButton).SaveTo(&this->EditableObjectDisplayName)
                .Anchor(EAnchor::Fill)
                .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
                .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
                .OnContentCommitted([this](WEditableTextButton& Self, LString const& Content, ETextCommit Commit)
                {
                    if (this->WorldViewer)
                    {
                        auto& Actors{this->WorldViewer->GetSelectedActors()};
                        if (Actors.size() == 1)
                        {
                            auto& Actor{*Actors.front().first};
                            if (Commit == ETextCommit::OnCleared)
                            {
                                Self.SetContent(Actor.GetEditorNameOrDefault());
                            }
                            else
                            {
                                Actor.EditorName = Content;
                                this->WorldViewer->SelectActors(Actors, true);
                            }
                        }
                    }
                })
            +
            NewStaticNode(WText).SaveTo(&this->ConnectedText)
                .MinDesiredSize({80_spt, 0})
                .MaxDesiredSize({80_spt, 0})
                .TextAlign(ETextHAlign::Center)
                .TextAlign(ETextVAlign::Center)
                .TextTint(Colors::Gray)
        ]
        +
        NewStaticNode(WHParent)
            .Anchor(EAnchor::HFill)
            .Padding({5_spt})
            .Space(5_spt)
        [
            NewStaticNode(WEditableTextButtonIconizedLeft).SaveTo(&this->ContainerSearch)
                .Anchor(EAnchor::Fill)
                .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
                .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
                .Icon("Icons/Jafg.Search")
                .PlaceholderContent("Search...")
        ]
        +
        NewStaticNode(WScrollRegion)
            .Anchor(EAnchor::Fill)
            .SkipBrushDraw(true)
        [
            NewStaticNode(WVParent).SaveTo(&this->Container).Anchor(EAnchor::Fill)
        ]
    ];

    this->OnWorldViewerUpdate();
}

void Jafg::WEditorWorldViewerInspector::Tick()
{
    for (auto& F: this->ComponentUpdateFunctions)
    {
        check(!!F)
        F();
    }

    Super::Tick();
}

void Jafg::WEditorWorldViewerInspector::Destruct()
{
    Super::Destruct();

    if (this->WorldViewer)
    {
        this->DisconnectFromViewer();
    }
}

void Jafg::WEditorWorldViewerInspector::_OnWorldViewerDestruct()
{
    check(this->WorldViewer)
    check(this->WorldViewer->_IsGarbage())
    this->DisconnectFromViewer();
    this->OnWorldViewerUpdate();
}

void Jafg::WEditorWorldViewerInspector::OnWorldViewerUpdate()
{
    this->UpdateConnectedArea();
    this->UpdateObjectDisplayName();
    this->UpdateObjectDetails();
}

Jafg::WEditorWorldViewer* Jafg::WEditorWorldViewerInspector::FindSmart() const noexcept
{
    auto* Result{this->FindInViewport(this->GetViewport())};
    if (!Result && &this->GetViewport() != &this->GetViewport().GetSurface().GetViewport())
    {
        Result = this->FindInViewport(this->GetViewport().GetSurface().GetViewport());
    }
    if (!Result)
    {
        for (auto& Surface: this->GetViewport().GetSurface().GetFrontend().GetSurfaces())
        {
            if (&*Surface == &this->GetViewport().GetSurface())
            {
                continue;
            }
            if (auto* Found{this->FindInViewport(Surface->GetViewport())})
            {
                Result = Found;
                break;
            }
        }
    }

    return Result;
}

Jafg::WEditorWorldViewer* Jafg::WEditorWorldViewerInspector::FindInViewport(LViewport const& Viewport) const noexcept
{
    for (auto& Node: Viewport.GetTopLevelWidgets())
    {
        if (auto* Result{this->FindInNode(*Node)})
        {
            return Result;
        }

        continue;
    }

    return nullptr;
}

Jafg::WEditorWorldViewer* Jafg::WEditorWorldViewerInspector::FindInNode(WNode& Node) const noexcept
{
    if (auto* Casted{Node.As<WEditorWorldViewer>()})
    {
        if (!Casted->HasInspector())
        {
            return Casted;
        }
    }

    if (auto* Parent{Node.As<WParent>()})
    {
        for (auto& Child: Parent->GetChildren())
        {
            if (auto* Result{this->FindInNode(*Child)})
            {
                return Result;
            }
        }
    }

    return nullptr;
}

void Jafg::WEditorWorldViewerInspector::DisconnectFromViewer()
{
    check(this->WorldViewer)

    check(this->WorldViewer->Inspector == this)
    this->WorldViewer->OnActorsSelected.Remove(&this->OnActorsSelectedHandle);
    this->WorldViewer->Inspector = nullptr;
    this->WorldViewer = nullptr;
}

bool Jafg::WEditorWorldViewerInspector::OnActorsSelected(TArray<std::pair<AActor*,AActorComponent*>> const& Old, TArray<std::pair<AActor*,AActorComponent*>> const& New)
{
    this->UpdateObjectDisplayName();
    this->UpdateObjectDetails();

    return {};
}

void Jafg::WEditorWorldViewerInspector::UpdateConnectedArea()
{
    check(this->ConnectedText)

    if (this->WorldViewer)
    {
        if (this->WorldViewer->IsOwnedPersonaControllerValid())
        {
            this->ConnectedText->SetContent("[connected]");
        }
        else
        {
            this->ConnectedText->SetContent("[waiting]");
        }
    }
    else
    {
        this->ConnectedText->SetContent("[disconnected]");
    }
}

void Jafg::WEditorWorldViewerInspector::UpdateObjectDisplayName()
{
    check(this->EditableObjectDisplayName)

    if (this->WorldViewer)
    {
        auto& Actors{this->WorldViewer->GetSelectedActors()};
        if (Actors.empty())
        {
            this->EditableObjectDisplayName->SetEnabled(false);
            this->EditableObjectDisplayName->EmptyContent();
            this->EditableObjectDisplayName->PlaceholderContent = "Object Display Name";
        }
        else if (Actors.size() == 1)
        {
            auto& Actor{*Actors.front().first};
            this->EditableObjectDisplayName->SetEnabled(true);
            this->EditableObjectDisplayName->SetContent(Actor.GetEditorNameOrDefault());
        }
        else
        {
            this->EditableObjectDisplayName->SetEnabled(false);
            this->EditableObjectDisplayName->EmptyContent();
            this->EditableObjectDisplayName->PlaceholderContent = "<Multiple Actors Selected>";
        }
    }
    else
    {
        this->EditableObjectDisplayName->SetEnabled(false);
        this->EditableObjectDisplayName->EmptyContent();
        this->EditableObjectDisplayName->PlaceholderContent = "Object Display Name";
    }
}

void Jafg::WEditorWorldViewerInspector::UpdateObjectDetails()
{
    check(this->Container && this->ContainerSearch)

    this->ComponentUpdateFunctions.clear();
    this->SelectedComponent = nullptr;
    this->Container->RemoveChildren();
    this->ContainerSearch->SetEnabled(false);

    if (App::IsTearingDown())
    {
        return;
    }
    if (!this->WorldViewer)
    {
        return;
    }

    if (this->WorldViewer->GetSelectedActors().empty())
    {
        this->Container->AddChild(NewStaticNode(WText)
            .Anchor(EAnchor::Fill)
            .Content("Select an object to see details.")
            .TextAlign(ETextHAlign::Center)
            .TextAlign(ETextVAlign::Center)
            .Unique()
            );
    }
    else if (this->WorldViewer->GetSelectedActors().size() == 1)
    {
        auto& Actor{this->WorldViewer->GetSelectedActors().front()};

        this->ContainerSearch->SetEnabled(true);
        this->Container->AddChild(NewStaticNode(WVRegion).SaveTo(&this->ComponentContainerWrapper)
            .Visibility(ENodeVisibility::Visible)
            .Anchor(EAnchor::HFill)
            .Padding({5_spt})
            .MinDesiredSize({0, 128_spt})
            .OutlineTint(Colors::Black)
            .OutlineThickness(1)
            .Tint(*GetSingleton<JUserPreferences>().AccentColor)
            .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
                {
                    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                    {
                        this->SelectComponent(nullptr);
                        return LNodeReply::Handled();
                    }
                }
                return LNodeReply::Unhandled();
            })
            .Unique()
            );
        this->Container->AddChild(NewStaticNode(WEditorBackground).SaveTo(&this->ComponentContainer).Unique());

        this->SelectComponent(Actor.second);
    }
    else
    {
        this->Container->AddChild(NewStaticNode(WText)
            .Anchor(EAnchor::Fill)
            .Content(algo::sprintf("Too many objects selected ({}).", this->WorldViewer->GetSelectedActors().size()))
            .TextAlign(ETextHAlign::Center)
            .TextAlign(ETextVAlign::Center)
            .Unique()
            );
    }
}

void Jafg::WEditorWorldViewerInspector::ReloadInnerComponents()
{
    check(IsValidFast(this->GetOuter(), this->ComponentContainerWrapper))
    check(this->WorldViewer)
    check(this->WorldViewer->GetSelectedActors().size() == 1)
    auto& Actor{*this->WorldViewer->GetSelectedActors().front().first};
    auto& Prefs{GetSingleton<JUserPreferences>()};

    this->ComponentContainerWrapper->RemoveChildren();

    auto Counter{0uz};

    auto AddComponent{[&](AActorComponent& Comp, std::size_t Indent)
    {
        this->ComponentContainerWrapper->AddChild(NewStaticNode(WTextButtonIconizedDouble)
            .Anchor(EAnchor::HFill)
            .LeftIcon("Icons/Jafg.Box")
            .Style(Prefs.EditorProximityBoxStyle2<LBoxBrush>(Counter++))
            .InAllBrushes<&LBoxBrush::Padding>({ENodeSize::StaticPoints, static_cast<f32>(Indent) * 20.0f, 0.0f, 0.0f, 0.0f})
            .Selectable(true)
            .Selected(&Comp == this->SelectedComponent)
            .Content(Comp.GetEditorNameOrDefault())
            .OnKeyEventFocused([this, Comp=&Comp](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
                {
                    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                    {
                        this->WorldViewer->SelectActors({{&Comp->GetOwningActor(),Comp}});
                        return LNodeReply::Handled();
                    }
                }
                return LNodeReply::Unhandled();
            })
            .Unique()
            );
    }};

    if (Actor.HasRootComponent())
    {
        auto AddSceneComponent{[&](this auto&& Self, ASceneComponent& SceneComp, std::size_t Indent) -> void
        {
            AddComponent(SceneComp, Indent);
            for (auto& Child: SceneComp.GetChildren())
            {
                Self(*Child, Indent + 1);
            }
        }};
        AddSceneComponent(Actor.GetRootComponent(), 0uz);

        if (Actor.GetComponents().size() > 1uz)
        {
            this->ComponentContainerWrapper->AddChild(NewStaticNode(WHSeparator).Unique());
        }
    }

    for (auto& Component: Actor.GetComponents())
    {
        if (!Actor.HasRootComponent() || &Actor.GetRootComponent() != Component.get())
        {
            AddComponent(*Component, 0uz);
        }
    }
}

void Jafg::WEditorWorldViewerInspector::SelectComponent(AActorComponent* Component)
{
    check(IsValidFast(this->GetOuter(), this->ComponentContainer))

    if (Component == this->SelectedComponent)
    {
        return;
    }

    this->ComponentUpdateFunctions.clear();
    this->ComponentContainer->RemoveChildren();
    this->SelectedComponent = Component;

    if (this->SelectedComponent)
    {
        auto& Prefs{GetSingleton<JUserPreferences>()};

        std::unordered_map<LString, TArray<TJxxUnique<WNode>>> Layout;
        auto Iterator{this->SelectedComponent->GetVirtualTable().FieldIter()};
        for (auto It{Iterator.begin()}; It != Iterator.end(); ++It)
        {
            if (JxxFieldBits::IsEditorVisible(It->Flags))
            {
                auto& Array{Layout[It.GetClass().GetFullyQualifiedName()]};
                TArray<TJxxUnique<WNode>> Xs;
                check(It->EditorFactory)
                TFunction2<void()> UpdateValue;
                Array.emplace_back(It->EditorFactory(this->GetViewport(), *this->SelectedComponent, UpdateValue).UniqueXs(&Xs));
                if (UpdateValue)
                {
                    this->ComponentUpdateFunctions.emplace_back(std::move(UpdateValue));
                }
                for (auto& X: Xs)
                {
                    Array.emplace_back(std::move(X));
                }
            }
        }

        TArray<LString> Categories; Categories.reserve(Layout.size());
        for (LString const& Category: Layout | algo::views::keys)
        {
            Categories.emplace_back(Category);
        }
        algo::sort(Categories, algo::lexicographical_string_compare);

        for (LString const& Category: Categories)
        {
            auto CategoryNodes{std::make_shared<TArray<WNode*>>()};
            this->ComponentContainer->AddChild(NewStaticNode(WTextButtonIconizedDouble)
                .Anchor(EAnchor::HFill)
                .InAllBrushes<&LBoxBrush::Tint>(*Prefs.ForegroundColorVariant)
                .LeftIcon("Icons/Jafg.ExtendDown")
                .Content(Category)
                .Selectable(true)
                .Selected(true)
                .OnKeyEventFocused([CategoryNodes](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    check(CategoryNodes.get())

                    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                    {
                        auto& Casted{Self.AsStatic<WTextButtonIconizedDouble>()};
                        Casted.SetSelected(!Casted.IsSelected());

                        if (Casted.IsSelected())
                        {
                            Casted.LeftIcon = LTexture2::FromAsset("Icons/Jafg.ExtendDown");
                        }
                        else
                        {
                            Casted.LeftIcon = LTexture2::FromAsset("Icons/Jafg.ExtendRight");
                        }

                        for (auto* Node: *CategoryNodes)
                        {
                            check(Node)
                            Node->SetVisibility(Casted.IsSelected() ? ENodeVisibility::IntransitiveHitTestInvisible : ENodeVisibility::Collapsed);
                        }
                        return LNodeReply::Handled();
                    }
                    return LNodeReply::Unhandled();
                })
                .Unique()
                );
            TArray<TJxxUnique<WNode>>& Nodes{Layout.at(Category)};
            for (auto& Node: Nodes)
            {
                CategoryNodes->emplace_back(Node.get());
                this->ComponentContainer->AddChild(std::move(Node));
            }
        }
    }

    this->ReloadInnerComponents();
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

    this->GizmoMeshes[TranslateX].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoTranslateX.glb"));
    this->GizmoMeshes[TranslateX].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[TranslateY].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoTranslateY.glb"));
    this->GizmoMeshes[TranslateY].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[TranslateZ].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoTranslateZ.glb"));
    this->GizmoMeshes[TranslateZ].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));

    this->GizmoMeshes[TranslateX_Var].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoTranslateX-var.glb"));
    this->GizmoMeshes[TranslateX_Var].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[TranslateY_Var].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoTranslateY-var.glb"));
    this->GizmoMeshes[TranslateY_Var].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[TranslateZ_Var].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoTranslateZ-var.glb"));
    this->GizmoMeshes[TranslateZ_Var].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));

    this->GizmoMeshes[RotateR].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoRotatorR.glb"));
    this->GizmoMeshes[RotateR].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[RotateY].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoRotatorY.glb"));
    this->GizmoMeshes[RotateY].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[RotateP].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoRotatorP.glb"));
    this->GizmoMeshes[RotateP].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));

    this->GizmoMeshes[ScaleX].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoScaleX.glb"));
    this->GizmoMeshes[ScaleX].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[ScaleY].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoScaleY.glb"));
    this->GizmoMeshes[ScaleY].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[ScaleZ].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoScaleZ.glb"));
    this->GizmoMeshes[ScaleZ].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));

    this->GizmoMeshes[ScaleX_Var].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoScaleX-var.glb"));
    this->GizmoMeshes[ScaleX_Var].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[ScaleY_Var].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoScaleY-var.glb"));
    this->GizmoMeshes[ScaleY_Var].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[ScaleZ_Var].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoScaleZ-var.glb"));
    this->GizmoMeshes[ScaleZ_Var].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));

    this->GizmoMeshes[PlaneXY].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoPlaneXY.glb"));
    this->GizmoMeshes[PlaneXY].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[PlaneYZ].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoPlaneYZ.glb"));
    this->GizmoMeshes[PlaneYZ].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));
    this->GizmoMeshes[PlaneXZ].SetMesh(LITERAL_TEXT("Content/Models/Editor/GizmoPlaneXZ.glb"));
    this->GizmoMeshes[PlaneXZ].SetMaterialInstance(Frontend.GetSubsystemChecked<JMaterialSubsystem>()->GetInstanceFromMaterialName("Jafg.Gizmo"));

    this->TextMaterialInstance = Frontend.GetSubsystemChecked<JMaterialSubsystem>()
        ->GetInstanceFromMaterialName("Jafg.VisualBatch"sv);
}

void Jafg::AEditorPersonaControllerComponent::ParentTick(f32 Dt)
{
    Super::ParentTick(Dt);

    auto Reduce{[Dt=static_cast<f32>(this->GetEngine().DeltaTime)](auto&& Range)
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

    Reduce(this->Lines);
    Reduce(this->Rays);
    Reduce(this->Aabbs);
    Reduce(this->Texts);

    this->Lines.append_range(this->NextLines);
    this->NextLines.clear();
    this->Rays.append_range(this->NextRays);
    this->NextRays.clear();
    this->Aabbs.append_range(this->NextAabbs);
    this->NextAabbs.clear();
    this->Texts.append_range(this->NextTexts);
    this->NextTexts.clear();

    if (this->IsSelectedActorValid())
    {
        if (auto* SceneComp{this->SelectedActor.Actor.second->As<ASceneComponent>()})
        {
            this->SetTranslationForGizmos(SceneComp->GetWorldTranslationSlow());
        }
    }
}

void Jafg::AEditorPersonaControllerComponent::Render(LActorRenderInfo const& Info) const
{
    Super::Render(Info);

    STAT_CYCLE_FUNCTION()

    this->RenderRays(Info);
    this->RenderGizmo(Info);
    this->RenderTexts(Info);
}

Jafg::WEditorWorldViewer::EGizmo Jafg::AEditorPersonaControllerComponent::SetSelectedGizmo(WEditorWorldViewer::EGizmo Gizmo) noexcept
{
    return std::exchange(this->Gizmo, Gizmo);
}

std::pair<Jafg::AActor*,Jafg::AActorComponent*> Jafg::AEditorPersonaControllerComponent::SetSelectedActor(WEditorWorldViewer* Origin, std::pair<AActor*,AActorComponent*> Actor) noexcept
{
    this->SelectedActor.Viewer = Origin;
    this->SelectedActor.Quaternion.reset();
    auto Result{std::exchange(this->SelectedActor.Actor, Actor)};

    if (this->IsSelectedActorValid())
    {
        if (auto* SceneComp{this->SelectedActor.Actor.second->As<ASceneComponent>()})
        {
            this->SetTranslationForGizmos(SceneComp->GetWorldTranslationSlow());
        }
    }

    return Result;
}

void Jafg::AEditorPersonaControllerComponent::SetHighlightedGizmoMesh(EGizmoMesh Mesh) noexcept
{
    check(GEngine)
    this->HighlightedGizmoMesh = {GEngine->FrameCount, Mesh};
}

Jafg::AEditorPersonaControllerComponent::EGizmoMesh Jafg::AEditorPersonaControllerComponent::GetHighlightedGizmoMesh() const noexcept
{
    check(GEngine)
    if (this->HighlightedGizmoMesh.Frame != GEngine->FrameCount)
    {
        this->HighlightedGizmoMesh.Mesh = GizmoCount;
    }
    return this->HighlightedGizmoMesh.Mesh;
}

Jafg::AEditorPersonaControllerComponent::EGizmoMesh Jafg::AEditorPersonaControllerComponent::TraceGizmo(LWorldMagRay3 const& Ray, rhi::extent2 Extent, LWorldEye const& Eye) noexcept
{
    if (this->Gizmo == WEditorWorldViewer::EGizmo::Select || !this->IsSelectedActorValid() || !this->SelectedActor.Actor.second->IsA<ASceneComponent>())
    {
        return GizmoCount;
    }

    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& Comp{*this->SelectedActor.Actor.second};
    this->SetTranslationForGizmos(Comp.AsStatic<ASceneComponent>().GetWorldTranslationSlow());

    auto HandleGizmoRange{[&]<std::size_t S>(std::array<EGizmoMesh, S> Meshes)
    {
        EGizmoMesh Result{GizmoCount};

        typedef maths::aabb_intersection_ray<LWorldReal,world_qual> Hit;
        std::vector<std::pair<EGizmoMesh,Hit>> Hits;

        for (EGizmoMesh Mesh: Meshes)
        {
            auto& Gizmo{this->GizmoMeshes[std::to_underlying(Mesh)]};
            LWorldAabb3 Aabb{Gizmo.GetAabb()};

            // float distance = maths::magnitude(Eye.translation - Gizmo.GetTranslation());

            glm::vec3 toGizmo = Gizmo.GetTranslation() - Eye.translation;
            float distance = glm::dot(toGizmo, Eye.front);

            float scale =
                20.f/*GIZMO_PIXEL_SIZE*/ *
                (2.0f * distance * std::tan(Eye.vert_fov * 0.5f)) /
                static_cast<f32>(Extent.height);


            Aabb.min *= scale;
            Aabb.max *= scale;

            Aabb = Aabb.apply(Gizmo.GetTransform());

            if (*Prefs.EditorVisualizeGizmoInteractions)
            {
                this->AddAabbNextTick({
                    .Tint = *Prefs.EditorGizmoVisualizationAabbTint,
                    .Value = Aabb,
                    });
            }

            if (auto r{maths::aabb_intersect_ray(Ray, Aabb)}; r.bHit)
            {
                Hits.emplace_back(Mesh, r);
            }
        }

        algo::sort(Hits, [](std::pair<EGizmoMesh,Hit> const& A, std::pair<EGizmoMesh,Hit> const& B)
        {
            return A.second.Enter < B.second.Enter;
        });
        if (!Hits.empty())
        {
            Result = Hits.front().first;
        }
        return Result;
    }};

    switch (this->Gizmo)
    {
    case WEditorWorldViewer::EGizmo::Translate: return HandleGizmoRange(std::array{TranslateX, TranslateY, TranslateZ, PlaneXY, PlaneXZ, PlaneYZ,});
    case WEditorWorldViewer::EGizmo::Rotate: return HandleGizmoRange(std::array{RotateR, RotateY, RotateP});
    case WEditorWorldViewer::EGizmo::Scale: return HandleGizmoRange(std::array{ScaleX, ScaleY, ScaleZ, PlaneXY, PlaneXZ, PlaneYZ,});
    case WEditorWorldViewer::EGizmo::Gizmo: return HandleGizmoRange(std::array{TranslateX_Var,TranslateY_Var,TranslateZ_Var,RotateR,RotateY,RotateP,ScaleX_Var,ScaleY_Var,ScaleZ_Var});
    default: LOG_FATAL(LogEditor, "Invalid gizmo type [{}].", std::to_underlying(this->Gizmo))
    }
}

void Jafg::AEditorPersonaControllerComponent::TraceForGizmo(EGizmoMesh Mesh, LWorldMagRay3 const& Ray, rhi::extent2 Extent, LEditorTraceOrigin const& Origin)
{
    check(!(this->Gizmo == WEditorWorldViewer::EGizmo::Select || !this->IsSelectedActorValid()))
    check(this->SelectedActor.Actor.second->IsA<ASceneComponent>())

    if (Origin.Event.Mods & EModBits::Alt)
    {
        /* TODO: Maybe allow to clone sub-components if seleted? */
        AActor& ClonedActor{this->SelectedActor.Actor.first->Clone()};
        Origin.Node.SelectActors({{&ClonedActor, &ClonedActor.GetRootComponent()}});
        check(this->SelectedActor.Actor.first == &ClonedActor && this->SelectedActor.Actor.second == &ClonedActor.GetRootComponent())
        check(this->IsSelectedActorValid())
    }

    auto& Comp{this->SelectedActor.Actor.second->AsStatic<ASceneComponent>()};
    this->SetTranslationForGizmos(Comp.GetWorldTranslationSlow());

    auto HandleGizmo{[&](EGizmoMesh Mesh, auto&& Handler)
    {
        this->UsedGizmoMesh = Mesh;
        this->SelectedActor.Quaternion = Comp.GetLocalRotator();

        Origin.Node.GetViewport().EmplaceUntil<ERawInputStateBits::Release>(Origin.Event.PhysicalKey,
        [this,Mesh,Extent=Extent,N=&Origin.Node,Handler,OriginTransform=this->SelectedActor.Actor.second->AsStatic<ASceneComponent>().GetLocalTransform()
            ,MDelta=std::optional<LVec2F>{},LDelta=std::optional<LWorldReal>{},Delta=std::optional<LWorldVec3>{}
            ,LastCursor=std::optional<LVec2F>{},DiscardedCursor=maths::zero_vector<LVec2F>](LRawInput const& Input) mutable
        {
            check(N)
            if (!N->IsPainted())
            {
                return true;
            }
            if (!N->GetViewport().GetSurface().HasMouseLocation())
            {
                return true;
            }
            if (!N->IsOwnedPersonaControllerValid())
            {
                return true;
            }
            auto& Ctrl{*N->GetOwnedPersonaControllerChecked()};
            if (!Ctrl.IsOwnedPawnValid())
            {
                return true;
            }

            auto& Prefs{GetSingleton<JUserPreferences>()};

            auto CursorLocation{N->GetViewport().GetSurface().GetMouseLocationValue()};
            if (!LastCursor)
            {
                LastCursor = CursorLocation;
            }
            LVec2F Location{CursorLocation - DiscardedCursor - N->GetAnchoredAndTranslatedTopLeftFromMostOuter(N->GetTranslationFromMostOuter())};

            if (algo::contains(std::array{RotateR,RotateY,RotateP}, Mesh))
            {
                if (!MDelta)
                {
                    MDelta = Location;
                    return false;
                }

                auto D{Location - *MDelta};
                Handler(Input, OriginTransform, Mesh, D.x - D.y, maths::zero_vector<LWorldVec3>);
            }
            else if (algo::contains(std::array{PlaneXY,PlaneXZ,PlaneYZ}, Mesh))
            {
                LWorldPlane3 GizmoPlane{[&]{ switch (Mesh)
                {
                    case PlaneYZ: return LWorldPlane3{.origin=this->GizmoMeshes[Mesh].GetTranslation(), .normal=maths::unit_vector_x<LWorldVec3>};
                    case PlaneXZ: return LWorldPlane3{.origin=this->GizmoMeshes[Mesh].GetTranslation(), .normal=maths::unit_vector_y<LWorldVec3>};
                    case PlaneXY: return LWorldPlane3{.origin=this->GizmoMeshes[Mesh].GetTranslation(), .normal=maths::unit_vector_z<LWorldVec3>};
                    default: LOG_FATAL(LogEditor, "Invalid gizmo index [{}].", std::to_underlying(Mesh))
                }}()};
                if (N->IsGridSpaceLocal())
                {
                    GizmoPlane.normal = OriginTransform.r * GizmoPlane.normal;
                }
                if (*Prefs.EditorVisualizeGizmoInteractions)
                {
                    this->AddRayNextTick({
                        .Tint = *Prefs.EditorGizmoVisualizationTint,
                        .Duration = OneTimeDraw,
                        .Value = LWorldMagRay3{GizmoPlane.origin, GizmoPlane.normal, *Prefs.EditorGizmoVisualizationLength},
                        });
                }

                auto Trace{LWorldMagRay3::from_ray(maths::screen_to_world_space(
                    {Extent.width,Extent.height}, Location, Ctrl.GetOwnedPawnChecked()->GetEye()), *Prefs.EditorTraceLength
                    )};
                LWorldReal LineDelta{maths::intersection_delta_on_plane(LWorldRay3{.origin=Trace.origin,.direction=Trace.direction}, GizmoPlane)};
                LWorldVec3 Point{Trace.origin + Trace.direction * LineDelta};
                if (*Prefs.EditorVisualizeGizmoInteractions)
                {
                    constexpr LWorldVec3 HitSize{0.2f};
                    this->AddAabbNextTick({
                        .Tint = *Prefs.EditorGizmoVisualizationHitTint,
                        .Duration = OneTimeDraw,
                        .Value = LWorldAabb3{Point - HitSize, Point + HitSize},
                        });
                }

                if (!LDelta)
                {
                    LDelta = LineDelta;
                }
                if (!Delta)
                {
                    Delta = Point - this->GizmoMeshes[Mesh].GetTranslation();
                    return false;
                }

                Point -= *Delta;
                if (Handler(Input, OriginTransform, Mesh, LineDelta - *LDelta, Point))
                {
                    DiscardedCursor += CursorLocation - *LastCursor;
                }
                else
                {
                    DiscardedCursor = maths::zero_vector<LVec2F>;
                }
            }
            else
            {
                LWorldVec3 GizmoRay{[&]{ switch (Mesh)
                {
                    case TranslateX: case TranslateX_Var: case ScaleX: case ScaleX_Var: return maths::right_vector<LWorldVec3>;
                    case TranslateY: case TranslateY_Var: case ScaleY: case ScaleY_Var: return maths::up_vector<LWorldVec3>;
                    case TranslateZ: case TranslateZ_Var: case ScaleZ: case ScaleZ_Var: return maths::forward_vector<LWorldVec3>;
                    default: LOG_FATAL(LogEditor, "Invalid gizmo index [{}].", std::to_underlying(Mesh))
                }}()};
                if (N->IsGridSpaceLocal())
                {
                    GizmoRay = OriginTransform.r * GizmoRay;
                }
                if (*Prefs.EditorVisualizeGizmoInteractions)
                {
                    this->AddRayNextTick({
                        .Tint = *Prefs.EditorGizmoVisualizationTint,
                        .Duration = OneTimeDraw,
                        .Value = LWorldMagRay3{this->GizmoMeshes[Mesh].GetTranslation(), GizmoRay, *Prefs.EditorGizmoVisualizationLength},
                        });
                }

                auto Trace{LWorldMagRay3::from_ray(maths::screen_to_world_space(
                    {Extent.width,Extent.height}, Location, Ctrl.GetOwnedPawnChecked()->GetEye()), *Prefs.EditorTraceLength
                    )};
                LWorldReal LineDelta{maths::closest_delta_on_line(
                    LWorldRay3{this->GizmoMeshes[Mesh].GetTranslation(), GizmoRay}, LWorldRay3{.origin=Trace.origin,.direction=Trace.direction}
                    )};
                LWorldVec3 Point{this->GizmoMeshes[Mesh].GetTranslation() + GizmoRay * LineDelta};
                if (*Prefs.EditorVisualizeGizmoInteractions)
                {
                    constexpr LWorldVec3 HitSize{0.2f};
                    this->AddAabbNextTick({
                        .Tint = *Prefs.EditorGizmoVisualizationHitTint,
                        .Duration = OneTimeDraw,
                        .Value = LWorldAabb3{Point - HitSize, Point + HitSize},
                        });
                }

                if (!LDelta)
                {
                    LDelta = LineDelta;
                }
                if (!Delta)
                {
                    Delta = Point - this->GizmoMeshes[Mesh].GetTranslation();
                    return false;
                }

                Point -= *Delta;
                if (Handler(Input, OriginTransform, Mesh, LineDelta - *LDelta, Point))
                {
                    DiscardedCursor += CursorLocation - *LastCursor;
                }
                else
                {
                    DiscardedCursor = maths::zero_vector<LVec2F>;
                }
            }

            LastCursor = CursorLocation;
            return false;
        }, [this,Viewer=&Origin.Node]
        {
            this->SelectedActor.Quaternion.reset();
            this->UsedGizmoMesh = GizmoCount;
            Viewer->GetViewport().GetSurface().SetInputMode(EInputModeBits::ShowMouseCursor);
        });
    }};

    auto HandleTranslation{[this,Viewer=&Origin.Node]
    (LRawInput const& Input, LWorldTrans const& T, EGizmoMesh Mesh, LWorldReal Delta, LWorldVec3 Point) mutable -> bool
    {
        bool Result{};

        if (!maths::eq_zero_e(Delta))
        {
            if (Viewer->IsTranslationGridSnapEnabled())
            {
                f32 Snap{Viewer->GetTranslationGridSnap()};
                check(Snap > 0.0f)
                auto Dir{Point - this->GizmoMeshes[Mesh].GetTranslation()};
                Dir = maths::normalize(Dir) * (maths::round(maths::magnitude(Dir) / Snap) * Snap);
                Point = this->GizmoMeshes[Mesh].GetTranslation() + Dir;
            }

            auto& Surface{Viewer->GetViewport().GetSurface()};
            if (Surface.HasPlatformKeyState(*Surface.GetFrontend().GetPhysicalKey(ELogicalKey::LeftShift), ERawInputStateBits::Hold))
            {
                Surface.SetInputMode(EInputModeBits::HideMouseCursor);
                Viewer->GetOwnedPersonaControllerChecked()->GetOwnedPawnChecked()->GetRootComponent().AddLocalTranslationByTeleport(Point - this->GizmoMeshes[Mesh].GetTranslation());
                Result = true;
            }
            else
            {
                Surface.SetInputMode(EInputModeBits::ShowMouseCursor);
            }

            if (auto* Parent{this->SelectedActor.Actor.second->AsStatic<ASceneComponent>().GetParent()})
            {
                auto ParentTranslation{Parent->GetWorldTranslationSlow()};
                auto RelativePoint{Point - ParentTranslation};
                this->SelectedActor.Actor.second->AsStatic<ASceneComponent>().SetLocalTranslationByTeleport(RelativePoint);
            }
            else
            {
                this->SelectedActor.Actor.second->AsStatic<ASceneComponent>().SetLocalTranslationByTeleport(Point);
            }
            this->SetTranslationForGizmos(this->SelectedActor.Actor.second->AsStatic<ASceneComponent>().GetWorldTranslationSlow());
        }

        return Result;
    }};

    auto HandleRotation{[this,Viewer=&Origin.Node]
    (LRawInput const& Input, LWorldTrans const& T, EGizmoMesh Mesh, LWorldReal Delta, LWorldVec3 const& Point) -> bool
    {
        Delta *= static_cast<LWorldReal>(0.5);
        if (!maths::eq_zero_e(Delta) && Viewer->IsRotationGridSnapEnabled())
        {
            f32 Snap{Viewer->GetRotationGridSnap()};
            check(Snap > 0.0f)
            Delta = maths::round(Delta / Snap) * Snap;
        }

        if (Mesh == RotateR)
        {
            if (Viewer->IsGridSpaceLocal())
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalRotatorByTeleport(T.r * maths::angle_axis(maths::radians(Delta), maths::unit_vector_x<LWorldVec3>));
            }
            else
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalRotatorByTeleport(maths::angle_axis(maths::radians(Delta), maths::unit_vector_x<LWorldVec3>) * T.r);
            }
        }
        else if (Mesh == RotateY)
        {
            if (Viewer->IsGridSpaceLocal())
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalRotatorByTeleport(T.r * maths::angle_axis(maths::radians(Delta), maths::unit_vector_y<LWorldVec3>));
            }
            else
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalRotatorByTeleport(maths::angle_axis(maths::radians(Delta), maths::unit_vector_y<LWorldVec3>) * T.r);
            }
        }
        else if (Mesh == RotateP)
        {
            if (Viewer->IsGridSpaceLocal())
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalRotatorByTeleport(T.r * maths::angle_axis(maths::radians(Delta), maths::unit_vector_z<LWorldVec3>));
            }
            else
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalRotatorByTeleport(maths::angle_axis(maths::radians(Delta), maths::unit_vector_z<LWorldVec3>) * T.r);
            }
        }

        return {};
    }};

    auto HandleScale{[this,Viewer=&Origin.Node]
    (LRawInput const& Input, LWorldTrans const& T, EGizmoMesh Mesh, LWorldReal Delta, LWorldVec3 const& Point) -> bool
    {
        if (!maths::eq_zero_e(Delta))
        {
            Delta *= static_cast<LWorldReal>(2.0);

            if (Viewer->IsScaleGridSnapEnabled())
            {
                f32 Snap{Viewer->GetScaleGridSnap()};
                check(Snap > 0.0f)

                Delta = maths::round(Delta / Snap) * Snap;
            }

            if (Mesh == ScaleX || Mesh == ScaleX_Var)
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalScaleByTeleport(T.s + LWorldVec3{Delta,0,0});
            }
            else if (Mesh == ScaleY || Mesh == ScaleY_Var)
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalScaleByTeleport(T.s + LWorldVec3{0,Delta,0});
            }
            else if (Mesh == ScaleZ || Mesh == ScaleZ_Var)
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalScaleByTeleport(T.s + LWorldVec3{0,0,Delta});
            }
            else if (Mesh == PlaneXY)
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalScaleByTeleport(T.s + LWorldVec3{Delta,Delta,0});
            }
            else if (Mesh == PlaneXZ)
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalScaleByTeleport(T.s + LWorldVec3{Delta,0,Delta});
            }
            else if (Mesh == PlaneYZ)
            {
                this->SelectedActor.Actor.first->GetRootComponent().SetLocalScaleByTeleport(T.s + LWorldVec3{0,Delta,Delta});
            }
            else
            {
                LOG_FATAL(LogEditor, "Invalid gizmo index [{}].", std::to_underlying(Mesh))
            }
        }

        return {};
    }};

    switch (this->Gizmo)
    {
    case WEditorWorldViewer::EGizmo::Translate:
    {
        check(algo::contains(std::array{TranslateX, TranslateY, TranslateZ, PlaneXY, PlaneXZ, PlaneYZ,}, Mesh))
        HandleGizmo(Mesh, HandleTranslation);
        break;
    }
    case WEditorWorldViewer::EGizmo::Rotate:
    {
        check(algo::contains(std::array{RotateR, RotateY, RotateP}, Mesh))
        HandleGizmo(Mesh, HandleRotation);
        break;
    }
    case WEditorWorldViewer::EGizmo::Scale:
    {
        check(algo::contains(std::array{ScaleX, ScaleY, ScaleZ, PlaneXY, PlaneXZ, PlaneYZ,}, Mesh))
        HandleGizmo(Mesh, HandleScale);
        break;
    }
    case WEditorWorldViewer::EGizmo::Gizmo:
    {
        check(algo::contains(std::array{TranslateX_Var, TranslateY_Var, TranslateZ_Var, RotateR, RotateY, RotateP, ScaleX_Var, ScaleY_Var, ScaleZ_Var,}, Mesh))
        return HandleGizmo(Mesh, [Mesh,HandleTranslation=std::move(HandleTranslation),HandleRotation,HandleScale]
            (LRawInput const& Input, LWorldTrans const& T, u32 Dir, LWorldReal Delta, LWorldVec3 const& Point) mutable
        {
            if (algo::contains(std::array{TranslateX_Var, TranslateY_Var, TranslateZ_Var}, Mesh))
            {
                return HandleTranslation(Input, T, Mesh, Delta, Point);
            }
            if (algo::contains(std::array{RotateR, RotateY, RotateP}, Mesh))
            {
                return HandleRotation(Input, T, Mesh, Delta, Point);
            }
            if (algo::contains(std::array{ScaleX_Var, ScaleY_Var, ScaleZ_Var}, Mesh))
            {
                return HandleScale(Input, T, Mesh, Delta, Point);
            }
            std::unreachable();
        });
    }
    default:
    {
        LOG_FATAL(LogEditor, "Invalid gizmo type [{}].", std::to_underlying(this->Gizmo))
    }
    }
}

void Jafg::AEditorPersonaControllerComponent::RenderRays(LActorRenderInfo const& Info) const
{
    STAT_CYCLE_FUNCTION()

    auto& Prefs{GetSingleton<JUserPreferences>()};
    check(this->RayInstance.get())

    if (*Prefs.EditorVisualizeAabbs)
    {
        for (auto& E: this->GetWorld().GetEmployees() | algo::views::filter([](auto& E){ return E->template IsA<AActor>(); }))
        {
            auto& A{*StaticCastChecked<AActor>(&*E)};
            LWorldAabb3 Aabb{A.GetTransformedActorAabb()};
            if (Aabb.empty())
            {
                continue;
            }
            this->Aabbs.emplace_back(*Prefs.EditorAabbVisualizationTint, OneTimeDraw, Aabb);

            if (*Prefs.EditorVisualizeTransitiveAabbs)
            {
                auto VisualizeComponentTransitive{[this,Prefs=&Prefs](this auto&& Self, ASceneComponent& Comp) -> void
                {
                    LWorldAabb3 Aabb{Comp.GetAabbForThisComponentOnly()};
                    if (!Aabb.empty())
                    {
                        this->Aabbs.emplace_back(*Prefs->EditorTransitiveAabbVisualizationTint, OneTimeDraw, Aabb.apply(Comp.GetWorldTransformSlow()));
                    }
                    for (auto& Child: Comp.GetChildren())
                    {
                        Self(*Child);
                    }
                }};
                VisualizeComponentTransitive(A.GetRootComponent());
            }
        }
    }

    if (this->Lines.empty() && this->Rays.empty() && this->Aabbs.empty())
    {
        return;
    }

    auto& Set{this->RayInstance->Vk_GetUniqueDescriptorSet(ShaderSpace, Info.Frame)};

    rhi::object_range<SSBO::Ray> DeviceRays{this->Lines.size() + this->Rays.size() + this->Aabbs.size() * 12, MaxLineCount};
    algo::for_each(this->Lines, [&DeviceRays](auto const& Line)
    {
        DeviceRays->emplace_back(Line->begin, Line->end, Line.Tint);
    });
    algo::for_each(this->Rays, [&DeviceRays](auto const& Ray)
    {
        DeviceRays->emplace_back(Ray->origin, Ray->direction * Ray->magnitude, Ray.Tint);
    });
    algo::for_each(this->Aabbs, [&DeviceRays](auto const& Aabb)
    {
        for (auto& edge: Aabb->edges())
        {
            DeviceRays->emplace_back(edge.origin, edge.direction, Aabb.Tint);
        }
    });

    DeviceRays.upload_and_update(Info.Frontend.Vk_GetDevice(), *Set, DebugLinesIndex, this->RayBuffers[Info.Frame]);
    UBO::ViewProj{Info.WorldData.proj * Info.WorldData.view}.upload_and_update(Info.Frontend.Vk_GetDevice(), *Set, ViewProjIndex, this->RayViewBuffers[Info.Frame]);

    Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *this->RayInstance->Material->Pipeline);
    Info.CommandBuffer.bindDescriptorSets2({
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .layout = *this->RayInstance->Material->Pipeline.pipeline_layout,
        .firstSet = 0,
        .descriptorSetCount = 1,
        .pDescriptorSets = &**Set,
        .dynamicOffsetCount = 0,
        .pDynamicOffsets = nullptr
        });
    Info.CommandBuffer.draw(2, static_cast<u32>(DeviceRays->size()), 0, 0);
}

namespace
{

template<Jafg::AEditorPersonaControllerComponent::EGizmoMesh g, typename T, maths::qualifier Q>
NODISCARD constexpr TQua<T,Q> GetGizmoRotation(maths::octant o) noexcept
{
    if constexpr (g == Jafg::AEditorPersonaControllerComponent::EGizmoMesh::RotateR)
    {
        switch(o)
        {
        case maths::octant::zero:  return TQua<T,Q>{TVec3<T,Q>{0, 0, 0}};
        case maths::octant::one:   return TQua<T,Q>{TVec3<T,Q>{0, 0, 0}};
        case maths::octant::two:   return TQua<T,Q>{TVec3<T,Q>{maths::half_pi_v<T>, 0, 0}};
        case maths::octant::three: return TQua<T,Q>{TVec3<T,Q>{maths::half_pi_v<T>, 0, 0}};
        case maths::octant::four:  return TQua<T,Q>{TVec3<T,Q>{0, maths::pi_v<T>, 0}};
        case maths::octant::five:  return TQua<T,Q>{TVec3<T,Q>{0, maths::pi_v<T>, 0}};
        case maths::octant::six:   return TQua<T,Q>{TVec3<T,Q>{maths::half_pi_v<T>, maths::pi_v<T>, 0}};
        case maths::octant::seven: return TQua<T,Q>{TVec3<T,Q>{maths::half_pi_v<T>, maths::pi_v<T>, 0}};
        }
    }
    else if constexpr (g == Jafg::AEditorPersonaControllerComponent::EGizmoMesh::RotateY)
    {
        switch(o)
        {
        case maths::octant::zero:  return TQua<T,Q>{TVec3<T,Q>{0, 0, 0}};
        case maths::octant::one:   return TQua<T,Q>{TVec3<T,Q>{0, -maths::half_pi_v<T>, 0}};
        case maths::octant::two:   return TQua<T,Q>{TVec3<T,Q>{0, 0, 0}};
        case maths::octant::three: return TQua<T,Q>{TVec3<T,Q>{0, -maths::half_pi_v<T>, 0}};
        case maths::octant::four:  return TQua<T,Q>{TVec3<T,Q>{0, maths::half_pi_v<T>, 0}};
        case maths::octant::five:  return TQua<T,Q>{TVec3<T,Q>{0, maths::pi_v<T>, 0}};
        case maths::octant::six:   return TQua<T,Q>{TVec3<T,Q>{0, maths::half_pi_v<T>, 0}};
        case maths::octant::seven: return TQua<T,Q>{TVec3<T,Q>{0, maths::pi_v<T>, 0}};
        }
    }
    else if constexpr (g == Jafg::AEditorPersonaControllerComponent::EGizmoMesh::RotateP)
    {
        switch(o)
        {
        case maths::octant::zero:  return TQua<T,Q>{TVec3<T,Q>{0, 0, 0}};
        case maths::octant::one:   return TQua<T,Q>{TVec3<T,Q>{0, maths::pi_v<T>, 0}};
        case maths::octant::two:   return TQua<T,Q>{TVec3<T,Q>{0, maths::pi_v<T>, maths::pi_v<T>}};
        case maths::octant::three: return TQua<T,Q>{TVec3<T,Q>{0, 0, maths::pi_v<T>}};
        case maths::octant::four:  return TQua<T,Q>{TVec3<T,Q>{0, 0, 0}};
        case maths::octant::five:  return TQua<T,Q>{TVec3<T,Q>{0, maths::pi_v<T>, 0}};
        case maths::octant::six:   return TQua<T,Q>{TVec3<T,Q>{0, maths::pi_v<T>, maths::pi_v<T>}};
        case maths::octant::seven: return TQua<T,Q>{TVec3<T,Q>{0, 0, maths::pi_v<T>}};
        }
    }
    else
    {
        static_assert(algo::always_false_v<T>);
    }
}

} /* ~Namespace <Anonymous> */

void Jafg::AEditorPersonaControllerComponent::RenderGizmo(LActorRenderInfo const& Info) const
{
    if (!this->IsSelectedActorValid() || !this->SelectedActor.Actor.second->IsA<ASceneComponent>())
    {
        return;
    }
    STAT_CYCLE_FUNCTION()

    auto& Prefs{GetSingleton<JUserPreferences>()};

    EGizmoMesh Highlight{this->GetHighlightedGizmoMesh()};
    auto UpdateTint{[&](EGizmoMesh Mesh, LColor Tint)
    {
        if (this->UsedGizmoMesh != GizmoCount)
        {
            if (this->UsedGizmoMesh == Mesh)
            {
                Tint = *Prefs.EditorAxisTintHighlighted;
            }
        }
        else if (Highlight == Mesh)
        {
            Tint = *Prefs.EditorAxisTintHighlighted;
        }
        auto& Instance{this->GizmoMeshes[Mesh].GetMaterialInstance()};
        auto& DescriptorSetInstance{Instance->Vk_GetUniqueDescriptorSet(GizmoSolidColorInputSpace, Info.Frame)};
        UBO::SolidColorInput{.Color=Tint}
            .upload_and_update(Info.Frontend.Vk_GetDevice()
                , *DescriptorSetInstance
                , GizmoSolidColorInputIndex
                , DescriptorSetInstance.Resources[GizmoSolidColorInputIndex].AsBuffer()
                );
    }};

    auto RenderTranslation{[&](EGizmoMesh X, EGizmoMesh Y, EGizmoMesh Z)
    {
        UpdateTint(X, *Prefs.EditorAxisTintX);
        UpdateTint(Y, *Prefs.EditorAxisTintY);
        UpdateTint(Z, *Prefs.EditorAxisTintZ);

        if (this->SelectedActor.Viewer->IsGridSpaceLocal())
        {
            for (auto& Mesh: std::array{X,Y,Z,})
            {
                this->GizmoMeshes[Mesh].SetRotator(this->SelectedActor.Actor.first->GetRootComponent().GetLocalRotator());
            }
        }
        else
        {
            for (auto& Mesh: std::array{X,Y,Z,})
            {
                this->GizmoMeshes[Mesh].SetRotator(maths::identity<LWorldQuat>);
            }
        }

        Info.PostRenderDelegates.emplace_back([this,X,Y,Z](LActorRenderInfo const& Info)
        {
            this->GizmoMeshes[X].ForceRenderWithOwnedMaterial(Info);
            this->GizmoMeshes[Y].ForceRenderWithOwnedMaterial(Info);
            this->GizmoMeshes[Z].ForceRenderWithOwnedMaterial(Info);
        });
    }};
    auto RenderRotator{[&]
    {
        maths::octant Octant;
        if (this->SelectedActor.Viewer->IsGridSpaceLocal())
        {
            LWorldQuat Q;
            if (this->SelectedActor.Quaternion)
            {
                Q = *this->SelectedActor.Quaternion;
            }
            else
            {
                Q = this->SelectedActor.Actor.first->GetRootComponent().GetLocalRotator();
            }
            Octant = maths::get_angled_octant(
                  this->SelectedActor.Actor.second->AsStatic<ASceneComponent>().GetWorldTranslationSlow()
                , Q
                , Info.PerspectiveEye.translation
                );
        }
        else
        {
            Octant = maths::get_octant(this->SelectedActor.Actor.second->AsStatic<ASceneComponent>().GetWorldTranslationSlow(), Info.PerspectiveEye.translation);
        }

        auto R{GetGizmoRotation<RotateR,LWorldReal,world_qual>(Octant)};
        auto Y{GetGizmoRotation<RotateY,LWorldReal,world_qual>(Octant)};
        auto P{GetGizmoRotation<RotateP,LWorldReal,world_qual>(Octant)};

        if (this->SelectedActor.Viewer->IsGridSpaceLocal())
        {
            if (this->SelectedActor.Quaternion)
            {
                R = *this->SelectedActor.Quaternion * R;
                Y = *this->SelectedActor.Quaternion * Y;
                P = *this->SelectedActor.Quaternion * P;
            }
            else
            {
                R = this->SelectedActor.Actor.first->GetRootComponent().GetLocalRotator() * R;
                Y = this->SelectedActor.Actor.first->GetRootComponent().GetLocalRotator() * Y;
                P = this->SelectedActor.Actor.first->GetRootComponent().GetLocalRotator() * P;
            }
        }

        UpdateTint(RotateR, *Prefs.EditorAxisTintX);
        UpdateTint(RotateY, *Prefs.EditorAxisTintY);
        UpdateTint(RotateP, *Prefs.EditorAxisTintZ);

        this->GizmoMeshes[RotateR].SetRotator(R);
        this->GizmoMeshes[RotateY].SetRotator(Y);
        this->GizmoMeshes[RotateP].SetRotator(P);

        Info.PostRenderDelegates.emplace_back([this](LActorRenderInfo const& Info)
        {
            this->GizmoMeshes[RotateR].ForceRenderWithOwnedMaterial(Info);
            this->GizmoMeshes[RotateY].ForceRenderWithOwnedMaterial(Info);
            this->GizmoMeshes[RotateP].ForceRenderWithOwnedMaterial(Info);
        });
    }};
    auto RenderScale{[&](EGizmoMesh X, EGizmoMesh Y, EGizmoMesh Z)
    {
        UpdateTint(X, *Prefs.EditorAxisTintX);
        UpdateTint(Y, *Prefs.EditorAxisTintY);
        UpdateTint(Z, *Prefs.EditorAxisTintZ);

        if (this->SelectedActor.Viewer->IsGridSpaceLocal())
        {
            for (auto& Mesh: std::array{X,Y,Z,})
            {
                this->GizmoMeshes[Mesh].SetRotator(this->SelectedActor.Actor.first->GetRootComponent().GetLocalRotator());
            }
        }
        else
        {
            for (auto& Mesh: std::array{X,Y,Z,})
            {
                this->GizmoMeshes[Mesh].SetRotator(maths::identity<LWorldQuat>);
            }
        }

        Info.PostRenderDelegates.emplace_back([this,X,Y,Z](LActorRenderInfo const& Info)
        {
            this->GizmoMeshes[X].ForceRenderWithOwnedMaterial(Info);
            this->GizmoMeshes[Y].ForceRenderWithOwnedMaterial(Info);
            this->GizmoMeshes[Z].ForceRenderWithOwnedMaterial(Info);
        });
    }};

    auto RenderPlanes{[&]
    {
        UpdateTint(PlaneXY, *Prefs.EditorAxisTintZ);
        UpdateTint(PlaneXZ, *Prefs.EditorAxisTintY);
        UpdateTint(PlaneYZ, *Prefs.EditorAxisTintX);

        if (this->SelectedActor.Viewer->IsGridSpaceLocal())
        {
            for (auto& Mesh: std::array{PlaneXY,PlaneXZ,PlaneYZ,})
            {
                this->GizmoMeshes[Mesh].SetRotator(this->SelectedActor.Actor.first->GetRootComponent().GetLocalRotator());
            }
        }
        else
        {
            for (auto& Mesh: std::array{PlaneXY,PlaneXZ,PlaneYZ,})
            {
                this->GizmoMeshes[Mesh].SetRotator(maths::identity<LWorldQuat>);
            }
        }

        Info.PostRenderDelegates.emplace_back([this](LActorRenderInfo const& Info)
        {
            this->GizmoMeshes[PlaneXY].ForceRenderWithOwnedMaterial(Info);
            this->GizmoMeshes[PlaneXZ].ForceRenderWithOwnedMaterial(Info);
            this->GizmoMeshes[PlaneYZ].ForceRenderWithOwnedMaterial(Info);
        });
    }};

    switch (this->Gizmo)
    {
    case WEditorWorldViewer::EGizmo::Translate:
    {
        RenderTranslation(TranslateX, TranslateY, TranslateZ);
        RenderPlanes();
        break;
    }
    case WEditorWorldViewer::EGizmo::Rotate:
    {
        RenderRotator();
        break;
    }
    case WEditorWorldViewer::EGizmo::Scale:
    {
        RenderScale(ScaleX, ScaleY, ScaleZ);
        RenderPlanes();
        break;
    }
    case WEditorWorldViewer::EGizmo::Gizmo:
    {
        RenderTranslation(TranslateX_Var, TranslateY_Var, TranslateZ_Var);
        RenderRotator();
        RenderScale(ScaleX_Var, ScaleY_Var, ScaleZ_Var);
        break;
    }
    default: break;
    }
}

void Jafg::AEditorPersonaControllerComponent::RenderTexts(LActorRenderInfo const& Info) const
{
    if (this->Texts.empty())
    {
        return;
    }

    STAT_CYCLE_FUNCTION()
    check(this->TextMaterialInstance.get())

    auto& Prefs{GetSingleton<JUserPreferences>()};

    auto& Frontend{this->GetFrontend()};
    auto& Subsystem{*Frontend.GetSubsystemChecked<JFontSubsystem>()};
    rhi::object_range<SSBO::VisualInstance> Range{SSBO::VisualInstance::default_count,SSBO::VisualInstance::default_count};

    auto ViewProjection{Info.WorldData.proj * Info.WorldData.view};
    for (auto& Text: this->Texts)
    {
        LWorldReal Distance{maths::magnitude(Info.PerspectiveEye.translation - Text.Location)};
        if (Distance > *Prefs.EditorVisualizationMaxDebugTextRenderDistance)
        {
            continue;
        }

        LVec4F Clip{ViewProjection * LVec4F{Text.Location, 1.0f}};
        if (Clip.w <= 0.0f)
        {
            continue;
        }

        auto FontSize{Text.Height * ((Info.VkViewport.height - Info.VkViewport.y) / (2 * maths::tan(Info.PerspectiveEye.vert_fov / 2) * Distance))};
        if (FontSize < *Prefs.EditorVisualizationDebugTextFontThreshold)
        {
            continue;
        }

        /* TODO: Most definitely cache this result. This line will become most probably a bottleneck later...
         *       But can it be cached with variadic font size (based of eye translation)? */
        LVec2F Pencil{maths::zero_vector<LVec2F>};
        auto Infos{Subsystem.GetGlyphInfos(Text.Text, FontSize, &Pencil, 0)};

        LVec3F Ndc{maths::xyz(Clip) / Clip.w};
        if (Ndc.z < 0.0f  || Ndc.z > 1.0f)
        {
            continue;
        }
        LVec2F Offset{
            Info.VkViewport.x + (Ndc.x * 0.5f + 0.5f) * Info.VkViewport.width,
            Info.VkViewport.y + (1.0f - (-Ndc.y * 0.5f + 0.5f)) * Info.VkViewport.height
            };
        for (auto& Glyph: Infos.GlyphInfos)
        {
            LRect2F Rect{{Offset.x + Glyph.Rect.x, Offset.y + Glyph.Rect.y}, {Glyph.Rect.z, Glyph.Rect.w}};
            if (maths::aabb(LRect2F{
                .offset = {Info.VkViewport.x, Info.VkViewport.y},
                .extent = {Info.VkViewport.width, Info.VkViewport.height},
                }, Rect))
            {
                Range->push_back({
                    .Rect = Rect,
                    .TexCoordRect = Glyph.TexCoordRect,
                    .Tint = Text.Tint,
                    .OutlineTint = Colors::White,
                    .OutlineThickness = 0,
                    .TextureIndex = Glyph.BindlessTextureIndex,
                    .SamplerIndex = Glyph.SamplerIndex,
                    .MsdfPixelRange = Glyph.MsdfPixelRange,
                    });
            }
        }
    }

    if (!Range->empty())
    {
        constexpr auto VisualSharedBinding{0uz};
        constexpr auto InstanceBufferBinding{1uz};

        auto& DsInstance{this->TextMaterialInstance->Vk_GetUniqueDescriptorSet(0, Info.Frame)};
        check(DsInstance.Resources.size() == 2)

        LVec2F Dimensions{Info.VkViewport.width, Info.VkViewport.height};
        auto VisualSharedWriteInfo{UBO::VisualShared{
                .Proj = glm::orthoRH_ZO(0.0f, Dimensions.x, 0.0f, Dimensions.y, 0.0f, 1.0f),
                .Gamma = *GetSingleton<JUserPreferences>().InterfaceGamma,
                }
            .upload(DsInstance.Resources[VisualSharedBinding].AsBuffer())
            .write_info(*DsInstance.Resources[VisualSharedBinding].AsBuffer())
            };

        auto InstanceBufferWriteInfo{Range.upload(DsInstance.Resources[InstanceBufferBinding].AsBuffer())};

        std::array Writes{
            vk::WriteDescriptorSet{
                .dstSet = *DsInstance,
                .dstBinding = VisualSharedBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = UBO::VisualShared::descriptor_type(),
                .pBufferInfo = &VisualSharedWriteInfo,
                },
            vk::WriteDescriptorSet{
                .dstSet = *DsInstance,
                .dstBinding = InstanceBufferBinding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = SSBO::VisualInstance::descriptor_type(),
                .pBufferInfo = &InstanceBufferWriteInfo
                },
            };
        Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});

        std::array<vk::DescriptorSet, 2> DescriptorSetsToBind;
        DescriptorSetsToBind[0] = *DsInstance;
        DescriptorSetsToBind[1] = *Frontend.GetSubsystemChecked<JTextureSubsystem>()->Vk_GetBindlessTextureArrayDescriptorSet();
        Info.CommandBuffer.bindDescriptorSets2({
            .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            .layout = *this->TextMaterialInstance->Material->Pipeline.pipeline_layout,
            .firstSet = 0,
            .descriptorSetCount = DescriptorSetsToBind.size(),
            .pDescriptorSets = DescriptorSetsToBind.data(),
            .dynamicOffsetCount = 0,
            .pDynamicOffsets = nullptr
            });

        Info.CommandBuffer.setPolygonModeEXT(vk::PolygonMode::eFill);
        Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *this->TextMaterialInstance->Material->Pipeline);
        Info.CommandBuffer.draw(4, static_cast<u32>(Range->size()), 0, 0);
    }
}

bool Jafg::AEditorCameraComponent::ActivateUserInputContext() noexcept
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

void Jafg::AEditorCameraComponent::OnHighlightTrace(WEditorWorldViewer& Viewer, rhi::extent2 Extent, LVec2F Location)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& Pawn{this->GetOwningPawn()};
    auto Eye{Pawn.GetEye()};

    auto Ray{LWorldMagRay3::from_ray(maths::screen_to_world_space({Extent.width,Extent.height}, Location, Eye), *Prefs.EditorTraceLength)};
    check(maths::normalized(Ray.direction))

    if (auto* PcComp{Pawn.GetOwningControllerChecked()->GetComponent<AEditorPersonaControllerComponent>()})
    {
        if (auto Mesh{PcComp->TraceGizmo(Ray, Extent, Eye)}; Mesh != AEditorPersonaControllerComponent::GizmoCount)
        {
            PcComp->SetHighlightedGizmoMesh(Mesh);
        }
    }
}

void Jafg::AEditorCameraComponent::OnTrace(WEditorWorldViewer& Viewer, bool bMultiselect, rhi::extent2 Extent, LVec2F Location, std::optional<LEditorTraceOrigin> Origin /* = {} */)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& Pawn{this->GetOwningPawn()};
    auto Eye{Pawn.GetEye()};

    auto Ray{LWorldMagRay3::from_ray(maths::screen_to_world_space({Extent.width,Extent.height}, Location, Eye), *Prefs.EditorTraceLength)};
    check(maths::normalized(Ray.direction))

    if (auto* PcComp{Pawn.GetOwningControllerChecked()->GetComponent<AEditorPersonaControllerComponent>()})
    {
        if (*Prefs.EditorVisualizeTraces)
        {
            PcComp->AddRay({.Tint = *Prefs.EditorTraceVisualizationTint, .Duration = *Prefs.EditorTraceVisualizationDuration, .Value = Ray,});
        }

        if (Origin)
        {
            check(&Origin->Node == &Viewer)
            if (auto Mesh{PcComp->TraceGizmo(Ray, Extent, Eye)}; Mesh != AEditorPersonaControllerComponent::GizmoCount)
            {
                PcComp->TraceForGizmo(Mesh, Ray, Extent, *Origin);
                return;
            }
        }
    }

    bool bHit{};
    for (auto& Hit: Pawn.GetWorld().LineTraceNonPhysical(Ray, {.bSingleHit=true}))
    {
        if (!Hit.Actor.IsEditorHitTestable())
        {
            continue;
        }

        bHit = true;
        if (*Prefs.EditorVisualizeTraceHits)
        {
            if (auto* Comp{this->GetOwningPawn().GetOwningControllerChecked()->GetComponent<AEditorPersonaControllerComponent>()})
            {
                constexpr auto offset{0.01f};
                Comp->AddAabb({
                    .Tint = *Prefs.EditorTraceHitVisualizationTint,
                    .Duration = *Prefs.EditorTraceHitVisualizationDuration,
                    .Value = {
                        .min = Hit.GlobalWorldLocation - LVec3F{offset},
                        .max = Hit.GlobalWorldLocation + LVec3F{offset},
                        }
                    });
            }
        }
        if (bMultiselect)
        {
            if (algo::contains(Viewer.GetSelectedActors(), &Hit.Actor, algo::pair_first))
            {
                Viewer.SelectActors(Viewer.GetSelectedActors()
                    | algo::views::filter([Actor=&Hit.Actor](auto& E){ return E.first != Actor; })
                    | algo::to_array_fn{Viewer.GetSelectedActors().size()}
                    );
            }
            else
            {
                auto Current{Viewer.GetSelectedActors()};
                /* When multi selecting, one can only select the root component of an actor. */
                Current.emplace_back(&Hit.Actor, &Hit.Actor.GetRootComponent());
                Viewer.SelectActors(std::move(Current));
            }
        }
        else
        {
            /*
             * TODO: When hitting an actor. We want to always select said actor, not a subcomponent.
             *       Only maybe when double clicking a specific sub-component select it.
             */
            // if double_click: Viewer.SelectActors({{&Hit.Actor, &Hit.Component.AsStatic<ASceneComponent>()}}); else:
            Viewer.SelectActors({{&Hit.Actor, &Hit.Actor.GetRootComponent()}});
        }
    }
    if (!bHit && !bMultiselect)
    {
        Viewer.SelectActors({});
    }
}

void Jafg::AEditorCameraComponent::OnMove(LInputActionValue const& Value)
{
    if (this->GetOwningActor().HasRootComponent())
    {
        auto& Sc{this->GetOwningActor().GetRootComponent()};
        LWorldVec3 Front{Sc.GetLocalRotator() * maths::forward_vector<LVec3F>};

        auto Value3D{Value.GetAxis3DValue() * this->VelocityMultiplier};

        Value3D *= this->GetWorld().GetDeltaTime();

        LWorldVec3 Delta{maths::zero_vector<LWorldVec3>};
        Delta += Front * Value3D.x;
        Delta += glm::normalize(glm::cross(Front, maths::up_vector<LWorldVec3>)) * Value3D.y;
        Delta += maths::up_vector<LWorldVec3> * Value3D.z;
        Sc.AddLocalTranslationByTeleport(Delta);
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
    if (this->GetOwningActor().HasRootComponent())
    {
        auto& Sc{this->GetOwningActor().GetRootComponent()};
        auto Value2D{Value.GetAxis2DValue() * this->Sensitivity};

        /* Yaw */
        Sc.AddLocalRotatorByTeleport(maths::angle_axis(-glm::radians(Value2D.x), maths::up_vector<LWorldVec3>));

        /* Pitch */
        LWorldReal Pitch{maths::clamp(this->CachedPitch + glm::radians(Value2D.y), glm::radians(-89.9f), glm::radians(89.9f))};
        Sc.AddLocalRotatorByTeleport(maths::angle_axis((this->CachedPitch - Pitch), Sc.GetLocalRotator() * maths::right_vector<LWorldVec3>));

        Sc.SetLocalRotatorByTeleport(maths::normalize(Sc.GetLocalRotator()));
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
    this->VelocityMultiplier = maths::clamp(
          this->VelocityMultiplier + Value.GetAxis1DValue() * this->VelocityMultiplierAcceleration
        , MinVelocityMultiplier
        , MaxVelocityMultiplier
        );
}
