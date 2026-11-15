// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"
#include "Rhi/Objects.h"
#include "Rhi/Light.h"
#include "Framework/FrontendForward.h"

namespace Jafg
{

struct LRenderInfo;

namespace SSBO
{

//#
//# How a single tile of the shadow atlas was rendered. A light references a contiguous range of these:
//# - Directional: One per cascade, from the closest (sharpest) to the farthest.
//# - Spot:        Exactly one.
//# - Point:       Six cube faces along +right, -right, +up, -up, +forward, -forward of the light.
//# - Rect:        The first five cube faces of a point light. Nothing is emitted towards -forward.
//# Lives at the second binding of the pbr light set, see #BindlessPbrLight.
//# @see Content/Shaders/include/Jafg/Pbr.slang
//#
struct PbrShadowView: rhi::ssbo_template<PbrShadowView, vk::ShaderStageFlagBits::eFragment>
{
    //# Only the name of the shared buffer. The descriptor set is the one of #BindlessPbrLight.
    NODISCARD static constexpr LStringView name() noexcept { return "Jafg::SSBO::PbrShadowViews"; }

    //# World space to clip space of the tile.
    LMat4F ViewProj;
    //# xy: Offset and zw: Size of the tile inside the atlas, in uv space.
    LVec4F AtlasRect;
    //# Bindless index of the shadow atlas.
    rhi::resource_index_t AtlasTexture{ rhi::invalid_resource_index };
    //# One over the extent of the atlas in texels.
    f32 AtlasTexelSize;
    //#
    //# The world space normal offset that is applied to the shaded point before it is looked up:
    //# NormalBias + NormalBiasPerMeter * (distance to the light). Already scaled by the world space size of a
    //# texel. Orthographic views have a constant texel size, perspective views one that grows with the distance.
    //#
    f32 NormalBias;
    f32 NormalBiasPerMeter;
};
static_assert(sizeof(PbrShadowView) % 16 == 0); // Required.
static_assert(sizeof(PbrShadowView) == 96, "Is the shader also modified?");
static_assert(rhi::ssbo<PbrShadowView>);

} /* ~Namespace SSBO */

struct LShadowSettings final
{
    //# Extent of the square shadow atlas in texels.
    u32 AtlasExtent{ 4096u };
    //# Extent of the tiles of directional cascades and spot lights in texels. Must divide #AtlasExtent.
    u32 TileExtent{ 1024u };
    //# Extent of the tiles of a single cube face of point and rect lights in texels. Must divide #TileExtent.
    u32 CubeFaceExtent{ 512u };
    //#
    //# How far (in texels of the shadow map) the shaded point is moved along its normal before it is looked up.
    //# Raise if surfaces shadow themselves (acne), lower if shadows detach from their casters.
    //#
    f32 NormalBias{ 1.5f };
    //# The far plane of lights with an unlimited range in meters.
    f32 UnlimitedRangeFar{ 64.0f };

    //# Number of cascades of directional lights. Clamped to [1,#LShadowAtlas::MaxCascades].
    u32 CascadeCount{ 4u };
    //# Up to which distance from the camera directional lights cast shadows, in meters.
    f32 DirectionalShadowDistance{ 64.0f };
    //#
    //# How the cascades split the distance. Zero splits uniformly, one logarithmically. Logarithmic splits spend
    //# more resolution close to the camera.
    //#
    f32 CascadeSplitLambda{ 0.8f };
    //#
    //# How far behind a cascade (towards the light) casters are still picked up, in meters. Casters further away
    //# throw no shadows into the cascade.
    //#
    f32 DirectionalCasterDistance{ 128.0f };
};

//#
//# A single depth texture that holds the shadow maps of all shadow casting lights as square tiles.
//#
//# Each frame:
//# 1. #Gather assigns tiles to the lights that want to cast shadows. Directional lights depend on the camera.
//# 2. #Render renders the depth of the scene into each tile.
//# 3. The scene samples the atlas through the global bindless array. #FindViews maps a light to its tiles.
//#
class LShadowAtlas final
{
public:

    //# Upper limit of #LShadowSettings::CascadeCount. Has to match the shader.
    static constexpr u32 MaxCascades{ 4u };

    struct CreateInfo
    {
        LFrontend const& Frontend;
        LShadowSettings const& Settings;
    };

    //# The camera that the scene is rendered from. The cascades of directional lights are fit around it.
    struct CameraInfo
    {
        LWorldEye const& Eye;
        //# Width over height of the viewport of the camera.
        f32 Aspect;
    };

    //# The view from which a light renders one of its shadow maps.
    struct View
    {
        LLight const* Light;
        //# The eye matching #ViewMatrix and #ProjMatrix. Some drawables need it.
        LWorldEye Eye;
        LMat4F ViewMatrix;
        LMat4F ProjMatrix;
        //# Offset of the tile in texels.
        vk::Offset2D Offset;
        //# Extent of the square tile in texels.
        u32 Extent;
    };

    //# The range of shadow views of a single light.
    struct LightViews
    {
        //# Index into #GetDeviceViews or #rhi::invalid_resource_index if the light is not shadowed.
        rhi::resource_index_t First{ rhi::invalid_resource_index };
        u32 Count{};
    };

    ENGINE_API void Initialize(CreateInfo const& Info);
    NODISCARD FORCEINLINE constexpr bool IsInitialized() const noexcept { return this->Extent > 0; }

    //#
    //# Assigns tiles to each of the given lights that wants to cast shadows. Lights that do not get all the tiles
    //# they need, because the atlas is full, are rendered without. Directional lights are served first.
    //#
    ENGINE_API void Gather(TArray<LLight const*> const& Lights, LShadowSettings const& Settings, CameraInfo const& Camera);
    //# Drops all views. Lights are rendered without shadows until the next #Gather.
    FORCEINLINE void Reset() noexcept { this->Views.clear(); this->DeviceViews.clear(); this->Assignments.clear(); }

    //#
    //# Renders all views. What is called once per view with the viewport and the scissor set to its tile.
    //# Leaves the atlas ready to be sampled by fragment shaders.
    //#
    ENGINE_API void Render(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info, View const& View)> What) const;

    NODISCARD FORCEINLINE constexpr TArray<View> const& GetViews() const noexcept { return this->Views; }
    NODISCARD FORCEINLINE constexpr bool HasViews() const noexcept { return !this->Views.empty(); }
    //# The device representations of #GetViews in the same order.
    NODISCARD FORCEINLINE constexpr TArray<SSBO::PbrShadowView> const& GetDeviceViews() const noexcept { return this->DeviceViews; }
    //# The views of the given light inside of #GetDeviceViews.
    NODISCARD ENGINE_API LightViews FindViews(LLight const& Light) const noexcept;

    //# The maximum number of views an atlas with the given settings can hold at the same time.
    NODISCARD static constexpr u32 GetMaxViews(LShadowSettings const& Settings) noexcept
    {
        u32 const TilesPerRow{Settings.AtlasExtent / Settings.TileExtent};
        u32 const FacesPerTileRow{Settings.TileExtent / Settings.CubeFaceExtent};
        return TilesPerRow * TilesPerRow * FacesPerTileRow * FacesPerTileRow;
    }

    NODISCARD FORCEINLINE constexpr auto const& GetImageView() const noexcept { return this->ImageView; }

    //# The owner has to put the atlas into the global bindless array and tell us where.
    FORCEINLINE constexpr void SetBindlessIndex(rhi::resource_index_t Index) noexcept { this->BindlessIndex = Index; }
    NODISCARD FORCEINLINE constexpr rhi::resource_index_t GetBindlessIndex() const noexcept { return this->BindlessIndex; }

    //# Whether the given light type can cast shadows at all.
    NODISCARD static constexpr bool SupportsShadows(ELight Type) noexcept
    {
        switch (Type)
        {
        case ELight::Directional:
        case ELight::Point:
        case ELight::Spot:
        case ELight::Rect:
            return true;
        }
        return false;
    }

private:

    //# Hands out tiles of the atlas for a single #Gather.
    struct Allocator
    {
        u32 NextTile{};
        //# Unused cube faces of tiles that have been split already.
        TArray<vk::Offset2D> FreeFaces;
    };
    NODISCARD std::optional<vk::Offset2D> AllocateTile(Allocator& Alloc) const noexcept;
    NODISCARD std::optional<vk::Offset2D> AllocateFace(Allocator& Alloc) const noexcept;

    //# Each allocates all tiles a light needs before adding any view. False (and nothing added) if the atlas is full.
    NODISCARD bool GatherDirectional(LLight const& Light, LShadowSettings const& Settings, CameraInfo const& Camera, Allocator& Alloc);
    NODISCARD bool GatherSpot(LLight const& Light, LShadowSettings const& Settings, Allocator& Alloc);
    NODISCARD bool GatherCube(LLight const& Light, LShadowSettings const& Settings, Allocator& Alloc);

    void AddView(View View, SSBO::PbrShadowView DeviceView);

    u32 Extent{};
    u32 TileExtent{};
    u32 CubeFaceExtent{};
    u32 TilesPerRow{};

    vk::Format Format{ vk::Format::eUndefined };
    rhi::device_image Image;
    vk::raii::ImageView ImageView{ nullptr };

    rhi::resource_index_t BindlessIndex{ rhi::invalid_resource_index };

    TArray<View> Views;
    TArray<SSBO::PbrShadowView> DeviceViews;
    TArray<std::pair<LLight const*, LightViews>> Assignments;
};

} /* ~Namespace Jafg */
