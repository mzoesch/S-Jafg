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
    float3 face_normal: FACENORMAL;
    float2 uv: TEXCOORD0;
    float3 scale: SCALE;
};

[[shader("vertex")]]
FRInput vertMain(Jafg::StaticMeshVertex input)
{
    FRInput result;
    result.pos = mul(world_data.proj, mul(world_data.view, mul(request.model, float4(input.pos, 1.0))));
    result.face_normal = input.normal;
    result.uv = input.uv;
    /* TODO: We may want to provide the scale as a host input. */
    result.scale = float3(length(request.model[0].xyz), length(request.model[1].xyz), length(request.model[2].xyz));
    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    Jafg::PbrMaterial material = materials[request.material];

    float3 n = abs(input.face_normal);
    float3 s = abs(input.scale);
    float2 uv_scale =
          n.x * float2(s.z, s.y)
        + n.y * float2(s.z, s.x)
        + n.z * float2(s.x, s.y);

    return bindless_textures[NonUniformResourceIndex(material.albedo)]
        .Sample(samplers[material.sampler], input.uv * uv_scale);
}
