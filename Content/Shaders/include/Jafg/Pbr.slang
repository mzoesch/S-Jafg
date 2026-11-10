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
    /// point, spot: world position.
    /// rect:        world position of the center.
    float3 pos;

    /// directional: unused
    /// else:        0 = unlimited, otherwise smooth cut-off radius (m)
    float range;

    /// Always normalized.
    /// directional: direction the light travels
    /// point:       unused
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

    /// rect: unit vector along the width axis (perp. to direction)
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
    /// Normalized direction of the mirror reflection of v.
    float3 r;
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

///
/// How the light of a single light source arrives at the shaded surface point.
///
/// Punctual lights arrive from exactly one direction. Area lights (spheres, discs and rectangles) are approximated
/// by one most representative point each for the diffuse and for the specular term.
/// [Karis 2013, "Real Shading in Unreal Engine 4"]
///
struct PbrIncidentLight
{
    /// Normalized direction from the surface towards the light for the diffuse term.
    float3 l;
    /// Cosine of the angle of incidence for the diffuse term. Range [0,1].
    float n_dot_l;
    /// Normalized direction from the surface towards the light for the specular term.
    float3 l_specular;
    /// Sine of the half angle that the source subtends as seen from the surface. Zero for punctual lights.
    float sin_a;
    /// Illuminance (already tinted) at a surface that directly faces the light.
    float3 illuminance;
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
        // The host uploads a single light without intensity if there are no lights at all.
        if (light.intensity <= 0.0 || !_should_show_light_type(light.type))
        {
            continue;
        }

        switch (light.type)
        {
        case LIGHT_DIRECTIONAL: direct += _evaluate_directional_light(light, ctx); break;
        case LIGHT_POINT:       direct += _evaluate_point_light(light, ctx);       break;
        case LIGHT_SPOT:        direct += _evaluate_spot_light(light, ctx);        break;
        case LIGHT_RECT:        direct += _evaluate_rect_light(light, ctx);        break;
        default:                break;
        }
    }

    // Ambient, so that unlit surfaces are not pitch-black. Treated as a uniform environment radiance until
    // we have IBL. Occlusion only affects indirect light, direct light would need proper shadows.
    // The screen space occlusion and the one of the material describe the same thing at different scales. Taking
    // the stronger one avoids darkening creases twice.
    float occlusion = min(surface.occlusion, _sample_screen_space_occlusion(translation));
    float3 ambient = _evaluate_ambient(ctx, world_data.ambient.xyz) * occlusion;

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
    float4 clip = mul(world_data.proj, mul(world_data.view, float4(world_pos, 1.0)));
    float2 uv = clip.xy / clip.w * 0.5 + 0.5;

    return bindless_textures[NonUniformResourceIndex(world_data.ssao_texture)]
        .SampleLevel(samplers[JAFG_SAMPLER_LINEAR_CLAMP_TO_EDGE], uv, 0.0).r;
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
    ctx.r = reflect(-ctx.v, ctx.n);
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

/// Normalizes without producing NaNs for degenerated (zero length) vectors.
float3 _safe_normalize(float3 v)
{
    return v * rsqrt(max(dot(v, v), 1e-12));
}

///
/// Evaluates the BRDF times the cosine of the angle of incidence for the light that arrives at the surface.
/// Returns the radiance reflected towards the camera.
///
float3 _evaluate_incident_light(PbrShadingContext ctx, PbrIncidentLight incident, float specular_scale)
{
    float3 result = float3(0.0, 0.0, 0.0);

    if (incident.n_dot_l > 0.0)
    {
        float3 h = _safe_normalize(ctx.v + incident.l);
        float3 f = _f_schlick(ctx.f0, saturate(dot(ctx.v, h)));

        // The energy reflected specularly cannot be diffused anymore.
        result += (1.0 - f) * ctx.diffuse_color * (incident.n_dot_l / PI);
    }

    float n_dot_l = saturate(dot(ctx.n, incident.l_specular));
    if (n_dot_l > 0.0 && specular_scale > 0.0)
    {
        float3 h = _safe_normalize(ctx.v + incident.l_specular);
        float n_dot_h = saturate(dot(ctx.n, h));
        float v_dot_h = saturate(dot(ctx.v, h));

        float  d = _d_ggx(n_dot_h, ctx.alpha);
        float  v = _v_smith_ggx_correlated(ctx.n_dot_v, n_dot_l, ctx.alpha);
        float3 f = _f_schlick(ctx.f0, v_dot_h);

        float energy = _area_specular_energy(ctx.alpha, incident.sin_a, v_dot_h);

        result += d * v * f * (n_dot_l * energy * specular_scale);
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
    float2 env_brdf = _env_brdf_approx(ctx.roughness, ctx.n_dot_v);
    float3 specular = ctx.f0 * env_brdf.x + env_brdf.y;
    return (ctx.diffuse_color + specular) * radiance;
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

    // Most representative point: The direction on the disc that is the closest to the reflection vector.
    float l_dot_r = dot(l, ctx.r);
    float3 to_r = _safe_normalize(ctx.r - l_dot_r * l);
    incident.l_specular = l_dot_r < cos_a ? _safe_normalize(cos_a * l + sin_a * to_r) : ctx.r;
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

    // Most representative point: The point on the sphere that is the closest to the reflection ray.
    float3 center_to_ray = dot(to_light, ctx.r) * ctx.r - to_light;
    float3 closest = to_light + center_to_ray * saturate(light.source_radius / max(length(center_to_ray), 1e-6));
    incident.l_specular = _safe_normalize(closest);
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

    // Most representative point: The point on the rectangle that is the closest to where the reflection ray hits the
    // plane of the light. Rays that miss the plane end up at the edge of the rectangle they are heading to.
    float t = plane_dist / max(-dot(ctx.r, light.direction), 1e-5);
    float3 center_to_hit = ctx.r * t - to_center;
    float2 closest = clamp(float2(dot(center_to_hit, right), dot(center_to_hit, up)), -half_size, half_size);
    incident.l_specular = _safe_normalize(to_center + right * closest.x + up * closest.y);
    incident.sin_a = sqrt(form_factor);

    // The illuminance of a surface that faces the light is its luminance times PI times the form factor.
    incident.illuminance = light.tint * (light.intensity * PI * form_factor * window);

    return _evaluate_incident_light(ctx, incident, light.specular_scale);
}

} /* ~Namespace Jafg */
