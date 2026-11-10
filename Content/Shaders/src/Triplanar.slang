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

    float3 face_normal: FACENORMAL;
    float3 scale: SCALE;
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

    result.face_normal = input.normal;
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
          n.x * float2(s.y, s.z)
        + n.y * float2(s.x, s.z)
        + n.z * float2(s.x, s.y);

    return Jafg::evaluate_pbr_sv_target(material, samplers[material.sampler]
        , input.translation, input.normal, input.uv * uv_scale, input.tangent);
}
