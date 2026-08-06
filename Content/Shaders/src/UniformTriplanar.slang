// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/WorldData.slang"
#include "Jafg/StaticMeshVertex.slang"
#include "Jafg/Pbr.slang"

[[vk::binding(0, 0),Shared("Jafg::UBO::WorldData")]] ConstantBuffer<WorldData> ubo;
[[vk::binding(0, 1),UpdateFrequency(Rarely)]] Texture2D base_color_map;
[[vk::binding(1, 1),UpdateFrequency(Rarely)]] SamplerState sampler;

[[vk::push_constant]] Jafg::PbrMaterial material;

struct FRInput 
{
    float4 pos: SV_POSITION;
    float3 face_normal: FACENORMAL;
    float2 uv: TEXCOORD0;
    float3 scale : SCALE;
};

[[shader("vertex")]]
FRInput vertMain(Jafg::StaticMeshVertex input)
{
    FRInput result;
    result.pos = mul(ubo.proj, mul(ubo.view, mul(material.model, float4(input.pos, 1.0))));
    result.face_normal = input.normal;
    result.uv = input.uv;
    /* TODO: We may want to provide the scale as a host input. */
    result.scale = float3(length(material.model[0].xyz), length(material.model[1].xyz), length(material.model[2].xyz));
    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    float3 n = abs(input.face_normal);
    float3 s = abs(input.scale);
    float2 uv_scale =
          n.x * float2(s.z, s.y)
        + n.y * float2(s.z, s.x)
        + n.z * float2(s.x, s.y);
    return base_color_map.Sample(sampler, input.uv * uv_scale) * material.baseColorFactor;
}
