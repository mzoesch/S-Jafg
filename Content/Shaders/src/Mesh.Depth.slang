// Copyright mzoesch. All rights reserved.

///
/// Depth prepass: Writes the linear view space depth (the distance along the view direction in meters) of the
/// scene. Used by screen space effects like the ambient occlusion.
///
/// Only of what is opaque: Translucent pbr materials are left out, alpha masks cut out.
///

#include "Jafg.slang"
#include "Jafg/MeshPbr.slang"

struct FRInput
{
    float4 pos: SV_POSITION;
    float view_depth: VIEWDEPTH;
    float2 uv: TEXCOORD0;
};

[[shader("vertex")]]
FRInput vertMain(Jafg::StaticMeshVertex input)
{
    float4 view_pos = mul(mesh.view, mul(mesh.model, float4(input.pos, 1.0)));

    FRInput result;
    result.pos = mul(mesh.proj, view_pos);
    // The view looks down the negative z-axis.
    result.view_depth = -view_pos.z;
    result.uv = input.uv;
    return result;
}

[[shader("fragment")]]
float1 fragMain(FRInput input): SV_TARGET
{
    float coverage;
    Jafg::PbrMaterial material;
    if (!Jafg::mesh_pbr_coverage(input.uv, coverage, material)
        || (mesh.material != INVALID_INDEX && Jafg::pbr_material_is_translucent(material)))
    {
        discard;
    }
    return input.view_depth;
}
