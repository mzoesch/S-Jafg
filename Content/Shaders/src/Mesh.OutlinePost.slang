// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

[[CxxName("Jafg::PC::Outline")]]
struct PushConstants
{
    float2 inverse_viewport_size;
    uint32_t thickness;
    LColor tint;
};

[[vk::binding(0, 0),UpdateFrequency(Rarely)]] uniform Texture2D<float> stencil_texture;
[[vk::binding(1, 0),UpdateFrequency(Rarely)]] SamplerState sampler;

[[vk::push_constant]] PushConstants outline;

struct FRInput
{
    float4 pos: SV_POSITION;
    float2 uv: TEXCOORD0;
};

[[shader("vertex")]]
FRInput vertMain(uint vertex_id: SV_VertexID)
{
    float2 positions[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0,  3.0),
        float2( 3.0, -1.0)
    };

    FRInput result;
    result.pos = float4(positions[vertex_id], 0.0, 1.0);
    result.uv = positions[vertex_id] * 0.5 + 0.5;
    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    float center = stencil_texture.Sample(sampler, input.uv);

    // Early out: only care about background pixels.
    if (center > 0.5)
    {
        discard;
    }

    // Convert pixel thickness to UV space.
    float2 texel = outline.inverse_viewport_size;
    float r = outline.thickness;
    
    float edge = 0.0;
    for (float y = -r; y <= r; y++)
    {
        for (float x = -r; x <= r; x++)
        {
            float2 offset = float2(x, y) * texel;
            float sample = stencil_texture.Sample(sampler, input.uv + offset);

            edge = max(edge, sample);
        }
    }

    if (edge < 0.5)
    {
        discard;
    }

    return maths::color_to_linear(outline.tint, 2.4);
}
