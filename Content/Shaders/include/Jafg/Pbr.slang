// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/WorldData.slang"

namespace Jafg
{

[[CxxName("Jafg::SSBO::PbrMaterial")]]
struct PbrMaterial
{
    /// Factors are multiplied with the sampled texture values if bound
    /// else the factors are used directly.
    float4 albedo_factor;
    float occlusion_factor;
    float roughness_factor;
    float metallic_factor;
    uint _pad0;
    float3 emissive_factor;

    float alpha_cutoff;
    float normal_scale;

    /// Texture bindings or INVALID_INDEX if not bound.
    uint albedo;
    uint normal;
    uint arm;
    uint emissive;
    uint sampler;
    uint flags;

    uint _pad1;

    float2 uv_scale;
    float2 uv_offset;
};

static const uint LIGHT_DIRECTIONAL = 0u;
static const uint LIGHT_POINT       = 1u;
static const uint LIGHT_SPOT        = 2u;
static const uint LIGHT_RECT        = 3u;

[[CxxName("Jafg::UBO::PbrLight")]]
struct PbrLight
{
    /// directional: unused
    /// point, rect: world position.
    /// rect:        center.
    float3 pos;

    /// 0 = unlimited, otherwise smooth cut-off radius (m)
    float range;

    /// directional: direction the light travels
    /// point:       unused
    /// spot:        direction the cone points
    /// rect:        emitting normal (front face)
    float3 direction;

    /// The type of the light.
    uint type;

    /// Color
    float3 tint;
    /// Color intensity
    float intensity;

    /// rect: unit vector along the width axis (perp. to direction)
    float3 right;
    /// rect: full width  (m)
    float width;

    /// rect: full height (m)
    float height;
    float spot_scale;    // spot: 1 / max(0.001, cosInner - cosOuter)
    float spot_Offset;   // spot: -cosOuter * spotScale

    /// LIGHT_FLAG_*
    uint flags;

    /// point/spot radius, or tan(SourceAngle/2) for directional
    float source_radius;
    float specular_scale;

    uint _pad0;
    uint _pad1;
};

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

/// Reflectance at normal incidence for dielectrics (~4%, IOR 1.5).
static const float3 DIELECTRIC_F0 = float3(0.04, 0.04, 0.04);

/// The fully resolved (factors * textures) surface properties of a PBR material at a given surface point.
struct PbrSurface
{
    /// Linear base color. Alpha is the coverage.
    float4 albedo;
    /// Ambient occlusion. Range [0,1] with 0 = fully occluded and 1 = not occluded.
    float occlusion;
    /// Perceptual roughness. Range [0,1] with 0 = smooth and 1 = rough.
    float roughness;
    /// Range [0,1] with 0 = dielectric and 1 = metallic.
    float metallic;
    /// Linear emitted radiance.
    float3 emissive;
};

/// Precomputed per-pixel values shared by all lights.
struct PbrShadingContext
{
    float3 world_pos;
    /// Normalized surface normal.
    float3 n;
    /// Normalized direction from the surface towards the camera.
    float3 v;
    float n_dot_v;

    /// Lambertian diffuse color. Metals have no diffuse.
    float3 diffuse_color;
    /// Specular reflectance at normal incidence.
    float3 f0;
    /// Clamped perceptual roughness.
    float roughness;
    /// GGX alpha (roughness^2).
    float alpha;
};

/// Main entry point to evaluate a PBR material for a given surface point.
float4 evaluate_pbr_sv_target(PbrMaterial material, SamplerState sampler
    , float3 translation, float3 normal, float2 uv, float4 tangent
    )
{
    static_assert(sizeof(Jafg::PbrMaterial) == 96, "Unexpected struct size");
    static_assert(sizeof(Jafg::PbrLight) == 96, "Unexpected struct size");
    static_assert(sizeof(Jafg::WorldData) == 192, "Unexpected struct size");

    PbrSurface surface = _evaluate_surface(material, sampler, uv);

    if (!_should_show_any_lighting()) // make this also a flag. so that we allow complete darkness.
    {
        return surface.albedo;
    }

    PbrShadingContext ctx = _make_shading_context(surface, translation, normal);

    // The descriptor range is set to exactly the number of uploaded lights.
    uint light_count;
    uint light_stride;
    lights.GetDimensions(light_count, light_stride);

    float3 direct = float3(0.0, 0.0, 0.0);
    for (uint i = 0; i < light_count; ++i)
    {
        PbrLight light = lights[i];
        if (!_should_show_light_type(light.type))
        {
            continue;
        }

        if (light.type == LIGHT_POINT)
        {
            direct += _evaluate_point_light(light, ctx);
        }
        // TODO: Other light types.
    }

    // Ambient, so that unlit surfaces are not pitch-black. Treated as a uniform environment radiance until
    // we have IBL. Occlusion only affects indirect light, direct light would need proper shadows.
    float3 ambient = _evaluate_ambient(ctx, world_data.ambient.xyz) * surface.occlusion;

    return float4(direct + ambient + surface.emissive, surface.albedo.a);
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
/// Resolves all material properties. Factors are multiplied with the sampled texture values if bound, else the
/// factors are used directly.
///
/// If a property is hidden by the editor visualization flags, the neutral value of a default constructed
/// material is used instead (white albedo, no occlusion, 0.5 roughness, dielectric, no emission).
///
PbrSurface _evaluate_surface(PbrMaterial material, SamplerState sampler, float2 uv)
{
    PbrSurface surface;

    // Albedo.
    surface.albedo = material.albedo_factor;
    if (material.albedo != INVALID_INDEX)
    {
        surface.albedo *= _sample(material.albedo, sampler, uv);
    }

    // AO, roughness, metallic (ARM): R = AO, G = roughness, B = metallic.
    // The texture is stored as UNORM, so the values are already linear.
    float3 arm = float3(material.occlusion_factor, material.roughness_factor, material.metallic_factor);
    if (material.arm != INVALID_INDEX)
    {
        arm *= _sample(material.arm, sampler, uv).rgb;
    }
    surface.occlusion = saturate(arm.r);
    surface.roughness = saturate(arm.g);
    surface.metallic  = saturate(arm.b);

    // Emissive. The texture is stored as SRGB, so the sampled value is already linear.
    surface.emissive = material.emissive_factor;
    if (material.emissive != INVALID_INDEX)
    {
        surface.emissive *= _sample(material.emissive, sampler, uv).rgb;
    }

#if JAFG_WITH_EDITOR
    if (!_should_show(WORLD_FLAG_SHOW_ALBEDO))
    {
        // Keep the alpha so that cutouts etc. still keep their silhouette.
        surface.albedo.rgb = float3(1.0, 1.0, 1.0);
    }
    if (!_should_show(WORLD_FLAG_SHOW_OCCLUSION))
    {
        surface.occlusion = 1.0;
    }
    if (!_should_show(WORLD_FLAG_SHOW_ROUGHNESS))
    {
        surface.roughness = 0.5;
    }
    if (!_should_show(WORLD_FLAG_SHOW_METALLIC))
    {
        surface.metallic = 0.0;
    }
    if (!_should_show(WORLD_FLAG_SHOW_EMISSIVE))
    {
        surface.emissive = float3(0.0, 0.0, 0.0);
    }
#endif /* JAFG_WITH_EDITOR */

    return surface;
}

PbrShadingContext _make_shading_context(PbrSurface surface, float3 world_pos, float3 normal)
{
    float3 cam_pos = float3(world_data.cam_pos_x, world_data.cam_pos_y, world_data.cam_pos_z);

    PbrShadingContext ctx;
    ctx.world_pos = world_pos;
    ctx.n = normalize(normal);
    ctx.v = normalize(cam_pos - world_pos);
    // Clamp to avoid the division by zero in the visibility term for surfaces seen exactly edge-on.
    ctx.n_dot_v = clamp(dot(ctx.n, ctx.v), 1e-4, 1.0);

    ctx.diffuse_color = surface.albedo.rgb * (1.0 - surface.metallic);
    ctx.f0 = lerp(DIELECTRIC_F0, surface.albedo.rgb, surface.metallic);
    ctx.roughness = max(surface.roughness, MIN_PERCEPTUAL_ROUGHNESS);
    ctx.alpha = ctx.roughness * ctx.roughness;

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

/// Schlick's approximation of the Fresnel term.
float3 _f_schlick(float3 f0, float v_dot_h)
{
    float f = pow(1.0 - v_dot_h, 5.0);
    return f0 + (1.0 - f0) * f;
}

///
/// Evaluates the BRDF times n_dot_l for a single light direction.
/// Returns the reflected radiance per unit of incoming radiance.
///
float3 _evaluate_brdf(PbrShadingContext ctx, float3 l, float specular_scale)
{
    float n_dot_l = saturate(dot(ctx.n, l));
    if (n_dot_l <= 0.0)
    {
        return float3(0.0, 0.0, 0.0);
    }

    float3 h = normalize(ctx.v + l);
    float n_dot_h = saturate(dot(ctx.n, h));
    float v_dot_h = saturate(dot(ctx.v, h));

    float  d = _d_ggx(n_dot_h, ctx.alpha);
    float  v = _v_smith_ggx_correlated(ctx.n_dot_v, n_dot_l, ctx.alpha);
    float3 f = _f_schlick(ctx.f0, v_dot_h);

    float3 specular = d * v * f * specular_scale;
    // The energy reflected specularly cannot be diffused anymore.
    float3 diffuse = (1.0 - f) * ctx.diffuse_color * (1.0 / PI);

    return (diffuse + specular) * n_dot_l;
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
    float2 env_brdf = _env_brdf_approx(ctx.roughness, ctx.n_dot_v);
    float3 specular = ctx.f0 * env_brdf.x + env_brdf.y;
    return (ctx.diffuse_color + specular) * radiance;
}

//////////////////////////////////////////////////////////////////////////
// Lights.
//////////////////////////////////////////////////////////////////////////

///
/// Distance attenuation of punctual lights.
///
/// Note: The ranged falloff is intentionally not physically based (no inverse square) so that the lights are
///       easily visible while testing. Replace with a windowed inverse square falloff once the light units
///       (ELightUnits) are respected on the host.
///
float _punctual_light_attenuation(PbrLight light, float dist)
{
    if (light.range > 0.0)
    {
        // Simple smooth falloff that reaches zero at the range.
        float falloff = saturate(1.0 - dist / light.range);
        return falloff * falloff;
    }

    return 1.0 / max(dist * dist, 1e-4);
}

float3 _evaluate_point_light(PbrLight light, PbrShadingContext ctx)
{
    float3 to_light = light.pos - ctx.world_pos;
    float dist = length(to_light);
    float3 l = to_light / max(dist, 1e-4);

    float attenuation = _punctual_light_attenuation(light, dist);
    if (attenuation <= 0.0)
    {
        return float3(0.0, 0.0, 0.0);
    }

    float3 radiance = light.tint * light.intensity * attenuation;
    return _evaluate_brdf(ctx, l, light.specular_scale) * radiance;
}

} /* ~Namespace Jafg */
