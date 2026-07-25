// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

[[CxxName("Jafg::UBO::VisualShared")]]
struct VisualShared
{
    float4x4 proj;
    float gamma;
};
[[vk::binding(0,0),UpdateFrequency(PerFrame)]] ConstantBuffer<VisualShared> visual_shared;

[[CxxName("Jafg::SSBO::VisualInstance")]]
struct VisualInstance
{
    /* (Position, Extent) in px. */
    float4 rect;
    /* (Min, Max) */
    float4 tex_coord_rect;
    float4 radii;

    LColor tint;
    LColor border_tint;
    LColor outline_tint;
    float outline_thickness;
    
    uint texture_index;
    uint sampler_index;
    float msdf_pixel_range;
    uint _pad0;
};
[[vk::binding(1,0),UpdateFrequency(PerFrame)]] StructuredBuffer<VisualInstance> instances;

[[vk::binding(0,1),Shared("Jafg::UBO::Bindless")]] SamplerState[] samplers;
[[vk::binding(1,1),Shared("Jafg::UBO::Bindless")]] Texture2D[] bindless_textures;

struct FRInput
{
    float4 pos: SV_POSITION;
    float2 uv: TEXCOORD0;
    float2 tex_uv: TEXCOORD1;
    uint instance_id: SV_InstanceID;
};

[shader("vertex")]
FRInput vertMain(uint vertex_id: SV_VertexID, uint instance_id: SV_InstanceID, uint offset: SV_StartInstanceLocation)
{
    VisualInstance instance = instances[offset + instance_id];
    float2 corner = float2((vertex_id >> 1), (vertex_id & 1));

    FRInput result;
    result.pos = mul(visual_shared.proj, float4(instance.rect.xy + corner * instance.rect.zw, 0.0, 1.0));
    result.uv = corner;
    result.tex_uv = lerp(instance.tex_coord_rect.xy, instance.tex_coord_rect.zw, corner);
    result.instance_id = offset + instance_id;

    return result;
}

[shader("fragment")]
float4 fragMain(FRInput input): SV_TARGET
{
    VisualInstance instance = instances[input.instance_id];

    float dist_left   = (input.uv.x - 0.0) * instance.rect.z;
    float dist_top    = (input.uv.y - 0.0) * instance.rect.w;
    float dist_right  = (1.0 - input.uv.x) * instance.rect.z;
    float dist_bottom = (1.0 - input.uv.y) * instance.rect.w;

    bool is_corner  = false;
    bool is_outline = false;

    float4 result;
    if (instance.msdf_pixel_range != 0.0f)
    {
        float4 tint = maths::color_to_linear(instance.tint, visual_shared.gamma);
        float3 msdf = (bindless_textures[NonUniformResourceIndex(instance.texture_index)].Sample(samplers[instance.sampler_index], input.tex_uv)).rgb;
        result = float4(tint.rgb, tint.w * clamp(instance.msdf_pixel_range * (maths::median_of_three(msdf.r, msdf.g, msdf.b) - 0.5) + 0.5, 0.0, 1.0));
    }
    else if (instance.sampler_index == JAFG_SAMPLER_LINEAR_CLAMP_TO_BORDER || instance.sampler_index == JAFG_SAMPLER_NEAREST_CLAMP_TO_BORDER)
    {
        // This is prob. better, but produces sometimes wrong results...
        // result = lerp(
        //     maths::color_to_linear(instance.tint) * bindless_textures[NonUniformResourceIndex(instance.texture_index)].Sample(samplers[instance.sampler_index], input.tex_uv),
        //     maths::color_to_linear(instance.border_tint),
        //     saturate(step(1.0, input.tex_uv.x) + step(input.tex_uv.x, 0.0) + step(1.0, input.tex_uv.y) + step(input.tex_uv.y, 0.0))
        //     );
        if (input.tex_uv.x < 0.0 || input.tex_uv.x > 1.0 || input.tex_uv.y < 0.0 || input.tex_uv.y > 1.0)
        {
            result = maths::color_to_linear(instance.border_tint, visual_shared.gamma);
        }
        else
        {
            result = maths::color_to_linear(instance.tint, visual_shared.gamma) * bindless_textures[NonUniformResourceIndex(instance.texture_index)].Sample(samplers[instance.sampler_index], input.tex_uv);
        }
    }
    else
    {
        result = maths::color_to_linear(instance.tint, visual_shared.gamma) * bindless_textures[NonUniformResourceIndex(instance.texture_index)].Sample(samplers[instance.sampler_index], input.tex_uv);
    }

    if (dist_left < instance.radii.x && dist_top < instance.radii.x) /* TL */
    {
        float dist = length(float2(instance.radii.x - dist_left, instance.radii.x - dist_top));
        if (dist > instance.radii.x)
        {
            discard;
        }
        else if (dist > instance.radii.x - instance.outline_thickness)
        {
            is_outline = true;
        }
        is_corner = true;
    }
    if (dist_right < instance.radii.y && dist_top < instance.radii.y) /* TR */
    {
        float dist = length(float2(instance.radii.y - dist_right, instance.radii.y - dist_top));
        if (dist > instance.radii.y)
        {
            discard;
        }
        else if (dist > instance.radii.y - instance.outline_thickness)
        {
            is_outline = true;
        }
        is_corner = true;
    }
    if (dist_right < instance.radii.z && dist_bottom < instance.radii.z) /* BR */
    {
        float dist = length(float2(instance.radii.z - dist_right, instance.radii.z - dist_bottom));
        if (dist > instance.radii.z)
        {
            discard;
        }
        else if (dist > instance.radii.z - instance.outline_thickness)
        {
            is_outline = true;
        }
        is_corner = true;
    }
    if (dist_left < instance.radii.w && dist_bottom < instance.radii.w) /* BL */
    {
        float dist = length(float2(instance.radii.w - dist_left, instance.radii.w - dist_bottom));
        if (dist > instance.radii.w)
        {
            discard;
        }
        else if (dist > instance.radii.w - instance.outline_thickness)
        {
            is_outline = true;
        }
        is_corner = true;
    }

    if (instance.outline_thickness > 0.0)
    {
        if (!is_corner)
        {
            if 
            (
                   dist_left   < instance.outline_thickness
                || dist_right  < instance.outline_thickness
                || dist_top    < instance.outline_thickness
                || dist_bottom < instance.outline_thickness
            )
            {
                is_outline = true;
            }
        }
        if (is_outline)
        {
            result = maths::color_to_linear(instance.outline_tint, visual_shared.gamma);
        }
    }

    return result;
}
