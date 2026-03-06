// Copyright mzoesch. All rights reserved.
#include "Jafg.slang"

struct VisualShared
{
    float4x4 Proj;
};
[[vk::binding(0, 0)]] ConstantBuffer<VisualShared> Shared;

struct InstanceData
{
    /* (Position, Size) in px. */
    float4 Rect;
    float4 Tint;
    float4 Radii;
    float4 OutlineTint;
    float4 TexCoordTint;
    /* (Min, Max) */
    float4 TexCoordRect;
    float  OutlineThickness;
};
[[vk::binding(0, 1)]] StructuredBuffer<InstanceData> InstanceBuffer;

struct FRInput
{
    float4 Position : SV_POSITION;
    /* In NDC space */
    float2 Size;
    float2 UV : TEXCOORD0;
    float2 TexUV : TEXCOORD1;
    uint InstanceID : SV_InstanceID;
};

[shader("vertex")]
FRInput vertMain(uint vertexId : SV_VertexID, uint instanceId : SV_InstanceID)
{
    InstanceData Instance = InstanceBuffer[instanceId];
    float2 Corner = float2((vertexId >> 1), (vertexId & 1));

    FRInput Result;
    Result.Position = mul(Shared.Proj, float4(Instance.Rect.xy + Corner * Instance.Rect.zw, 0.0, 1.0));
    Result.Size = mul(Shared.Proj, float4(Instance.Rect.zw, 0.0, 0.0)).xy;
    Result.UV = Corner;
    Result.TexUV = lerp(Instance.TexCoordRect.xy, Instance.TexCoordRect.zw, Corner);
    Result.InstanceID = instanceId;

    return Result;
}

[shader("fragment")]
float4 fragMain(FRInput Input) : SV_TARGET
{
    InstanceData Instance = InstanceBuffer[Input.InstanceID];

    float DistLeft   = (Input.UV.x - 0.0) * Input.Size.x;
    float DistTop    = (Input.UV.y - 0.0) * Input.Size.y;
    float DistRight  = (1.0 - Input.UV.x) * Input.Size.x;
    float DistBottom = (1.0 - Input.UV.y) * Input.Size.y;

    bool bCorner  = false;
    bool bOutline = false;

    float4 Result = Instance.Tint;

    if (DistLeft < Instance.Radii.x && DistTop < Instance.Radii.x) /* TL */
    {
        float dist = length(float2(Instance.Radii.x - DistLeft, Instance.Radii.x - DistTop));
        if (dist > Instance.Radii.x)
        {
            discard;
        }
        else if (dist > Instance.Radii.x - Instance.OutlineThickness)
        {
            bOutline = true;
        }
        bCorner = true;
    }
    else if (DistRight < Instance.Radii.y && DistTop < Instance.Radii.y) /* TR */
    {
        float dist = length(float2(Instance.Radii.y - DistRight, Instance.Radii.y - DistTop));
        if (dist > Instance.Radii.y)
        {
            discard;
        }
        else if (dist > Instance.Radii.y - Instance.OutlineThickness)
        {
            bOutline = true;
        }
        bCorner = true;
    }
    else if (DistRight < Instance.Radii.z && DistBottom < Instance.Radii.z) /* BR */
    {
        float dist = length(float2(Instance.Radii.z - DistRight, Instance.Radii.z - DistBottom));
        if (dist > Instance.Radii.z)
        {
            discard;
        }
        else if (dist > Instance.Radii.z - Instance.OutlineThickness)
        {
            bOutline = true;
        }
        bCorner = true;
    }
    else if (DistLeft < Instance.Radii.w && DistBottom < Instance.Radii.w) /* BL */
    {
        float dist = length(float2(Instance.Radii.w - DistLeft, Instance.Radii.w - DistBottom));
        if (dist > Instance.Radii.w)
        {
            discard;
        }
        else if (dist > Instance.Radii.w - Instance.OutlineThickness)
        {
            bOutline = true;
        }
        bCorner = true;
    }

    if (!bCorner) 
    {
        if 
        (
               DistLeft   < Instance.OutlineThickness 
            || DistRight  < Instance.OutlineThickness 
            || DistTop    < Instance.OutlineThickness 
            || DistBottom < Instance.OutlineThickness
        )
        {
            bOutline = true;
        }
    }

    if (bOutline)
    {
        Result = Instance.OutlineTint;
    }

    return Result;
}
