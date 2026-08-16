// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/WorldData.slang"
#include "Jafg/StaticMeshVertex.slang"

[[vk::binding(0, 0),Shared("Jafg::UBO::WorldData")]] ConstantBuffer<WorldData> ubo;
[[vk::binding(0, 1),UpdateFrequency(Rarely)]] TextureCube texture_cube;
[[vk::binding(1, 1),UpdateFrequency(Rarely)]] SamplerState sampler;

[[CxxName("Jafg::Detail::SkyboxVertex")]]
struct VSInput
{
    float3 position: POSITION;
};

struct FRInput
{
    float4 pos: SV_POSITION;
    float3 direction: DIRECTION;
};

[[shader("vertex")]]
FRInput vertMain(VSInput input)
{
    FRInput result;

    float4x4 view = ubo.view;
    view[0].w = 0.0;
    view[1].w = 0.0;
    view[2].w = 0.0;
    result.pos = mul(ubo.proj, mul(view, float4(input.position, 1.0))).xyww;

    result.direction = input.position;

    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    float3 direction = normalize(input.direction);
    // TODO: Make this an option. There are many textures that want to be flipped and many that don't.
    direction.x = -direction.x;
    return texture_cube.Sample(sampler, direction);
}
