// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/UserWidget.h"
#include "Framework/Lackey.h"
#include "Rhi/RenderTarget.h"
#include "Rhi/Shadow.h"
#include "Rhi/Light.h"
#include "Framework/Surface.h"
#include "Framework/EditorForward.h"
#include "User/UserPreferencesForward.h"
#include "WorldViewer.generated.h"

namespace Jafg
{

class AEyeComponent;

//# A widget to perspectively view a world in a primitive manner.
DECLARE_JAFG_WIDGET()
class ENGINE_API WWorldViewer : public WUserWidget, public LLocalLackey
{
    GENERATED_CLASS_BODY()

protected:

    explicit WWorldViewer(LNodeDynamicInit const& Init) noexcept
        : Super{Init}, LLocalLackey{this->RenderTargetViewport}
    , RenderTarget{}, RenderTargetViewport{Init.Outer.GetSurface(), this->RenderTarget.GetExtentAsLValue()}
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetShouldTick(true);
        this->_ctor_SetBackgroundTint();
    }
    template<typename TCxxClass>
    explicit WWorldViewer(TNodeStaticInit<TCxxClass> const& Init) noexcept
        : Super{Init}, LLocalLackey{Init.Outer}
    , RenderTarget{}, RenderTargetViewport{Init.Outer.GetSurface(), this->RenderTarget.GetExtentAsLValue()}
    {
        this->SetVisibility(ENodeVisibility::Visible);
        this->SetShouldTick(true);
        this->_ctor_SetBackgroundTint();
    }

public:

    static constexpr rhi::extent2 MinViewportExtent{128,128};

    virtual ~WWorldViewer() override;

    virtual void Construct() override;
    virtual void Tick(f32 Dt) override;
    virtual void Draw(LNodeRenderInfo const& Info) const override;

    virtual void OnFocusLost() override;

    NODISCARD FORCEINLINE constexpr LRenderTarget const& GetWorldRenderTarget() const noexcept { return this->RenderTarget; }
    NODISCARD FORCEINLINE constexpr LViewport& GetWorldRenderTargetViewport() noexcept { return this->RenderTargetViewport; }
    NODISCARD FORCEINLINE constexpr LViewport const& GetWorldRenderTargetViewport() const noexcept { return this->RenderTargetViewport; }

    NODISCARD ENGINE_API LRect2F GetWorldTargetRectFromMostOuter() const noexcept;
    NODISCARD ENGINE_API LRect2F GetWorldTargetRectFromMostOuter(LVec2F Translation) const noexcept;

    //# Maps a location in surface space to the pixels of the world render target. Empty if outside the world.
    NODISCARD ENGINE_API std::optional<LVec2F> SurfaceToWorldTargetLocation(LVec2F SurfaceLocation) const noexcept;
    //# Maps a location in the pixels of the world render target to surface space.
    NODISCARD ENGINE_API std::optional<LVec2F> WorldTargetToSurfaceLocation(LVec2F TargetLocation) const noexcept;

    virtual LVec2F GetWorldViewExtent() const noexcept override;
    virtual std::optional<LVec2F> GetCursorLocationInWorldView() const noexcept override;
    virtual bool SetCursorLocationInWorldView(LVec2F Location) noexcept override;

    LColor BorderTint{ Colors::Black };

    //#
    //# The desired extent.
    //# If not set, then the extent of the render target will be determined by the size of this node.
    //#
    std::optional<rhi::extent2> DesiredViewportExtent;
    FORCEINLINE bool IsManual() const noexcept { return this->DesiredViewportExtent.has_value(); }

    void TravelTo(LWorld& World);
    void Disconnect();

    void OnPerspectiveDepthTestChanged();

    NODISCARD FORCEINLINE constexpr EWorldRenderFlags GetWorldRenderPreferences() const noexcept { return this->WorldRenderPreferences; }
    FORCEINLINE void SetWorldRenderPreferences(EWorldRenderFlags New) noexcept { this->WorldRenderPreferences = New; }
    NODISCARD FORCEINLINE constexpr EPolygonMode GetPolygonMode() const noexcept { return this->PolygonMode; }
    FORCEINLINE void SetPolygonMode(EPolygonMode New) noexcept { this->PolygonMode = New; }

    //#
    //# Screen space ambient occlusion. Requires a second (depth only) pass of the world.
    //#
    NODISCARD FORCEINLINE constexpr bool IsAmbientOcclusionEnabled() const noexcept { return this->bAmbientOcclusion; }
    FORCEINLINE void SetAmbientOcclusionEnabled(bool bEnabled) noexcept { this->bAmbientOcclusion = bEnabled; }
    LAmbientOcclusionSettings AmbientOcclusion;

    //#
    //# Shadows of the lights that cast them. Requires one depth only pass of the world per shadow casting light.
    //# The atlas is only allocated once the first light wants to cast shadows.
    //#
    NODISCARD FORCEINLINE constexpr bool IsShadowsEnabled() const noexcept { return this->bShadows; }
    FORCEINLINE void SetShadowsEnabled(bool bEnabled) noexcept { this->bShadows = bEnabled; }
    //# Changes to the extents only apply before #Construct. All other settings apply immediately.
    LShadowSettings ShadowSettings;

    NODISCARD FORCEINLINE constexpr auto const& Vk_GetWorldDescriptorSet(std::size_t Frame) const noexcept { return this->WorldDescriptorSets[Frame]; }
    NODISCARD FORCEINLINE constexpr auto const& Vk_GetWorldDescriptorSets() const noexcept { return this->WorldDescriptorSets; }
    NODISCARD FORCEINLINE constexpr auto const& Vk_GetPbrLightDescriptorSet(std::size_t Frame) const noexcept { return this->PbrLightDescriptorSets[Frame]; }
    NODISCARD FORCEINLINE constexpr auto const& Vk_GetPbrLightDescriptorSets() const noexcept { return this->PbrLightDescriptorSets; }

protected:

    virtual void InitializeRenderTarget();
    virtual bool OnPreDrawImpl(LRenderInfo const& Info);
    virtual void OnPostWorldDrawImpl(LRenderInfo const& Info, APersonaController& Ctrl, LWorldEye const& Eye) {}

    virtual LTransientPersona::Local GetTransientPersona() noexcept;
    virtual void OnConnect() {}
    virtual void OnDisconnect();

    LRaiiViewportHandle& GetConsumeHandle() noexcept { return this->ConsumeHandle; }
    void BindConsumeHandle();
    void DispatchInputDelegates();

    NODISCARD constexpr auto GetLastUnstableExtent() const noexcept { return this->LastUnstableExtent; }

private:

    bool OnPreDraw(LRenderInfo const& Info);
    void PreDraw(LRenderInfo const& Info);

    //# Keeps a confined mouse cursor inside the world, where the platform allows to move the cursor.
    void ConfineMouseCursorToWorld();

    void _ctor_SetBackgroundTint();

    //# Points the materials of the ambient occlusion to the (new) targets of the render target.
    void InitializeAmbientOcclusion();
    NODISCARD bool ShouldDrawAmbientOcclusion() const noexcept;
    void DrawAmbientOcclusion(LRenderInfo const& Info);

    NODISCARD bool ShouldDrawShadows() const noexcept;
    //# Allocates the atlas and puts it into the global bindless array. False if that failed.
    NODISCARD bool InitializeShadowAtlas();
    //#
    //# Assigns the shadow casting lights of the world to the atlas and renders their shadow maps.
    //# False if nothing has been drawn, e.g. because no light casts shadows.
    //#
    NODISCARD bool DrawShadows(LRenderInfo const& Info);

    WNode* Placeholder{};

    algo::clock::time_point LastUnstableDiff;
    std::optional<rhi::extent2> LastUnstableExtent;

    bool bDatedRenderTarget{};
    LRenderTarget RenderTarget;
    LViewport RenderTargetViewport;
    LRaiiPreDrawHandle OnPreDrawHandle{this->GetViewport().GetSurface().OnPreRender};

    LRaiiViewportHandle ConsumeHandle{this->GetViewport().OnEarlyTick};

    EWorldRenderFlags WorldRenderPreferences{ EWorldRenderBits::All };
    EPolygonMode PolygonMode{ EPolygonMode::Fill };

    bool bAmbientOcclusion{ true };
    //# Whether the ambient occlusion has been drawn for the frame that is currently recorded.
    bool bAmbientOcclusionDrawn{};
    //# Stays the same for our whole lifetime, even if the render target gets recreated.
    rhi::resource_index_t AmbientOcclusionBindlessIndex{ rhi::invalid_resource_index };
    LMaterialInstanceRef SceneDepthMaterialInstance;
    LMaterialInstanceRef AmbientOcclusionMaterialInstance;
    LMaterialInstanceRef AmbientOcclusionBlurMaterialInstance;

    bool bShadows{ true };
    //# Whether the shadows have been drawn for the frame that is currently recorded.
    bool bShadowsDrawn{};
    //# Set if the atlas could not be made bindless. Shadows stay disabled for our whole lifetime then.
    bool bShadowAtlasFailed{};
    LShadowAtlas ShadowAtlas;
    LMaterialInstanceRef ShadowMaterialInstance;

    rhi::frame_array<vk::raii::DescriptorSet> WorldDescriptorSets JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    rhi::frame_array<rhi::mapped_device_buffer> WorldBuffers;
    // TODO: Is this correct here, or move to the light subsystem? I think later on we might cull lights
    //       based of eye translation. Therefore having one per eye is correct and not one at the frontend level?
    std::size_t PbrLightLimit{ 256uz }; // TODO: Make dynamic.
    rhi::frame_array<rhi::mapped_device_buffer> PbrLightBuffers;
    //# Allocated for the capacity of the atlas. They are bound to the second binding of #PbrLightDescriptorSets.
    std::size_t PbrShadowViewLimit{};
    rhi::frame_array<rhi::mapped_device_buffer> PbrShadowViewBuffers;
    rhi::frame_array<vk::raii::DescriptorSet> PbrLightDescriptorSets JAFG_VK_FRAME_ARRAY_INIT(nullptr);
};

} /* ~Namespace Jafg */
