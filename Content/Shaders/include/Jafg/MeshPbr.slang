// Copyright mzoesch. All rights reserved.

///
/// For passes that draw every mesh with the same material, but still respect the coverage of the pbr material that the
/// mesh is drawn with otherwise (e.g., alpha masks in the shadows). @see Jafg::PC::MeshPbr
///

#pragma once

#include "Jafg.slang"
#include "Jafg/PbrMaterial.slang"
#include "Jafg/StaticMeshVertex.slang"

[[CxxName("Jafg::PC::MeshPbr")]]
struct MeshPbr
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    /// The pbr material the mesh is drawn with otherwise or INVALID_INDEX.
    uint material;
    uint _pad0;
    uint _pad1;
    uint _pad2;
};

[[vk::push_constant]] MeshPbr mesh;

[[vk::binding(0, 0),Shared("Jafg::UBO::Bindless")]] SamplerState[] samplers;
[[vk::binding(1, 0),Shared("Jafg::UBO::Bindless")]] Texture2D[] bindless_textures;

[[vk::binding(0, 1),Shared("Jafg::SSBO::PbrMaterials")]] StructuredBuffer<Jafg::PbrMaterial> materials;

namespace Jafg
{

///
/// The coverage of the pbr material of the mesh at the uv of the mesh. False if alpha masked away. One for meshes
/// without a pbr material. Has to be called in uniform control flow.
///
bool mesh_pbr_coverage(float2 uv, out float coverage, out PbrMaterial material)
{
    coverage = 1.0;
    material = (PbrMaterial)0;
    if (mesh.material == INVALID_INDEX)
    {
        return true;
    }

    material = materials[mesh.material];
    // The material is uniform for the whole draw call.
    float alpha = 1.0;
    if (pbr_material_uses_alpha(material) && material.base_color_texture != INVALID_INDEX)
    {
        alpha = bindless_textures[NonUniformResourceIndex(material.base_color_texture)]
            .Sample(samplers[NonUniformResourceIndex(material.sampler)], pbr_material_uv(material, uv)).a;
    }
    return pbr_material_coverage(material, alpha, coverage);
}

} /* ~Namespace Jafg */
