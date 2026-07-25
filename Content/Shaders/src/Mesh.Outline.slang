// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/MVP.slang"
#include "Jafg/VSDefaultInput.slang"

[[vk::push_constant]] MVP mvp;

struct FRInput
{
    float4 pos: SV_POSITION;
};

[[shader("vertex")]]
FRInput vertMain(VSInput input)
{
    FRInput result;
    result.pos = mul(mvp.proj, mul(mvp.view, mul(mvp.model, float4(input.pos, 1.0))));
    return result;
}

[[shader("fragment")]]
float1 fragMain(FRInput input): SV_TARGET
{
    return 1.0;
}
