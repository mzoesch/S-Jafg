// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/StaticMesh.h"


namespace Jafg
{

inline EStaticMeshResult LStaticMesh::ReloadModel(
      EUploadBehavior Behavior /* = EUploadBehavior::Immediate */
    , EUploadHostMemoryBehavior HostMemoryBehavior /* = EUploadHostMemoryBehavior::Free */
    )
{
    LOG_VERBOSE(LogRhi, "Reloading static mesh from path [{}].", this->Path)

    if (Finder::DoesFileExist(this->Path) == false)
    {
        return EStaticMeshResult::FileNotFound;
    }

    tinyobj::attrib_t Attrib;
    std::vector<tinyobj::shape_t> Shapes;
    std::vector<tinyobj::material_t> Materials;
    LString Warning;
    LString Error;

    auto Result{ tinyobj::LoadObj(&Attrib, &Shapes, &Materials, &Warning, &Error, this->Path.c_str()) };
    if (Warning.empty() == false) { LOG_WARNING(LogRhi, "tinyobj: {}", Warning) }
    if (Result == false) { LOG_ERROR(LogRhi, "tinyobj: {}", Error) }
    if (Result == false) { return EStaticMeshResult::LoadingError; }

    std::unordered_map<LVertex, u32> UniqueVertices;

    algo::orphan(&this->Vertices);
    algo::orphan(&this->Indices);

    for (auto const& Shape : Shapes)
    {
        for (auto const& Idx : Shape.mesh.indices)
        {
            LVertex V;

            V.Location = {
                Attrib.vertices[3 * Idx.vertex_index + 0],
                Attrib.vertices[3 * Idx.vertex_index + 1],
                Attrib.vertices[3 * Idx.vertex_index + 2]
            };

            V.TexCoord = {
                Attrib.texcoords[2 * Idx.texcoord_index + 0],
                1.0f - Attrib.texcoords[2 * Idx.texcoord_index + 1]
            };

            V.Color = {1.0f, 1.0f, 1.0f};

            if (UniqueVertices.contains(V) == false)
            {
                UniqueVertices[V] = static_cast<uint32_t>(this->Vertices.size());
                this->Vertices.push_back(V);
            }

            Indices.push_back(UniqueVertices[V]);
        }
    }

    if (Behavior == EUploadBehavior::Immediate)
    {
        this->Upload(HostMemoryBehavior);
    }

    return EStaticMeshResult::Success;
}

inline void LStaticMesh::Upload(EUploadHostMemoryBehavior Behavior /* = EUploadHostMemoryBehavior::Free */)
{
    check( this->Vertices.size() > 0 )
    check( this->Indices.size() > 0 )

    this->VertexBuffer = GEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Vertex({
        .BufferCopy = vk::BufferCopy{ 0, 0, sizeof(this->Vertices[0]) * this->Vertices.size() },
        .Data = this->Vertices.data(),
        }));

    this->IndexBuffer = GEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Index({
        .BufferCopy = vk::BufferCopy{ 0, 0, sizeof(this->Indices[0]) * this->Indices.size() },
        .Data = this->Indices.data(),
        }));

    this->IndexCount = static_cast<u64>(this->Indices.size());

    if (Behavior == EUploadHostMemoryBehavior::Free)
    {
        algo::orphan(&this->Vertices);
        algo::orphan(&this->Indices);
    }

    return;
}

} /* ~Namespace Jafg */
