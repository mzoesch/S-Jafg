// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/MVP.slang"
#include "Jafg/VSDefaultInput.slang"

[[vk::push_constant]] MVP mvp;

struct FRInput 
{
    float4 pos: SV_POSITION;
    float2 uv: TEXCOORD0;
};

[[shader("vertex")]]
FRInput vertMain(VSInput input)
{
    FRInput result;
    result.pos = mul(mvp.proj, mul(mvp.view, mul(mvp.model, float4(input.pos, 1.0))));
    result.uv = input.uv;
    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    return float4(input.uv, 0.0, 1.0);
}
