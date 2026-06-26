// Copyright mzoesch. All rights reserved.
#include "Jafg.slang"

struct FRInput
{
    float4 Position: SV_POSITION;
    float2 UV: TEXCOORD0;
};

[[CxxName("Jafg::PC::Outline")]]
struct PushConstants
{
    float2 InverseViewportSize;
    uint32_t Thickness;
    LColor Tint;
};

[[vk::binding(0, 0),UpdateFrequency(Rarely)]] uniform Texture2D<float> stencilTexture;
[[vk::binding(1, 0),UpdateFrequency(Rarely)]] SamplerState sampler;

[[vk::push_constant]] PushConstants outline;

[[shader("vertex")]]
FRInput vertMain(uint VertexID: SV_VertexID)
{
    float2 positions[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0,  3.0),
        float2( 3.0, -1.0)
    };

    FRInput result;
    result.Position = float4(positions[VertexID], 0.0, 1.0);
    result.UV = positions[VertexID] * 0.5 + 0.5;
    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    float center = stencilTexture.Sample(sampler, input.UV);

    // Early out: only care about background pixels
    if (center > 0.5)
        discard;

    float edge = 0.0;

    // Convert pixel thickness to UV space
    float2 texel = outline.InverseViewportSize;
    float r = outline.Thickness;

    // Square kernel
    for (float y = -r; y <= r; y++)
    {
        for (float x = -r; x <= r; x++)
        {
            float2 offset = float2(x, y) * texel;
            float sample = stencilTexture.Sample(sampler, input.UV + offset);

            edge = max(edge, sample);
        }
    }

    if (edge < 0.5)
        discard;

    return maths::color_to_linear(outline.Tint, 2.4);
}
