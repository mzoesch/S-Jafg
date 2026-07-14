// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/ViewProj.slang"

[[CxxName("Jafg::SSBO::Ray")]]
struct Line
{
    float4 Start; // expects w to be 1.0.
    float4 End; // Actually a float3+LColor (combined to save space).
};

struct FRInput
{
    float4 Position: SV_POSITION;
    LColor Color: COLOR;
};

[[vk::binding(0, 0),UpdateFrequency(PerFrame)]] StructuredBuffer<Line> Lines;
[[vk::binding(1, 0),UpdateFrequency(PerFrame)]] ConstantBuffer<ViewProj> VP;

[[shader("vertex")]]
FRInput vertMain(uint VertexID: SV_VertexID, uint InstanceID: SV_InstanceID)
{
    Line Instance = Lines[InstanceID];

    FRInput Result;
    Result.Position = mul(VP.Matrix, (VertexID == 0) 
        ? Instance.Start 
        : float4(Instance.End.xyz, 1.0)
        );
    Result.Color = asuint(Instance.End.w);
    return Result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    return maths::color_to_linear(input.Color, 2.2);
}
