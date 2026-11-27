// Copyright mzoesch. All rights reserved.

#include "Widgets/WorldViewer.h"
#include "Framework/EyeComponent.h"
#include "Nodes/Text.h"
#include "Rhi/NodeRenderInfo.h"
#include "User/UserPreferences.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/StaticMeshComponent.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/LightSubsystem.h"
#include "Nodes/ScrollRegion.h"

namespace
{

//#
//# The node of the tree that a click at the location goes to, as #Jafg::LViewport::DispatchInputs finds it: The
//# topmost drawn node that can be hit there. Children are over their parent, later children over earlier ones.
//# Null if there is none.
//#
Jafg::WNode const* FindHitTestableNodeAt(Jafg::WNode const& Node, LVec2F Location) noexcept
{
    if (!Node.ShouldNowDraw())
    {
        return nullptr;
    }

    /* Scroll regions only show and sweep their content inside of them. */
    if (auto const* Parent{Node.As<Jafg::WParent>()};
        Parent && Parent->CanChildrenBeHitTestable()
        && (!Node.As<Jafg::WScrollRegion>() || Node.AabbTest({.Translation = Node.GetTranslationFromMostOuter()}, Location)))
    {
        for (auto It{Parent->GetChildren().rbegin()}; It != Parent->GetChildren().rend(); ++It)
        {
            check(It->get())
            if (Jafg::WNode const* Hit{FindHitTestableNodeAt(**It, Location)})
            {
                return Hit;
            }
        }
    }

    if (Node.IsHitTestable() && Node.AabbTest({.Translation = Node.GetTranslationFromMostOuter()}, Location))
    {
        return &Node;
    }
    return nullptr;
}

} /* ~Namespace <Anonymous> */

Jafg::WWorldViewer::~WWorldViewer()
{
    this->GetViewport().GetSurface().GetMutableFrontend()._Vk_WaitIdle();
}

void Jafg::WWorldViewer::Construct()
{
    Super::Construct();

    this->PolygonMode = *GetSingleton<JUserPreferences>().PolygonMode;

    this->RenderTargetViewport.Vk_OnLateInit();
    this->WorldViewportInputHandle = LRaiiViewportHandle::Make(this->GetViewport().OnEarlyTick, [this]
    {
        this->DispatchWorldViewportInputs();
        return false;
    });
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
    this->ThicknessMaterialInstance = MutableMaterialSubsystem.GetInstanceFromName(JafgInfo, "Materials/Mesh.BackDepth");
    check(this->ThicknessMaterialInstance.get())
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

    this->ConfineMouseCursorToWorld();

    this->LastDt = Dt;
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

std::optional<LVec2F> Jafg::WWorldViewer::SurfaceToWorldTargetLocation(LVec2F SurfaceLocation) const noexcept
{
    if (!this->RenderTarget.IsInitialized())
    {
        return {};
    }

    LRect2F const Rect{this->GetWorldTargetRectFromMostOuter()};
    LVec2F const Extent{this->RenderTarget.GetExtent().ToVec<f32>()};
    if (Rect.extent.x <= 0.0f || Rect.extent.y <= 0.0f || Extent.x <= 0.0f || Extent.y <= 0.0f)
    {
        return {};
    }

    LVec2F const Normalized{(SurfaceLocation - Rect.offset) / Rect.extent};
    if (Normalized.x < 0.0f || Normalized.y < 0.0f || Normalized.x > 1.0f || Normalized.y > 1.0f)
    {
        return {};
    }

    /* Same mapping as the texture coordinates in #Draw. */
    LVec4F const UVs{rhi::uv::fit(rhi::uv::identity, Extent, this->GetAnchoredSize_v2(), rhi::tex_coord_behavior::FitAspect)};
    return LVec2F{
        (UVs.x + Normalized.x * (UVs.z - UVs.x)) * Extent.x,
        (UVs.y + Normalized.y * (UVs.w - UVs.y)) * Extent.y,
        };
}

std::optional<LVec2F> Jafg::WWorldViewer::WorldTargetToSurfaceLocation(LVec2F TargetLocation) const noexcept
{
    if (!this->RenderTarget.IsInitialized())
    {
        return {};
    }

    LRect2F const Rect{this->GetWorldTargetRectFromMostOuter()};
    LVec2F const Extent{this->RenderTarget.GetExtent().ToVec<f32>()};
    LVec4F const UVs{rhi::uv::fit(rhi::uv::identity, Extent, this->GetAnchoredSize_v2(), rhi::tex_coord_behavior::FitAspect)};
    LVec2F const UVExtent{UVs.z - UVs.x, UVs.w - UVs.y};
    if (Extent.x <= 0.0f || Extent.y <= 0.0f || UVExtent.x <= 0.0f || UVExtent.y <= 0.0f)
    {
        return {};
    }

    LVec2F const Normalized{
        (TargetLocation.x / Extent.x - UVs.x) / UVExtent.x,
        (TargetLocation.y / Extent.y - UVs.y) / UVExtent.y,
        };
    return Rect.offset + Normalized * Rect.extent;
}

std::optional<LRect2F> Jafg::WWorldViewer::GetWorldViewRectInParentViewport() const noexcept
{
    if (!this->RenderTarget.IsInitialized())
    {
        return {};
    }
    return this->GetWorldTargetRectFromMostOuter();
}

LVec2F Jafg::WWorldViewer::GetWorldViewExtent() const noexcept
{
    return this->RenderTarget.IsInitialized() ? this->RenderTarget.GetExtent().ToVec<f32>() : LVec2F{0.0f};
}

std::optional<LVec2F> Jafg::WWorldViewer::GetCursorLocationInWorldView() const noexcept
{
    auto const& Surface{this->GetViewport().GetSurface()};
    if (!Surface.HasMouseLocationForOrtho())
    {
        return {};
    }
    return this->SurfaceToWorldTargetLocation(Surface.GetMouseLocationValue());
}

bool Jafg::WWorldViewer::SetCursorLocationInWorldView(LVec2F Location) noexcept
{
    auto& Surface{this->GetViewport().GetSurface()};
    if (!Surface.IsShowMouseCursor() || !Surface.CanWarpMouseCursor())
    {
        return false;
    }

    if (auto SurfaceLocation{this->WorldTargetToSurfaceLocation(Location)})
    {
        return Surface.WarpMouseCursor(*SurfaceLocation);
    }
    return false;
}

void Jafg::WWorldViewer::ConfineMouseCursorToWorld()
{
    if (!this->UserInput._bCurrentlyConsuming || this->UserInput.IsConsumingMouse() || !this->UserInput.IsConfiningMouse())
    {
        return;
    }

    auto& Surface{this->GetViewport().GetSurface()};
    if (!Surface.HasMouseLocationForOrtho() || !Surface.CanWarpMouseCursor() || !this->RenderTarget.IsInitialized())
    {
        return;
    }

    /* The surface itself confines the cursor. This keeps it out of the rest of the surface (e.g., editor panels). */
    LRect2F const Rect{this->GetWorldTargetRectFromMostOuter()};
    if (Rect.extent.x < 2.0f || Rect.extent.y < 2.0f)
    {
        return;
    }

    LVec2F const Location{Surface.GetMouseLocationValue()};
    LVec2F const Clamped{maths::clamp(Location, Rect.offset, Rect.offset + Rect.extent - LVec2F{1.0f})};
    if (Clamped != Location)
    {
        (void)Surface.WarpMouseCursor(Clamped);
    }
}

void Jafg::WWorldViewer::DispatchWorldViewportInputs()
{
    /* E.g., in a tab that is not shown. Nothing of the world can be seen or used then. */
    if (!this->RenderTarget.IsInitialized() || !this->GetMostOuterParent().IsNodeInVisiblePath(*this))
    {
        this->RenderTargetViewport.SetCursorLocation({});
        return;
    }

    std::optional<LVec2F> Cursor;
    /* Only widgets that can be hit use the cursor (e.g., not a HUD). #IsTopmostAt walks every widget of our viewport. */
    bool const bCanBeHit{algo::any_of(this->RenderTargetViewport.GetTopLevelWidgets(), [](WUserWidget const* Widget)
    {
        return Widget->ShouldNowDraw() && Widget->ShouldCheckForInputs();
    })};
    if (std::optional<LVec2F> const Location{this->GetViewport().GetCursorLocation()};
        bCanBeHit && Location && this->IsTopmostAt(*Location))
    {
        Cursor = this->SurfaceToWorldTargetLocation(*Location);
    }
    this->RenderTargetViewport.SetCursorLocation(Cursor);
    this->RenderTargetViewport.DispatchInputs(this->LastDt);
}

bool Jafg::WWorldViewer::IsTopmostAt(LVec2F Location) const noexcept
{
    auto const& TopLevelWidgets{this->GetViewport().GetTopLevelWidgets()};
    for (auto It{TopLevelWidgets.rbegin()}; It != TopLevelWidgets.rend(); ++It)
    {
        check(*It)
        if (WNode const* Hit{::FindHitTestableNodeAt(**It, Location)})
        {
            return Hit == this;
        }
    }
    return false;
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
        .bTranslucency = true,
        });
    this->InitializeAmbientOcclusion();
    this->InitializeTranslucency();

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

void Jafg::WWorldViewer::InitializeTranslucency()
{
    check(this->RenderTarget.HasTranslucency())

    auto& Frontend{this->GetViewport().GetSurface().GetMutableFrontend()};
    auto& TextureSubsystem{*Frontend.GetSubsystemChecked<JTextureSubsystem>()};

    /* The translucent draws sample the targets through the global bindless array. Our slots stay ours. */
    auto Bind{[&](rhi::resource_index_t& Index, vk::ImageView View, LStringView What)
    {
        if (Index == rhi::invalid_resource_index)
        {
            Index = TextureSubsystem._Vk_AddTransientImageToGlobalBindlessArray(View).value_or(rhi::invalid_resource_index);
            if (Index == rhi::invalid_resource_index)
            {
                LOG_WARNING(LogVulkan, "[{}]: Failed to make the {} bindless. Out of binding points. Translucent draws "
                    "will not see it.", this->GetNameAsString(), What)
            }
            return;
        }

        //# Reuse our slot, as the old image view is gone.
        vk::DescriptorImageInfo Info{
            .imageView = View,
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
            };
        std::array Writes{vk::WriteDescriptorSet{
            .dstSet = *TextureSubsystem.Vk_GetBindlessTextureArrayDescriptorSet(),
            .dstBinding = UBO::Bindless::ArrayBinding,
            .dstArrayElement = Index,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eSampledImage,
            .pImageInfo = &Info,
            },};
        Frontend.Vk_GetDevice().updateDescriptorSets(Writes, {});
    }};
    Bind(this->SceneColorBindlessIndex, this->RenderTarget.GetSceneColorImageView(), "scene color");
    Bind(this->ThicknessBindlessIndex, this->RenderTarget.GetThicknessImageView(), "thickness");
}

void Jafg::WWorldViewer::PrepareTranslucency(LActorRenderInfo const& Info) const
{
    this->RenderTarget.InterruptRendering(Info, [this, &Info](LRenderInfo const& Between)
    {
        this->RenderTarget.CopySceneColor(Between);
        /* Always, so that the thickness is never the one of the last frame. */
        this->RenderTarget.RenderThickness(Between, [this, &Info](LRenderInfo const&)
        {
            for (auto& Translucent: Info.TranslucentDraws)
            {
                if (Translucent.bThickness)
                {
                    Translucent.Draw(Info, this->ThicknessMaterialInstance.get());
                }
            }
        });
    });
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
    return this->IsOwnedPersonaControllerValid() && this->GetOwnedPersonaControllerChecked()->GetView().has_value();
}

void Jafg::WWorldViewer::DrawAmbientOcclusion(LRenderInfo const& Info)
{
    auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
    auto Eye{*Ctrl.GetView()};

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
        Ctrl.GetWorld().Draw(Info, {
            .Eye = Eye,
            //# The depth has to contain the same as the scene that samples the occlusion.
            .FrustumOverride = this->GetFrustumOverride(),
            .Instance = &*this->SceneDepthMaterialInstance,
            .Filter = Filter,
            .bUpdateWorldDataSet = false,
            .Viewer = &Ctrl,
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
    return this->IsOwnedPersonaControllerValid() && this->GetOwnedPersonaControllerChecked()->GetView().has_value();
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
    auto& World{this->GetOwnedPersonaControllerChecked()->GetWorld()};
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
    auto const Eye{*this->GetOwnedPersonaControllerChecked()->GetView()};
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
        auto& Ctrl{*this->GetOwnedPersonaControllerChecked()};
        if (std::optional<LWorldEye> const View{Ctrl.GetView()})
        {
            auto Eye{*View};
            std::optional<TArray<AActor*>> Filter;

            algo::transparent_unordered_string_map<vk::DescriptorSet> SharedSets;
            SharedSets.emplace(UBO::WorldData::name(), this->WorldDescriptorSets[Info.Frame]);
            SharedSets.emplace(SSBO::BindlessPbrLight::name(), this->PbrLightDescriptorSets[Info.Frame]);

            algo::transparent_unordered_string_map<rhi::mapped_device_buffer*> SharedBuffers;
            SharedBuffers.emplace(UBO::WorldData::name(), &this->WorldBuffers[Info.Frame]);
            SharedBuffers.emplace(SSBO::BindlessPbrLight::name(), &this->PbrLightBuffers[Info.Frame]);
            SharedBuffers.emplace(SSBO::PbrShadowView::name(), &this->PbrShadowViewBuffers[Info.Frame]);

            /* Wireframes are drawn as they are. */
            LWorld::DrawInfo::LTranslucency const Translucency{
                .SceneColorTexture = this->SceneColorBindlessIndex,
                .SceneThicknessTexture = this->ThicknessBindlessIndex,
                .Prepare = [this](LActorRenderInfo const& Info) { this->PrepareTranslucency(Info); },
                };
            bool const bTranslucency{this->PolygonMode == EPolygonMode::Fill && this->RenderTarget.HasTranslucency()
                && this->SceneColorBindlessIndex != rhi::invalid_resource_index
                && this->ThicknessBindlessIndex != rhi::invalid_resource_index};

            Ctrl.GetWorld().Draw(Info, {
                .Eye = Eye,
                .FrustumOverride = this->GetFrustumOverride(),
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
                .Translucency = bTranslucency ? &Translucency : nullptr,
                .Viewer = &Ctrl,
                });

            this->OnPostWorldDrawImpl(Info, Ctrl, Eye);
        }
    }
    this->RenderTargetViewport.Draw(Info);
}

void Jafg::WWorldViewer::_ctor_SetBackgroundTint()
{
    this->BorderTint = *GetSingleton<JUserPreferences>().ViewportBackgroundTint;
}
