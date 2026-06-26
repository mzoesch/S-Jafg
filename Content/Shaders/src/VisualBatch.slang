// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"

[[CxxName("Jafg::UBO::VisualShared")]]
struct LVisualShared
{
    float4x4 Proj;
    float    Gamma;
};
[[vk::binding(0, 0),Shared("Jafg::UBO::VisualShared")]] ConstantBuffer<LVisualShared> VisualShared;

[[vk::binding(0, 1),Shared("Jafg::UBO::Bindless")]] SamplerState[] Samplers;
[[vk::binding(1, 1),Shared("Jafg::UBO::Bindless")]] Texture2D[] BindlessTextures;

[[CxxName("Jafg::LVisualInstance")]]
struct InstanceData
{
    /* (Position, Extent) in px. */
    float4 Rect;
    /* (Min, Max) */
    float4 TexCoordRect;
    float4 Radii;

    LColor Tint;
    LColor BorderTint;
    LColor OutlineTint;
    float OutlineThickness;
    
    uint TextureIndex;
    uint SamplerIndex;
    float MsdfPixelRange;
    uint _pad1;
};
[[vk::binding(0, 2),UpdateFrequency(PerFrame)]] StructuredBuffer<InstanceData> InstanceBuffer;

struct FRInput
{
    float4 Position: SV_POSITION;
    float2 UV: TEXCOORD0;
    float2 TexUV: TEXCOORD1;
    uint InstanceID: SV_InstanceID;
};

[shader("vertex")]
FRInput vertMain(uint VertexID: SV_VertexID, uint InstanceID: SV_InstanceID, uint Offset: SV_StartInstanceLocation)
{
    InstanceData Instance = InstanceBuffer[Offset + InstanceID];
    float2 Corner = float2((VertexID >> 1), (VertexID & 1));

    FRInput Result;
    Result.Position = mul(VisualShared.Proj, float4(Instance.Rect.xy + Corner * Instance.Rect.zw, 0.0, 1.0));
    Result.UV = Corner;
    Result.TexUV = lerp(Instance.TexCoordRect.xy, Instance.TexCoordRect.zw, Corner);
    Result.InstanceID = Offset + InstanceID;

    return Result;
}

[shader("fragment")]
float4 fragMain(FRInput Input): SV_TARGET
{
    InstanceData Instance = InstanceBuffer[Input.InstanceID];

    float DistLeft   = (Input.UV.x - 0.0) * Instance.Rect.z;
    float DistTop    = (Input.UV.y - 0.0) * Instance.Rect.w;
    float DistRight  = (1.0 - Input.UV.x) * Instance.Rect.z;
    float DistBottom = (1.0 - Input.UV.y) * Instance.Rect.w;

    bool bCorner  = false;
    bool bOutline = false;

    float4 Result;
    if (Instance.MsdfPixelRange != 0.0f)
    {
        float4 Tint = maths::color_to_linear(Instance.Tint, VisualShared.Gamma);
        float3 msdf = (BindlessTextures[NonUniformResourceIndex(Instance.TextureIndex)].Sample(Samplers[Instance.SamplerIndex], Input.TexUV)).rgb;
        Result = float4(Tint.rgb, Tint.w * clamp(Instance.MsdfPixelRange * (maths::median_of_three(msdf.r, msdf.g, msdf.b) - 0.5) + 0.5, 0.0, 1.0));
    }
    else if (Instance.SamplerIndex == JAFG_SAMPLER_LINEAR_CLAMP_TO_BORDER || Instance.SamplerIndex == JAFG_SAMPLER_NEAREST_CLAMP_TO_BORDER)
    {
        // This is prob. better, but produces sometimes wrong results...
        // Result = lerp(
        //     maths::color_to_linear(Instance.Tint) * BindlessTextures[NonUniformResourceIndex(Instance.TextureIndex)].Sample(Samplers[Instance.SamplerIndex], Input.TexUV),
        //     maths::color_to_linear(Instance.BorderTint),
        //     saturate(step(1.0, Input.TexUV.x) + step(Input.TexUV.x, 0.0) + step(1.0, Input.TexUV.y) + step(Input.TexUV.y, 0.0))
        //     );
        
        if (Input.TexUV.x < 0.0 || Input.TexUV.x > 1.0 || Input.TexUV.y < 0.0 || Input.TexUV.y > 1.0)
        {
            Result = maths::color_to_linear(Instance.BorderTint, VisualShared.Gamma);
        }
        else
        {
            Result = maths::color_to_linear(Instance.Tint, VisualShared.Gamma) * BindlessTextures[NonUniformResourceIndex(Instance.TextureIndex)].Sample(Samplers[Instance.SamplerIndex], Input.TexUV);
        }
    }
    else
    {
        Result = maths::color_to_linear(Instance.Tint, VisualShared.Gamma) * BindlessTextures[NonUniformResourceIndex(Instance.TextureIndex)].Sample(Samplers[Instance.SamplerIndex], Input.TexUV);
    }

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
    if (DistRight < Instance.Radii.y && DistTop < Instance.Radii.y) /* TR */
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
    if (DistRight < Instance.Radii.z && DistBottom < Instance.Radii.z) /* BR */
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
    if (DistLeft < Instance.Radii.w && DistBottom < Instance.Radii.w) /* BL */
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

    if (Instance.OutlineThickness > 0.0)
    {
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
            Result = maths::color_to_linear(Instance.OutlineTint, VisualShared.Gamma);
        }
    }

    return Result;
}
