// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

struct Line
{
    float4 start; // expects w to be 1.0
    float4 end;
};

struct VP
{
    float4x4 ViewProj;
};

struct FRInput
{
    float4 Position : SV_POSITION;
};

[[vk::binding(0, 0)]] StructuredBuffer<Line> Lines;
[[vk::binding(1, 0)]] ConstantBuffer<VP> vp;

[[shader("vertex")]]
FRInput vertMain(uint VertexID : SV_VertexID, uint InstanceID : SV_InstanceID)
{
    Line line = Lines[InstanceID];
    float4 pos = (VertexID == 0) ? line.start : line.end;

    FRInput result;
    result.Position = mul(vp.ViewProj, pos);
    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input) : SV_TARGET
{
    return float4(1.0, 0.0, 0.0, 1.0);
}
