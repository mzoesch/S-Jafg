// Copyright mzoesch. All rights reserved.

///
/// The default pbr material for translucent surfaces: Transmissive, not fully opaque or alpha blended ones. Drawn
/// after everything opaque, from the farthest to the closest, without writing the depth.
///
/// Blends with dual sources: target = radiance + target * transmittance. The radiance contains the light that the
/// surface reflects, emits and refracts (from the copy of the opaque scene). The transmittance lets through what is
/// already on the target behind the surface: Where it is not covered, and through smooth thin walls.
/// The back faces of double sided materials are drawn first (Materials/Pbr.Translucent.BackFaces).
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

struct FROutput
{
    [[vk::location(0)]] float4 radiance: SV_Target0;
    [[vk::location(0), vk::index(1)]] float4 transmittance: SV_Target1;
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
FROutput fragMain(FRInput input, bool front_facing: SV_IsFrontFace)
{
    Jafg::PbrMaterial material = materials[request.material];

    Jafg::PbrGeometry geometry;
    geometry.world_pos = input.translation;
    geometry.normal = input.normal;
    geometry.uv = input.uv;
    geometry.tangent = input.tangent;
    geometry.frag_coord = input.pos;
    geometry.front_facing = front_facing;

    Jafg::PbrResult result = Jafg::evaluate_pbr(material, geometry, true);
    /* After everything that needs derivatives. */
    if (!result.covered)
    {
        discard;
    }

    FROutput output;
    output.radiance = float4(result.radiance, result.coverage);
    output.transmittance = float4(saturate(result.transmittance), 1.0 - result.coverage);
    return output;
}
