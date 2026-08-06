// Copyright mzoesch. All rights reserved.

#include "Jafg.slang"
#include "Jafg/SolidColorInput.slang"
#include "Jafg/WorldData.slang"
#include "Jafg/MVP.slang"
#include "Jafg/StaticMeshVertex.slang"

static const float GIZMO_PIXEL_SIZE = 20.0;

[[vk::binding(0,0),UpdateFrequency(PerFrame)]] ConstantBuffer<SolidColorInput> sci;
[[vk::binding(0,1),Shared("Jafg::UBO::WorldData")]] ConstantBuffer<WorldData> world_data;

[[vk::push_constant]] MVP mvp;

struct FRInput
{
    float4 pos: SV_POSITION;
    float4 color: COLOR;
};

[[shader("vertex")]]
FRInput vertMain(Jafg::StaticMeshVertex input)
{
    FRInput result;

    float distance = -mul(mvp.view, mul(mvp.model, float4(0,0,0,1))).z;
    float scale = GIZMO_PIXEL_SIZE 
        * (2.0 * distance * tan(world_data.vertical_fov * 0.5)) / world_data.viewport_height;
    float4 world_pos = mul(mvp.model, float4(input.pos * scale, 1.0));
    result.pos = mul(mvp.proj, mul(mvp.view, world_pos));

    result.color = maths::color_to_linear(sci.color, 2.2);

    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input): SV_TARGET
{
    return input.color;
}
