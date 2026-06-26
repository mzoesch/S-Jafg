// Copyright mzoesch. All rights reserved.
#include "Jafg.slang"
#include "Jafg/MVP.slang"
#include "Jafg/VSDefaultInput.slang"

struct FRInput
{
    float4 Position : SV_POSITION;
};

[[vk::push_constant]] MVP pcs;

[[shader("vertex")]]
FRInput vertMain(VSInput input)
{
    FRInput result;
    result.Position = mul(pcs.Proj, mul(pcs.View, mul(pcs.Model, float4(input.Position, 1.0))));
    return result;
}

[[shader("fragment")]]
float1 fragMain(FRInput input) : SV_TARGET
{
    return 1.0;
}
