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
            .InAllBrushesChained<&LRegionBrush::Tint, &LRegionBrush::Background>
                (Colors::White, LRegionBrush::Icon("Icons/Jafg.Menu"))
            .InAllBrushesChained<&LRegionBrush::Radii>(LVec4F{50.0f})
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

    auto& Prefs{GetSingleton<JUserPreferences>()};
    if (*Prefs.EditorAutoLaunchLastWorld && !Prefs.EditorLastWorldName->empty() && !Prefs.EditorLastWorldLevelName->empty())
    {
        auto World{this->GetMutableEngine().SummonWorld({.HumanReadableName=*Prefs.EditorLastWorldName})};
        this->QueueTravelTo(World.Browse(*Prefs.EditorLastWorldLevelName));
    }

    return;
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

    return;
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
        Info.AddInstance({
            .Rect = {.Offset=this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Translation),
                     .Extent=this->GetAnchoredSize_v2()},
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

    return;
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

    return;
}

Jafg::LNodeReply Jafg::WWorldViewer::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
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
                return LNodeReply::Handled();
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

    return;
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

    return;
}

void Jafg::WWorldViewer::OnPerspectiveDepthTestChanged()
{
    if (this->RenderTarget.IsInitialized())
    {
        auto& Prefs{GetSingleton<JUserPreferences>()};
        LOG_VERBOSE(LogWidgets, "Perspective depth test changed to [{}].", *Prefs.PerspectiveDepthTest)
        this->bDatedRenderTarget = true;
    }

    return;
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
        .bDepthTest = *GetSingleton<JUserPreferences>().PerspectiveDepthTest,
        });

    if (!this->OnPreDrawHandle)
    {
        this->OnPreDrawHandle = LRaiiPreDrawHandle::Make(this->GetViewport().GetSurface().OnPreRender,
        [this](LRenderInfo const& Info)
        {
            if (this->GetMostOuterParent().IsNodeInVisiblePath(*this))
            {
                this->RenderTarget.Render(Info, std::bind(&WWorldViewer::PreDraw, this, std::placeholders::_1));
            }
            return false;
        });
    }

    return;
}

bool Jafg::WWorldViewer::OnPreDraw(LRenderInfo const& Info)
{
    if (this->GetMostOuterParent().IsNodeInVisiblePath(*this))
    {
        this->RenderTarget.Render(Info, std::bind(&WWorldViewer::PreDraw, this, std::placeholders::_1));
    }
    return {};
}

void Jafg::WWorldViewer::PreDraw(LRenderInfo const& Info)
{
    if (this->IsOwnedPersonaControllerValid())
    {
        if (auto& Ctrl{*this->GetOwnedPersonaControllerChecked()}; Ctrl.IsOwnedPawnValid())
        {
            auto& Pawn{*Ctrl.GetOwnedPawnChecked()};
            Pawn.GetWorld().Draw(Info, Pawn.GetEye());
        }
    }

    this->RenderTargetViewport.Draw(Info);

    return;
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
    this->UserInput.DispatchInputDelegates(Ctrl);

    return;
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
                            .Style(Prefs.EditorEditableTextButtonStyle())
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
                            .Style(Prefs.EditorEditableTextButtonStyle())
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
                        this->QueueTravelTo(World.Browse(NewWorldNodes->LevelName->GetContent()));
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
                    .Checked(*Prefs.PerspectiveDepthTest)
                    .OnKeyEventFocused([this](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                    {
                        if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton))
                            && Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation)
                            )
                        {
                            auto& MutablePrefs{GetMutableSingleton<JUserPreferences>()};
                            MutablePrefs.PerspectiveDepthTest = !*MutablePrefs.PerspectiveDepthTest;
                            Self.AsStatic<WCheckmarkButton>().SetChecked(*MutablePrefs.PerspectiveDepthTest);
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
                    MutablePrefs.PerspectiveDepthTest = !*MutablePrefs.PerspectiveDepthTest;
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
