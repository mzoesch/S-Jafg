// Copyright mzoesch. All rights reserved.

///
/// Shadow pass: Writes only the depth of the scene as seen from a light into its tile of the shadow atlas.
/// The pipeline has no color attachment. @see Engine/include/Rhi/Shadow.h
///
/// Respects the coverage of the pbr materials: Alpha masks cut out their shadows. Translucent surfaces (transmissive
/// or not fully opaque) block only a part of the light, which a shadow map cannot store. They are dithered instead,
/// which the filter of the lookup softens into a lighter shadow.
///

#include "Jafg.slang"
#include "Jafg/MeshPbr.slang"

struct FRInput
{
    float4 pos: SV_POSITION;
    float2 uv: TEXCOORD0;
};

[[shader("vertex")]]
FRInput vertMain(Jafg::StaticMeshVertex input)
{
    FRInput result;
    result.pos = mul(mesh.proj, mul(mesh.view, mul(mesh.model, float4(input.pos, 1.0))));
    result.uv = input.uv;
    return result;
}

[[shader("fragment")]]
void fragMain(FRInput input)
{
    float coverage;
    Jafg::PbrMaterial material;
    if (!Jafg::mesh_pbr_coverage(input.uv, coverage, material))
    {
        discard;
    }
    if (mesh.material != INVALID_INDEX)
    {
        float blocked = Jafg::pbr_material_shadow_coverage(material, coverage);
        if (blocked < 1.0 && Jafg::interleaved_gradient_noise(input.pos.xy) >= blocked)
        {
            discard;
        }
    }
}
