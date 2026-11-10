// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/StaticMeshVertex.slang"
#include "Jafg/Pbr.slang"

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
    return Jafg::evaluate_pbr_sv_target(material, samplers[material.sampler]
        , input.translation, input.normal, input.uv, input.tangent);
}
