// Copyright mzoesch. All rights reserved.
#include "Jafg.slang"
#include "Bindings/Jafg.PerspectiveCamera.slang"

struct PC_Model
{
    float4x4 model;
};
[[vk::push_constant]] ConstantBuffer<PC_Model> pc;

struct VSInput
{
    float3 position;
    float3 normal;
    float2 tex_coord;
};

struct VSOutput
{
    float4 position : SV_Position;
    float3 normal;
    float2 tex_coord;
};

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;
    output.position =
        mul(
            camera.proj,
            mul(camera.view, mul(pc.model, float4(input.position, 1.0)))
            );
    output.normal = input.normal;
    output.tex_coord = input.tex_coord;

    return output;
}

[[vk::binding(0, 1)]] Sampler2D texture;

#if !JAFG_PREFER_NORMALS
static const float3 AmbientColor = float3(0.4);
static const float3 LightDirection = normalize(float3(-0.7, -0.5, 0.8));
static const float  DiffuseStrength = 0.3;
#endif /* !JAFG_PREFER_NORMALS */

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET
{
#if JAFG_PREFER_NORMALS
    return float4(vertIn.normal * 0.5 + 0.5, 1.0);
#else /* JAFG_PREFER_NORMALS */
    float Diffuse = max(0, dot(vertIn.normal, LightDirection)) * DiffuseStrength;
    float4 ResultingLight = float4(AmbientColor + Diffuse, 1.0);
    return texture.Sample(vertIn.tex_coord) * ResultingLight;
#endif /* !JAFG_PREFER_NORMALS */
}
