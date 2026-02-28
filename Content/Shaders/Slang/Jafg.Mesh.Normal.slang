// Copyright mzoesch. All rights reserved.
#include "Jafg.slang"
#include "Jafg/MVP.slang"
#include "Jafg/VSDefaultInput.slang"

struct FRInput 
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
};

[[vk::push_constant]] MVP pcs;

[[shader("vertex")]]
FRInput vertMain(VSInput input)
{
    FRInput result;
    result.Position = mul(pcs.Proj, mul(pcs.View, mul(pcs.Model, float4(input.Position, 1.0))));
    result.Normal = normalize(mul((float3x3)pcs.Model, input.Normal));
    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input) : SV_TARGET
{
    return float4(input.Normal * 0.5 + 0.5, 1.0);
}
