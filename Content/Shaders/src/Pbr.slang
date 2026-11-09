// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/WorldData.slang"
#include "Jafg/StaticMeshVertex.slang"
#include "Jafg/Pbr.slang"

[[vk::binding(0, 0),Shared("Jafg::UBO::WorldData")]] ConstantBuffer<WorldData> world_data;

[[vk::binding(0, 1),Shared("Jafg::UBO::Bindless")]] SamplerState[] samplers;
[[vk::binding(1, 1),Shared("Jafg::UBO::Bindless")]] Texture2D[] bindless_textures;

[[vk::binding(0, 2),Shared("Jafg::SSBO::PbrMaterials")]] StructuredBuffer<Jafg::PbrMaterial> materials;
[[vk::binding(0, 3),Shared("Jafg::SSBO::PbrLights")]] StructuredBuffer<Jafg::PbrLight> lights;

[[vk::push_constant]] Jafg::PbrPushConstants request;

struct FRInput 
{
    float4 pos: SV_POSITION;
    float3 translation: TRANSLATION;
    float3 normal: NORMAL;
    float2 uv: TEXCOORD0;
    float4 tangent: TANGENT;
};

[[shader("vertex")]]
FRInput vertMain(Jafg::StaticMeshVertex input)
{
    static_assert(sizeof(Jafg::PbrMaterial) == 96, "Unexpected struct size");

    float4 world_pos = mul(request.model, float4(input.pos, 1.0));

    FRInput result;

    result.pos = mul(world_data.proj, mul(world_data.view, world_pos));
    result.translation = world_pos.xyz;
    result.normal = normalize(mul((float3x3)request.model, input.normal));
    result.uv = input.uv;
    result.tangent = input.tangent;

    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    Jafg::PbrMaterial material = materials[request.material];
    return bindless_textures[NonUniformResourceIndex(material.albedo)]
        .Sample(samplers[material.sampler], input.uv);
}
