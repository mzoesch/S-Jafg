// Copyright mzoesch. All rights reserved.

///
/// Depth prepass: Writes the linear view space depth (the distance along the view direction in meters) of the
/// scene. Used by screen space effects like the ambient occlusion.
///

#include "Jafg.slang"
#include "Jafg/MVP.slang"
#include "Jafg/StaticMeshVertex.slang"

[[vk::push_constant]] MVP mvp;

struct FRInput
{
    float4 pos: SV_POSITION;
    float view_depth: VIEWDEPTH;
};

[[shader("vertex")]]
FRInput vertMain(Jafg::StaticMeshVertex input)
{
    float4 view_pos = mul(mvp.view, mul(mvp.model, float4(input.pos, 1.0)));

    FRInput result;
    result.pos = mul(mvp.proj, view_pos);
    // The view looks down the negative z-axis.
    result.view_depth = -view_pos.z;
    return result;
}

[[shader("fragment")]]
float1 fragMain(FRInput input): SV_TARGET
{
    return input.view_depth;
}
