// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

namespace Jafg
{

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

} /* ~Namespace Jafg */
