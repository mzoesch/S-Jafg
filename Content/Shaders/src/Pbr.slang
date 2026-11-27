// Copyright mzoesch. All rights reserved.

///
/// The default pbr material for opaque and alpha masked surfaces. Double sided materials are drawn with the same
/// shader without culling (Materials/Pbr.DoubleSided). @see Pbr.Translucent.slang
///

#include "Jafg.slang"
#include "Jafg/StaticMeshVertex.slang"
#include "Jafg/Pbr.slang"

struct FRInput 
{
    float4 pos: SV_POSITION;
    float3 translation: TRANSLATION;
    float3 normal: NORMAL;
    float2 uv: TEXCOORD0;
    float4 tangent: TANGENT;
};

[[shader("vertex")]]
FRInput vertMain(Jafg::StaticMeshVertex input)
{
    float4 world_pos = mul(request.model, float4(input.pos, 1.0));

    FRInput result;

    result.pos = mul(world_data.proj, mul(world_data.view, world_pos));
    result.translation = world_pos.xyz;
    float3x3 model = (float3x3)request.model;
    result.normal = normalize(mul(model, input.normal));
    result.uv = input.uv;
    /* Not normalized: Meshes without tangents have zero ones. Mirroring flips the handedness of the frame. */
    result.tangent = float4(mul(model, input.tangent.xyz), determinant(model) < 0.0 ? -input.tangent.w : input.tangent.w);

    return result;
}

[[shader("fragment")]]
float4 fragMain(FRInput input, bool front_facing: SV_IsFrontFace): SV_TARGET
{
    Jafg::PbrMaterial material = materials[request.material];

    Jafg::PbrGeometry geometry;
    geometry.world_pos = input.translation;
    geometry.normal = input.normal;
    geometry.uv = input.uv;
    geometry.tangent = input.tangent;
    geometry.frag_coord = input.pos;
    geometry.front_facing = front_facing;

    Jafg::PbrResult result = Jafg::evaluate_pbr(material, geometry, false);
    /* After everything that needs derivatives. */
    if (!result.covered)
    {
        discard;
    }
    return float4(result.radiance, 1.0);
}
