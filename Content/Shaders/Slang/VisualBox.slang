struct VSInput
{
    float2 InPosition;
};

struct VSOutput 
{
    float4 Position : SV_Position;
};

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;
    output.Position = float4(input.InPosition, 0.0, 1.0);
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput fragIn) : SV_TARGET
{
    return float4(1.0, 0.0, 0.0, 1.0);
}
