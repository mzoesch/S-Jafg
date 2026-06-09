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

#if JAFG_WITH_EDITOR

namespace
{

struct LState
{
    constexpr LState() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LState)

    typedef std::variant<Jafg::TButtonStyle<Jafg::LRegionBrush>*, Jafg::TButtonStyle<Jafg::LBoxBrush>*> StyleVariant;
    typedef std::variant<Jafg::LRegionBrush*, Jafg::LBoxBrush*> BrushVariant;

    std::size_t FrameCount{};
    Jafg::EStyleBits Bit{Jafg::EStyleBits::Identity};
    TArray<std::tuple<StyleVariant, BrushVariant, TFunction2<bool()>>> Pairs;

    template<typename T>
    static decltype(auto) Lambda(auto&& SharedState) noexcept
    {
        return [SharedState](auto&& F)
        {
            T& Button{*F.GetRawNode().template AsChecked<T>()};
            SharedState->EmplaceNodes(Button, SharedState, [&Button]{ return Button.IsEnabled(); });
        };
    }

    template<typename T>
        requires (!std::is_const_v<T> && (std::is_base_of_v<Jafg::WTextButton, T> || std::is_base_of_v<Jafg::WButton, T>))
    void EmplaceNodes(T& Node, auto&& SharedState, auto&& EnabledFunctor) noexcept
    {
        check(GEngine)
        check(!Node.OnBrushChangedEvent.IsValid())

        this->Pairs.emplace_back(&Node.Style, &Node.Brush, std::forward<decltype(EnabledFunctor)>(EnabledFunctor));
        Node.OnBrushChangedEvent = [this, SharedState](auto& Self, Jafg::EStyleBits Bit)
        {
            (void)SharedState;

            if (this->FrameCount < GEngine->FrameCount || Bit != Jafg::EStyleBits::Normal)
            {
                this->FrameCount = GEngine->FrameCount;
                this->Bit = Bit;
                for (auto& [StyleVariant, BrushVariant, Enabled]: this->Pairs)
                {
                    check(!!Enabled)
                    std::visit([&Self, &Enabled, Bit](auto&& StylePtr, auto&& BrushPtr)
                    {
                        if (static_cast<void*>(&Self.Brush) != static_cast<void*>(BrushPtr) && Enabled())
                        {
                            if constexpr ((std::same_as<std::remove_cvref_t<std::decay_t<decltype(StylePtr)>>, Jafg::TButtonStyle<Jafg::LRegionBrush>*>
                                    && std::same_as<std::remove_cvref_t<std::decay_t<decltype(BrushPtr)>>, Jafg::LRegionBrush*>)
                                || (std::same_as<std::remove_cvref_t<std::decay_t<decltype(StylePtr)>>, Jafg::TButtonStyle<Jafg::LBoxBrush>*>
                                    && std::same_as<std::remove_cvref_t<std::decay_t<decltype(BrushPtr)>>, Jafg::LBoxBrush*>))
                            {
                                Jafg::ApplyStyleBit(*StylePtr, *BrushPtr, Bit);
                            }
                            else
                            {
                                std::unreachable();
                            }
                        }
                    }, StyleVariant, BrushVariant);
                }
            }
            else
            {
                Jafg::ApplyStyleBit(Self.Style, Self.Brush, Bit);
            }
        };

        return;
    }
};

Jafg::LFactoryButton EditorResetButton(Jafg::LViewport& Viewport, bool bEnabled, TFunction2<void(Jafg::WButton& Self)> OnAction, auto&& Delegate)
{
    using namespace Jafg;
    auto& Prefs{GetSingleton<JUserPreferences>()};

    return NewNode(Viewport).Class<WButton>()
        .Anchor(EAnchor::VFill)
        .MinDesiredSize({24_spt, 20})
        .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::BorderTint>(*Prefs.ForegroundColorVariant)
        .InBrush<EStyleBits::InactiveCombi, &LRegionBrush::BorderTint>(*Prefs.ForegroundColor)
        .InBrush<EStyleBits::ActiveCombi|EStyleBits::Normal, &LRegionBrush::Tint>(Colors::White)
        .InBrush<EStyleBits::Disabled, &LRegionBrush::Tint>(Colors::Gray)
        .InAllBrushes<&LRegionBrush::Background>(LRegionBrush::Icon("Icons/Jafg.Reset"))
        .Enabled(bEnabled)
        .Delegate(Delegate)
        .OnKeyEventFocused([OnAction=std::move(OnAction)](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event) mutable
        {
            if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
            {
                check(!!OnAction)
                OnAction(Self.AsStatic<WButton>());
                return LNodeReply::Handled();
            }
            return LNodeReply::Unhandled();
        });
}

} /* ~Namespace <Anonymous> */

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
        check(Pointer.get() && Pointer->Vector)
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
        + MakeContent(Translation, NewNode(Info.Viewport).Class<WInput_Vector3>(LVec3D{Info.Field.T}).SaveTo(&Translation->Vector)
            .OnVectorChanged([Pointer=Translation, Field=&Info.Field](WInput_Vector3& Self)
            {
                check(Pointer.get() && Pointer->Label && Pointer->Content && Pointer->Vector && Pointer->Reset)
                Field->T = Self.Get<f64>();
                Pointer->Reset->SetEnabled(Field->T != maths::zero_vector<LVec3F>);
            }))
        + MakeReset(Translation, maths::zero_vector<LVec3D>, Info.Field.T != maths::zero_vector<LVec3F>)
    ]
    + NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
    [
        MakeLabel(Rotation, "Rotation")
        + MakeContent(Rotation, NewNode(Info.Viewport).Class<WInput_Vector3>(LVec3D{maths::euler_angles_deg(Info.Field.R)}).SaveTo(&Rotation->Vector)
            .OnVectorChanged([Pointer=Rotation, Field=&Info.Field](WInput_Vector3& Self)
            {
                check(Pointer.get() && Pointer->Label && Pointer->Content && Pointer->Vector && Pointer->Reset)
                Field->R = maths::rotator_deg(Self.Get<f64>());
                Pointer->Reset->SetEnabled(Field->R != maths::identity<LWorldQuat>);
            }))
        + MakeReset(Rotation, maths::zero_vector<LVec3D>, Info.Field.R != maths::identity<LWorldQuat>)
    ]
    + NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
    [
        MakeLabel(Scale, "Scale")
        + MakeContent(Scale, NewNode(Info.Viewport).Class<WInput_Vector3>(LVec3D{Info.Field.S}).SaveTo(&Scale->Vector)
            .OnVectorChanged([Pointer=Scale, Field=&Info.Field](WInput_Vector3& Self)
            {
                check(Pointer.get() && Pointer->Label && Pointer->Content && Pointer->Vector && Pointer->Reset)
                Field->S = Self.Get<f64>();
                Pointer->Reset->SetEnabled(Field->T != maths::zero_vector<LVec3F>);
            }))
        + MakeReset(Scale, maths::one_vector<LVec3D>, Info.Field.S != maths::one_vector<LVec3F>)
    ];
}

template<>
Jafg::Detail::LNodeFactoryBase Jafg::GetEditorNode<LString>(TEditorNodeCreateInfo<LString> const& Info) noexcept
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    struct MyState : public LState
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
        NewNode(Info.Viewport).Class<WTextButton>()
            .MinDesiredSize({128_spt, 0})
            .Anchor(EAnchor::VFill)
            .InBrushChained<EStyleBits::ActiveCombi, &LBoxBrush::Tint, &LBoxBrush::Padding>(*Prefs.ForegroundColorVariant, {20_spt, 0, 0, 0})
            .InBrushChained<EStyleBits::InactiveCombi, &LBoxBrush::Tint, &LBoxBrush::Padding>(*Prefs.ForegroundColor, {20_spt, 0, 0, 0})
            .InTextBrushChained<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray)
            .InAllTextBrushes<&LTextBoxBrush::TextVAlign>(ETextVAlign::Center)
            .Content(Info.What)
            .Delegate(LState::Lambda<WTextButton>(SharedState))
        + NewNode(Info.Viewport).Class<WButton>()
            .Anchor(EAnchor::HFill)
            .Visibility(ENodeVisibility::Visible)
            .Padding({12_spt, 3})
            .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::Tint>(*Prefs.ForegroundColorVariant)
            .InBrush<EStyleBits::InactiveCombi, &LRegionBrush::Tint>(*Prefs.ForegroundColor)
            .Delegate(MyState::Lambda<WButton>(SharedState))
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
                    return;
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
                    return;
                })
        ]
        + ::EditorResetButton(Info.Viewport, Info.Default && Info.Field != Info.Default, [SharedState, Field=&Info.Field](WButton& Self)
        {
            check(SharedState.get())
            check(SharedState->Default)
            *Field = *SharedState->Default;
            check(SharedState->EditableTextButton && SharedState->ResetButton)
            SharedState->EditableTextButton->SetContent(*Field);
            SharedState->ResetButton->SetEnabled(*Field != *SharedState->Default);
            return;
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

    return;
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

    return;
}

void Jafg::WEditor::BeginClassLife(LBeginClassLifeInfo const& Info)
{
    Super::BeginClassLife(Info);
    Finder::CreateDirectories(WEditor::GetUserLayoutsPath());
    return;
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

    this->ApplyEditorLayout(this->LoadEditorLayout(*Prefs.EditorLastLayout));

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

TArray<LPath> const& Jafg::WEditor::DiscoverLayouts()
{
    LOG_VERBOSE(LogEditor, "Discovering editor layouts.")

    this->DiscoveredLayouts.clear();

    this->DiscoveredLayouts.emplace_back(Finder::GetDefaultConfigDir()/"DefaultEditorLayout.json");
    this->DiscoveredLayouts.append_range(Finder::FindFilesRecursively(WEditor::GetUserLayoutsPath(), true, ".*\\.json"));

    LOG_VERBOSE(LogEditor, "Discovered [{}] layouts at:", this->DiscoveredLayouts.size())
    for (auto const& Layout : this->DiscoveredLayouts)
    {
        LOG_VERBOSE(LogEditor, " - {}", Layout);
    }

    return this->GetDiscoveredLayouts();
}

Jafg::LEditorLayout Jafg::WEditor::LoadEditorLayout(LPath Path)
{
    LOG_VERBOSE(LogEditor, "[{}]: Loading.", Path);

    LEditorLayout Result = json::parse(Finder::ReadFile(Path), nullptr, false).get<LEditorLayout>();
    Result.Path = std::move(Path);

    if (Result.Surfaces.empty())
    {
        LOG_FATAL(LogEditor, "[{}]: Layout must contain at least one surface. Failed to load layout.", Result.Path);
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
    LOG_VERBOSE(LogEditor, "[{}]: Applying layout.", Layout.Path);

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

    for (auto It{Layout.Surfaces.begin() + 1}; It != Layout.Surfaces.end(); ++It)
    {
        LOG_FATAL(LogEditor, "Currently only one surface is allowed.")
    }

    return;
}
