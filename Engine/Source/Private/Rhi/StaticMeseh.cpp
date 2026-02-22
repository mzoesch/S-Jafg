// Copyright mzoesch. All rights reserved.

#include "Rhi/StaticMesh.h"
#include "Engine/Engine.h"

Jafg::LStaticMesh::EResult Jafg::LStaticMesh::LoadToHost()
{
    check(this->Path.empty() == false)
    LOG_VERBOSE(LogRhi, "Loading static mesh from path [{}].", this->Path)

    if (Finder::DoesFileExist(this->Path) == false)
    {
        return EResult::FileNotFound;
    }

    if (this->IsOnHost())
    {
        LOG_VERBOSE(LogRhi, "Static mesh [{}] already loaded to host memory. Freeing previous host memory and reloading.", this->Path)
        this->FreeFromHost();
    }
    check(this->Vertices.empty() && this->Indices.empty())

    tinyobj::attrib_t Attrib;
    std::vector<tinyobj::shape_t> Shapes;
    std::vector<tinyobj::material_t> Materials;
    LString Warning;
    LString Error;

#if JAFG_PLATFORM_USES_UTF8
    auto Result{tinyobj::LoadObj(&Attrib, &Shapes, &Materials, &Warning, &Error, this->Path.c_str())};
#else /* JAFG_PLATFORM_USES_UTF8 */
    auto Result{tinyobj::LoadObj(&Attrib, &Shapes, &Materials, &Warning, &Error, this->Path.string().c_str())};
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    if (Warning.empty() == false) { LOG_WARNING(LogRhi, "tinyobj: {}", Warning) }
    if (Result == false) { LOG_ERROR(LogRhi, "tinyobj: {}", Error) }
    if (Result == false) { return EResult::LoadingError; }

    std::unordered_map<Vertex, u32> UniqueVertices;
    for (auto const& Shape : Shapes)
    {
        for (auto const& Idx : Shape.mesh.indices)
        {
            LStaticMesh::Vertex V;

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

    return EResult::Success;
}

void Jafg::LStaticMesh::LoadToDevice()
{
    check(this->IsOnHost())
    LOG_VERBOSE(LogRhi, "Loading static mesh [{}] to device.", this->Path)

    if (this->IsOnDevice())
    {
        LOG_VERBOSE(LogRhi, "Static mesh [{}] already loaded to device memory. Freeing previous device memory and reloading.", this->Path)
        this->FreeFromDevice();
    }
    check(this->IndexCount == 0 && this->VertexBuffer.GetAllocation() == nullptr && this->IndexBuffer.GetAllocation() == nullptr)

    this->VertexBuffer = GEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Vertex({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(this->Vertices[0]) * this->Vertices.size()},
        .Data = this->Vertices.data(),
        }));

    this->IndexBuffer = GEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Index({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(this->Indices[0]) * this->Indices.size()},
        .Data = this->Indices.data(),
        }));

    this->IndexCount = static_cast<u32>(this->Indices.size());

    return;
}

void Jafg::LStaticMesh::DrawIndex(LRenderInfo const& Info) const
{
    check(this->IndexCount > 0)
    check(this->VertexBuffer.GetBuffer() && this->IndexBuffer.GetBuffer())

    Info.CommandBuffer.bindVertexBuffers(0, this->VertexBuffer.GetBuffer(), {0});
    Info.CommandBuffer.bindIndexBuffer(this->IndexBuffer.GetBuffer(), 0, vk::IndexTypeValue<decltype(this->Indices)::value_type>::value);

    Info.CommandBuffer.drawIndexed(static_cast<uint32_t>(this->IndexCount), 1, 0, 0, 0);

    return;
}
