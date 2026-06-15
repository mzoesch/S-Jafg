// Copyright mzoesch. All rights reserved.

#include "Widgets/WorldViewer.h"
#include "Engine/Engine.h"
#include "Nodes/Viewport.h"
#include "Platform/Surface.h"
#include "Framework/Frontend.h"
#include "Nodes/Button.h"
#include "Nodes/Region.h"
#include "Nodes/DropDown.h"
#include "Nodes/HParent.h"
#include "Nodes/TextButton.h"
#include "Rhi/NodeRenderInfo.h"
#include "User/UserPreferences.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "Nodes/Spacer.h"
#include "Nodes/Text.h"
#include "Widgets/Input_Vector2.h"
#include "Nodes/HButton.h"
#include "Engine/World.h"
#include "Nodes/EditableTextButton.h"
#include "Nodes/CheckmarkButton.h"
#include "Nodes/ScrollRegion.h"
#include "Nodes/VParent.h"
#include "Core/App.h"
#include "Widgets/Editor.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/ShaderSubsystem.h"
#include "Rhi/OutlineRendering.h"

Jafg::WWorldViewer::~WWorldViewer()
{
    this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
}

void Jafg::WWorldViewer::Construct()
{
    Super::Construct();

    this->RenderTargetViewport.Vk_OnLateInit();

    BeginStyling(*this).StaticRoot<WText>().SaveTo(&this->Placeholder)
        .Visibility(ENodeVisibility::Visible)
        .Anchor(EAnchor::Fill)
        .TextScale(ETextScale::Header)
        .TextAlign(ETextHAlign::Center)
        .TextAlign(ETextVAlign::Center)
        .Content("No World Loaded")
        ;

    BeginStyling(*this).StaticRoot<WHParent>()
        .Padding(3_pt)
    [
        NewStaticNode(WButton)
            .MinDesiredSize(25_spt2)
            .InAllBrushesChained<&LRegionBrush::Background, &LRegionBrush::Radii>(LRegionBrush::Icon("Icons/Jafg.Menu"), LVec4F{50.0f})
            .InBrushChained<EStyleBits::ActiveCombi, &LRegionBrush::Tint, &LRegionBrush::BorderTint>(Colors::White, LColor{0x4c})
            .InBrushChained<EStyleBits::InactiveCombi, &LRegionBrush::Tint, &LRegionBrush::BorderTint>(LColor{0xA0}, LColor{0x3c})
            .InAllBrushesChained<&LRegionBrush::OutlineThickness, &LRegionBrush::OutlineTint>(1, Colors::Black)
            .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                {
                    this->CreateMenuDropDown(Info.CursorLocation.value_or(maths::zero_vector<LVec2F>));
                    return LNodeReply::Handled();
                }
                return LNodeReply::Unhandled();
            })
    ];

    auto& MaterialSubsystem{*this->GetMutableFrontend().GetSubsystemChecked<JMaterialSubsystem>()};


    this->SelectionMaterialInstance = MaterialSubsystem.GetInstanceFromMaterialName("Jafg.Mesh.Outline");
    check(this->SelectionMaterialInstance.get())
    this->PostSelectionMaterialInstance = MaterialSubsystem.GetInstanceFromMaterialName("Jafg.Mesh.OutlinePost");
    check(this->PostSelectionMaterialInstance.get())

    auto& Prefs{GetSingleton<JUserPreferences>()};
    if (*Prefs.EditorAutoLaunchLastWorld && !Prefs.EditorLastWorldName->empty() && !Prefs.EditorLastWorldLevelName->empty())
    {
        auto World{this->GetMutableEngine().SummonWorld({.HumanReadableName=*Prefs.EditorLastWorldName})};
        // this->QueueTravelTo(World.Browse(*Prefs.EditorLastWorldLevelName));
        this->QueueTravelTo(World.Browse("LevelEditor"));
    }
}

void Jafg::WWorldViewer::Tick()
{
    Super::Tick();

    if (this->bDatedRenderTarget)
    {
        LOG_VERBOSE(LogWidgets, "[{}]: Render target is dated. Recreating render target.", this->GetNameAsString())
        this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
        this->InitializeRenderTarget();
    }
    else if (this->DesiredViewportExtent)
    {
        if (*this->DesiredViewportExtent != this->RenderTarget.GetExtent())
        {
            LOG_TRACE(LogWidgets, "[{}]: Desired viewport extent changed to [{}]. Recreating render target.", this->GetNameAsString(), *this->DesiredViewportExtent)
            this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
            this->InitializeRenderTarget();
        }
        this->LastUnstableExtent.reset();
    }
    else if (rhi::extent2 UnstableSize{rhi::extent2::from_vec(this->GetAnchoredSize_v2())}; UnstableSize.width > 0 && UnstableSize.height > 0)
    {
        if (!this->RenderTarget.IsInitialized())
        {
            this->InitializeRenderTarget();
        }
        else
        {
            constexpr f32 Threshold{0.2f};
            if (this->RenderTarget.GetExtent() == UnstableSize)
            {
                this->LastUnstableExtent.reset();
            }
            else if (!this->LastUnstableExtent || *this->LastUnstableExtent != UnstableSize)
            {
                this->LastUnstableDiff = algo::now();
                this->LastUnstableExtent = UnstableSize;
            }
            else if (algo::time_diff(this->LastUnstableDiff, algo::now()) > Threshold)
            {
                LOG_TRACE(LogWidgets, "[{}]: Desired viewport extent changed to [{}]. Recreating render target.", this->GetNameAsString(), UnstableSize)
                this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
                this->InitializeRenderTarget();
                this->LastUnstableExtent.reset();
            }
        }
    }

    if (!this->ConsumeHandle.IsValid())
    {
        if (this->UserInput._bCurrentlyConsuming)
        {
            this->ConsumeHandle = LRaiiViewportHandle::Make(this->GetViewport().OnLateTick, [this]{ this->DispatchInputDelegates(); return false; });
        }
    }
}

void Jafg::WWorldViewer::Draw(LNodeRenderInfo const& Info) const
{
    if (this->RenderTarget.IsInitialized())
    {
        check(!!this->RenderTarget.GetMsaa().Image.GetBuffer())
        if (!this->RenderTarget.IsResolvedBindless())
        {
            this->RenderTarget._SetResolvedBindlessIndex(
                Info.Frontend._VK_AddTransientImageToGlobalBindlessArray(this->RenderTarget.GetResolved().ImageView)
                .value_or(INDEX_NONE)
                );
            if (!this->RenderTarget.IsResolvedBindless())
            {
                LOG_FATAL(LogVulkan, "[{}]: Failed to make render target texture bindless. Out of binding points."
                    , this->GetNameAsString())
            }
        }

        LRect2F Rect;
        if (this->IsManual())
        {
            Rect = {.Offset=this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
                    .Extent=this->GetAnchoredSize_v2()};
        }
        else
        {
            Rect = {.Offset=maths::round(this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation)),
                    .Extent=this->RenderTarget.GetExtent().ToVec<f32>()};
        }

        Info.AddInstance({
            .Rect = Rect,
            .TexCoordRect = {rhi::uv::fit(rhi::uv::identity
                , this->RenderTarget.GetExtent().ToVec<f32>(), this->GetAnchoredSize_v2()
                , rhi::tex_coord_behavior::FitAspect)},
            .Tint = Colors::White,
            .BorderTint = this->BorderTint,
            .TextureIndex = this->RenderTarget.GetResolvedBindlessIndex(),
            .SamplerIndex = UBO::Bindless::LinearClampToBorderSamplerIdx,
            });
    }

    Super::Draw(Info);
}

void Jafg::WWorldViewer::Destruct()
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

void Jafg::WWorldViewer::OnFocusLost()
{
    Super::OnFocusLost();

    if (this->ConsumeHandle.IsValid())
    {
        LOG_TRACE(LogWidgetFramework, "[{}]: Escaping user input capture due to focus loss.", this->GetNameAsString())
        check(this->UserInput._bCurrentlyConsuming)
        this->UserInput._bCurrentlyConsuming = false;
        this->ConsumeHandle.Unbind();
        if (this->Viewport.GetSurface()._GetNativeHandleDangerous())
        {
            this->Viewport.GetSurface().SetInputMode(EInputModeBits::ShowMouseCursor);
        }
    }
}

Jafg::LNodeReply Jafg::WWorldViewer::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
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
                        Comp->OnTrace(this->RenderTarget.GetExtent(), Location);
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
            if (!this->ConsumeHandle.IsValid())
            {
                LOG_TRACE(LogWidgetFramework, "[{}]: Entering user input capture.", this->GetNameAsString())
                check(!this->UserInput._bCurrentlyConsuming)
                this->UserInput._bCurrentlyConsuming = true;
                this->Viewport.GetSurface().SetInputMode(this->UserInput.IsConsumingMouse()
                    ? EInputModeBits::HideMouseCursor : EInputModeBits::ShowMouseCursor);

                if (!this->ConsumeHandle.IsValid())
                {
                    if (this->UserInput._bCurrentlyConsuming)
                    {
                        this->ConsumeHandle = LRaiiViewportHandle::Make(this->GetViewport().OnLateTick, [this]{ this->DispatchInputDelegates(); return false; });
                    }
                }

                return LNodeReply::Handled(false);
            }
        }
    }

    if (Event.Is<ERawInputStateBits::Press>(this->Viewport.GetSurface().GetFrontend().GetPhysicalKey(ELogicalKey::F1)))
    {
        if (Event.Mods & EModBits::Shift)
        {
            if (this->ConsumeHandle.IsValid())
            {
                LOG_TRACE(LogWidgetFramework, "[{}]: Escaping user input capture.", this->GetNameAsString())
                check(this->UserInput._bCurrentlyConsuming)
                this->UserInput._bCurrentlyConsuming = false;
                this->ConsumeHandle.Unbind();
                this->Viewport.GetSurface().SetInputMode(EInputModeBits::ShowMouseCursor);
                return LNodeReply::Handled();
            }
        }
    }

    return Super::OnKeyEventFocused(Info, Event);
}

void Jafg::WWorldViewer::TravelTo(LWorld& World)
{
    LOG_VERBOSE(LogWorld, "[{}]: Traveling to world [{}@{}]."
        , this->GetNameAsString(), World.GetHumanReadableName(), World.GetUnderlyingLevelName())

    if (auto Result{World.Login({LTransientPersona::Local{
        .Lackey=*this
        }})})
    {
        check(this->IsOwnedPersonaControllerValid())
    }
    else
    {
        LOG_ERROR(LogWidgetFramework,
            "[{}]: Failed to login to world [{}@{}]. Reason: {}",
            this->GetNameAsString(), World.GetHumanReadableName(), World.GetUnderlyingLevelName(), Result.error()
            )
    }

    check(this->Placeholder)
    this->Placeholder->SetVisibility(ENodeVisibility::Collapsed);

    if (this->Hierarchy)
    {
        this->Hierarchy->OnWorldViewerUpdate();
    }
    if (this->Inspector)
    {
        this->Inspector->OnWorldViewerUpdate();
    }
}

void Jafg::WWorldViewer::QueueTravelTo(LWorld& World)
{
    if (this->QueuedTravelWorld)
    {
        LOG_WARNING(LogWidgetFramework, "[{}]: Already have a queued world travel to. Aborting previous queued travel."
            , this->GetNameAsString())
        this->QueuedTravelWorld = nullptr;
        this->QueueHandle.Unbind();
    }

    if (World.GetWorldState() == EWorldState::Running)
    {
        this->TravelTo(World);
    }

    this->QueuedTravelWorld = &World;
    this->QueueHandle = LRaiiViewportHandle::Make(this->GetViewport().OnLateTick, [this]
    {
        check(this->QueuedTravelWorld)

        if (!this->GetEngine().IsWorldValid(this->QueuedTravelWorld))
        {
            LOG_WARNING(LogWidgetFramework, "[{}]: Queued world is no longer valid. Aborting travel.", this->GetNameAsString())
            this->QueuedTravelWorld = nullptr;
            return true;
        }

        if (this->QueuedTravelWorld->GetWorldState() == EWorldState::TearingDown || this->QueuedTravelWorld->GetWorldState() == EWorldState::WaitingForKill)
        {
            LOG_WARNING(LogWidgetFramework, "[{}]: Queued world is tearing down or waiting for kill. Aborting travel.", this->GetNameAsString())
            this->QueuedTravelWorld = nullptr;
            return true;
        }

        if (this->QueuedTravelWorld->GetWorldState() == EWorldState::Running)
        {
            this->TravelTo(*std::exchange(this->QueuedTravelWorld, nullptr));
            return true;
        }

        return false;
    });
}

void Jafg::WWorldViewer::OnPerspectiveDepthTestChanged()
{
    if (this->RenderTarget.IsInitialized())
    {
        auto& Prefs{GetSingleton<JUserPreferences>()};
        LOG_VERBOSE(LogWidgets, "Perspective depth test changed to [{}].", *Prefs.EditorPerspectiveDepthTestHint)
        this->bDatedRenderTarget = true;
    }
}

void Jafg::WWorldViewer::SelectActors(TArray<AActor*> Actors, bool bForce /* = false */)
{
    check(algo::all_of(Actors, [](auto* Actor){ return !!Actor; }))

    if (!bForce && this->SelectedActors == Actors)
    {
        return;
    }

    auto Old{std::exchange(this->SelectedActors, std::move(Actors))};
    LOG_TRACE(LogWidgetFramework, "[{}]: Selected [{}] actors.", this->GetNameAsString(), this->SelectedActors.size())
    this->OnActorsSelected.Broadcast(Old, this->SelectedActors);
}

void Jafg::WWorldViewer::InitializeRenderTarget()
{
    this->bDatedRenderTarget = false;

    this->RenderTarget.Initialize({
        .Frontend = this->GetViewport().GetSurface().GetFrontend(),
        .Extent = this->DesiredViewportExtent.has_value() ? *this->DesiredViewportExtent : rhi::extent2::from_vec(this->GetAnchoredSize_v2()),
        .SampleCount = this->GetViewport().GetSurface().GetFrontend().Vk_GetMaxMsaaSampleCount(),
        .ResolveMode = vk::ResolveModeFlagBits::eAverage,
        .ClearColor = LinearColors::DeepSkyBlue,
        .bDepthTest = *GetSingleton<JUserPreferences>().EditorPerspectiveDepthTestHint,
        .bAllowSelection = true,
        });

    if (!this->OnPreDrawHandle)
    {
        this->OnPreDrawHandle = LRaiiPreDrawHandle::Make(this->GetViewport().GetSurface().OnPreRender,
            std::bind(&WWorldViewer::OnPreDraw, this, std::placeholders::_1));
    }

    auto& MaterialSubsystem{*this->GetMutableFrontend().GetSubsystemChecked<JMaterialSubsystem>()};
    auto& Frontend{this->GetFrontend()};
    auto Binding = MaterialSubsystem.GetBinding(*this->PostSelectionMaterialInstance, "StencilTexture");
    vk::DescriptorImageInfo Info{
        .imageView = this->GetWorldRenderTarget().GetSelectedImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };
    check(Info.sampler == nullptr)
    auto It{algo::find(this->PostSelectionMaterialInstance->InfrequentDescriptorSets, Binding.Layout, [](auto const& E){ return E.first; })};
    check(It != this->PostSelectionMaterialInstance->InfrequentDescriptorSets.end())
    std::array Writes{
        vk::WriteDescriptorSet{
            .dstSet = *It->second,
            .dstBinding = Binding.Set,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eSampledImage,
            .pImageInfo = &Info,
            },
        };
    Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
}

bool Jafg::WWorldViewer::OnPreDraw(LRenderInfo const& Info)
{
    if (this->GetMostOuterParent().IsNodeInVisiblePath(*this))
    {
        // TODO: Move this to after the main draw?
        //       Currently we cannot depth test against the main draw. But if we do we may have the option
        //       To discard fragments if they failed to depth test against the main draw. Currently everything is just 1.0 in the depth
        //       Then we can also remove the depth image from this auxiliary render target.
        if (!this->SelectedActors.empty())
        {
            this->RenderTarget.RenderSelected(Info, std::bind(&WWorldViewer::PreDrawSelected, this, std::placeholders::_1));
        }

        this->RenderTarget.Render(Info, std::bind(&WWorldViewer::PreDraw, this, std::placeholders::_1));
    }
    return {};
}

void Jafg::WWorldViewer::PreDrawSelected(LRenderInfo const& Info)
{
    check(!this->SelectedActors.empty())

    auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
    auto& Pawn{*Ctrl.GetOwnedPawnChecked()};

    Pawn.GetWorld().Draw(Info, Pawn.GetEye(), &*this->SelectionMaterialInstance, this->SelectedActors);
}

void Jafg::WWorldViewer::PreDraw(LRenderInfo const& Info)
{
    if (this->IsOwnedPersonaControllerValid())
    {
        if (auto& Ctrl{*this->GetOwnedPersonaControllerChecked()}; Ctrl.IsOwnedPawnValid())
        {
            auto& Pawn{*Ctrl.GetOwnedPawnChecked()};
            Pawn.GetWorld().Draw(Info, Pawn.GetEye(), nullptr, {});

            if (!this->SelectedActors.empty())
            {
                check(this->PostSelectionMaterialInstance->FrequentDescriptorSets[Info.Frame].empty())
                check(this->PostSelectionMaterialInstance->InfrequentDescriptorSets.size() == 1)

                auto& ShaderSubsystem{*this->GetFrontend().GetSubsystemChecked<JShaderSubsystem>()};

                auto& FetchedMaterial{this->PostSelectionMaterialInstance->Material->FetchedMaterial};
                auto& Pipeline{this->PostSelectionMaterialInstance->Material->Pipeline};
                algo::for_each(this->PostSelectionMaterialInstance->InfrequentDescriptorSets, [&](auto& Set)
                {
                    auto const& [Idx, DescriptorSet] = Set;
                    Info.CommandBuffer.bindDescriptorSets2({
                        .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                        .layout = Pipeline.Layout,
                        .firstSet = 0,
                        .descriptorSetCount = 1,
                        .pDescriptorSets = &*DescriptorSet,
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
                    .layout = *Pipeline.Layout,
                    .stageFlags = PC::Outline::Flags(),
                    .offset = 0,
                    .size = sizeof(PC::Outline),
                    .pValues = &pc
                    });

                Info.CommandBuffer.draw(3, 1, 0, 0);
            }
        }
    }

    this->RenderTargetViewport.Draw(Info);
}

void Jafg::WWorldViewer::DispatchInputDelegates()
{
    check(this->IsOwnedPersonaControllerValid())

    if (algo::any_of(this->Viewport.GetSurface().GetUnconsumedInputs(), [this](LRawInput const& Input)
    {
        return Input.PhysicalKey == *this->Viewport.GetSurface().GetFrontend().GetPhysicalKey(ELogicalKey::F1);
    }))
    {
        JAFG_PLATFORM_NO_DISCARD_CTRL_PATH
    }

    auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
    this->UserInput._DispatchInputDelegates(Ctrl);
}

void Jafg::WWorldViewer::CreateMenuDropDown(LVec2F Where)
{
    struct LSharedNodes
    {
        WText* DisplayText{};
        WInput_Vector2* VectorInput{};
    };
    auto SharedNodes{std::make_shared<LSharedNodes>()};
    auto Res{std::make_shared<LVec2u64>(this->RenderTarget.GetExtent().width, this->RenderTarget.GetExtent().height)};
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
                                this->DesiredViewportExtent = WWorldViewer::DefaultExtent;
                            }

                            auto& Btn{Self.AsStatic<WCheckmarkButton>()};
                            Btn.SetChecked(!this->DesiredViewportExtent.has_value());

                            check(SharedNodes->DisplayText && SharedNodes->VectorInput)
                            LVec2u64 ShownVector{this->DesiredViewportExtent.has_value()
                                ? this->DesiredViewportExtent->ToVec<u64>()
                                : this->LastUnstableExtent.has_value()
                                    ? this->LastUnstableExtent->ToVec<u64>()
                                    : this->RenderTargetViewport.GetExtent().ToVec<u64>()};
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
                        this->DesiredViewportExtent = WWorldViewer::DefaultExtent;
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
                            if constexpr (IS_COMPILED_LOG(LogWidgets, Verbose))
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
                    for (auto& Track : Tracks)
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
                    WEditableTextButton* LevelName{};
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
                        + NewNode(Viewport).Class<WEditableTextButton>().SaveTo(&NewWorldNodes->LevelName)
                            .MinDesiredSize({128_spt, 0})
                            .Padding({2_spt, 0})
                            .Style(Prefs.EditorEditableTextButtonStyle<LBoxBrush>())
                            .TextStyle(Prefs.EditorEditableTextButtonTextStyle())
                            .Content(*Prefs.EditorLastWorldLevelName)
                            .PlaceholderContent("Level Name")
                    ];},});
                Result.emplace_back(LDropDownNodeOption{
                    .Selector = {
                        .DisplayName = "Create New World",
                        },
                    .OnAction = [this, NewWorldNodes](auto&&...)
                    {
                        check(NewWorldNodes.get() && NewWorldNodes->Name && NewWorldNodes->LevelName)
                        auto& Prefs{GetMutableSingleton<JUserPreferences>()};
                        Prefs.EditorLastWorldName = NewWorldNodes->Name->GetContent();
                        Prefs.EditorLastWorldLevelName = NewWorldNodes->LevelName->GetContent();
                        auto World{this->GetMutableEngine().SummonWorld({.HumanReadableName=NewWorldNodes->Name->GetContent()})};
                        // this->QueueTravelTo(World.Browse(NewWorldNodes->LevelName->GetContent()));
                        this->QueueTravelTo(World.Browse("LevelEditor"));
                        return algo::reply::unhandled();
                    },});
                return Result;
            },},},
        LDropDownNodeCustom{
            .OnCreate=[](LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)
            {
                auto& Prefs{GetSingleton<JUserPreferences>()};
                return NewNode(Viewport).Class<WSpacer>().Width(2_spt)
                + NewNode(Viewport).Class<WCheckmarkButton>()
                    .Anchor(EAnchor::CenterLeft)
                    .Checked(*Prefs.EditorAutoLaunchLastWorld)
                    .OnKeyEventFocused([](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                            && Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation)
                            )
                        {
                            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                            MutablePrefs.EditorAutoLaunchLastWorld = !*MutablePrefs.EditorAutoLaunchLastWorld;
                            Self.AsStatic<WCheckmarkButton>().SetChecked(*MutablePrefs.EditorAutoLaunchLastWorld);
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewNode(Viewport).Class<WText>()
                    .Anchor(EAnchor::HFill)
                    .Visibility(ENodeVisibility::TransitiveHitTestInvisible)
                    .Padding({6_spt, 0.0f, 0.0f, 0.0f})
                    .Content("Auto-Launch Last World");
                },
            .OnAction=[](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorAutoLaunchLastWorld = !*MutablePrefs.EditorAutoLaunchLastWorld;
                    return LDropDownNodeCustom::reply::handled(true);
                }
                return LDropDownNodeCustom::reply::unhandled();
            },},
        LDropDownNodeCustom{
            .OnCreate=[this](LViewport& Viewport, WDismissibleFloatingWidget& FloatingWidget)
            {
                auto& Prefs{GetSingleton<JUserPreferences>()};
                return NewNode(Viewport).Class<WSpacer>().Width(2_spt)
                + NewNode(Viewport).Class<WCheckmarkButton>()
                    .Anchor(EAnchor::CenterLeft)
                    .Checked(*Prefs.EditorPerspectiveDepthTestHint)
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                            && Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation)
                            )
                        {
                            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                            MutablePrefs.EditorPerspectiveDepthTestHint = !*MutablePrefs.EditorPerspectiveDepthTestHint;
                            Self.AsStatic<WCheckmarkButton>().SetChecked(*MutablePrefs.EditorPerspectiveDepthTestHint);
                            this->OnPerspectiveDepthTestChanged();
                            return LNodeReply::Handled();
                        }
                        return LNodeReply::Unhandled();
                    })
                + NewNode(Viewport).Class<WText>()
                    .Anchor(EAnchor::HFill)
                    .Visibility(ENodeVisibility::TransitiveHitTestInvisible)
                    .Padding({6_spt, 0.0f, 0.0f, 0.0f})
                    .Content("Depth test");
                },
            .OnAction=[this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                {
                    auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                    MutablePrefs.EditorPerspectiveDepthTestHint = !*MutablePrefs.EditorPerspectiveDepthTestHint;
                    this->OnPerspectiveDepthTestChanged();
                    return LDropDownNodeCustom::reply::handled(true);
                }
                return LDropDownNodeCustom::reply::unhandled();
            },},
        });

    return;
}

void Jafg::WWorldViewer::_ctor_SetBackgroundTint()
{
    this->BorderTint = *GetSingleton<JUserPreferences>().ViewportBackgroundTint;
}

void Jafg::WWorldViewerHierarchy::Construct()
{
    Super::Construct();

    check(!this->WorldViewer)
    this->WorldViewer = this->FindSmart();
    check(!this->WorldViewer->HasHierarchy())
    this->WorldViewer->Hierarchy = this;
    check(!this->OnActorsSelectedHandle.IsValid())
    this->OnActorsSelectedHandle = this->WorldViewer->OnActorsSelected.Emplace(this, &WWorldViewerHierarchy::OnActorsSelected);

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
                .MinDesiredSize(WWorldViewerHierarchy::TypeSize)
                .MaxDesiredSize(WWorldViewerHierarchy::TypeSize)
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

void Jafg::WWorldViewerHierarchy::Destruct()
{
    Super::Destruct();

    if (this->WorldViewer)
    {
        this->DisconnectFromViewer();
    }

    return;
}

void Jafg::WWorldViewerHierarchy::_OnWorldViewerDestruct()
{
    check(this->WorldViewer)
    check(this->WorldViewer->_IsGarbage())
    this->DisconnectFromViewer();
    this->OnWorldViewerUpdate();

    return;
}

void Jafg::WWorldViewerHierarchy::OnWorldViewerUpdate()
{
    this->UpdateConnectedArea();
    this->UpdateWorldObjectList({}, this->WorldViewer ? this->WorldViewer->GetSelectedActors() : TArray<AActor*>{});
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

Jafg::WWorldViewer* Jafg::WWorldViewerHierarchy::FindSmart() const noexcept
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

Jafg::WWorldViewer* Jafg::WWorldViewerHierarchy::FindInViewport(LViewport const& Viewport) const noexcept
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

Jafg::WWorldViewer* Jafg::WWorldViewerHierarchy::FindInNode(WNode& Node) const noexcept
{
    if (auto* Casted{Node.As<WWorldViewer>()})
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

void Jafg::WWorldViewerHierarchy::DisconnectFromViewer()
{
    check(this->WorldViewer)
    check(this->WorldViewer->Hierarchy == this)
    this->WorldViewer->OnActorsSelected.Remove(&this->OnActorsSelectedHandle);
    this->WorldViewer->Hierarchy = nullptr;
    this->WorldViewer = nullptr;

    return;
}

bool Jafg::WWorldViewerHierarchy::OnActorsSelected(TArray<AActor*> const& Old, TArray<AActor*> const& New)
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

void Jafg::WWorldViewerHierarchy::UpdateConnectedArea()
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

void Jafg::WWorldViewerHierarchy::UpdateWorldObjectList(TArray<AActor*> const& Old, TArray<AActor*> const& New)
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

            this->Container->AddChild(NewStaticNode(Detail::WWorldViewerHierarchyObjectHButton).Actor(Actor)
                .Anchor(EAnchor::HFill)
                .Style(Prefs.EditorProximityBoxStyle2<LRegionBrush>(Counter++))
                .Padding(ListPadding)
                .Selectable(true)
                .Selected(algo::contains(New, Actor))
                .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    return this->OnWorldObjectListKeyEventFocus(Self.AsStatic<Detail::WWorldViewerHierarchyObjectHButton>(), Info, Event);
                })
                .OnKeyEventUnfocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                {
                    return this->OnWorldObjectListKeyEventUnfocus(Self.AsStatic<Detail::WWorldViewerHierarchyObjectHButton>(), Info, Event);
                })
                [
                    NewStaticNode(WText).Content(Actor->GetEditorNameOrDefault())
                        .Anchor(EAnchor::HFill)
                    +
                    NewStaticNode(WText).Content(Actor->GetNameAsString())
                        .MinDesiredSize(WWorldViewerHierarchy::TypeSize)
                        .MaxDesiredSize(WWorldViewerHierarchy::TypeSize)
                        .TextTint(Colors::Gray)
                ]
                .Unique());
            }
    }

    return;
}

void Jafg::WWorldViewerHierarchy::UpdateSelectedWorldObjectText(std::size_t Count, std::size_t Shown, std::size_t Selected)
{
    check(this->SelectedWorldObjectText)
    this->SelectedWorldObjectText->SetContent(algo::sprintf("{}/{} objects ({} selected)", Shown, Count, Selected));
    return;
}

Jafg::LNodeReply Jafg::WWorldViewerHierarchy::OnWorldObjectListKeyEventFocus(Detail::WWorldViewerHierarchyObjectHButton& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
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
                    auto& CastedChild{Child->AsStatic<Detail::WWorldViewerHierarchyObjectHButton>()};
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
                        if (!algo::contains(Current, Actor))
                        {
                            Current.emplace_back(Actor);
                        }
                    }
                    this->WorldViewer->SelectActors(std::move(Current));
                }
            }
            else if (Event.Mods & EModBits::Control)
            {
                if (algo::contains(this->WorldViewer->GetSelectedActors(), Self.Actor))
                {
                    this->WorldViewer->SelectActors(this->WorldViewer->GetSelectedActors()
                        | algo::views::filter([Actor = Self.Actor](auto* E){ return E != Actor; })
                        | algo::to_array_fn{this->WorldViewer->GetSelectedActors().size()}
                        );
                }
                else
                {
                    auto Current{this->WorldViewer->GetSelectedActors()};
                    Current.emplace_back(Self.Actor);
                    this->WorldViewer->SelectActors(std::move(Current));
                }
                this->LastContainerElemSelected = Self.Actor;
            }
            else
            {
                this->WorldViewer->SelectActors({Self.Actor});
                this->LastContainerElemSelected = Self.Actor;
            }

            return LNodeReply::Handled();
        }
    }

    return  {};
}

Jafg::LNodeReply Jafg::WWorldViewerHierarchy::OnWorldObjectListKeyEventUnfocus(Detail::WWorldViewerHierarchyObjectHButton& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    check(Self.Actor)

    if (Info.CursorLocation && this->AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
    {
        if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
        {
            check(this->WorldViewer)
            if (algo::contains(this->WorldViewer->GetSelectedActors(), Self.Actor))
            {
                this->WorldViewer->SelectActors(this->WorldViewer->GetSelectedActors()
                    | algo::views::filter([Actor = Self.Actor](auto* E){ return E != Actor; })
                    | algo::to_array_fn{this->WorldViewer->GetSelectedActors().size()}
                    );
            }
            else
            {
                auto Current{this->WorldViewer->GetSelectedActors()};
                Current.emplace_back(Self.Actor);
                this->WorldViewer->SelectActors(std::move(Current));
            }

            this->LastContainerElemSelected = Self.Actor;
            return LNodeReply::Handled();
        }
    }

    return {};
}

void Jafg::WWorldViewerInspector::Construct()
{
    Super::Construct();

    check(!this->WorldViewer)
    this->WorldViewer = this->FindSmart();
    check(!this->WorldViewer->HasInspector())
    this->WorldViewer->Inspector = this;
    check(!this->OnActorsSelectedHandle.IsValid())
    this->OnActorsSelectedHandle = this->WorldViewer->OnActorsSelected.Emplace(this, &WWorldViewerInspector::OnActorsSelected);

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
                            auto& Actor{*Actors.front()};
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

void Jafg::WWorldViewerInspector::Destruct()
{
    Super::Destruct();

    if (this->WorldViewer)
    {
        this->DisconnectFromViewer();
    }
}

void Jafg::WWorldViewerInspector::_OnWorldViewerDestruct()
{
    check(this->WorldViewer)
    check(this->WorldViewer->_IsGarbage())
    this->DisconnectFromViewer();
    this->OnWorldViewerUpdate();
}

void Jafg::WWorldViewerInspector::OnWorldViewerUpdate()
{
    this->UpdateConnectedArea();
    this->UpdateObjectDisplayName();
    this->UpdateObjectDetails();
}

Jafg::WWorldViewer* Jafg::WWorldViewerInspector::FindSmart() const noexcept
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

Jafg::WWorldViewer* Jafg::WWorldViewerInspector::FindInViewport(LViewport const& Viewport) const noexcept
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

Jafg::WWorldViewer* Jafg::WWorldViewerInspector::FindInNode(WNode& Node) const noexcept
{
    if (auto* Casted{Node.As<WWorldViewer>()})
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

void Jafg::WWorldViewerInspector::DisconnectFromViewer()
{
    check(this->WorldViewer)

    check(this->WorldViewer->Inspector == this)
    this->WorldViewer->OnActorsSelected.Remove(&this->OnActorsSelectedHandle);
    this->WorldViewer->Inspector = nullptr;
    this->WorldViewer = nullptr;
}

bool Jafg::WWorldViewerInspector::OnActorsSelected(TArray<AActor*> const& Old, TArray<AActor*> const& New)
{
    this->UpdateObjectDisplayName();
    this->UpdateObjectDetails();

    return {};
}

void Jafg::WWorldViewerInspector::UpdateConnectedArea()
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

void Jafg::WWorldViewerInspector::UpdateObjectDisplayName()
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
            auto& Actor{*Actors.front()};
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

void Jafg::WWorldViewerInspector::UpdateObjectDetails()
{
    check(this->Container && this->ContainerSearch)

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
        auto& Actor{*this->WorldViewer->GetSelectedActors().front()};

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

        if (Actor.HasRootComponent())
        {
            this->SelectComponent(&Actor.GetRootComponent());
        }
        else
        {
            check(this->SelectedComponent == nullptr)
            this->ReloadInnerComponents();
        }
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

void Jafg::WWorldViewerInspector::ReloadInnerComponents()
{
    check(IsValidFast(this->GetOuter(), this->ComponentContainerWrapper))
    check(this->WorldViewer)
    check(this->WorldViewer->GetSelectedActors().size() == 1)
    auto& Actor{*this->WorldViewer->GetSelectedActors().front()};
    auto& Prefs{GetSingleton<JUserPreferences>()};

    this->ComponentContainerWrapper->RemoveChildren();

    auto Counter{0uz};
    for (auto& Component: Actor.GetComponents())
    {
        this->ComponentContainerWrapper->AddChild(NewStaticNode(WTextButtonIconizedDouble)
            .Anchor(EAnchor::HFill)
            .LeftIcon("Icons/Jafg.Box")
            .Style(Prefs.EditorProximityBoxStyle2<LBoxBrush>(Counter++))
            .Selectable(true)
            .Selected(&*Component == this->SelectedComponent)
            .Content(Component->GetEditorNameOrDefault())
            .OnKeyEventFocused([this, Component = &*Component](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
            {
                if (Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
                {
                    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
                    {
                        this->SelectComponent(Component);
                        return LNodeReply::Handled();
                    }
                }
                return LNodeReply::Unhandled();
            })
            .Unique()
            );
    }
}

void Jafg::WWorldViewerInspector::SelectComponent(AActorComponent* Component)
{
    check(IsValidFast(this->GetOuter(), this->ComponentContainer))

    if (Component == this->SelectedComponent)
    {
        return;
    }

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
                Array.emplace_back(It->EditorFactory(this->GetViewport(), *this->SelectedComponent).UniqueXs(&Xs));
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
