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

    if (center > 0.5)
    {
        discard;
    }

    float2 texel = outline.inverse_viewport_size;
    int r = int(outline.thickness);

    float edge = 0.0;
    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x <= r; x++)
        {
            float2 uv = input.uv + float2(x, y) * texel;

            if (any(uv < 0.0) || any(uv > 1.0))
            {
                continue;
            }

            edge = max(edge, stencil_texture.SampleLevel(sampler, uv, 0));
        }
    }

    if (edge < 0.5)
    {
        discard;
    }

    return maths::color_to_linear(outline.tint, 2.4);
}
