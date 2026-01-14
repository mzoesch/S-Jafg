struct UBO_Camera
{
    float4x4 view;
    float4x4 proj;
};
[[vk::binding(0, 0)]] ConstantBuffer<UBO_Camera> camera;

struct PC_Model
{
    float4x4 model;
};
[[vk::push_constant]] ConstantBuffer<PC_Model> pc;

struct VSInput
{
    float3 position;
    float3 color;
    float2 tex_coord;
};

struct VSOutput
{
    float4 position : SV_Position;
    float3 color;
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
    output.color = input.color;
    output.tex_coord = input.tex_coord;

    return output;
}

[[vk::binding(1, 0)]] Sampler2D texture;

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET
{
   return texture.Sample(vertIn.tex_coord);
}
