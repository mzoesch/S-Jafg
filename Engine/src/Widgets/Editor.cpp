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
#include "Framework/SceneComponent.h"
#include "Framework/LackeyForward.h"
#include "Framework/PersonaController.h"
#include "Platform/Surface.h"
#include "User/Input/InputTypes.h"
#include "User/Input/InputActionValue.h"
#include "Framework/Actor.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/StaticMeshComponent.h"
#include "User/LocalEgo.h"
#include "Rhi/ViewProj.h"
#include "Rhi/SolidColor.h"

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
        Comp.SetTranslation(LWorldVec3{0,0,10});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Plane.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{0,0,4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cube.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{2,0,4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Sphere.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{4,0,4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Icosphere.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{6,0,4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cylinder.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{8,0,4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Cone.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{10,0,4});
    });

    SpawnObject(TWorldStaticInit<AActor>{this->GetWorld()})
    ->EmplaceRootComponent<AStaticMeshComponent>([MaterialInstance](AStaticMeshComponent& Comp)
    {
        Comp.SetMesh(LITERAL_TEXT("Content/Models/Torus.glb"));
        Comp.SetMaterialInstance(std::move(MaterialInstance));
        Comp.SetTranslation(LWorldVec3{12,0,4});
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

    if (this->IsSelectedActorValid())
    {
        this->SetTranslationForGizmos(this->SelectedActor->GetRootComponent().GetTranslation());
    }
}

void Jafg::AEditorPersonaControllerComponent::Render(LActorRenderInfo const& Info) const
{
    Super::Render(Info);

    this->RenderRays(Info);
    this->RenderGizmo(Info);
}

Jafg::EGizmo Jafg::AEditorPersonaControllerComponent::SetSelectedGizmo(EGizmo Gizmo) noexcept
{
    return std::exchange(this->Gizmo, Gizmo);
}

Jafg::AActor* Jafg::AEditorPersonaControllerComponent::SetSelectedActor(WWorldViewer* Origin, AActor* Actor) noexcept
{
    this->SelectedActor.Viewer = Origin;
    this->SelectedActor.Quaternion.reset();
    auto* Result{std::exchange(this->SelectedActor.Actor, Actor)};

    if (this->IsSelectedActorValid())
    {
        this->SetTranslationForGizmos(this->SelectedActor->GetRootComponent().GetTranslation());
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
    if (this->Gizmo == EGizmo::Select || !this->IsSelectedActorValid())
    {
        return GizmoCount;
    }

    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& Comp{this->SelectedActor->GetRootComponent()};
    this->SetTranslationForGizmos(Comp.GetTranslation());

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
    case EGizmo::Translate: return HandleGizmoRange(std::array{TranslateX, TranslateY, TranslateZ, PlaneXY, PlaneXZ, PlaneYZ,});
    case EGizmo::Rotate: return HandleGizmoRange(std::array{RotateR, RotateY, RotateP});
    case EGizmo::Scale: return HandleGizmoRange(std::array{ScaleX, ScaleY, ScaleZ, PlaneXY, PlaneXZ, PlaneYZ,});
    case EGizmo::Gizmo: return HandleGizmoRange(std::array{TranslateX_Var,TranslateY_Var,TranslateZ_Var,RotateR,RotateY,RotateP,ScaleX_Var,ScaleY_Var,ScaleZ_Var});
    default: LOG_FATAL(LogEditor, "Invalid gizmo type [{}].", std::to_underlying(this->Gizmo))
    }
}

void Jafg::AEditorPersonaControllerComponent::TraceForGizmo(EGizmoMesh Mesh, LWorldMagRay3 const& Ray, rhi::extent2 Extent, LEditorTraceOrigin const& Origin)
{
    check(!(this->Gizmo == EGizmo::Select || !this->IsSelectedActorValid()))

    auto& Comp{this->SelectedActor->GetRootComponent()};
    this->SetTranslationForGizmos(Comp.GetTranslation());

    auto HandleGizmo{[&](EGizmoMesh Mesh, auto&& Handler)
    {
        this->UsedGizmoMesh = Mesh;
        this->SelectedActor.Quaternion = Comp.GetRotator();

        Origin.Node.GetViewport().EmplaceUntil<ERawInputStateBits::Release>(Origin.Event.PhysicalKey,
        [this,Mesh,Extent=Extent,N=&Origin.Node,Handler,OriginTransform=this->SelectedActor->GetRootComponent().GetTransform()
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

            // TODO: Fix translation
            auto CursorLocation{N->GetViewport().GetSurface().GetMouseLocationValue()};
            if (!LastCursor)
            {
                LastCursor = CursorLocation;
            }
            LVec2F Location{CursorLocation - DiscardedCursor - N->GetAnchoredAndTranslatedTopLeftFromMostOuter(maths::zero_vector<LVec2F>)};

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
                Viewer->GetOwnedPersonaControllerChecked()->GetOwnedPawnChecked()->GetRootComponent().AddTranslation(Point - this->GizmoMeshes[Mesh].GetTranslation());
                Result = true;
            }
            else
            {
                Surface.SetInputMode(EInputModeBits::ShowMouseCursor);
            }

            this->SelectedActor->GetRootComponent().SetTranslation(Point);
            this->SetTranslationForGizmos(this->SelectedActor->GetRootComponent().GetTranslation());
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
                this->SelectedActor->GetRootComponent().SetRotator(T.r * maths::angle_axis(maths::radians(Delta), maths::unit_vector_x<LWorldVec3>));
            }
            else
            {
                this->SelectedActor->GetRootComponent().SetRotator(maths::angle_axis(maths::radians(Delta), maths::unit_vector_x<LWorldVec3>) * T.r);
            }
        }
        else if (Mesh == RotateY)
        {
            if (Viewer->IsGridSpaceLocal())
            {
                this->SelectedActor->GetRootComponent().SetRotator(T.r * maths::angle_axis(maths::radians(Delta), maths::unit_vector_y<LWorldVec3>));
            }
            else
            {
                this->SelectedActor->GetRootComponent().SetRotator(maths::angle_axis(maths::radians(Delta), maths::unit_vector_y<LWorldVec3>) * T.r);
            }
        }
        else if (Mesh == RotateP)
        {
            if (Viewer->IsGridSpaceLocal())
            {
                this->SelectedActor->GetRootComponent().SetRotator(T.r * maths::angle_axis(maths::radians(Delta), maths::unit_vector_z<LWorldVec3>));
            }
            else
            {
                this->SelectedActor->GetRootComponent().SetRotator(maths::angle_axis(maths::radians(Delta), maths::unit_vector_z<LWorldVec3>) * T.r);
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
                this->SelectedActor->GetRootComponent().SetScale(T.s + LWorldVec3{Delta,0,0});
            }
            else if (Mesh == ScaleY || Mesh == ScaleY_Var)
            {
                this->SelectedActor->GetRootComponent().SetScale(T.s + LWorldVec3{0,Delta,0});
            }
            else if (Mesh == ScaleZ || Mesh == ScaleZ_Var)
            {
                this->SelectedActor->GetRootComponent().SetScale(T.s + LWorldVec3{0,0,Delta});
            }
            else if (Mesh == PlaneXY)
            {
                this->SelectedActor->GetRootComponent().SetScale(T.s + LWorldVec3{Delta,Delta,0});
            }
            else if (Mesh == PlaneXZ)
            {
                this->SelectedActor->GetRootComponent().SetScale(T.s + LWorldVec3{Delta,0,Delta});
            }
            else if (Mesh == PlaneYZ)
            {
                this->SelectedActor->GetRootComponent().SetScale(T.s + LWorldVec3{0,Delta,Delta});
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
    case EGizmo::Translate:
    {
        check(algo::contains(std::array{TranslateX, TranslateY, TranslateZ, PlaneXY, PlaneXZ, PlaneYZ,}, Mesh))
        HandleGizmo(Mesh, HandleTranslation);
        break;
    }
    case EGizmo::Rotate:
    {
        check(algo::contains(std::array{RotateR, RotateY, RotateP}, Mesh))
        HandleGizmo(Mesh, HandleRotation);
        break;
    }
    case EGizmo::Scale:
    {
        check(algo::contains(std::array{ScaleX, ScaleY, ScaleZ, PlaneXY, PlaneXZ, PlaneYZ,}, Mesh))
        HandleGizmo(Mesh, HandleScale);
        break;
    }
    case EGizmo::Gizmo:
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
    auto& Prefs{GetSingleton<JUserPreferences>()};
    check(this->RayInstance.get())

    if (*Prefs.EditorVisualizeMeshAabbs)
    {
        for (auto& E: this->GetWorld().GetEmployees() | algo::views::filter([](auto& E){ return E->template IsA<AActor>(); }))
        {
            auto& A{*StaticCastChecked<AActor>(&*E)};
            for (auto& Comp: A.GetComponents() | algo::views::filter([](auto& C){ return C->template IsA<ASceneComponent>(); }))
            {
                auto& Sc{*StaticCastChecked<ASceneComponent>(&*Comp)};
                if (Sc.ShouldRender())
                {
                    this->Aabbs.emplace_back(*Prefs.EditorMeshAabbVisualizationTint, OneTimeDraw, Sc.GetAabb().apply(Sc.GetTransform()));
                }
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
    if (!this->IsSelectedActorValid())
    {
        return;
    }

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
                this->GizmoMeshes[Mesh].SetRotator(this->SelectedActor->GetRootComponent().GetRotator());
            }
        }
        else
        {
            for (auto& Mesh: std::array{X,Y,Z,})
            {
                this->GizmoMeshes[Mesh].SetRotator(maths::identity<LWorldQuat>);
            }
        }

        this->GizmoMeshes[X].Render(Info);
        this->GizmoMeshes[Y].Render(Info);
        this->GizmoMeshes[Z].Render(Info);
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
                Q = this->SelectedActor->GetRootComponent().GetRotator();
            }
            Octant = maths::get_angled_octant(
                  this->SelectedActor->GetRootComponent().GetTranslation()
                , Q
                , Info.PerspectiveEye.translation
                );
        }
        else
        {
            Octant = maths::get_octant(this->SelectedActor->GetRootComponent().GetTranslation(), Info.PerspectiveEye.translation);
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
                R = this->SelectedActor->GetRootComponent().GetRotator() * R;
                Y = this->SelectedActor->GetRootComponent().GetRotator() * Y;
                P = this->SelectedActor->GetRootComponent().GetRotator() * P;
            }
        }

        UpdateTint(RotateR, *Prefs.EditorAxisTintX);
        UpdateTint(RotateY, *Prefs.EditorAxisTintY);
        UpdateTint(RotateP, *Prefs.EditorAxisTintZ);

        this->GizmoMeshes[RotateR].SetRotator(R);
        this->GizmoMeshes[RotateY].SetRotator(Y);
        this->GizmoMeshes[RotateP].SetRotator(P);

        this->GizmoMeshes[RotateR].Render(Info);
        this->GizmoMeshes[RotateY].Render(Info);
        this->GizmoMeshes[RotateP].Render(Info);
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
                this->GizmoMeshes[Mesh].SetRotator(this->SelectedActor->GetRootComponent().GetRotator());
            }
        }
        else
        {
            for (auto& Mesh: std::array{X,Y,Z,})
            {
                this->GizmoMeshes[Mesh].SetRotator(maths::identity<LWorldQuat>);
            }
        }

        this->GizmoMeshes[X].Render(Info);
        this->GizmoMeshes[Y].Render(Info);
        this->GizmoMeshes[Z].Render(Info);
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
                this->GizmoMeshes[Mesh].SetRotator(this->SelectedActor->GetRootComponent().GetRotator());
            }
        }
        else
        {
            for (auto& Mesh: std::array{PlaneXY,PlaneXZ,PlaneYZ,})
            {
                this->GizmoMeshes[Mesh].SetRotator(maths::identity<LWorldQuat>);
            }
        }

        this->GizmoMeshes[PlaneXY].Render(Info);
        this->GizmoMeshes[PlaneXZ].Render(Info);
        this->GizmoMeshes[PlaneYZ].Render(Info);
    }};

    switch (this->Gizmo)
    {
    case EGizmo::Translate:
    {
        RenderTranslation(TranslateX, TranslateY, TranslateZ);
        RenderPlanes();
        break;
    }
    case EGizmo::Rotate:
    {
        RenderRotator();
        break;
    }
    case EGizmo::Scale:
    {
        RenderScale(ScaleX, ScaleY, ScaleZ);
        RenderPlanes();
        break;
    }
    case EGizmo::Gizmo:
    {
        RenderTranslation(TranslateX_Var, TranslateY_Var, TranslateZ_Var);
        RenderRotator();
        RenderScale(ScaleX_Var, ScaleY_Var, ScaleZ_Var);
        break;
    }
    default: break;
    }
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

void Jafg::AEditorCameraComponent::OnHighlightTrace(WWorldViewer& Viewer, rhi::extent2 Extent, LVec2F Location)
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

void Jafg::AEditorCameraComponent::OnTrace(WWorldViewer& Viewer, bool bMultiselect, rhi::extent2 Extent, LVec2F Location, std::optional<LEditorTraceOrigin> Origin /* = {} */)
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
        bHit = true;
        if (auto* Comp{this->GetOwningPawn().GetOwningControllerChecked()->GetComponent<AEditorPersonaControllerComponent>()})
        {
            if (*Prefs.EditorVisualizeTraceHits)
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
            if (algo::contains(Viewer.GetSelectedActors(), &Hit.Actor))
            {
                Viewer.SelectActors(Viewer.GetSelectedActors()
                    | algo::views::filter([Actor=&Hit.Actor](auto* E){ return E != Actor; })
                    | algo::to_array_fn{Viewer.GetSelectedActors().size()}
                    );
            }
            else
            {
                auto Current{Viewer.GetSelectedActors()};
                Current.emplace_back(&Hit.Actor);
                Viewer.SelectActors(std::move(Current));
            }
        }
        else
        {
            Viewer.SelectActors({&Hit.Actor});
        }
    }
    if (!bHit && !bMultiselect)
    {
        Viewer.SelectActors({});
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
        Sc->AddRotator(maths::angle_axis((this->CachedPitch - Pitch), Sc->GetRotator() * maths::right_vector<LWorldVec3>), ESceneSweep::Teleport);

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
    this->VelocityMultiplier = maths::clamp(
          this->VelocityMultiplier + Value.GetAxis1DValue() * this->VelocityMultiplierAcceleration
        , MinVelocityMultiplier
        , MaxVelocityMultiplier
        );
}
