// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/MVP.slang"
#include "Jafg/StaticMeshVertex.slang"

[[vk::push_constant]] MVP mvp;

struct FRInput 
{
    float4 pos: SV_POSITION;
    float3 normal: TEXCOORD0;
};

[[shader("vertex")]]
FRInput vertMain(Jafg::StaticMeshVertex input)
{
    FRInput result;
    result.pos = mul(mvp.proj, mul(mvp.view, mul(mvp.model, float4(input.pos, 1.0))));
    result.normal = normalize(mul((float3x3)mvp.model, input.normal));
    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    return float4(input.normal * 0.5 + 0.5, 1.0);
}
