// Copyright mzoesch. All rights reserved.

#include "Widgets/WorldViewer.h"
#include "Nodes/Text.h"
#include "Rhi/NodeRenderInfo.h"
#include "User/UserPreferences.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/LightSubsystem.h"

Jafg::WWorldViewer::~WWorldViewer()
{
    this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
}

void Jafg::WWorldViewer::Construct()
{
    Super::Construct();

    this->PolygonMode = *GetSingleton<JUserPreferences>().PolygonMode;

    this->RenderTargetViewport.Vk_OnLateInit();
    BeginStyling(*this).StaticRoot<WText>().SaveTo(&this->Placeholder)
        .Visibility(ENodeVisibility::Visible)
        .Anchor(EAnchor::Fill)
        .TextScale(ETextScale::Header)
        .TextAlign(ETextHAlign::Center)
        .TextAlign(ETextVAlign::Center)
        .Content("No World Loaded")
        ;

    LOG_VERBOSE(LogWidgets, "[{}]: Allocating world descriptor sets and buffers.", this->GetNameAsString())

    auto& Frontend{this->GetFrontend()};
    auto& MaterialSubsystem{*Frontend.GetSubsystemChecked<JMaterialSubsystem>()};

    this->WorldBuffers = Frontend.Vk_CreateFrequentMappedBuffer(UBO::WorldData::buffer_create_info());
    check(algo::all_of(this->WorldBuffers, [](auto const& Buffer)
    {
        return !!Buffer.data();
    }))
    this->PbrLightBuffers = Frontend.Vk_CreateFrequentMappedBuffer(SSBO::BindlessPbrLight::buffer_create_info(this->PbrLightLimit));
    check(algo::all_of(this->PbrLightBuffers, [](auto const& Buffer)
    {
        return !!Buffer.data();
    }))
    //# Enough for an atlas that is filled with cube faces only, the smallest tiles there are.
    check(this->ShadowSettings.TileExtent > 0 && this->ShadowSettings.AtlasExtent >= this->ShadowSettings.TileExtent)
    check(this->ShadowSettings.CubeFaceExtent > 0 && this->ShadowSettings.TileExtent >= this->ShadowSettings.CubeFaceExtent)
    this->PbrShadowViewLimit = LShadowAtlas::GetMaxViews(this->ShadowSettings);
    this->PbrShadowViewBuffers = Frontend.Vk_CreateFrequentMappedBuffer(SSBO::PbrShadowView::buffer_create_info(this->PbrShadowViewLimit));
    check(algo::all_of(this->PbrShadowViewBuffers, [](auto const& Buffer)
    {
        return !!Buffer.data();
    }))

    TArray<vk::DescriptorSetLayout> FrequentLayouts;
    auto AppendFrequentLayout{[&](vk::DescriptorSetLayout Layout)
    {
        for (auto Frame{0uz}; Frame < Frontend.Vk_GetNumberOfFramesInFlight(); ++Frame)
        {
            FrequentLayouts.emplace_back(Layout);
        }
    }};
    AppendFrequentLayout(MaterialSubsystem.GetSharedDescriptorSetLayout<UBO::WorldData>());
    AppendFrequentLayout(MaterialSubsystem.GetSharedDescriptorSetLayout<SSBO::BindlessPbrLight>());

    std::vector Sets{rhi::vk_allocate(Frontend.Vk_GetDevice(), vk::DescriptorSetAllocateInfo{
        .descriptorPool = Frontend.Vk_GetDescriptorPool(),
        .descriptorSetCount = static_cast<u32>(FrequentLayouts.size()),
        .pSetLayouts = FrequentLayouts.data(),
        })};

    auto Idx{0uz};
    auto RetrieveFrequentSet{[&](auto&& Target)
    {
        for (auto Frame{0uz}; Frame < Frontend.Vk_GetNumberOfFramesInFlight(); ++Frame)
        {
            check(Idx < Sets.size())
            Target[Frame] = std::move(Sets[Idx++]);
        }
    }};
    RetrieveFrequentSet(this->WorldDescriptorSets);
    RetrieveFrequentSet(this->PbrLightDescriptorSets);

    check(algo::all_of(Sets, [](auto const& Set)
    {
        return !*Set;
    }))

    auto& MutableMaterialSubsystem{*this->GetMutableFrontend().GetSubsystemChecked<JMaterialSubsystem>()};
    this->SceneDepthMaterialInstance = MutableMaterialSubsystem.GetInstanceFromName(JafgInfo, "Materials/Mesh.Depth");
    check(this->SceneDepthMaterialInstance.get())
    this->AmbientOcclusionMaterialInstance = MutableMaterialSubsystem.GetInstanceFromName(JafgInfo, "Materials/Ssao");
    check(this->AmbientOcclusionMaterialInstance.get())
    this->AmbientOcclusionBlurMaterialInstance = MutableMaterialSubsystem.GetInstanceFromName(JafgInfo, "Materials/SsaoBlur");
    check(this->AmbientOcclusionBlurMaterialInstance.get())
    this->ShadowMaterialInstance = MutableMaterialSubsystem.GetInstanceFromName(JafgInfo, "Materials/Mesh.Shadow");
    check(this->ShadowMaterialInstance.get())
}

void Jafg::WWorldViewer::Tick(f32 Dt)
{
    Super::Tick(Dt);

    auto& Surface{this->GetViewport().GetSurface()};

    if (this->bDatedRenderTarget)
    {
        LOG_VERBOSE(LogWidgets, "[{}]: Render target is dated. Recreating render target.", this->GetNameAsString())
        Surface.GetMutableFrontend()._Vk_WaitIdle();
        this->InitializeRenderTarget();
    }
    else if (this->DesiredViewportExtent)
    {
        if (*this->DesiredViewportExtent != this->RenderTarget.GetExtent())
        {
            LOG_TRACE(LogWidgets, "[{}]: Desired viewport extent changed to [{}]. Recreating render target.", this->GetNameAsString(), *this->DesiredViewportExtent)
            Surface.GetMutableFrontend()._Vk_WaitIdle();
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
                Surface.GetMutableFrontend()._Vk_WaitIdle();
                this->InitializeRenderTarget();
                this->LastUnstableExtent.reset();
            }
        }
    }

    if (this->UserInput._bCurrentlyConsuming && !this->ConsumeHandle.IsValid())
    {
        this->BindConsumeHandle();
        check(this->ConsumeHandle.IsValid())
    }

    this->RenderTargetViewport.Tick(Dt);
}

void Jafg::WWorldViewer::Draw(LNodeRenderInfo const& Info) const
{
    if (this->RenderTarget.IsInitialized())
    {
        check(!!*this->RenderTarget.GetMsaa().Image)
        if (!this->RenderTarget.IsResolvedBindless())
        {
            this->RenderTarget._SetResolvedBindlessIndex(
                Info.Frontend.GetSubsystemChecked<JTextureSubsystem>()->_Vk_AddTransientImageToGlobalBindlessArray(this->RenderTarget.GetResolved().ImageView)
                .value_or(INDEX_NONE)
                );
            if (!this->RenderTarget.IsResolvedBindless())
            {
                LOG_FATAL(LogVulkan, "[{}]: Failed to make render target texture bindless. Out of binding points."
                    , this->GetNameAsString())
            }
        }

        LRect2F Rect{this->GetWorldTargetRectFromMostOuter(Info.Translation)};

        Info.AddInstance({
            .Rect = Rect,
            .TexCoordRect = {rhi::uv::fit(rhi::uv::identity
                , this->RenderTarget.GetExtent().ToVec<f32>(), this->GetAnchoredSize_v2()
                , rhi::tex_coord_behavior::FitAspect)},
            .Tint = Colors::White,
            .BorderTint = this->BorderTint,
            .TextureIndex = this->RenderTarget.GetResolvedBindlessIndex(),
            .SamplerIndex = UBO::Bindless::LinearClampToBorderSamplerIdx,
            .Flags = SSBO::EVisualInstanceFlagBits::IgnoreAlpha,
            });
    }

    Super::Draw(Info);
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
        check(!this->ConsumeHandle.IsValid())
        if (this->GetViewport().GetSurface()._GetNativeHandleDangerous())
        {
            this->GetViewport().GetSurface().SetInputMode(EInputModeBits::ShowMouseCursor);
        }
    }
    check(!this->ConsumeHandle.IsValid())
}

LRect2F Jafg::WWorldViewer::GetWorldTargetRectFromMostOuter() const noexcept
{
    return this->GetWorldTargetRectFromMostOuter(this->GetTranslationFromMostOuter());
}

LRect2F Jafg::WWorldViewer::GetWorldTargetRectFromMostOuter(LVec2F Translation) const noexcept
{
    LRect2F Rect;
    if (this->IsManual())
    {
        Rect = {.offset=this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation),
                .extent=this->GetAnchoredSize_v2()};
    }
    else
    {
        Rect = {.offset=maths::round(this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Translation)),
                .extent=this->RenderTarget.GetExtent().ToVec<f32>()};
    }
    return Rect;
}

void Jafg::WWorldViewer::TravelTo(LWorld& World)
{
    if (auto* Ctrl{this->GetOwnedPersonaController()})
    {
        if (&Ctrl->GetWorld() == &World)
        {
            LOG_VERBOSE(LogWorld, "[{}]: Already in world [{}]. Rejecting travel request."
                , this->GetNameAsString(), World.GetDetailedHumanReadableName())
            return;
        }

        this->Disconnect();
        check(!this->IsOwnedPersonaControllerValid())
    }

    LOG_VERBOSE(LogWorld, "[{}]: Traveling to world [{}]."
        , this->GetNameAsString(), World.GetDetailedHumanReadableName())

    if (auto Result{World.Login({this->GetTransientPersona()})})
    {
        check(this->IsOwnedPersonaControllerValid())
    }
    else
    {
        LOG_FATAL(LogWidgetFramework, "[{}]: Failed to login to world [{}]. Reason: {}"
            , this->GetNameAsString() , World.GetDetailedHumanReadableName(), Result.error())
    }

    check(this->Placeholder)
    this->Placeholder->SetVisibility(ENodeVisibility::Collapsed);

    this->OnConnect();
}

void Jafg::WWorldViewer::Disconnect()
{
    if (auto* Ctrl{this->GetOwnedPersonaController()})
    {
        LOG_VERBOSE(LogWidgetFramework, "[{}]: Disconnecting from world [{}]."
            , this->GetNameAsString(), Ctrl->GetWorld().GetHumanReadableName())

        this->OnDisconnect();

        if (!this->GetOwnedPersonaController())
        {
            check(this->Placeholder)
            this->Placeholder->SetVisibility(ENodeVisibility::Visible);
        }
    }
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

void Jafg::WWorldViewer::InitializeRenderTarget()
{
    this->bDatedRenderTarget = false;

    this->RenderTarget.Initialize({
        .Frontend = this->GetViewport().GetSurface().GetFrontend(),
        .Extent = this->DesiredViewportExtent.has_value() ? *this->DesiredViewportExtent : rhi::extent2::from_vec(this->GetAnchoredSize_v2()),
        .SampleCount = this->GetViewport().GetSurface().GetFrontend().Vk_GetMaxMsaaSampleCount(),
        .ResolveMode = vk::ResolveModeFlagBits::eAverage,
        .ClearColor = LinearColors::CornflowerBlue,
        .bDepthTest = *GetSingleton<JUserPreferences>().EditorPerspectiveDepthTestHint,
        .bAllowSelection = true,
        .bAmbientOcclusion = true,
        });
    this->InitializeAmbientOcclusion();

    if (!this->OnPreDrawHandle)
    {
        this->OnPreDrawHandle = LRaiiPreDrawHandle::Make(this->GetViewport().GetSurface().OnPreRender,
            std::bind(&WWorldViewer::OnPreDraw, this, std::placeholders::_1));
    }
}

void Jafg::WWorldViewer::InitializeAmbientOcclusion()
{
    check(this->RenderTarget.HasAmbientOcclusion())

    auto& Frontend{this->GetViewport().GetSurface().GetMutableFrontend()};
    auto& TextureSubsystem{*Frontend.GetSubsystemChecked<JTextureSubsystem>()};

    auto WriteTexture{[&](LMaterialInstance& Instance, LStringView Name, vk::ImageView View)
    {
        auto Binding{Instance.GetBinding(Name)};
        vk::DescriptorImageInfo Info{
            .imageView = View,
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
            };
        auto It{algo::find(Instance.InfrequentDescriptorSets, Binding.space, &LMaterialInstance::DescriptorSetInstance::Space)};
        check(It != Instance.InfrequentDescriptorSets.end())
        std::array Writes{vk::WriteDescriptorSet{
            .dstSet = **It,
            .dstBinding = Binding.index,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eSampledImage,
            .pImageInfo = &Info,
            },};
        Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
    }};
    WriteTexture(*this->AmbientOcclusionMaterialInstance, "depth_texture", this->RenderTarget.GetSceneDepthImageView());
    WriteTexture(*this->AmbientOcclusionBlurMaterialInstance, "occlusion_texture", this->RenderTarget.GetAmbientOcclusionImageView());
    WriteTexture(*this->AmbientOcclusionBlurMaterialInstance, "depth_texture", this->RenderTarget.GetSceneDepthImageView());

    //# The scene samples the blurred occlusion through the global bindless array.
    if (this->AmbientOcclusionBindlessIndex == rhi::invalid_resource_index)
    {
        this->AmbientOcclusionBindlessIndex = TextureSubsystem
            ._Vk_AddTransientImageToGlobalBindlessArray(this->RenderTarget.GetAmbientOcclusionBlurImageView())
            .value_or(rhi::invalid_resource_index);
        if (this->AmbientOcclusionBindlessIndex == rhi::invalid_resource_index)
        {
            LOG_WARNING(LogVulkan, "[{}]: Failed to make ambient occlusion bindless. Out of binding points. Disabling it."
                , this->GetNameAsString())
        }
    }
    else
    {
        //# Reuse our slot, as the old image view is gone.
        vk::DescriptorImageInfo Info{
            .imageView = this->RenderTarget.GetAmbientOcclusionBlurImageView(),
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
            };
        std::array Writes{vk::WriteDescriptorSet{
            .dstSet = *TextureSubsystem.Vk_GetBindlessTextureArrayDescriptorSet(),
            .dstBinding = UBO::Bindless::ArrayBinding,
            .dstArrayElement = this->AmbientOcclusionBindlessIndex,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eSampledImage,
            .pImageInfo = &Info,
            },};
        Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
    }
}

bool Jafg::WWorldViewer::ShouldDrawAmbientOcclusion() const noexcept
{
    if (!this->bAmbientOcclusion || !this->RenderTarget.HasAmbientOcclusion()
        || this->AmbientOcclusionBindlessIndex == rhi::invalid_resource_index)
    {
        return false;
    }
    //# The depth of a wireframe is of no use. And there is no point in drawing it if it is not shown.
    if (this->PolygonMode != EPolygonMode::Fill || !(this->WorldRenderPreferences & EWorldRenderBits::ShowSsao))
    {
        return false;
    }
    return this->IsOwnedPersonaControllerValid() && this->GetOwnedPersonaControllerChecked()->IsOwnedPawnValid();
}

void Jafg::WWorldViewer::DrawAmbientOcclusion(LRenderInfo const& Info)
{
    auto& Pawn{*this->GetOwnedPersonaControllerChecked()->GetOwnedPawnChecked()};
    auto Eye{Pawn.GetEye()};

    auto DrawFullscreen{[](LRenderInfo const& Info, LMaterialInstance const& Instance, auto const& PushConstants)
    {
        auto& Pipeline{Instance.Material->Pipeline};
        for (auto& Set: Instance.InfrequentDescriptorSets)
        {
            Info.CommandBuffer.bindDescriptorSets2({
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .layout = *Pipeline.pipeline_layout,
                .firstSet = Set.Space,
                .descriptorSetCount = 1,
                .pDescriptorSets = &**Set,
                .dynamicOffsetCount = 0,
                .pDynamicOffsets = nullptr
                });
        }
        Info.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *Pipeline);
        Info.CommandBuffer.setPolygonModeEXT(vk::PolygonMode::eFill);
        PushConstants.upload(Info.CommandBuffer, Pipeline);
        Info.CommandBuffer.draw(3, 1, 0, 0);
    }};

    this->RenderTarget.RenderSceneDepth(Info, [&](LRenderInfo const& Info)
    {
        std::optional<TArray<AActor*>> Filter;
        Pawn.GetWorld().Draw(Info, {
            .Eye = Eye,
            .Instance = &*this->SceneDepthMaterialInstance,
            .Filter = Filter,
            .bUpdateWorldDataSet = false,
            });
    });

    LVec2F Extent{this->RenderTarget.GetExtent().ToVec<f32>()};
    f32 TanHalfFov{static_cast<f32>(maths::tan(Eye.vert_fov * 0.5f))};

    this->RenderTarget.RenderAmbientOcclusion(Info, [&](LRenderInfo const& Info)
    {
        DrawFullscreen(Info, *this->AmbientOcclusionMaterialInstance, PC::Ssao{
            .ViewportExtent = Extent,
            .TanHalfFov = {TanHalfFov * Extent.x / Extent.y, TanHalfFov},
            .Radius = this->AmbientOcclusion.Radius,
            .Bias = this->AmbientOcclusion.Bias,
            .Intensity = this->AmbientOcclusion.Intensity,
            .Power = this->AmbientOcclusion.Power,
            });
    });
    this->RenderTarget.RenderAmbientOcclusionBlur(Info, [&](LRenderInfo const& Info)
    {
        DrawFullscreen(Info, *this->AmbientOcclusionBlurMaterialInstance, PC::SsaoBlur{
            .ViewportExtent = Extent,
            .DepthThreshold = this->AmbientOcclusion.BlurDepthThreshold,
            });
    });
}

bool Jafg::WWorldViewer::ShouldDrawShadows() const noexcept
{
    if (!this->bShadows || this->bShadowAtlasFailed)
    {
        return false;
    }
    //# Wireframes are unlit. And there is no point in drawing them if they are not shown.
    if (this->PolygonMode != EPolygonMode::Fill || !(this->WorldRenderPreferences & EWorldRenderBits::ShowShadows))
    {
        return false;
    }
    return this->IsOwnedPersonaControllerValid() && this->GetOwnedPersonaControllerChecked()->IsOwnedPawnValid();
}

bool Jafg::WWorldViewer::InitializeShadowAtlas()
{
    check(!this->ShadowAtlas.IsInitialized())
    check(!this->bShadowAtlasFailed)

    auto& Frontend{this->GetViewport().GetSurface().GetMutableFrontend()};
    auto& TextureSubsystem{*Frontend.GetSubsystemChecked<JTextureSubsystem>()};

    LOG_VERBOSE(LogWidgets, "[{}]: Allocating shadow atlas of [{}x{}] texels.", this->GetNameAsString()
        , this->ShadowSettings.AtlasExtent, this->ShadowSettings.AtlasExtent)

    this->ShadowAtlas.Initialize({
        .Frontend = Frontend,
        .Settings = this->ShadowSettings,
        });
    //# The views are uploaded into buffers that were allocated for the settings during construction.
    check(LShadowAtlas::GetMaxViews(this->ShadowSettings) <= this->PbrShadowViewLimit)

    //# The scene samples the atlas through the global bindless array. The slot stays ours for our whole lifetime.
    if (auto Index{TextureSubsystem._Vk_AddTransientImageToGlobalBindlessArray(*this->ShadowAtlas.GetImageView())}; Index)
    {
        this->ShadowAtlas.SetBindlessIndex(*Index);
        return true;
    }

    LOG_WARNING(LogVulkan, "[{}]: Failed to make shadow atlas bindless. Out of binding points. Disabling shadows."
        , this->GetNameAsString())
    this->bShadowAtlasFailed = true;
    return false;
}

bool Jafg::WWorldViewer::DrawShadows(LRenderInfo const& Info)
{
    auto& Pawn{*this->GetOwnedPersonaControllerChecked()->GetOwnedPawnChecked()};
    auto& World{Pawn.GetWorld()};
    auto const& Lights{World.GetSubsystemChecked<JLightSubsystem>()->GetLights()};

    //# Most worlds have no shadow casting light at all. Do not allocate the atlas for them.
    if (!this->ShadowAtlas.IsInitialized())
    {
        bool const bAnyCaster{algo::any_of(Lights, [](LLight const* Light)
        {
            return Light->bCastShadows && LShadowAtlas::SupportsShadows(Light->Type);
        })};
        if (!bAnyCaster || !this->InitializeShadowAtlas())
        {
            return false;
        }
    }

    //# The cascades of directional lights are fit around the camera of this viewer.
    auto const Eye{Pawn.GetEye()};
    auto const Extent{this->RenderTarget.GetExtent().ToVec<f32>()};
    this->ShadowAtlas.Gather(Lights, this->ShadowSettings, {
        .Eye = Eye,
        .Aspect = Extent.y > 0.0f ? Extent.x / Extent.y : 1.0f,
        });
    if (!this->ShadowAtlas.HasViews())
    {
        return false;
    }

    this->ShadowAtlas.Render(Info, [&](LRenderInfo const& Info, LShadowAtlas::View const& View)
    {
        std::optional<TArray<AActor*>> Filter;
        World.Draw(Info, {
            .Eye = View.Eye,
            .ViewOverride = LWorld::LViewOverride{
                .View = View.ViewMatrix,
                .Proj = View.ProjMatrix,
                },
            .Instance = &*this->ShadowMaterialInstance,
            .Filter = Filter,
            .bUpdateWorldDataSet = false,
            });
    });

    return true;
}

bool Jafg::WWorldViewer::OnPreDrawImpl(LRenderInfo const& Info)
{
    //# Shadow maps first. They do not depend on any other pass.
    this->bShadowsDrawn = this->ShouldDrawShadows() && this->DrawShadows(Info);

    this->bAmbientOcclusionDrawn = this->ShouldDrawAmbientOcclusion();
    if (this->bAmbientOcclusionDrawn)
    {
        this->DrawAmbientOcclusion(Info);
    }

    this->RenderTarget.Render(Info, std::bind(&WWorldViewer::PreDraw, this, std::placeholders::_1));
    return {};
}

Jafg::LTransientPersona::Local Jafg::WWorldViewer::GetTransientPersona() noexcept
{
    return LTransientPersona::Local{
        .Lackey=*this,
        .bEditor=false,
        };
}

void Jafg::WWorldViewer::OnDisconnect()
{
    this->PossessPersonaController(nullptr);
}

void Jafg::WWorldViewer::BindConsumeHandle()
{
    this->ConsumeHandle = LRaiiViewportHandle::Make(this->GetViewport().OnEarlyTick, [this]
    {
        this->DispatchInputDelegates();
        return false;
    });
}

void Jafg::WWorldViewer::DispatchInputDelegates()
{
    auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
    this->UserInput._DispatchInputDelegates(Ctrl);
}

bool Jafg::WWorldViewer::OnPreDraw(LRenderInfo const& Info)
{
    if (this->GetMostOuterParent().IsNodeInVisiblePath(*this))
    {
        return this->OnPreDrawImpl(Info);
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
            auto Eye{Pawn.GetEye()};
            std::optional<TArray<AActor*>> Filter;

            algo::transparent_unordered_string_map<vk::DescriptorSet> SharedSets;
            SharedSets.emplace(UBO::WorldData::name(), this->WorldDescriptorSets[Info.Frame]);
            SharedSets.emplace(SSBO::BindlessPbrLight::name(), this->PbrLightDescriptorSets[Info.Frame]);

            algo::transparent_unordered_string_map<rhi::mapped_device_buffer*> SharedBuffers;
            SharedBuffers.emplace(UBO::WorldData::name(), &this->WorldBuffers[Info.Frame]);
            SharedBuffers.emplace(SSBO::BindlessPbrLight::name(), &this->PbrLightBuffers[Info.Frame]);
            SharedBuffers.emplace(SSBO::PbrShadowView::name(), &this->PbrShadowViewBuffers[Info.Frame]);

            Pawn.GetWorld().Draw(Info, {
                .Eye = Eye,
                .Instance = nullptr,
                .SharedSets = std::move(SharedSets),
                .SharedBuffers = std::move(SharedBuffers),
                .Filter = Filter,
                .Preferences = this->WorldRenderPreferences,
                .PolygonMode = this->PolygonMode,
                .PbrLightLimit = this->PbrLightLimit,
                .PbrShadowViewLimit = this->PbrShadowViewLimit,
                .Shadows = this->bShadowsDrawn ? &this->ShadowAtlas : nullptr,
                .AmbientOcclusionTexture = this->bAmbientOcclusionDrawn
                    ? this->AmbientOcclusionBindlessIndex : rhi::invalid_resource_index,
                });

            this->OnPostWorldDrawImpl(Info, Ctrl, Pawn, Eye);
        }
    }
    this->RenderTargetViewport.Draw(Info);
}

void Jafg::WWorldViewer::_ctor_SetBackgroundTint()
{
    this->BorderTint = *GetSingleton<JUserPreferences>().ViewportBackgroundTint;
}
