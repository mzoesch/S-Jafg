// Copyright mzoesch. All rights reserved.

///
/// Back depth pass: Writes the linear view space depth (m) of the closest back faces of translucent draws that need
/// the thickness of their volume. The difference to the depth of their front faces is how much they absorb and how
/// far they refract. Drawn with front face culling. Alpha masks cut out.
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
    if (!Jafg::mesh_pbr_coverage(input.uv, coverage, material))
    {
        discard;
    }
    return input.view_depth;
}
