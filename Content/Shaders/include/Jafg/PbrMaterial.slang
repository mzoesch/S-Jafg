// Copyright mzoesch. All rights reserved.

///
/// The parameters of a pbr material, without any resource bindings. Shared by every shader that reads pbr materials,
/// e.g., the shadows that respect their coverage.
///

#pragma once

#include "Jafg.slang"

namespace Jafg
{

///
/// The parameters of the OpenPBR Surface model and the extensions of Jafg. Factors are multiplied with the sampled
/// texture values if bound, else the factors are used directly. Lengths are in centimeters (the thin film thickness in
/// micrometers), angles in degrees.
/// [Andersson et al. 2025, "OpenPBR Surface Specification v1.1"]
/// @see Jafg::LPbrMaterialParameters for the meaning, the ranges and the defaults of every parameter.
///
[[CxxName("Jafg::SSBO::PbrMaterial")]]
struct PbrMaterial
{
    float3 base_color;
    float base_weight;
    float base_metalness;
    float base_diffuse_roughness;
    float specular_weight;
    float specular_roughness;

    float3 specular_color;
    float specular_ior;
    float specular_roughness_anisotropy;
    float geometry_tangent_rotation;
    float occlusion;
    float geometry_opacity;

    float3 transmission_color;
    float transmission_weight;
    float3 transmission_scatter;
    float transmission_depth;
    float transmission_scatter_anisotropy;
    float transmission_dispersion_scale;
    float transmission_dispersion_abbe_number;
    float subsurface_weight;

    float3 subsurface_color;
    float subsurface_radius;
    float3 subsurface_radius_scale;
    float subsurface_scatter_anisotropy;

    float3 coat_color;
    float coat_weight;
    float coat_roughness;
    float coat_roughness_anisotropy;
    float coat_ior;
    float coat_darkening;

    float3 fuzz_color;
    float fuzz_weight;
    float3 emission_color;
    float emission_luminance;

    float fuzz_roughness;
    float thin_film_weight;
    float thin_film_thickness;
    float thin_film_ior;

    float geometry_coat_tangent_rotation;
    float alpha_cutoff;
    float normal_scale;
    float coat_normal_scale;

    float2 uv_scale;
    float2 uv_offset;

    /// Texture bindings or INVALID_INDEX if not bound. @see Jafg::LPbrMaterialTemplate::TextureBindings
    uint base_color_texture;
    uint normal_texture;
    uint arm_texture;
    uint emission_texture;
    uint specular_texture;
    uint anisotropy_texture;
    uint thin_film_texture;
    uint subsurface_texture;
    uint coat_normal_texture;

    uint sampler;
    /// PBR_MATERIAL_FLAG_*
    uint flags;
    /// geometry_thin_walled. Zero or one.
    uint geometry_thin_walled;
};

/// Stable across the shader barrier. @see Jafg::EPbrMaterialBits
static const uint PBR_MATERIAL_FLAG_ALPHA_MASK      = 0x1u << 0;
static const uint PBR_MATERIAL_FLAG_DOUBLE_SIDED    = 0x1u << 1;
static const uint PBR_MATERIAL_FLAG_DIRECTX_NORMALS = 0x1u << 2;
static const uint PBR_MATERIAL_FLAG_ALPHA_BLEND     = 0x1u << 3;

/// The texture coordinates of the material for the ones of the mesh.
float2 pbr_material_uv(PbrMaterial material, float2 uv)
{
    return uv * material.uv_scale + material.uv_offset;
}

bool pbr_material_is_thin_walled(PbrMaterial material)
{
    return material.geometry_thin_walled != 0u;
}

bool pbr_material_is_double_sided(PbrMaterial material)
{
    return pbr_material_is_thin_walled(material) || (material.flags & PBR_MATERIAL_FLAG_DOUBLE_SIDED) != 0u;
}

/// Whether the coverage of the material comes from the alpha of the base color texture.
bool pbr_material_uses_alpha(PbrMaterial material)
{
    return (material.flags & (PBR_MATERIAL_FLAG_ALPHA_MASK | PBR_MATERIAL_FLAG_ALPHA_BLEND)) != 0u;
}

/// Whether the material is drawn translucent. Equals Jafg::LPbrMaterialParameters::IsTranslucent.
bool pbr_material_is_translucent(PbrMaterial material)
{
    bool transmissive = material.transmission_weight > 0.0 && material.base_metalness < 1.0;
    if ((material.flags & PBR_MATERIAL_FLAG_ALPHA_MASK) != 0u)
    {
        return transmissive;
    }
    return material.geometry_opacity < 1.0 || (material.flags & PBR_MATERIAL_FLAG_ALPHA_BLEND) != 0u || transmissive;
}

///
/// The coverage (geometry_opacity times the alpha of the base color texture, if used) and whether the surface is
/// there at all at the point: Alpha masked materials discard below the alpha cutoff and are fully opaque elsewhere.
/// base_color_alpha is the sampled alpha of the base color texture, or one if there is none.
///
bool pbr_material_coverage(PbrMaterial material, float base_color_alpha, out float coverage)
{
    coverage = saturate(material.geometry_opacity);
    if (pbr_material_uses_alpha(material))
    {
        coverage *= base_color_alpha;
    }
    if ((material.flags & PBR_MATERIAL_FLAG_ALPHA_MASK) != 0u)
    {
        if (coverage < material.alpha_cutoff)
        {
            return false;
        }
        coverage = 1.0;
    }
    return true;
}

///
/// How much light the surface blocks from a light that shines through it, for shadow maps that can only be fully lit
/// or fully shadowed. Range [0,1] with 0 = lets everything through.
/// T_pbr = 1 - alpha * (1 - (1 - metalness) * T_dielectric), with the transmittance of the dielectric base through its
/// interface (both ways) and its tint. [OpenPBR, "Opacity and transparency"]
///
float pbr_material_shadow_coverage(PbrMaterial material, float coverage)
{
    float f0 = (material.specular_ior - 1.0) / (material.specular_ior + 1.0);
    f0 = saturate(f0 * f0 * material.specular_weight);
    float3 tint = saturate(material.transmission_color);
    float t_dielectric = saturate(material.transmission_weight) * (1.0 - f0) * (1.0 - f0) * (tint.r + tint.g + tint.b) / 3.0;
    float transmittance = 1.0 - coverage * (1.0 - (1.0 - saturate(material.base_metalness)) * t_dielectric);
    return 1.0 - transmittance;
}

/// Blue noise like threshold in [0,1) for the pixel. [Jimenez 2014, "Next Generation Post Processing in Call of Duty"]
float interleaved_gradient_noise(float2 pixel)
{
    return frac(52.9829189 * frac(dot(pixel, float2(0.06711056, 0.00583715))));
}

} /* ~Namespace Jafg */
