// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/WorldData.slang"
#include "Jafg/PbrMaterial.slang"

namespace Jafg
{

static const uint LIGHT_DIRECTIONAL = 0u;
static const uint LIGHT_POINT       = 1u;
static const uint LIGHT_SPOT        = 2u;
static const uint LIGHT_RECT        = 3u;

[[CxxName("Jafg::UBO::PbrLight")]]
struct PbrLight
{
    /// directional: unused
    /// point, spot: world position.
    /// rect:        world position of the center.
    float3 pos;

    /// directional: unused
    /// else:        0 = unlimited, otherwise smooth cut-off radius (m)
    float range;

    /// Always normalized.
    /// directional: direction the light travels
    /// point:       forward axis of the light. Only orients the cube faces of the shadows.
    /// spot:        direction the cone points
    /// rect:        emitting normal (front face)
    float3 direction;

    /// The type of the light (LIGHT_*).
    uint type;

    /// Color
    float3 tint;
    /// Color intensity. Lights without intensity are skipped.
    /// directional: illuminance (lx) at a surface that directly faces the light
    /// point, spot: luminous intensity (cd)
    /// rect:        luminance (cd/m^2) of the emitting face
    float intensity;

    /// rect:  unit vector along the width axis (perp. to direction)
    /// point: right axis of the light. Only orients the cube faces of the shadows.
    float3 right;
    /// rect: full width  (m)
    float width;

    /// rect: full height (m)
    float height;
    float spot_scale;    // spot: 1 / max(0.001, cosInner - cosOuter)
    float spot_offset;   // spot: -cosOuter * spotScale

    /// LIGHT_FLAG_*
    uint flags;

    /// point/spot radius (m), or tan(SourceAngle/2) for directional
    float source_radius;
    /// Multiplier for the specular highlights of this light.
    float specular_scale;

    /// Index of the first of the shadow_views of this light or INVALID_INDEX if it casts no shadows.
    uint shadow;
    /// Number of shadow_views starting at shadow. What they are depends on the type, see PbrShadowView.
    uint shadow_count;
};

///
/// How a tile of the shadow atlas was rendered. A light references a contiguous range of these:
/// directional: One per cascade, from the closest (sharpest) to the farthest.
/// spot:        Exactly one.
/// point:       Six cube faces along +right, -right, +up, -up, +direction, -direction of the light,
///              with up = cross(right, direction).
/// rect:        The first five cube faces of a point light. Nothing is emitted towards -direction.
/// @see Engine/include/Rhi/Shadow.h
///
[[CxxName("Jafg::SSBO::PbrShadowView")]]
struct PbrShadowView
{
    /// World space to clip space of the tile.
    float4x4 view_proj;
    /// xy: Offset and zw: Size of the tile inside the atlas, in uv space.
    float4 atlas_rect;
    /// Bindless index of the shadow atlas.
    uint atlas_texture;
    /// One over the extent of the atlas in texels.
    float atlas_texel_size;
    /// World space normal offset: normal_bias + normal_bias_per_meter * (distance to the light).
    float normal_bias;
    float normal_bias_per_meter;
};

/// Upper limit of the cascades of a directional light. @see LShadowAtlas::MaxCascades
static const uint MAX_SHADOW_CASCADES = 4u;

/// How far the filter kernel reaches from the looked up point, in texels. @see KernelMarginTexels in Shadow.cpp
static const float SHADOW_KERNEL_MARGIN_TEXELS = 2.5;

///
/// Note, that this struct exceeds the minimum size of a push constant block (128 bytes)
/// opposed by the Vulkan spec (pre 1.4).
///
[[CxxName("Jafg::PC::Pbr")]]
struct PbrPushConstants
{
    float4x4 model;
    uint material;
    uint _pad0;
    uint _pad1;
    uint _pad2;
};

} /* ~Namespace Jafg */

[[vk::binding(0, 0),Shared("Jafg::UBO::WorldData")]] ConstantBuffer<Jafg::WorldData> world_data;

[[vk::binding(0, 1),Shared("Jafg::UBO::Bindless")]] SamplerState[] samplers;
[[vk::binding(1, 1),Shared("Jafg::UBO::Bindless")]] Texture2D[] bindless_textures;

[[vk::binding(0, 2),Shared("Jafg::SSBO::PbrMaterials")]] StructuredBuffer<Jafg::PbrMaterial> materials;

[[vk::binding(0, 3),Shared("Jafg::SSBO::PbrLights")]] StructuredBuffer<Jafg::PbrLight> lights;
[[vk::binding(1, 3),Shared("Jafg::SSBO::PbrLights")]] StructuredBuffer<Jafg::PbrShadowView> shadow_views;

/*
 * Bind your own in your shader source files if any.
 * Jafg currently supports up to seven simultaneously bound descriptor sets.
 */

[[vk::push_constant]] Jafg::PbrPushConstants request;

namespace Jafg
{

/// Lower bound for the perceptual roughness. Avoids a degenerated GGX lobe (and with it specular aliasing and
/// precision issues) for perfectly smooth surfaces.
static const float MIN_PERCEPTUAL_ROUGHNESS = 0.045;

/// Centimeters (the length unit of OpenPBR) to meters (the one of Jafg).
static const float CM_TO_M = 0.01;

/// Where the surface point is and how it is oriented. As interpolated from the vertices.
struct PbrGeometry
{
    float3 world_pos;
    /// Not normalized.
    float3 normal;
    /// Of the mesh. The material applies its own transform.
    float2 uv;
    /// Not normalized. xyz is zero for meshes without tangents. w is the handedness of the bitangent.
    float4 tangent;
    /// SV_Position: xy are the pixel center coordinates.
    float4 frag_coord;
    /// SV_IsFrontFace. Back faces of double sided materials are shaded with flipped normals.
    bool front_facing;
};

/// What the material does to the pixel.
struct PbrResult
{
    /// Radiance leaving the surface towards the camera, already weighted by the coverage.
    float3 radiance;
    ///
    /// Per channel factor on what is already behind the surface on the target (dual source blending): Light that
    /// passes the surface without being refracted, and what is not covered by it. Ignored by opaque draws.
    ///
    float3 transmittance;
    /// Range [0,1] with 0 = not there.
    float coverage;
    /// False if alpha masked away. Has to be discarded by the caller (after everything that needs derivatives).
    bool covered;
};

/// The fully resolved (factors * textures) properties of a pbr material at a given surface point.
struct PbrSurface
{
    float3 base_color;
    float metalness;
    float diffuse_roughness;
    /// Ambient occlusion. Range [0,1] with 0 = fully occluded and 1 = not occluded.
    float occlusion;
    float coverage;
    bool covered;

    float specular_weight;
    float3 specular_color;
    /// Perceptual roughness. Range [0,1] with 0 = smooth and 1 = rough.
    float specular_roughness;
    float specular_anisotropy;
    /// Of the anisotropy in tangent space (normalized), as of the anisotropy texture.
    float2 anisotropy_direction;

    float subsurface_weight;
    float3 subsurface_color;

    float thin_film_weight;
    float thin_film_thickness;

    /// Linear emitted radiance below the coat and the fuzz.
    float3 emission;
};

/// Precomputed per-pixel values shared by all lights.
struct PbrShadingContext
{
    float3 world_pos;
    /// Normalized direction from the surface towards the camera.
    float3 v;
    /// Normalized vertex normal. Flipped on back faces of double sided materials.
    float3 n_geometric;
    bool thin_walled;
    bool translucent;

    /// Normalized shading normal of the base. Perturbed by the normal map, if any.
    float3 n;
    /// Normalized tangent and bitangent of the anisotropy, orthogonal to n. Only set if anisotropic.
    float3 t;
    float3 b;
    /// Normalized direction of the mirror reflection of v.
    float3 r;
    float n_dot_v;

    /// Range [0,1] with 0 = dielectric and 1 = metallic.
    float metalness;
    /// Albedo of the diffuse BRDF of the dielectric base.
    float3 diffuse_albedo;
    /// Range [0,1] with 0 = Lambertian.
    float diffuse_roughness;
    /// Diffuse albedo of everything opaque (dielectric base and subsurface) below the specular.
    float3 diffuse_color;
    /// The part of the dielectric base that transmits the light into its volume instead of diffusing it.
    float transmission_weight;

    float subsurface_weight;
    float3 subsurface_color;
    /// Direction of the scattering. Range [-1,1].
    float subsurface_anisotropy;
    /// Per color channel. How far the diffuse light wraps around the terminator. Range [0,1].
    float3 subsurface_wrap;
    /// The light that scatters below the surface does not see the small bumps of it.
    float3 n_subsurface;
    /// Moves the shadow lookup away from the surface (m), so that the wrapped light is not self shadowed.
    float shadow_offset;

    /// Specular reflectance of the dielectric at normal and grazing incidence.
    float f0_dielectric;
    float f90_dielectric;
    /// Index of refraction of the dielectric relative to the outside, as seen by the transmitted light.
    float eta;
    /// Non-physical tint of the reflection of the dielectric.
    float3 specular_tint;
    float specular_weight;
    /// Specular reflectance of the metal at normal incidence, and its tint near grazing incidence (F82).
    float3 f0_metal;
    float3 f82_tint;

    float thin_film_weight;
    /// Reflectance of the thin film on top of the dielectric and on top of the metal as seen from v.
    float3 thin_film_dielectric;
    float3 thin_film_metal;

    /// Clamped perceptual roughness of the base. Roughened by the coat.
    float roughness;
    /// GGX alpha (roughness^2).
    float alpha;
    bool anisotropic;
    /// GGX alpha along t and b. Equal to alpha if isotropic.
    float alpha_t;
    float alpha_b;
    /// Isotropic GGX alpha with the same peak as the (anisotropic) lobe. For the energy of area lights.
    float alpha_area;

    /// Scale and bias to apply to the reflectance of the lobe. @see _env_brdf_approx
    float2 env_brdf;
    /// Compensates the energy that a single scattering GGX lobe loses on rough surfaces.
    float3 energy_compensation;
    /// Directional albedo of the (untinted) dielectric specular as seen from v. What is below it gets the rest.
    float3 e_dielectric;

    /// Coat. Its frame is the geometric one unless it has a normal map of its own.
    float coat_weight;
    float3 coat_color;
    float3 n_coat;
    float3 t_coat;
    float3 b_coat;
    float3 r_coat;
    float n_dot_v_coat;
    float coat_ior;
    float coat_f0;
    float coat_roughness;
    float coat_alpha;
    bool coat_anisotropic;
    float coat_alpha_t;
    float coat_alpha_b;
    float coat_alpha_area;
    /// Directional albedo of the coat as seen from v.
    float e_coat;
    /// Cosine of the view direction refracted into the coat.
    float coat_mu_v;
    /// Darkening and saturation of the base by the light that bounces inside of the coat.
    float3 coat_darkening;

    /// Fuzz. Its normal is between the one of the base and of the coat.
    float fuzz_weight;
    float3 fuzz_color;
    float fuzz_roughness;
    float3 n_fuzz;
    float n_dot_v_fuzz;
    /// Directional albedo of the fuzz as seen from v.
    float e_fuzz;
};

/// Orthonormal frame of the surface. The bitangent follows the glTF convention: cross(n, t) * handedness.
struct PbrTangentFrame
{
    float3 t;
    float3 b;
    float3 n;
};

///
/// How the light of a single light source arrives at the shaded surface point.
///
/// Punctual lights arrive from exactly one direction. Area lights (spheres, discs and rectangles) are approximated
/// by one most representative point each for the diffuse and for the specular terms.
/// [Karis 2013, "Real Shading in Unreal Engine 4"]
///
struct PbrIncidentLight
{
    /// Normalized direction from the surface towards the light for the diffuse term.
    float3 l;
    /// Cosine of the angle of incidence for the diffuse term. Range [0,1].
    float n_dot_l;
    /// Normalized direction from the surface towards the light for the specular term of the base and of the coat.
    float3 l_specular;
    float3 l_coat;
    /// Sine of the half angle that the source subtends as seen from the surface. Zero for punctual lights.
    float sin_a;
    /// Illuminance (already tinted) at a surface that directly faces the light.
    float3 illuminance;
};

/// Main entry point to evaluate a pbr material for a given surface point.
PbrResult evaluate_pbr(PbrMaterial material, PbrGeometry geometry, bool translucent)
{
    static_assert(sizeof(Jafg::PbrMaterial) == 304, "Unexpected struct size");
    static_assert(sizeof(Jafg::PbrLight) == 96, "Unexpected struct size");
    static_assert(sizeof(Jafg::PbrShadowView) == 96, "Unexpected struct size");
    static_assert(sizeof(Jafg::WorldData) == 208, "Unexpected struct size");

    SamplerState sampler = samplers[NonUniformResourceIndex(material.sampler)];
    float2 uv = pbr_material_uv(material, geometry.uv);

    PbrSurface surface = _evaluate_surface(material, sampler, uv);

    PbrResult result;
    result.coverage = surface.coverage;
    result.covered = surface.covered;
    result.transmittance = float3(1.0, 1.0, 1.0) - surface.coverage;

    if (!_should_show_any_lighting()) // make this also a flag. so that we allow complete darkness.
    {
        result.radiance = surface.base_color * surface.coverage;
        return result;
    }

    // Back faces of double sided materials face away from the interpolated normal.
    float3 geometric_normal = _safe_normalize(geometry.normal);
    float4 tangent = geometry.tangent;
    if (!geometry.front_facing && pbr_material_is_double_sided(material))
    {
        geometric_normal = -geometric_normal;
        tangent.w = -tangent.w;
    }

    PbrShadingContext ctx = _make_shading_context(material, sampler, surface, geometry.world_pos, geometric_normal
        , uv, tangent, translucent);

    // The descriptor range is set to exactly the number of uploaded lights.
    uint light_count;
    uint light_stride;
    lights.GetDimensions(light_count, light_stride);

    float3 direct = float3(0.0, 0.0, 0.0);
    for (uint i = 0; i < light_count; ++i)
    {
        PbrLight light = lights[i];
        // The host uploads a single light without intensity if there are no lights at all.
        if (light.intensity <= 0.0 || !_should_show_light_type(light.type))
        {
            continue;
        }

        float3 contribution = float3(0.0, 0.0, 0.0);
        switch (light.type)
        {
        case LIGHT_DIRECTIONAL: contribution = _evaluate_directional_light(light, ctx); break;
        case LIGHT_POINT:       contribution = _evaluate_point_light(light, ctx);       break;
        case LIGHT_SPOT:        contribution = _evaluate_spot_light(light, ctx);        break;
        case LIGHT_RECT:        contribution = _evaluate_rect_light(light, ctx);        break;
        default:                break;
        }

        // Only look up the shadow where the light arrives at all. Saves the samples outside of cones and ranges.
        if (light.shadow != INVALID_INDEX && any(contribution > 0.0))
        {
            // The shadow lookup is offset along the interpolated vertex normal, never along a perturbed one. Thin
            // walls are also lit through their back, which is where the lookup has to move to then.
            float3 normal = ctx.n_geometric;
            if (ctx.thin_walled && dot(normal, _approximate_light_direction(light, ctx.world_pos)) < 0.0)
            {
                normal = -normal;
            }
            contribution *= _sample_shadow(light, ctx.world_pos + normal * ctx.shadow_offset, normal);
        }
        direct += contribution;
    }

    // Ambient, so that unlit surfaces are not pitch-black. Treated as a uniform environment radiance until
    // we have IBL. Occlusion only affects indirect light, direct light is occluded by the shadows above.
    // The screen space occlusion and the one of the material describe the same thing at different scales. Taking
    // the stronger one avoids darkening creases twice. Translucent surfaces are not part of the screen space one.
    float occlusion = translucent ? surface.occlusion : min(surface.occlusion, _sample_screen_space_occlusion(geometry.world_pos));
    float3 ambient_radiance = world_data.ambient.xyz;
    float3 ambient = _evaluate_ambient(ctx, ambient_radiance) * occlusion;

    float3 emission = surface.emission * _emission_transmittance(ctx);

    result.radiance = (direct + ambient + emission) * surface.coverage;

    // Whatever lets the light through the dielectric base.
    if (translucent && ctx.transmission_weight > 0.0 && ctx.metalness < 1.0)
    {
        float3 through = _transmission_weight(ctx) * surface.coverage;
        float3 background;
        float3 passthrough;
        _evaluate_transmission(material, ctx, geometry, ambient_radiance * occlusion, background, passthrough);
        result.radiance += through * background;
        result.transmittance += through * passthrough;
    }

    return result;
}

/// Evaluates an opaque pbr material for a front facing surface point. For shaders that bring their own geometry.
float4 evaluate_pbr_sv_target(PbrMaterial material, SamplerState sampler
    , float3 translation, float3 normal, float2 uv, float4 tangent
    )
{
    PbrGeometry geometry;
    geometry.world_pos = translation;
    geometry.normal = normal;
    geometry.uv = uv;
    geometry.tangent = tangent;
    geometry.frag_coord = float4(0.0, 0.0, 0.0, 1.0);
    geometry.front_facing = true;

    PbrResult result = evaluate_pbr(material, geometry, false);
    return float4(result.radiance, 1.0);
}

//////////////////////////////////////////////////////////////////////////
// Editor visualization flags.
//////////////////////////////////////////////////////////////////////////

/// Whether the given WORLD_FLAG_SHOW_* bit is set. Always true in non editor builds.
bool _should_show(uint flag)
{
#if JAFG_WITH_EDITOR
    return (world_data.Flags & flag) != 0u;
#else /* JAFG_WITH_EDITOR */
    return true;
#endif /* !JAFG_WITH_EDITOR */
}

/// Whether any light type is shown at all. If not, the scene is rendered unlit (full bright).
bool _should_show_any_lighting()
{
#if JAFG_WITH_EDITOR
    const uint mask = WORLD_FLAG_SHOW_DIRECTIONAL_LIGHTS
                    | WORLD_FLAG_SHOW_POINT_LIGHTS
                    | WORLD_FLAG_SHOW_SPOT_LIGHTS
                    | WORLD_FLAG_SHOW_RECT_LIGHTS
                    ;
    return (world_data.Flags & mask) != 0u;
#else /* JAFG_WITH_EDITOR */
    return true;
#endif /* !JAFG_WITH_EDITOR */
}

/// Whether lights of the given type (LIGHT_*) should contribute to the lighting.
bool _should_show_light_type(uint type)
{
#if JAFG_WITH_EDITOR
    switch (type)
    {
    case LIGHT_DIRECTIONAL: return (world_data.Flags & WORLD_FLAG_SHOW_DIRECTIONAL_LIGHTS) != 0u;
    case LIGHT_POINT:       return (world_data.Flags & WORLD_FLAG_SHOW_POINT_LIGHTS)       != 0u;
    case LIGHT_SPOT:        return (world_data.Flags & WORLD_FLAG_SHOW_SPOT_LIGHTS)        != 0u;
    case LIGHT_RECT:        return (world_data.Flags & WORLD_FLAG_SHOW_RECT_LIGHTS)        != 0u;
    default:                return false;
    }
#else /* JAFG_WITH_EDITOR */
    return true;
#endif /* !JAFG_WITH_EDITOR */
}

//////////////////////////////////////////////////////////////////////////
// Screen space.
//////////////////////////////////////////////////////////////////////////

/// Where the world position is on the target, in uv space.
float2 _project_to_screen(float3 world_pos)
{
    float4 clip = mul(world_data.proj, mul(world_data.view, float4(world_pos, 1.0)));
    return clip.xy / clip.w * 0.5 + 0.5;
}

/// Distance of the world position in front of the camera along the view direction (m).
float _view_depth(float3 world_pos)
{
    // The view looks down the negative z-axis.
    return -mul(world_data.view, float4(world_pos, 1.0)).z;
}

///
/// The screen space ambient occlusion at the given world position. Range [0,1] with 0 = fully occluded and
/// 1 = not occluded, which is also returned if the view has no screen space ambient occlusion.
///
float _sample_screen_space_occlusion(float3 world_pos)
{
    if (world_data.ssao_texture == INVALID_INDEX)
    {
        return 1.0;
    }
#if JAFG_WITH_EDITOR
    if (!_should_show(WORLD_FLAG_SHOW_SSAO))
    {
        return 1.0;
    }
#endif /* JAFG_WITH_EDITOR */

    // The occlusion target covers the same viewport as we are rendering to.
    return bindless_textures[NonUniformResourceIndex(world_data.ssao_texture)]
        .SampleLevel(samplers[JAFG_SAMPLER_LINEAR_CLAMP_TO_EDGE], _project_to_screen(world_pos), 0.0).r;
}

//////////////////////////////////////////////////////////////////////////
// Shadows.
//////////////////////////////////////////////////////////////////////////

///
/// Bilinearly filtered depth comparison (2x2 PCF) at the given uv of the atlas. One gather instead of four samples.
/// Range [0,1] with 0 = fully in shadow and 1 = fully lit.
///
float _shadow_compare_bilinear(Texture2D atlas, float2 uv, float receiver_depth, float atlas_extent, float texel_size)
{
    // Texel space with the texel centers at whole numbers.
    float2 texel = uv * atlas_extent - 0.5;
    float2 base = floor(texel);
    float2 f = texel - base;

    // Gathers the four texels around the shared corner at base + 1, in the order:
    // x = (0,1), y = (1,1), z = (1,0), w = (0,0) relative to base.
    float4 occluder_depth = atlas.GatherRed(samplers[JAFG_SAMPLER_NEAREST_CLAMP_TO_EDGE], (base + 1.0) * texel_size);
    float4 lit = step(receiver_depth, occluder_depth);

    return lerp(lerp(lit.w, lit.z, f.x), lerp(lit.x, lit.y, f.x), f.y);
}

///
/// Projects the world position into the shadow view. False if it is not covered by the view, with the given margin
/// in texels of the tile kept free at its borders.
///
bool _project_shadow(PbrShadowView view, float3 world_pos, float margin_texels, out float3 ndc)
{
    ndc = float3(0.0, 0.0, 0.0);

    float4 clip = mul(view.view_proj, float4(world_pos, 1.0));
    if (clip.w <= 0.0)
    {
        return false;
    }
    ndc = clip.xyz / clip.w;

    float tile_texels = view.atlas_rect.z / view.atlas_texel_size;
    float limit = 1.0 - 2.0 * margin_texels / tile_texels;
    return all(abs(ndc.xy) <= limit) && ndc.z >= 0.0 && ndc.z <= 1.0;
}

///
/// The visibility from a single shadow view (3x3 PCF). Range [0,1] with 0 = fully in shadow and 1 = fully lit,
/// which is also returned outside of the volume covered by the view.
/// dist_to_light scales the normal offset of perspective views.
///
float _sample_shadow_view(PbrShadowView view, float3 world_pos, float3 geometric_normal, float dist_to_light)
{
    // Normal offset: Move the receiver away from the surface by about a texel of the shadow map. Fights acne without
    // detaching the shadows. The texels of perspective views grow with the distance to the light.
    float3 receiver = world_pos + geometric_normal * (view.normal_bias + view.normal_bias_per_meter * dist_to_light);

    float3 ndc;
    if (!_project_shadow(view, receiver, 0.0, ndc))
    {
        return 1.0;
    }

    float2 tile_min = view.atlas_rect.xy;
    float2 tile_max = view.atlas_rect.xy + view.atlas_rect.zw;
    float2 uv = tile_min + (ndc.xy * 0.5 + 0.5) * view.atlas_rect.zw;

    // The 3x3 kernel reaches two texels away from its center (one tap plus the bilinear footprint). Keep the
    // whole kernel inside of the tile, else it would read the depth of the neighboring tiles.
    float texel_size = view.atlas_texel_size;
    float atlas_extent = 1.0 / texel_size;
    uv = clamp(uv, tile_min + 2.0 * texel_size, tile_max - 2.0 * texel_size);

    Texture2D atlas = bindless_textures[NonUniformResourceIndex(view.atlas_texture)];

    float visibility = 0.0;
    [unroll]
    for (int y = -1; y <= 1; ++y)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            visibility += _shadow_compare_bilinear(atlas, uv + float2(x, y) * texel_size, ndc.z
                , atlas_extent, texel_size);
        }
    }

    return visibility / 9.0;
}

///
/// The visibility of the light from the given world position. Range [0,1] with 0 = fully in shadow and
/// 1 = fully lit, which is also returned where the light has no shadow map, e.g. beyond the last cascade.
///
float _sample_shadow(PbrLight light, float3 world_pos, float3 geometric_normal)
{
#if JAFG_WITH_EDITOR
    if (!_should_show(WORLD_FLAG_SHOW_SHADOWS))
    {
        return 1.0;
    }
#endif /* JAFG_WITH_EDITOR */

    switch (light.type)
    {
    case LIGHT_DIRECTIONAL:
    {
        // The first (sharpest) cascade that covers the point, including the reach of the filter kernel.
        uint count = min(light.shadow_count, MAX_SHADOW_CASCADES);
        for (uint i = 0; i < count; ++i)
        {
            PbrShadowView view = shadow_views[light.shadow + i];
            float3 ndc;
            if (_project_shadow(view, world_pos, SHADOW_KERNEL_MARGIN_TEXELS, ndc))
            {
                return _sample_shadow_view(view, world_pos, geometric_normal, 0.0);
            }
        }
        return 1.0;
    }
    case LIGHT_SPOT:
    {
        return _sample_shadow_view(shadow_views[light.shadow], world_pos, geometric_normal
            , length(light.pos - world_pos));
    }
    case LIGHT_POINT:
    case LIGHT_RECT:
    {
        // The cube face along the dominant axis of the direction from the light, in the basis of the light.
        float3 to_surface = world_pos - light.pos;
        float3 up = cross(light.right, light.direction);
        float3 local = float3(dot(to_surface, light.right), dot(to_surface, up), dot(to_surface, light.direction));
        float3 a = abs(local);

        uint face;
        if (a.x >= a.y && a.x >= a.z)
        {
            face = local.x >= 0.0 ? 0u : 1u;
        }
        else if (a.y >= a.z)
        {
            face = local.y >= 0.0 ? 2u : 3u;
        }
        else
        {
            face = local.z >= 0.0 ? 4u : 5u;
        }

        // Rect lights have no face behind them. Nothing that is lit lies there.
        if (face >= light.shadow_count)
        {
            return 1.0;
        }
        return _sample_shadow_view(shadow_views[light.shadow + face], world_pos, geometric_normal, length(to_surface));
    }
    default:
        return 1.0;
    }
}

//////////////////////////////////////////////////////////////////////////
// Material evaluation.
//////////////////////////////////////////////////////////////////////////

///
/// Samples the texture at the given bindless index.
/// Note: The material is uniform for the whole draw call, so all branches on texture indices are uniform and
/// implicit derivatives inside of them are well-defined.
///
float4 _sample(uint index, SamplerState sampler, float2 uv)
{
    return bindless_textures[NonUniformResourceIndex(index)].Sample(sampler, uv);
}

///
/// Resolves all material properties that can be textured. Factors are multiplied with the sampled texture values if
/// bound, else the factors are used directly.
///
/// If a property is hidden by the editor visualization flags, the neutral value of a default constructed
/// material is used instead (white base color, no occlusion, 0.5 roughness, dielectric, no emission).
///
PbrSurface _evaluate_surface(PbrMaterial material, SamplerState sampler, float2 uv)
{
    PbrSurface surface;

    // Base color and coverage. The texture is stored as SRGB, so the sampled color is already linear.
    float4 base_color = float4(material.base_color, 1.0);
    if (material.base_color_texture != INVALID_INDEX)
    {
        base_color *= _sample(material.base_color_texture, sampler, uv);
    }
    // OpenPBR scales the base color of both, the dielectric and the metal.
    surface.base_color = saturate(base_color.rgb) * saturate(material.base_weight);
    surface.covered = pbr_material_coverage(material, base_color.a, surface.coverage);

    // AO, roughness, metalness (ARM): R = AO, G = roughness, B = metalness.
    // The texture is stored as UNORM, so the values are already linear.
    float3 arm = float3(material.occlusion, material.specular_roughness, material.base_metalness);
    if (material.arm_texture != INVALID_INDEX)
    {
        arm *= _sample(material.arm_texture, sampler, uv).rgb;
    }
    surface.occlusion = saturate(arm.r);
    surface.specular_roughness = saturate(arm.g);
    surface.metalness = saturate(arm.b);
    surface.diffuse_roughness = saturate(material.base_diffuse_roughness);

    // Specular: rgb = color (SRGB), a = weight.
    float4 specular = float4(material.specular_color, material.specular_weight);
    if (material.specular_texture != INVALID_INDEX)
    {
        specular *= _sample(material.specular_texture, sampler, uv);
    }
    surface.specular_color = saturate(specular.rgb);
    surface.specular_weight = max(specular.a, 0.0);

    // Anisotropy: rg = direction in tangent space, b = strength. [KHR_materials_anisotropy]
    surface.specular_anisotropy = saturate(material.specular_roughness_anisotropy);
    surface.anisotropy_direction = float2(1.0, 0.0);
    if (material.anisotropy_texture != INVALID_INDEX)
    {
        float3 anisotropy = _sample(material.anisotropy_texture, sampler, uv).rgb;
        float2 direction = anisotropy.rg * 2.0 - 1.0;
        surface.anisotropy_direction = dot(direction, direction) > 1e-6 ? normalize(direction) : float2(1.0, 0.0);
        surface.specular_anisotropy *= anisotropy.b;
    }

    // Subsurface: rgb = color (SRGB), a = weight.
    float4 subsurface = float4(material.subsurface_color, material.subsurface_weight);
    if (material.subsurface_texture != INVALID_INDEX)
    {
        subsurface *= _sample(material.subsurface_texture, sampler, uv);
    }
    surface.subsurface_color = saturate(subsurface.rgb);
    surface.subsurface_weight = saturate(subsurface.a);

    // Thin film: r = weight, g = thickness.
    float2 thin_film = float2(material.thin_film_weight, material.thin_film_thickness);
    if (material.thin_film_texture != INVALID_INDEX)
    {
        thin_film *= _sample(material.thin_film_texture, sampler, uv).rg;
    }
    surface.thin_film_weight = saturate(thin_film.r);
    surface.thin_film_thickness = max(thin_film.g, 0.0);

    // Emission. The texture is stored as SRGB, so the sampled value is already linear.
    surface.emission = max(material.emission_color, 0.0) * max(material.emission_luminance, 0.0);
    if (material.emission_texture != INVALID_INDEX)
    {
        surface.emission *= _sample(material.emission_texture, sampler, uv).rgb;
    }

#if JAFG_WITH_EDITOR
    if (!_should_show(WORLD_FLAG_SHOW_ALBEDO))
    {
        // Keep the coverage so that cutouts etc. still keep their silhouette.
        surface.base_color = float3(1.0, 1.0, 1.0);
        surface.subsurface_color = float3(1.0, 1.0, 1.0);
    }
    if (!_should_show(WORLD_FLAG_SHOW_OCCLUSION))
    {
        surface.occlusion = 1.0;
    }
    if (!_should_show(WORLD_FLAG_SHOW_ROUGHNESS))
    {
        surface.specular_roughness = 0.5;
    }
    if (!_should_show(WORLD_FLAG_SHOW_METALLIC))
    {
        surface.metalness = 0.0;
    }
    if (!_should_show(WORLD_FLAG_SHOW_EMISSIVE))
    {
        surface.emission = float3(0.0, 0.0, 0.0);
    }
#endif /* JAFG_WITH_EDITOR */

    return surface;
}

///
/// The tangent frame of the surface. Meshes without tangents get one from the screen space derivatives of the
/// position and the texture coordinates. [Schueler 2013, "Followup: Normal Mapping Without Precomputed Tangents"]
/// Has to be called in uniform control flow.
///
PbrTangentFrame _make_tangent_frame(float3 world_pos, float3 n, float2 uv, float4 tangent)
{
    float3 dp_dx = ddx(world_pos);
    float3 dp_dy = ddy(world_pos);
    float2 duv_dx = ddx(uv);
    float2 duv_dy = ddy(uv);

    PbrTangentFrame frame;
    frame.n = n;

    float3 t = tangent.xyz - n * dot(n, tangent.xyz);
    if (dot(t, t) > 1e-8)
    {
        frame.t = normalize(t);
        frame.b = cross(n, frame.t) * (tangent.w < 0.0 ? -1.0 : 1.0);
        return frame;
    }

    float3 dp_dy_perp = cross(dp_dy, n);
    float3 dp_dx_perp = cross(n, dp_dx);
    // The gradients of u and v on the surface, scaled by the signed area of the pixel on the surface. Its sign
    // depends on the handedness of the screen relative to the normal.
    float area_sign = dot(cross(dp_dx, dp_dy), n) < 0.0 ? -1.0 : 1.0;
    float3 dp_du = (dp_dy_perp * duv_dx.x + dp_dx_perp * duv_dy.x) * area_sign;
    float3 dp_dv = (dp_dy_perp * duv_dx.y + dp_dx_perp * duv_dy.y) * area_sign;

    frame.t = _safe_normalize(dp_du - n * dot(n, dp_du));
    // The texture coordinates of glTF grow downwards in the image, the green channel of normal maps points upwards.
    frame.b = cross(n, frame.t);
    if (dot(frame.b, dp_dv) > 0.0)
    {
        frame.b = -frame.b;
    }
    return frame;
}

/// The normal of a tangent space normal map in the frame.
float3 _sample_normal_map(PbrMaterial material, SamplerState sampler, uint texture, float scale, float2 uv, PbrTangentFrame frame)
{
    float3 ts = _sample(texture, sampler, uv).xyz * 2.0 - 1.0;
    if ((material.flags & PBR_MATERIAL_FLAG_DIRECTX_NORMALS) != 0u)
    {
        ts.y = -ts.y;
    }
    ts.xy *= scale;
    return _safe_normalize(frame.t * ts.x + frame.b * ts.y + frame.n * ts.z);
}

///
/// The tangent and the bitangent of an anisotropic lobe around the (perturbed) normal n: The tangent of the frame,
/// turned towards its bitangent by the direction (in tangent space) and then by the rotation (degrees).
/// Orthonormalized around n as specified by OpenPBR (Gram-Schmidt). The sign of the bitangent does not matter, the
/// lobe is symmetric.
///
void _make_anisotropy_frame(PbrTangentFrame frame, float3 n, float2 direction, float rotation_deg, out float3 t, out float3 b)
{
    float3 dir = frame.t * direction.x + frame.b * direction.y;
    // The direction turned by a quarter towards the bitangent.
    float3 perpendicular = frame.b * direction.x - frame.t * direction.y;
    float s, c;
    sincos(radians(rotation_deg), s, c);
    dir = c * dir + s * perpendicular;

    t = dir - n * dot(n, dir);
    if (dot(t, t) < 1e-8)
    {
        // Degenerated: Any direction orthogonal to the normal.
        t = abs(n.y) < 0.99 ? cross(float3(0.0, 1.0, 0.0), n) : cross(float3(1.0, 0.0, 0.0), n);
    }
    t = normalize(t);
    b = cross(n, t);
}

/// Specular reflectance at normal incidence of an interface from the incident into the transmitted medium.
float3 _ior_to_f0(float3 transmitted_ior, float incident_ior)
{
    float3 r = (transmitted_ior - incident_ior) / (transmitted_ior + incident_ior);
    return r * r;
}

/// Index of refraction of the medium that has the given reflectance at normal incidence against air.
float3 _f0_to_ior(float3 f0)
{
    float3 s = sqrt(clamp(f0, 0.0, 0.9999));
    return (1.0 + s) / (1.0 - s);
}

///
/// The reflectance at normal incidence of an interface with the relative index of refraction eta, modulated by the
/// specular weight: F0' = min(weight * F0, 1). The returned eta is the one of an interface with that reflectance, on the
/// same side of one as eta. [OpenPBR, "Dielectric base"]
///
float _modulate_f0(float eta, float weight, out float eta_modulated)
{
    float f0 = _ior_to_f0(float3(eta, eta, eta), 1.0).x;
    float f0_modulated = clamp(weight * f0, 0.0, 0.9999);
    float epsilon = (eta >= 1.0 ? 1.0 : -1.0) * sqrt(f0_modulated);
    eta_modulated = (1.0 + epsilon) / (1.0 - epsilon);
    return f0_modulated;
}

/// GGX alphas of the lobe with the perceptual roughness and the anisotropy. The mapping of OpenPBR keeps the area of
/// the lobe about the same as the isotropic one.
void _make_alphas(float roughness, float anisotropy, out float alpha, out float alpha_t, out float alpha_b, out float alpha_area)
{
    const float min_alpha = MIN_PERCEPTUAL_ROUGHNESS * MIN_PERCEPTUAL_ROUGHNESS;
    alpha = roughness * roughness;
    alpha_t = alpha;
    alpha_b = alpha;
    if (anisotropy > 0.0)
    {
        float a = 1.0 - saturate(anisotropy);
        float t = alpha * sqrt(2.0 / (1.0 + a * a));
        alpha_t = max(t, min_alpha);
        alpha_b = max(a * t, min_alpha);
    }
    alpha_area = sqrt(alpha_t * alpha_b);
}

PbrShadingContext _make_shading_context(PbrMaterial material, SamplerState sampler, PbrSurface surface
    , float3 world_pos, float3 geometric_normal, float2 uv, float4 tangent, bool translucent
    )
{
    float3 cam_pos = float3(world_data.cam_pos_x, world_data.cam_pos_y, world_data.cam_pos_z);

    PbrShadingContext ctx;
    ctx.world_pos = world_pos;
    ctx.v = normalize(cam_pos - world_pos);
    ctx.n_geometric = geometric_normal;
    ctx.thin_walled = pbr_material_is_thin_walled(material);
    ctx.translucent = translucent;

    // Weights of the layers.

    ctx.coat_weight = saturate(material.coat_weight);
    ctx.fuzz_weight = saturate(material.fuzz_weight);
    ctx.metalness = surface.metalness;
    ctx.transmission_weight = saturate(material.transmission_weight);

    // Geometry. Every branch on the material is uniform for the whole draw call.

    bool normal_mapped = material.normal_texture != INVALID_INDEX && material.normal_scale > 0.0;
    bool coat_normal_mapped = material.coat_normal_texture != INVALID_INDEX && material.coat_normal_scale > 0.0
        && ctx.coat_weight > 0.0;
#if JAFG_WITH_EDITOR
    normal_mapped = normal_mapped && _should_show(WORLD_FLAG_SHOW_NORMAL);
    coat_normal_mapped = coat_normal_mapped && _should_show(WORLD_FLAG_SHOW_NORMAL);
#endif /* JAFG_WITH_EDITOR */
    ctx.anisotropic = surface.specular_anisotropy > 0.0;
    ctx.coat_anisotropic = ctx.coat_weight > 0.0 && material.coat_roughness_anisotropy > 0.0;

    ctx.n = geometric_normal;
    ctx.n_coat = geometric_normal;
    ctx.t = float3(0.0, 0.0, 0.0);
    ctx.b = float3(0.0, 0.0, 0.0);
    ctx.t_coat = float3(0.0, 0.0, 0.0);
    ctx.b_coat = float3(0.0, 0.0, 0.0);
    if (normal_mapped || coat_normal_mapped || ctx.anisotropic || ctx.coat_anisotropic)
    {
        PbrTangentFrame frame = _make_tangent_frame(world_pos, geometric_normal, uv, tangent);

        if (normal_mapped)
        {
            ctx.n = _sample_normal_map(material, sampler, material.normal_texture, material.normal_scale, uv, frame);
        }
        if (coat_normal_mapped)
        {
            ctx.n_coat = _sample_normal_map(material, sampler, material.coat_normal_texture, material.coat_normal_scale, uv, frame);
        }
        if (ctx.anisotropic)
        {
            _make_anisotropy_frame(frame, ctx.n, surface.anisotropy_direction, material.geometry_tangent_rotation, ctx.t, ctx.b);
        }
        if (ctx.coat_anisotropic)
        {
            _make_anisotropy_frame(frame, ctx.n_coat, float2(1.0, 0.0), material.geometry_coat_tangent_rotation, ctx.t_coat, ctx.b_coat);
        }
    }

    ctx.r = reflect(-ctx.v, ctx.n);
    // Clamp to avoid the division by zero in the visibility term for surfaces seen exactly edge-on.
    ctx.n_dot_v = clamp(dot(ctx.n, ctx.v), 1e-4, 1.0);
    ctx.r_coat = reflect(-ctx.v, ctx.n_coat);
    ctx.n_dot_v_coat = clamp(dot(ctx.n_coat, ctx.v), 1e-4, 1.0);

    // Coat. Its lobe is a smooth dielectric interface that is never tinted.

    ctx.coat_color = saturate(material.coat_color);
    ctx.coat_ior = max(material.coat_ior, 1e-3);
    ctx.coat_f0 = _ior_to_f0(float3(ctx.coat_ior, ctx.coat_ior, ctx.coat_ior), 1.0).x;
    ctx.coat_roughness = max(saturate(material.coat_roughness), MIN_PERCEPTUAL_ROUGHNESS);
    _make_alphas(ctx.coat_roughness, ctx.coat_anisotropic ? material.coat_roughness_anisotropy : 0.0
        , ctx.coat_alpha, ctx.coat_alpha_t, ctx.coat_alpha_b, ctx.coat_alpha_area);
    float2 coat_env = _env_brdf_approx(ctx.coat_roughness, ctx.n_dot_v_coat);
    ctx.e_coat = ctx.coat_weight > 0.0 ? ctx.coat_f0 * coat_env.x + coat_env.y : 0.0;
    // Snell's law into the coat.
    ctx.coat_mu_v = sqrt(saturate(1.0 - (1.0 - ctx.n_dot_v_coat * ctx.n_dot_v_coat) / (ctx.coat_ior * ctx.coat_ior)));

    // Specular lobe of the base. A coat roughens what is below it.

    float base_roughness = surface.specular_roughness;
    if (ctx.coat_weight > 0.0)
    {
        float rb4 = base_roughness * base_roughness * base_roughness * base_roughness;
        float rc4 = material.coat_roughness * material.coat_roughness * material.coat_roughness * material.coat_roughness;
        base_roughness = lerp(base_roughness, pow(min(1.0, rb4 + 2.0 * rc4), 0.25), ctx.coat_weight);
    }
    ctx.roughness = max(base_roughness, MIN_PERCEPTUAL_ROUGHNESS);
    _make_alphas(ctx.roughness, ctx.anisotropic ? surface.specular_anisotropy : 0.0
        , ctx.alpha, ctx.alpha_t, ctx.alpha_b, ctx.alpha_area);

    // Base.

    ctx.diffuse_albedo = surface.base_color;
    ctx.diffuse_roughness = surface.diffuse_roughness;

    ctx.subsurface_weight = surface.subsurface_weight;
    ctx.subsurface_color = surface.subsurface_color;
    ctx.subsurface_anisotropy = clamp(material.subsurface_scatter_anisotropy, -0.95, 0.95);
    ctx.subsurface_wrap = float3(0.0, 0.0, 0.0);
    ctx.n_subsurface = ctx.n;
    ctx.shadow_offset = 0.0;
    if (ctx.subsurface_weight > 0.0 && !ctx.thin_walled)
    {
        // How far the light travels inside relative to the radius of the curvature of the surface. The curvature is
        // the change of the normal per change of the position, so a sphere of radius R has a curvature of 1 / R.
        float3 dn = abs(ddx(geometric_normal)) + abs(ddy(geometric_normal));
        float3 dp = abs(ddx(world_pos)) + abs(ddy(world_pos));
        float curvature = length(dn) / max(length(dp), 1e-6);

        // Forward scattering carries the light deeper before it turns around: The transport mean free path.
        float3 mean_free_path = max(material.subsurface_radius, 0.0) * CM_TO_M * saturate(material.subsurface_radius_scale)
            / (1.0 - ctx.subsurface_anisotropy);
        ctx.subsurface_wrap = saturate(mean_free_path * curvature);

        // A sphere lights up to sin(a) = wrap behind its terminator. Without the offset, the shadow map would cut
        // that light off again. This lifts the lookup just far enough to leave the shadow of the sphere there.
        float wrap = min(max(ctx.subsurface_wrap.r, max(ctx.subsurface_wrap.g, ctx.subsurface_wrap.b)), 0.75);
        ctx.shadow_offset = ctx.subsurface_weight * (rsqrt(1.0 - wrap * wrap) - 1.0) / max(curvature, 1e-3);

        // The light that diffuses below the surface does not see its small bumps.
        ctx.n_subsurface = _safe_normalize(lerp(ctx.n, geometric_normal, saturate(wrap * 4.0)));
    }
    ctx.diffuse_color = lerp(surface.base_color, ctx.subsurface_color, ctx.subsurface_weight) * (1.0 - ctx.transmission_weight);

    // Specular reflectance. The base is seen through the coat, which changes the contrast of the interface.

    ctx.specular_weight = surface.specular_weight;
    ctx.specular_tint = surface.specular_color;

    float ior = max(material.specular_ior, 1e-3);
    float eta_coat = ior / ctx.coat_ior;
    // A coat that is optically denser than the base would totally reflect. Use the inverse contrast instead.
    if (ctx.coat_ior > ior)
    {
        eta_coat = ctx.coat_ior / ior;
    }
    float eta_surface = lerp(ior, eta_coat, ctx.coat_weight);
    float eta_unused;
    ctx.f0_dielectric = _modulate_f0(eta_surface, ctx.specular_weight, eta_unused);
    // The one at grazing incidence fades out with very low reflectances, so that there is no specular at all at zero
    // weight (or an index of refraction of one).
    ctx.f90_dielectric = saturate(50.0 * ctx.f0_dielectric);
    // The transmitted light refracts into the base from the outside, regardless of the coat.
    _modulate_f0(ior, ctx.specular_weight, ctx.eta);

    ctx.f0_metal = surface.base_color;
    ctx.f82_tint = surface.specular_color;

    ctx.thin_film_weight = surface.thin_film_weight;
    ctx.thin_film_dielectric = float3(0.0, 0.0, 0.0);
    ctx.thin_film_metal = float3(0.0, 0.0, 0.0);
    if (ctx.thin_film_weight > 0.0)
    {
        // Evaluated once for the view direction instead of per light. Cheaper and indistinguishable.
        float thickness_nm = surface.thin_film_thickness * 1000.0;
        float film_ior = max(material.thin_film_ior, 1.0);
        if (ctx.metalness < 1.0)
        {
            float3 f0 = float3(ctx.f0_dielectric, ctx.f0_dielectric, ctx.f0_dielectric);
            ctx.thin_film_dielectric = _thin_film_fresnel(1.0, film_ior, ctx.n_dot_v, thickness_nm, f0);
        }
        if (ctx.metalness > 0.0)
        {
            ctx.thin_film_metal = _thin_film_fresnel(1.0, film_ior, ctx.n_dot_v, thickness_nm, ctx.f0_metal);
        }
    }

    // Multiple scattering. The lobe with a reflectance of one reflects env_brdf.x + env_brdf.y, the rest would
    // have been reflected again by further microfacets. [Fdez-Aguera 2019, "A Multiple-Scattering Microfacet Model
    // for Real-Time Image-based Lighting"], [Turquin 2019, "Practical multiple scattering compensation for
    // microfacet models"]
    ctx.env_brdf = _env_brdf_approx(ctx.roughness, ctx.n_dot_v);
    float single_scattering = max(ctx.env_brdf.x + ctx.env_brdf.y, 0.05);
    float3 f0 = lerp(ctx.specular_tint * ctx.f0_dielectric, ctx.f0_metal * min(ctx.specular_weight, 1.0), ctx.metalness);
    ctx.energy_compensation = 1.0 + f0 * (1.0 / single_scattering - 1.0);

    // The directional albedo of the dielectric specular lobe. The thin film is already evaluated for v.
    float lobe = ctx.env_brdf.x + ctx.env_brdf.y;
    ctx.e_dielectric = saturate(lerp(ctx.f0_dielectric * ctx.env_brdf.x + ctx.f90_dielectric * ctx.env_brdf.y
        , ctx.thin_film_dielectric * lobe, ctx.thin_film_weight));

    // Coat darkening: Light that is reflected back by the base is partly reflected again by the coat (internally),
    // which darkens and saturates the base. [OpenPBR, "Coat darkening"]
    ctx.coat_darkening = float3(1.0, 1.0, 1.0);
    if (ctx.coat_weight > 0.0 && material.coat_darkening > 0.0)
    {
        // The albedo of the base at normal incidence.
        float3 e_dielectric_base = ctx.specular_tint * ctx.f0_dielectric + (1.0 - ctx.f0_dielectric) * ctx.diffuse_color;
        float3 e_base = lerp(e_dielectric_base, ctx.f0_metal * min(ctx.specular_weight, 1.0), ctx.metalness);
        // How much of the light that goes up inside of the coat it reflects back down: Diffuse (hemispherical
        // average of the Fresnel term from inside) for rough bases, specular (Fresnel at the view angle) for smooth.
        float e_fresnel = ctx.coat_f0 + (1.0 - ctx.coat_f0) / 21.0;
        float k_rough = 1.0 - (1.0 - e_fresnel) / (ctx.coat_ior * ctx.coat_ior);
        float k_smooth = _f_schlick(float3(ctx.coat_f0, ctx.coat_f0, ctx.coat_f0), ctx.n_dot_v_coat).x;
        float r_dielectric = lerp(1.0, surface.specular_roughness, saturate(ctx.f0_dielectric));
        float r_base = lerp(r_dielectric, surface.specular_roughness, ctx.metalness);
        float k = lerp(k_smooth, k_rough, r_base);
        float3 delta = (1.0 - k) / (1.0 - saturate(e_base) * k);
        ctx.coat_darkening = lerp(float3(1.0, 1.0, 1.0), delta, ctx.coat_weight * saturate(material.coat_darkening));
    }

    // Fuzz. On top of everything, its normal is between the one of the base and of the coat.

    ctx.fuzz_color = saturate(material.fuzz_color);
    ctx.fuzz_roughness = clamp(material.fuzz_roughness, 0.01, 1.0);
    ctx.n_fuzz = _safe_normalize(lerp(ctx.n, ctx.n_coat, ctx.coat_weight));
    ctx.n_dot_v_fuzz = clamp(dot(ctx.n_fuzz, ctx.v), 1e-4, 1.0);
    ctx.e_fuzz = ctx.fuzz_weight > 0.0 ? saturate(_zeltner_sheen_dir_albedo(ctx.n_dot_v_fuzz, ctx.fuzz_roughness)) : 0.0;

    return ctx;
}

//////////////////////////////////////////////////////////////////////////
// BRDF (Cook-Torrance, GGX / Smith height-correlated / Schlick).
//////////////////////////////////////////////////////////////////////////

/// GGX / Trowbridge-Reitz normal distribution function.
float _d_ggx(float n_dot_h, float alpha)
{
    float a2 = alpha * alpha;
    float d = (n_dot_h * a2 - n_dot_h) * n_dot_h + 1.0;
    return a2 / (PI * d * d);
}

///
/// Height-correlated Smith-GGX visibility term. This already includes the 1 / (4 * n_dot_l * n_dot_v)
/// denominator of the Cook-Torrance microfacet BRDF.
///
float _v_smith_ggx_correlated(float n_dot_v, float n_dot_l, float alpha)
{
    float a2 = alpha * alpha;
    float ggx_v = n_dot_l * sqrt(n_dot_v * n_dot_v * (1.0 - a2) + a2);
    float ggx_l = n_dot_v * sqrt(n_dot_l * n_dot_l * (1.0 - a2) + a2);
    return 0.5 / max(ggx_v + ggx_l, 1e-5);
}

/// Anisotropic GGX normal distribution function. t and b are the tangent and the bitangent.
float _d_ggx_anisotropic(float n_dot_h, float t_dot_h, float b_dot_h, float alpha_t, float alpha_b)
{
    float a2 = alpha_t * alpha_b;
    float3 d = float3(alpha_b * t_dot_h, alpha_t * b_dot_h, a2 * n_dot_h);
    float d2 = dot(d, d);
    float b2 = a2 / max(d2, 1e-12);
    return a2 * b2 * b2 / PI;
}

/// Anisotropic version of _v_smith_ggx_correlated.
float _v_smith_ggx_correlated_anisotropic(float alpha_t, float alpha_b
    , float t_dot_v, float b_dot_v, float n_dot_v
    , float t_dot_l, float b_dot_l, float n_dot_l
    )
{
    float lambda_v = n_dot_l * length(float3(alpha_t * t_dot_v, alpha_b * b_dot_v, n_dot_v));
    float lambda_l = n_dot_v * length(float3(alpha_t * t_dot_l, alpha_b * b_dot_l, n_dot_l));
    return 0.5 / max(lambda_v + lambda_l, 1e-5);
}

/// D * V of a (possibly anisotropic) GGX lobe around n for the directions v and l.
float _ggx_lobe(float3 n, float3 t, float3 b, bool anisotropic, float alpha, float alpha_t, float alpha_b
    , float3 v, float3 l, float3 h, float n_dot_v, float n_dot_l
    )
{
    float n_dot_h = saturate(dot(n, h));
    if (anisotropic)
    {
        float d = _d_ggx_anisotropic(n_dot_h, dot(t, h), dot(b, h), alpha_t, alpha_b);
        float vis = _v_smith_ggx_correlated_anisotropic(alpha_t, alpha_b
            , dot(t, v), dot(b, v), n_dot_v
            , dot(t, l), dot(b, l), n_dot_l
            );
        return d * vis;
    }
    return _d_ggx(n_dot_h, alpha) * _v_smith_ggx_correlated(n_dot_v, n_dot_l, alpha);
}

/// Schlick's approximation of the Fresnel term.
float3 _f_schlick(float3 f0, float v_dot_h)
{
    float f = pow(1.0 - v_dot_h, 5.0);
    return f0 + (1.0 - f0) * f;
}

/// Schlick's approximation of the Fresnel term with a custom reflectance at grazing incidence.
float3 _f_schlick(float3 f0, float3 f90, float v_dot_h)
{
    float f = pow(1.0 - v_dot_h, 5.0);
    return f0 + (f90 - f0) * f;
}

///
/// Fresnel term of metals. Schlick's approximation that is bent down near grazing incidence, so that it matches the
/// given tint at the angle where real metals have their dip (acos(1/7), about 82 degrees).
/// [Hoffman 2023, "Generalization of Adobe's Fresnel Model"], as specified by OpenPBR.
///
float3 _f_schlick_f82(float3 f0, float3 f82_tint, float v_dot_h)
{
    const float mu_bar = 1.0 / 7.0;
    const float denominator = 0.056652780; // mu_bar * (1 - mu_bar)^6
    float3 f_schlick_bar = _f_schlick(f0, mu_bar);
    float3 b = f_schlick_bar * (1.0 - f82_tint) / denominator;

    float one_minus = 1.0 - v_dot_h;
    float one_minus_6 = one_minus * one_minus * one_minus;
    one_minus_6 *= one_minus_6;
    return saturate(_f_schlick(f0, v_dot_h) - b * v_dot_h * one_minus_6);
}

/// Fresnel term of the dielectric without its (non-physical) tint. What it does not reflect enters the base.
float3 _fresnel_dielectric(PbrShadingContext ctx, float v_dot_h)
{
    float3 f0 = float3(ctx.f0_dielectric, ctx.f0_dielectric, ctx.f0_dielectric);
    float3 f90 = float3(ctx.f90_dielectric, ctx.f90_dielectric, ctx.f90_dielectric);
    return lerp(_f_schlick(f0, f90, v_dot_h), ctx.thin_film_dielectric, ctx.thin_film_weight);
}

float3 _fresnel_metal(PbrShadingContext ctx, float v_dot_h)
{
    float3 f = lerp(_f_schlick_f82(ctx.f0_metal, ctx.f82_tint, v_dot_h), ctx.thin_film_metal, ctx.thin_film_weight);
    return f * min(ctx.specular_weight, 1.0);
}

/// Fresnel term of the coat.
float _fresnel_coat(PbrShadingContext ctx, float v_dot_h)
{
    return _f_schlick(float3(ctx.coat_f0, ctx.coat_f0, ctx.coat_f0), v_dot_h).x;
}

//////////////////////////////////////////////////////////////////////////
// Diffuse BRDF and subsurface.
//////////////////////////////////////////////////////////////////////////

static const float FON_C1 = 0.5 - 2.0 / (3.0 * PI);
static const float FON_C2 = 2.0 / 3.0 - 28.0 / (15.0 * PI);

/// Approximated directional albedo of the Fujii Oren-Nayar BRDF for a white surface.
float _e_fon_approx(float mu, float r)
{
    float mu_comp = 1.0 - mu;
    const float g1 = 0.0571085289;
    const float g2 = 0.491881867;
    const float g3 = -0.332181442;
    const float g4 = 0.0714429953;
    float g_over_pi = mu_comp * (g1 + mu_comp * (g2 + mu_comp * (g3 + mu_comp * g4)));
    return (1.0 + r * g_over_pi) / (1.0 + FON_C1 * r);
}

///
/// Energy-preserving Oren-Nayar BRDF with the albedo rho and the roughness r. Equals Lambert for r = 0. The single
/// scattering of the Fujii Oren-Nayar model plus a lobe for the energy that multiple scattering adds.
/// [Portsmouth et al. 2025, "EON: A practical energy-preserving rough diffuse BRDF"], as specified by OpenPBR.
///
float3 _f_eon(float3 rho, float r, float n_dot_l, float n_dot_v, float l_dot_v)
{
    float s = l_dot_v - n_dot_l * n_dot_v;
    float s_over_t = s > 0.0 ? s / max(max(n_dot_l, n_dot_v), 1e-6) : s;
    float a = 1.0 / (1.0 + FON_C1 * r);
    float3 f_ss = (rho / PI) * a * (1.0 + r * s_over_t);

    float e_o = _e_fon_approx(n_dot_v, r);
    float e_i = _e_fon_approx(n_dot_l, r);
    float e_avg = a * (1.0 + FON_C2 * r);
    float3 rho_ms = (rho * rho) * e_avg / (1.0 - rho * (1.0 - e_avg));
    const float eps = 1e-7;
    float3 f_ms = (rho_ms / PI) * max(eps, 1.0 - e_o) * max(eps, 1.0 - e_i) / max(eps, 1.0 - e_avg);

    return f_ss + f_ms;
}

/// The diffuse BRDF of the given albedo: Energy-preserving Oren-Nayar, or Lambert if smooth.
float3 _f_diffuse(PbrShadingContext ctx, float3 albedo, float3 n, float3 l)
{
    if (ctx.diffuse_roughness > 0.0)
    {
        return _f_eon(albedo, ctx.diffuse_roughness, saturate(dot(n, l)), saturate(dot(n, ctx.v)), dot(l, ctx.v));
    }
    return albedo / PI;
}

///
/// The diffuse of the subsurface times the cosine. Instead of diffusing the light across the surface, it wraps
/// around the terminator, per color channel as far as the light travels inside relative to the curvature. The
/// wrapping keeps the energy of a lit sphere.
/// cos_l is the unclamped cosine of the angle of incidence, n_dot_l the one with the horizon handling of area lights.
///
float3 _subsurface_diffuse(PbrShadingContext ctx, float cos_l, float n_dot_l)
{
    float3 w = ctx.subsurface_wrap;
    float3 wrapped = saturate((cos_l + w) / ((1.0 + w) * (1.0 + w)));
    // Hands over to the area light handling where nothing is wrapped.
    float3 cosine = lerp(float3(n_dot_l, n_dot_l, n_dot_l), wrapped, saturate(w * 20.0));
    return ctx.subsurface_color / PI * cosine;
}

//////////////////////////////////////////////////////////////////////////
// Fuzz.
//////////////////////////////////////////////////////////////////////////

///
/// Directional albedo of the sheen of the microflake fuzz for the cosine of the view mu and the roughness r. Gaussian
/// fit to the tabulated albedo. [Zeltner et al. 2022, "Practical Multiple-Scattering Sheen Using Linearly Transformed
/// Cosines"], as specified by OpenPBR, in the form of MaterialX.
///
float _zeltner_sheen_dir_albedo(float mu, float r)
{
    float s = r * (0.0206607 + 1.58491 * r) / (0.0379424 + r * (1.32227 + r));
    float m = r * (-0.193854 + r * (-1.14885 + r * (1.7932 - 0.95943 * r * r))) / (0.046391 + r);
    float o = r * (0.000654023 + (-0.0207818 + 0.119681 * r) * r) / (1.26264 + r * (-1.92021 + r));
    float x = (mu - m) / s;
    return exp(-0.5 * x * x) / (s * sqrt(2.0 * PI)) + o;
}

/// Rational fits to the coefficients of the inverse matrix of the linearly transformed cosine.
float _zeltner_sheen_ltc_a_inv(float mu, float r)
{
    return (2.58126 * mu + 0.813703 * r) * r / (1.0 + 0.310327 * mu * mu + 2.60994 * mu * r);
}

float _zeltner_sheen_ltc_b_inv(float mu, float r)
{
    return sqrt(1.0 - mu) * (r - 1.0) * r * r * r / (0.0000254053 + 1.71228 * mu - 1.71506 * mu * r + 1.34174 * r * r);
}

///
/// The distribution of the light that the fuzz reflects towards v, times the cosine of the angle of incidence. Its
/// integral over all directions is one, the directional albedo is applied by the caller.
///
float _zeltner_sheen(float3 l, float3 v, float3 n, float n_dot_v, float r)
{
    // A basis with x in the plane of v and n, which orients the lobe.
    float3 x = v - n * n_dot_v;
    float len_sqr = dot(x, x);
    x = len_sqr > 1e-8 ? x * rsqrt(len_sqr) : (abs(n.y) < 0.99 ? normalize(cross(float3(0.0, 1.0, 0.0), n)) : normalize(cross(float3(1.0, 0.0, 0.0), n)));
    float3 y = cross(n, x);
    float3 w = float3(dot(l, x), dot(l, y), dot(l, n));

    float a_inv = _zeltner_sheen_ltc_a_inv(n_dot_v, r);
    float b_inv = _zeltner_sheen_ltc_b_inv(n_dot_v, r);

    // Back into the original configuration (a clamped cosine).
    float3 wo = float3(a_inv * w.x + b_inv * w.z, a_inv * w.y, w.z);
    float wo_len_sqr = dot(wo, wo);

    // D(w) = Do(wo) * |M^-1| / dot(wo, wo)^2 with Do the cosine distribution.
    float a = a_inv / wo_len_sqr;
    return max(wo.z, 0.0) / PI * a * a;
}

//////////////////////////////////////////////////////////////////////////
// Thin film iridescence.
//////////////////////////////////////////////////////////////////////////

/// The XYZ color matching functions in Fourier space at the given optical path difference (nm), as linear Rec. 709.
float3 _thin_film_sensitivity(float opd, float3 shift)
{
    float phase = 2.0 * PI * opd * 1.0e-9;
    const float3 val = float3(5.4856e-13, 4.4201e-13, 5.2481e-13);
    const float3 pos = float3(1.6810e+06, 1.7953e+06, 2.2084e+06);
    const float3 var = float3(4.3278e+09, 9.3046e+09, 6.6121e+09);

    float3 xyz = val * sqrt(2.0 * PI * var) * cos(pos * phase + shift) * exp(-phase * phase * var);
    xyz.x += 9.7470e-14 * sqrt(2.0 * PI * 4.5282e+09) * cos(2.2399e+06 * phase + shift.x) * exp(-4.5282e+09 * phase * phase);
    xyz /= 1.0685e-7;

    const float3x3 xyz_to_rec709 = float3x3(
         3.2404542, -1.5371385, -0.4985314,
        -0.9692660,  1.8760108,  0.0415560,
         0.0556434, -0.2040259,  1.0572252
        );
    return mul(xyz_to_rec709, xyz);
}

///
/// Reflectance of a thin film (Airy summation of the first orders) on top of a base with the given reflectance.
/// [Belcour & Barla 2017, "A Practical Extension to Microfacet Theory for the Modeling of Varying Iridescence"],
/// in the form of the glTF reference viewer (KHR_materials_iridescence).
///
float3 _thin_film_fresnel(float outside_ior, float film_ior, float cos_theta1, float thickness_nm, float3 base_f0)
{
    // The film vanishes towards zero thickness.
    film_ior = lerp(outside_ior, film_ior, smoothstep(0.0, 0.03, thickness_nm));

    // Snell's law into the film.
    float sin_theta2_sqr = (outside_ior / film_ior) * (outside_ior / film_ior) * (1.0 - cos_theta1 * cos_theta1);
    float cos_theta2_sqr = 1.0 - sin_theta2_sqr;
    if (cos_theta2_sqr < 0.0)
    {
        // Total internal reflection.
        return float3(1.0, 1.0, 1.0);
    }
    float cos_theta2 = sqrt(cos_theta2_sqr);

    // First interface (outside to film).
    float r12 = _f_schlick(_ior_to_f0(float3(film_ior, film_ior, film_ior), outside_ior), cos_theta1).x;
    float t121 = 1.0 - r12;
    float phi12 = film_ior < outside_ior ? PI : 0.0;
    float phi21 = PI - phi12;

    // Second interface (film to base).
    float3 base_ior = _f0_to_ior(base_f0);
    float3 r23 = _f_schlick(_ior_to_f0(base_ior, film_ior), cos_theta2);
    float3 phi23 = PI * (1.0 - step(float3(film_ior, film_ior, film_ior), base_ior));

    float opd = 2.0 * film_ior * thickness_nm * cos_theta2;
    float3 phi = phi21 + phi23;

    float3 r123 = clamp(r12 * r23, 1e-5, 0.9999);
    float3 r123_sqrt = sqrt(r123);
    float3 rs = t121 * t121 * r23 / (1.0 - r123);

    // The order zero, then pairs of diracs.
    float3 result = r12 + rs;
    float3 cm = rs - t121;
    [unroll]
    for (int m = 1; m <= 2; ++m)
    {
        cm *= r123_sqrt;
        result += cm * 2.0 * _thin_film_sensitivity(float(m) * opd, float(m) * phi);
    }

    // Out of gamut colors may be negative.
    return max(result, float3(0.0, 0.0, 0.0));
}

/// Normalizes without producing NaNs for degenerated (zero length) vectors.
float3 _safe_normalize(float3 v)
{
    return v * rsqrt(max(dot(v, v), 1e-12));
}

//////////////////////////////////////////////////////////////////////////
// Layering.
//////////////////////////////////////////////////////////////////////////

///
/// Transmittance of the coat along the path of the light with the cosines (inside of the coat) of the incident and
/// the outgoing direction. The coat color is the transmittance in and out at normal incidence.
///
float3 _coat_absorption(PbrShadingContext ctx, float mu_in, float mu_out)
{
    float exponent = 0.5 * (1.0 / max(mu_in, 0.05) + 1.0 / max(mu_out, 0.05));
    return pow(max(ctx.coat_color, 1e-4), exponent);
}

/// Cosine of the direction with the cosine mu against the coat normal, refracted into the coat.
float _coat_refracted_cosine(PbrShadingContext ctx, float mu)
{
    return sqrt(saturate(1.0 - (1.0 - mu * mu) / (ctx.coat_ior * ctx.coat_ior)));
}

/// What reaches the base through the coat and the fuzz (and back), as seen from v, without the absorption of the coat.
float3 _layers_view_transmittance(PbrShadingContext ctx)
{
    float coat = lerp(1.0, 1.0 - ctx.e_coat, ctx.coat_weight);
    float fuzz = 1.0 - ctx.fuzz_weight * ctx.e_fuzz;
    return float3(coat, coat, coat) * fuzz;
}

///
/// How much of the emission of the base gets through the coat and the fuzz towards v. The coat absorbs along the
/// way out, the fuzz scatters it away. [OpenPBR, "Emission"]
///
float3 _emission_transmittance(PbrShadingContext ctx)
{
    float3 coat = lerp(float3(1.0, 1.0, 1.0), pow(max(ctx.coat_color, 1e-4), 0.5 / max(ctx.coat_mu_v, 0.05)), ctx.coat_weight);
    return coat * (1.0 - ctx.fuzz_weight * ctx.e_fuzz);
}

///
/// Evaluates the BRDF times the cosine of the angle of incidence for the light that arrives at the surface.
/// Returns the radiance reflected towards the camera.
///
float3 _evaluate_incident_light(PbrShadingContext ctx, PbrIncidentLight incident, float specular_scale)
{
    float3 base = float3(0.0, 0.0, 0.0);

    // Diffuse and subsurface of the dielectric. What transmits into the volume is not diffused.
    // The subsurface also receives light from behind the terminator, thin walls through their back.
    float diffuse_weight = (1.0 - ctx.metalness) * (1.0 - ctx.transmission_weight);
    if (diffuse_weight > 0.0 && (incident.n_dot_l > 0.0 || ctx.subsurface_weight > 0.0))
    {
        float cos_l = dot(ctx.n, incident.l);

        float3 diffuse = float3(0.0, 0.0, 0.0);
        if (incident.n_dot_l > 0.0)
        {
            diffuse = _f_diffuse(ctx, ctx.diffuse_albedo, ctx.n, incident.l) * incident.n_dot_l;
        }
        if (ctx.subsurface_weight > 0.0)
        {
            float3 subsurface;
            if (ctx.thin_walled)
            {
                // A sheet that splits the scattered light between both of its sides. Forward scattering lets
                // more through. [OpenPBR, "Thin-walled subsurface"]
                // The lobe of the light from behind is the one of the front, mirrored through the sheet.
                float g = ctx.subsurface_anisotropy;
                float3 reflected = _f_diffuse(ctx, ctx.subsurface_color, ctx.n, incident.l) * incident.n_dot_l;
                float3 l_mirrored = incident.l - 2.0 * cos_l * ctx.n;
                float3 through = _f_diffuse(ctx, ctx.subsurface_color, ctx.n, l_mirrored) * saturate(-cos_l);
                subsurface = 0.5 * (1.0 - g) * reflected + 0.5 * (1.0 + g) * through;
            }
            else
            {
                subsurface = _subsurface_diffuse(ctx, dot(ctx.n_subsurface, incident.l), incident.n_dot_l);
            }
            diffuse = lerp(diffuse, subsurface, ctx.subsurface_weight);
        }

        // The energy reflected by the dielectric cannot be diffused anymore.
        float3 h = _safe_normalize(ctx.v + incident.l);
        float3 transmitted = 1.0 - _fresnel_dielectric(ctx, saturate(dot(ctx.v, h)));

        base += transmitted * diffuse * diffuse_weight;
    }

    // Specular of the dielectric and of the metal.
    float n_dot_l = saturate(dot(ctx.n, incident.l_specular));
    if (n_dot_l > 0.0 && specular_scale > 0.0)
    {
        float3 l = incident.l_specular;
        float3 h = _safe_normalize(ctx.v + l);
        float v_dot_h = saturate(dot(ctx.v, h));

        float lobe = _ggx_lobe(ctx.n, ctx.t, ctx.b, ctx.anisotropic, ctx.alpha, ctx.alpha_t, ctx.alpha_b
            , ctx.v, l, h, ctx.n_dot_v, n_dot_l);
        float3 f = lerp(ctx.specular_tint * _fresnel_dielectric(ctx, v_dot_h), _fresnel_metal(ctx, v_dot_h), ctx.metalness);
        float energy = _area_specular_energy(ctx.alpha_area, incident.sin_a, v_dot_h);

        base += lobe * f * ctx.energy_compensation * (n_dot_l * energy * specular_scale);
    }

    // The coat on top of the base. [OpenPBR, "Coat"]
    float3 result = base;
    if (ctx.coat_weight > 0.0)
    {
        float coat_n_dot_l = saturate(dot(ctx.n_coat, incident.l_coat));
        float3 coat = float3(0.0, 0.0, 0.0);
        if (coat_n_dot_l > 0.0 && specular_scale > 0.0)
        {
            float3 l = incident.l_coat;
            float3 h = _safe_normalize(ctx.v + l);
            float v_dot_h = saturate(dot(ctx.v, h));
            float lobe = _ggx_lobe(ctx.n_coat, ctx.t_coat, ctx.b_coat, ctx.coat_anisotropic, ctx.coat_alpha
                , ctx.coat_alpha_t, ctx.coat_alpha_b, ctx.v, l, h, ctx.n_dot_v_coat, coat_n_dot_l);
            float energy = _area_specular_energy(ctx.coat_alpha_area, incident.sin_a, v_dot_h);
            coat = lobe * _fresnel_coat(ctx, v_dot_h) * (coat_n_dot_l * energy * specular_scale);
        }

        float mu_l = _coat_refracted_cosine(ctx, saturate(dot(ctx.n_coat, incident.l)));
        float3 below = _coat_absorption(ctx, mu_l, ctx.coat_mu_v) * (1.0 - ctx.e_coat) * ctx.coat_darkening;
        result = ctx.coat_weight * coat + lerp(float3(1.0, 1.0, 1.0), below, ctx.coat_weight) * base;
    }

    // The fuzz on top of everything. It does not tint what is below it. [OpenPBR, "Fuzz"]
    if (ctx.fuzz_weight > 0.0)
    {
        float3 fuzz = ctx.fuzz_color * ctx.e_fuzz * _zeltner_sheen(incident.l, ctx.v, ctx.n_fuzz, ctx.n_dot_v_fuzz, ctx.fuzz_roughness);
        result = ctx.fuzz_weight * fuzz + (1.0 - ctx.fuzz_weight * ctx.e_fuzz) * result;
    }

    return result * incident.illuminance;
}

///
/// Energy conservation of the specular term for an area light that subtends the half angle a as seen from the
/// surface. Returns exactly one for punctual lights.
///
/// The most representative point is shaded like a punctual light. That adds energy, as this point stays at the
/// peak of the lobe for as long as the reflection ray hits the source. It is compensated by the ratio of the
/// normalization factors of the GGX lobe and of a GGX lobe that is widened by the angle of the source.
/// [Karis 2013, "Real Shading in Unreal Engine 4"], with the widening Unreal Engine 4 uses since then, which keeps
/// the reflected energy at or slightly below the one of the real area light, also at grazing angles.
///
float _area_specular_energy(float alpha, float sin_a, float v_dot_h)
{
    float a2 = alpha * alpha;
    float widened_a2 = a2 + 0.25 * sin_a * (3.0 * alpha + sin_a) / (v_dot_h + 0.001);
    return a2 / widened_a2;
}

///
/// Analytical approximation of the pre-integrated split-sum environment BRDF.
/// [Karis 2014, "Physically Based Shading on Mobile"]
/// Returns the scale (x) and bias (y) to apply to f0.
///
float2 _env_brdf_approx(float roughness, float n_dot_v)
{
    const float4 c0 = float4(-1.0, -0.0275, -0.572,  0.022);
    const float4 c1 = float4( 1.0,  0.0425,  1.040, -0.040);
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * n_dot_v)) * r.x + r.y;
    return float2(-1.04, 1.04) * a004 + r.zw;
}

/// Reflected radiance for a uniform environment with the given radiance. Stand-in until we have proper IBL.
float3 _evaluate_ambient(PbrShadingContext ctx, float3 radiance)
{
    float2 env = ctx.env_brdf;
    float lobe = env.x + env.y;

    // The directional albedo of the specular lobe. The thin film is already evaluated for the view direction. The
    // tint of metals bends the part of the reflectance that rises towards grazing incidence.
    float3 metal = lerp(ctx.f0_metal * env.x + ctx.f82_tint * env.y
        , ctx.thin_film_metal * lobe, ctx.thin_film_weight) * min(ctx.specular_weight, 1.0);
    float3 specular = lerp(ctx.specular_tint * ctx.e_dielectric, metal, ctx.metalness) * ctx.energy_compensation;

    // The energy reflected by the dielectric cannot be diffused anymore. A uniform environment lights thin walls
    // from both sides, so their subsurface reflects all of its albedo.
    float3 diffuse = ctx.diffuse_color * (1.0 - ctx.e_dielectric) * (1.0 - ctx.metalness);

    float3 result = diffuse + specular;

    if (ctx.coat_weight > 0.0)
    {
        float3 below = _coat_absorption(ctx, ctx.coat_mu_v, ctx.coat_mu_v) * (1.0 - ctx.e_coat) * ctx.coat_darkening;
        result = ctx.coat_weight * ctx.e_coat + lerp(float3(1.0, 1.0, 1.0), below, ctx.coat_weight) * result;
    }
    if (ctx.fuzz_weight > 0.0)
    {
        result = ctx.fuzz_weight * ctx.fuzz_color * ctx.e_fuzz + (1.0 - ctx.fuzz_weight * ctx.e_fuzz) * result;
    }

    return result * radiance;
}

//////////////////////////////////////////////////////////////////////////
// Transmission.
//////////////////////////////////////////////////////////////////////////

///
/// The part of the light from behind the surface that comes through to v: What the dielectric base transmits into its
/// volume (instead of reflecting or diffusing it) and what gets through the coat and the fuzz above.
///
float3 _transmission_weight(PbrShadingContext ctx)
{
    float3 weight = (1.0 - ctx.metalness) * ctx.transmission_weight * (1.0 - ctx.e_dielectric);
    if (ctx.coat_weight > 0.0)
    {
        float3 coat = pow(max(ctx.coat_color, 1e-4), 0.5 / max(ctx.coat_mu_v, 0.05)) * (1.0 - ctx.e_coat);
        weight *= lerp(float3(1.0, 1.0, 1.0), coat, ctx.coat_weight);
    }
    return weight * (1.0 - ctx.fuzz_weight * ctx.e_fuzz);
}

/// Henyey-Greenstein phase function for the cosine of the angle between the directions of propagation.
float _phase_henyey_greenstein(float g, float cos_theta)
{
    float g2 = g * g;
    float d = max(1.0 + g2 - 2.0 * g * cos_theta, 1e-4);
    return (1.0 - g2) / (4.0 * PI * d * sqrt(d));
}

/// Normalized direction from the surface towards the light. Exact for punctual lights, the center for area lights.
float3 _approximate_light_direction(PbrLight light, float3 world_pos)
{
    return light.type == LIGHT_DIRECTIONAL ? -light.direction : _safe_normalize(light.pos - world_pos);
}

/// The scene behind the surface at the uv on the target, blurred by the level of detail.
float3 _sample_scene_color(float2 uv, float lod)
{
    return bindless_textures[NonUniformResourceIndex(world_data.scene_color_texture)]
        .SampleLevel(samplers[JAFG_SAMPLER_LINEAR_CLAMP_TO_EDGE], saturate(uv), lod).rgb;
}

///
/// The light that the volume scatters towards v, per unit of scattered light: The ambient light and the directional
/// lights (with their shadows). The scattering is multiple, so the light diffuses through the volume like through a
/// diffuse surface: Its lit side glows the most, its other side a little through it. Forward scattering lets more of the
/// light through that travels towards v.
///
float3 _volume_inscattering(PbrShadingContext ctx, float g, float3 ambient_radiance)
{
    uint light_count;
    uint light_stride;
    lights.GetDimensions(light_count, light_stride);

    float3 result = ambient_radiance;
    for (uint i = 0; i < light_count; ++i)
    {
        PbrLight light = lights[i];
        if (light.type != LIGHT_DIRECTIONAL || light.intensity <= 0.0 || !_should_show_light_type(light.type))
        {
            continue;
        }
        float visibility = light.shadow != INVALID_INDEX ? _sample_shadow(light, ctx.world_pos, ctx.n_geometric) : 1.0;
        float diffused = saturate(0.5 + 0.5 * dot(ctx.n_geometric, -light.direction));
        float forward = 1.0 + g * dot(light.direction, ctx.v);
        result += light.tint * light.intensity / PI * diffused * forward * visibility;
    }
    return result;
}

///
/// The light that comes through the surface from behind it, before the weight of the transmission.
///
/// background:  Light (radiance) from the scene behind the surface on the target, refracted and absorbed by the volume.
///              Only translucent draws of a view that has a copy of its opaque scene can see it.
/// passthrough: Per channel factor on what is already behind the surface on the target (dual source blending). Thin
///              walls that are smooth do not refract, so they see everything that was drawn before them, also other
///              translucent surfaces.
///
/// The volume is as thick as the distance to the closest back face behind the surface on the target. Its color is
/// what white light becomes after transmission_depth, its scattering turns it cloudy. Refraction uses the index of
/// refraction of the base, per color channel if it has dispersion. [OpenPBR, "Translucent base", "Dispersion"]
///
void _evaluate_transmission(PbrMaterial material, PbrShadingContext ctx, PbrGeometry geometry, float3 ambient_radiance
    , out float3 background, out float3 passthrough)
{
    background = float3(0.0, 0.0, 0.0);
    passthrough = float3(0.0, 0.0, 0.0);

    float3 tint = saturate(material.transmission_color);
    bool has_scene = world_data.scene_color_texture != INVALID_INDEX;

    // Rough transmission blurs what is behind. [glTF reference viewer, KHR_materials_transmission]
    uint width = 1u;
    uint height = 1u;
    uint levels = 1u;
    if (has_scene)
    {
        bindless_textures[NonUniformResourceIndex(world_data.scene_color_texture)].GetDimensions(0u, width, height, levels);
    }
    float max_lod = float(levels - 1u);
    float2 screen_uv = geometry.frag_coord.xy / float2(width, height);

    if (ctx.thin_walled)
    {
        // A thin absorbing sheet: Through both interfaces, with all the light that bounces between them.
        float f = ctx.e_dielectric.g;
        float3 sheet = tint * (1.0 - f) / max(1.0 - f * f * tint * tint, 1e-4);

        float blur = has_scene ? saturate((ctx.roughness - MIN_PERCEPTUAL_ROUGHNESS) * 4.0) : 0.0;
        if (blur > 0.0)
        {
            float lod = log2(float(max(width, height))) * ctx.roughness;
            background = sheet * _sample_scene_color(screen_uv, min(lod, max_lod)) * blur;
        }
        passthrough = sheet * (1.0 - blur);
        return;
    }

    if (!has_scene)
    {
        // Nothing to refract. At least let the scene through.
        passthrough = tint;
        return;
    }

    // Thickness from the back faces on the target. Zero for open meshes without any.
    float thickness = 0.0;
    if (world_data.scene_thickness_texture != INVALID_INDEX)
    {
        float back = bindless_textures[NonUniformResourceIndex(world_data.scene_thickness_texture)]
            .SampleLevel(samplers[JAFG_SAMPLER_NEAREST_CLAMP_TO_EDGE], screen_uv, 0.0).r;
        if (back > 0.0)
        {
            thickness = max(back - _view_depth(ctx.world_pos), 0.0);
        }
    }

    // Per color channel index of refraction. Cauchy's equation fit to the abbe number. [OpenPBR, "Dispersion"]
    float3 iors = float3(ctx.eta, ctx.eta, ctx.eta);
    bool dispersive = material.transmission_dispersion_scale > 0.0;
    if (dispersive)
    {
        const float lambda_c = 0.6563;
        const float lambda_d = 0.5876;
        const float lambda_f = 0.4861;
        const float3 lambda_rgb = float3(0.630, 0.532, 0.465);
        float abbe = max(material.transmission_dispersion_abbe_number, 1e-3) / material.transmission_dispersion_scale;
        float b = (ctx.eta - 1.0) / (abbe * (1.0 / (lambda_f * lambda_f) - 1.0 / (lambda_c * lambda_c)));
        float a = ctx.eta - b / (lambda_d * lambda_d);
        iors = a + b / (lambda_rgb * lambda_rgb);
    }

    // The roughness of the transmission is the one of the specular, its effect grows with the index of refraction.
    float lod = log2(float(max(width, height))) * ctx.roughness * saturate(ctx.eta * 2.0 - 2.0);
    lod = min(lod, max_lod);

    float3 refracted;
    [unroll]
    for (int c = 0; c < 3; ++c)
    {
        float3 dir = refract(-ctx.v, ctx.n, 1.0 / max(iors[c], 1e-3));
        // Total internal reflection: Nothing refracts, the light comes straight through.
        dir = dot(dir, dir) > 1e-8 ? dir : -ctx.v;
        float2 uv = _project_to_screen(ctx.world_pos + dir * thickness);
        float3 color = _sample_scene_color(uv, lod);
        refracted[c] = color[c];
        if (!dispersive)
        {
            refracted = color;
            break;
        }
    }

    float depth = max(material.transmission_depth, 0.0) * CM_TO_M;
    if (depth <= 0.0)
    {
        // No medium inside: The color is a tint only. [OpenPBR, "Translucent base"]
        background = refracted * tint;
        return;
    }

    // Beer-Lambert. White light becomes the transmission color after the depth. The scattering is part of the
    // extinction, the absorption the rest of it. Negative absorptions are shifted away.
    float3 scatter = saturate(material.transmission_scatter);
    float3 extinction = -log(max(tint, 1e-4)) / depth;
    float3 scattering = scatter / depth;
    float3 absorption = extinction - scattering;
    absorption -= min(min(min(absorption.r, absorption.g), absorption.b), 0.0);
    extinction = absorption + scattering;

    float3 direct = exp(-extinction * thickness);
    // Light that has been scattered at least once, but not absorbed. It arrives diffused: Forwards from what is
    // behind (blurred), the rest from the lights around.
    float3 scattered = max(exp(-absorption * thickness) - direct, 0.0);

    float g = clamp(material.transmission_scatter_anisotropy, -0.95, 0.95);
    float forward = 0.25 * (1.0 + g);
    float3 behind = _sample_scene_color(screen_uv, max_lod * 0.6);
    float3 diffused = lerp(_volume_inscattering(ctx, g, ambient_radiance), behind, forward);

    background = direct * refracted + scattered * diffused;
}

//////////////////////////////////////////////////////////////////////////
// Lights.
//////////////////////////////////////////////////////////////////////////

/// Smooth window that reaches zero at the range of a light. A range of zero is unlimited.
float _range_window(float dist_sqr, float range)
{
    if (range <= 0.0)
    {
        return 1.0;
    }

    float ratio = dist_sqr / (range * range);
    float window = saturate(1.0 - ratio * ratio);
    return window * window;
}

///
/// Distance attenuation of lights that emit from a point or a sphere: Inverse square falloff that is windowed to
/// smoothly reach zero at the range of the light.
/// [Karis 2013, "Real Shading in Unreal Engine 4"]
///
float _distance_attenuation(float dist_sqr, float range, float source_radius)
{
    // The falloff is only an inverse square outside of the source. This also avoids the singularity at the center.
    float min_dist = max(source_radius, 0.01);
    return _range_window(dist_sqr, range) / max(dist_sqr, min_dist * min_dist);
}

///
/// Angular attenuation of spot lights: Smooth falloff from the inner to the outer cone angle.
/// to_surface is the normalized direction from the light towards the surface.
///
float _spot_attenuation(PbrLight light, float3 to_surface)
{
    float attenuation = saturate(dot(to_surface, light.direction) * light.spot_scale + light.spot_offset);
    return attenuation * attenuation;
}

///
/// Cosine of the angle of incidence for a spherical light source that subtends the half angle a as seen from the
/// surface (sin_a_sqr = sin^2(a)). Unlike a clamped cosine, this accounts for sources that are only partially above
/// the horizon of the surface. Equals the clamped cosine for punctual lights.
///
float _sphere_horizon_cos_wrap(float n_dot_l, float sin_a_sqr)
{
    float sin_a = sqrt(sin_a_sqr);
    if (n_dot_l < sin_a)
    {
        n_dot_l = max(n_dot_l, -sin_a);
        // Hermite spline approximation of the horizon clipped form factor of a sphere:
        // Zero with a slope of zero at -sin_a, and sin_a with a slope of one at sin_a.
        n_dot_l = (sin_a + n_dot_l) * (sin_a + n_dot_l) / max(4.0 * sin_a, 1e-6);
    }

    return n_dot_l;
}

///
/// Contribution of a single edge between two points on the unit sphere to the vector form factor of a polygon.
/// Uses a rational fit of theta / sin(theta) that already includes the 1 / (2 * PI) normalization.
/// [Hill & Heitz 2016, "Real-Time Area Lighting: a Journey from Research to Production"]
///
float3 _integrate_edge(float3 v1, float3 v2)
{
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206 * y) * y;
    float b = 3.4175940 + (4.1616724 + y) * y;
    float v = a / b;

    float theta_sin_theta = x > 0.0 ? v : 0.5 * rsqrt(max(1.0 - x * x, 1e-7)) - v;

    return cross(v1, v2) * theta_sin_theta;
}

/// Most representative point of a disc of the angular radius a around l for the reflection vector r.
float3 _representative_disc(float3 l, float cos_a, float sin_a, float3 r)
{
    float l_dot_r = dot(l, r);
    float3 to_r = _safe_normalize(r - l_dot_r * l);
    return l_dot_r < cos_a ? _safe_normalize(cos_a * l + sin_a * to_r) : r;
}

/// Most representative point of a sphere of the radius around (to_light from the surface) for the reflection vector r.
float3 _representative_sphere(float3 to_light, float radius, float3 r)
{
    float3 center_to_ray = dot(to_light, r) * r - to_light;
    float3 closest = to_light + center_to_ray * saturate(radius / max(length(center_to_ray), 1e-6));
    return _safe_normalize(closest);
}

///
/// Most representative point of a rectangle for the reflection vector r: The point on the rectangle that is the
/// closest to where the reflection ray hits the plane of the light. Rays that miss the plane end up at the edge of the
/// rectangle they are heading to.
///
float3 _representative_rect(PbrLight light, float3 to_center, float plane_dist, float3 up, float2 half_size, float3 r)
{
    float t = plane_dist / max(-dot(r, light.direction), 1e-5);
    float3 center_to_hit = r * t - to_center;
    float2 closest = clamp(float2(dot(center_to_hit, light.right), dot(center_to_hit, up)), -half_size, half_size);
    return _safe_normalize(to_center + light.right * closest.x + up * closest.y);
}

float3 _evaluate_directional_light(PbrLight light, PbrShadingContext ctx)
{
    // Direction towards the center of the source.
    float3 l = -light.direction;

    // The source is a disc with the angular radius a, where source_radius = tan(a).
    float cos_a = rsqrt(1.0 + light.source_radius * light.source_radius);
    float sin_a = light.source_radius * cos_a;

    PbrIncidentLight incident;
    incident.l = l;
    incident.n_dot_l = _sphere_horizon_cos_wrap(dot(ctx.n, l), sin_a * sin_a);
    incident.l_specular = _representative_disc(l, cos_a, sin_a, ctx.r);
    incident.l_coat = ctx.coat_weight > 0.0 ? _representative_disc(l, cos_a, sin_a, ctx.r_coat) : incident.l_specular;
    incident.sin_a = sin_a;

    incident.illuminance = light.tint * light.intensity;

    return _evaluate_incident_light(ctx, incident, light.specular_scale);
}

float3 _evaluate_point_light(PbrLight light, PbrShadingContext ctx)
{
    float3 to_light = light.pos - ctx.world_pos;
    float dist_sqr = dot(to_light, to_light);

    float attenuation = _distance_attenuation(dist_sqr, light.range, light.source_radius);
    if (attenuation <= 0.0)
    {
        return float3(0.0, 0.0, 0.0);
    }

    float dist = max(sqrt(dist_sqr), 1e-4);
    float3 l = to_light / dist;

    // The source is a sphere that subtends the half angle a as seen from the surface.
    float sin_a = saturate(light.source_radius / dist);

    PbrIncidentLight incident;
    incident.l = l;
    incident.n_dot_l = _sphere_horizon_cos_wrap(dot(ctx.n, l), sin_a * sin_a);
    incident.l_specular = _representative_sphere(to_light, light.source_radius, ctx.r);
    incident.l_coat = ctx.coat_weight > 0.0 ? _representative_sphere(to_light, light.source_radius, ctx.r_coat) : incident.l_specular;
    incident.sin_a = sin_a;

    incident.illuminance = light.tint * (light.intensity * attenuation);

    return _evaluate_incident_light(ctx, incident, light.specular_scale);
}

/// A spot light is a point light that is masked by a cone.
float3 _evaluate_spot_light(PbrLight light, PbrShadingContext ctx)
{
    float cone = _spot_attenuation(light, _safe_normalize(ctx.world_pos - light.pos));
    if (cone <= 0.0)
    {
        return float3(0.0, 0.0, 0.0);
    }

    return _evaluate_point_light(light, ctx) * cone;
}

///
/// Rectangular area light that emits from its front face only.
///
/// The diffuse term integrates the rectangle analytically and is only approximated where the light is partially
/// below the horizon of the surface. The specular term is a most representative point approximation, as an
/// accurate solution (linearly transformed cosines) would require lookup tables.
///
float3 _evaluate_rect_light(PbrLight light, PbrShadingContext ctx)
{
    float3 to_center = light.pos - ctx.world_pos;

    // Distance of the surface in front of the plane of the light. Nothing is emitted from the back face.
    float plane_dist = -dot(to_center, light.direction);
    if (plane_dist <= 0.0)
    {
        return float3(0.0, 0.0, 0.0);
    }

    float window = _range_window(dot(to_center, to_center), light.range);
    if (window <= 0.0)
    {
        return float3(0.0, 0.0, 0.0);
    }

    float3 right = light.right;
    float3 up = cross(right, light.direction);
    float2 half_size = 0.5 * float2(light.width, light.height);
    float3 ex = right * half_size.x;
    float3 ey = up * half_size.y;

    // The corners projected onto the unit sphere around the surface. With this winding, the integrated vector
    // points towards the light for every surface in front of it.
    float3 p0 = _safe_normalize(to_center - ex - ey);
    float3 p1 = _safe_normalize(to_center + ex - ey);
    float3 p2 = _safe_normalize(to_center + ex + ey);
    float3 p3 = _safe_normalize(to_center - ex + ey);

    // The vector form factor. Its direction is the mean direction the light arrives from, and its length is the
    // form factor (the fraction of the hemisphere the light covers, cosine weighted) of a surface that faces it.
    float3 form = _integrate_edge(p0, p1) + _integrate_edge(p1, p2)
                + _integrate_edge(p2, p3) + _integrate_edge(p3, p0);
    float form_length = length(form);
    if (form_length <= 1e-7)
    {
        return float3(0.0, 0.0, 0.0);
    }
    float form_factor = saturate(form_length);

    PbrIncidentLight incident;
    incident.l = form / form_length;
    // Handle the horizon like for a sphere that has the same form factor: sin^2(a) = form factor.
    incident.n_dot_l = _sphere_horizon_cos_wrap(dot(ctx.n, incident.l), form_factor);
    incident.l_specular = _representative_rect(light, to_center, plane_dist, up, half_size, ctx.r);
    incident.l_coat = ctx.coat_weight > 0.0 ? _representative_rect(light, to_center, plane_dist, up, half_size, ctx.r_coat) : incident.l_specular;
    incident.sin_a = sqrt(form_factor);

    // The illuminance of a surface that faces the light is its luminance times PI times the form factor.
    incident.illuminance = light.tint * (light.intensity * PI * form_factor * window);

    return _evaluate_incident_light(ctx, incident, light.specular_scale);
}

} /* ~Namespace Jafg */
