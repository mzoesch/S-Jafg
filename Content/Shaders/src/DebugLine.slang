// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/Ray.slang"
#include "Jafg/ViewProj.slang"

[[vk::binding(0, 0),UpdateFrequency(PerFrame)]] StructuredBuffer<Ray> rays;
[[vk::binding(1, 0),UpdateFrequency(PerFrame)]] ConstantBuffer<ViewProj> view_proj;

struct FRInput
{
    float4 pos: SV_POSITION;
    LColor color: COLOR;
};

[[shader("vertex")]]
FRInput vertMain(uint vertex_id: SV_VertexID, uint instance_id: SV_InstanceID)
{
    Ray instance = rays[instance_id];

    FRInput result;
    result.pos = mul(view_proj.matrix, (vertex_id == 0) 
        ? instance.start 
        : float4(instance.end.xyz, 1.0)
        );
    result.color = asuint(instance.end.w);
    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    return maths::color_to_linear(input.color, 2.2);
}
