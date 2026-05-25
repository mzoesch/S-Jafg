// Copyright mzoesch. All rights reserved.

#include "Rhi/StaticMesh.h"
#include "Engine/Engine.h"
#include "Rhi/VertexInput.h"
#include "Rhi/RenderInfo.h"

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-W#warnings"
    #pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
    #pragma clang diagnostic ignored "-Wdeprecated-literal-operator"
#endif /* JAFG_WITH_CLANG */
    #define TINYOBJLOADER_IMPLEMENTATION
    #include <tiny_obj_loader.h>
    #define TINYGLTF_IMPLEMENTATION
    #include <tiny_gltf.h>
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */

static_assert(sizeof(uint8_t) == sizeof(u8));
static_assert(sizeof(uint16_t) == sizeof(u16));
static_assert(sizeof(uint32_t) == sizeof(u32));

namespace
{

Jafg::LRegisterDeviceVertexInput<Jafg::LStaticMesh::Vertex> StaticMeshVertexRegistration{};

enum struct ETinyObjHint
{
    Binary,
    Json,
};

Jafg::LStaticMesh::EResult LoadViaTinyGltf(Jafg::LStaticMesh& Target, ETinyObjHint Hint)
{
    typedef Jafg::LStaticMesh::Vertex LVertex;

    tinygltf::TinyGLTF Ldr;
    tinygltf::Model Model;
    LString Warning;
    LString Error;

    bool Result{};
    if (Hint == ETinyObjHint::Binary)
    {
        LOG_TRACE(LogRhi, "Loading static mesh [{}] via tinygltf as binary glTF.", Target.GetPath())
#if JAFG_PLATFORM_USES_UTF8
        Result = Ldr.LoadBinaryFromFile(&Model, &Warning, &Error, Target.GetPath().native().c_str());
#else /* JAFG_PLATFORM_USES_UTF8 */
        Result = Ldr.LoadBinaryFromFile(&Model, &Error, &Warning, Target.GetPath().string().c_str());
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    }
    else
    {
        check(Hint == ETinyObjHint::Json)
#if JAFG_PLATFORM_USES_UTF8
        Result = Ldr.LoadASCIIFromFile(&Model, &Warning, &Error, Target.GetPath().native().c_str());
#else /* JAFG_PLATFORM_USES_UTF8 */
        Result = Ldr.LoadASCIIFromFile(&Model, &Error, &Warning, Target.GetPath().string().c_str());
#endif /* !JAFG_PLATFORM_USES_UTF8 */
    }
    if (Warning.empty() == false) { LOG_WARNING(LogRhi, "tinygltf: {}", Warning) }
    if (Error.empty() == false) { LOG_ERROR(LogRhi, "tinygltf: {}", Error) }
    if (Result == false)
    {
        return Jafg::LStaticMesh::EResult::LoadingError;
    }

    {
        auto& Asset{Model.asset};
        LOG_TRACE(LogRhi, "[{}]: Asset: v[{}>={}] g[{}]",
            Target.GetPath(), Asset.version, Asset.minVersion, Asset.generator
            )
    }

    LOG_TRACE(LogRhi, "[{}]: {} meshes, {} materials, {} textures, {} samplers, {} animations, {} skins, {} nodes"
                        ", {} buffers, {} buffer views, {} accessors."
        , Target.GetPath(), Model.meshes.size(), Model.materials.size(), Model.textures.size(), Model.samplers.size()
        , Model.animations.size(), Model.skins.size(), Model.nodes.size(), Model.buffers.size(), Model.bufferViews.size(), Model.accessors.size()
        )

    std::unordered_map<Jafg::LStaticMesh::Vertex, u32> uniqueVertices;
    for (const auto &Mesh : Model.meshes)
    {
        for (const  auto &Primitive : Mesh.primitives)
        {
            tinygltf::Accessor const& IdxAccessor{Model.accessors[Primitive.indices]};
            tinygltf::BufferView const& IdxBufferView{Model.bufferViews[IdxAccessor.bufferView]};
            tinygltf::Buffer const& IdxBuffer{Model.buffers[IdxBufferView.buffer]};

            if (Primitive.attributes.contains("POSITION") == false)
            {
                LOG_FATAL(LogRhi, "[{}]: Primitive [{}] has no POSITION attribute.", Target.GetPath(), Primitive.indices)
            }
            tinygltf::Accessor const& PosAccessor{Model.accessors[Primitive.attributes.at("POSITION")]};
            tinygltf::BufferView const& PosBufferView{Model.bufferViews[PosAccessor.bufferView]};
            tinygltf::Buffer const& PosBuffer{Model.buffers[PosBufferView.buffer]};

            tinygltf::Accessor const* NormalAccessor{};
            tinygltf::BufferView const* NormalBufferView{};
            tinygltf::Buffer const* NormalBuffer{};
            if (auto It{Primitive.attributes.find("NORMAL")}; It != Primitive.attributes.end())
            {
                NormalAccessor   = &Model.accessors[It->second];
                NormalBufferView = &Model.bufferViews[NormalAccessor->bufferView];
                NormalBuffer     = &Model.buffers[NormalBufferView->buffer];
            }
            else
            {
                LOG_WARNING(LogRhi, "[{}]: Primitive [{}] has no NORMAL attribute.", Target.GetPath(), Primitive.indices)
            }

            tinygltf::Accessor const* TexCoordAccessor{};
            tinygltf::BufferView const* TexCoordBufferView{};
            tinygltf::Buffer const* TexCoordBuffer{};
            if (auto It{Primitive.attributes.find("TEXCOORD_0")}; It != Primitive.attributes.end())
            {
                TexCoordAccessor   = &Model.accessors[It->second];
                TexCoordBufferView = &Model.bufferViews[TexCoordAccessor->bufferView];
                TexCoordBuffer     = &Model.buffers[TexCoordBufferView->buffer];
            }
            else
            {
                LOG_WARNING(LogRhi, "[{}]: Primitive [{}] has no TEXCOORD_0 attribute.", Target.GetPath(), Primitive.indices)
            }

            tinygltf::Accessor const* TangentAccessor{};
            tinygltf::BufferView const* TangentBufferView{};
            tinygltf::Buffer const* TangentBuffer{};
            if (auto It{Primitive.attributes.find("TANGENT")}; It != Primitive.attributes.end())
            {
                TangentAccessor   = &Model.accessors[It->second];
                TangentBufferView = &Model.bufferViews[TangentAccessor->bufferView];
                TangentBuffer     = &Model.buffers[TangentBufferView->buffer];
            }
            else
            {
                LOG_WARNING(LogRhi, "[{}]: Primitive [{}] has no TANGENT attribute.", Target.GetPath(), Primitive.indices)
            }

            uint32_t baseVertex = static_cast<uint32_t>(Target.Vertices.size());

            for (auto Idx{0uz}; Idx < PosAccessor.count; ++Idx)
            {
                LVertex Vertex{};

                auto* pPosition{reinterpret_cast<f32 const*>(&PosBuffer.data[
                    PosBufferView.byteOffset + PosAccessor.byteOffset + Idx * sizeof(decltype(Vertex.Position))
                    ])};
                Vertex.Position = {pPosition[0], pPosition[1], pPosition[2]};

                if (NormalAccessor)
                {
                    check(NormalBufferView && NormalBuffer)
                    auto* pNormal{reinterpret_cast<f32 const*>(&NormalBuffer->data[
                        NormalBufferView->byteOffset + NormalAccessor->byteOffset + Idx * sizeof(decltype(Vertex.Normal))
                        ])};
                    Vertex.Normal = {pNormal[0], pNormal[1], pNormal[2]};
                }
                else
                {
                    Vertex.Normal = {0.0f, 0.0f, 0.0f};
                }

                if (TexCoordAccessor)
                {
                    check(TexCoordBufferView && TexCoordBuffer)
                    auto* pTexCoord{reinterpret_cast<f32 const*>(&TexCoordBuffer->data[
                        TexCoordBufferView->byteOffset + TexCoordAccessor->byteOffset + Idx * sizeof(decltype(Vertex.TexCoord))
                        ])};
                    Vertex.TexCoord = {pTexCoord[0], pTexCoord[1]};
                }
                else
                {
                    Vertex.TexCoord = {0.0f, 0.0f};
                }

                if (TangentAccessor)
                {
                    check(TangentBufferView && TangentBuffer)
                    auto* pTangent{reinterpret_cast<f32 const*>(&TangentBuffer->data[
                        TangentBufferView->byteOffset + TangentAccessor->byteOffset + Idx * sizeof(decltype(Vertex.Tangent))
                        ])};
                    Vertex.Tangent = {pTangent[0], pTangent[1], pTangent[2], pTangent[3]};
                }
                else
                {
                    Vertex.Tangent = {0.0f, 0.0f, 0.0f, 1.0f};
                }

                Target.Vertices.emplace_back(std::move(Vertex));
            }

            auto const* pIdxData{&IdxBuffer.data[IdxBufferView.byteOffset + IdxAccessor.byteOffset]};
            auto IdxCount{IdxAccessor.count};
            auto IdxStride{
                IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT   ? sizeof(u32) :
                IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(u16) :
                IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE  ? sizeof(u8) :
                0uz
                };
            if (IdxStride == 0uz)
            {
                LOG_FATAL(LogRhi, "[{}]: Could not infer index stride from component type [{}].",
                    Target.GetPath(), IdxAccessor.componentType
                    )
            }

            Target.Indices.reserve(Target.Indices.size() + IdxCount);
            for (auto Idx{0uz}; Idx < IdxCount; ++Idx)
            {
                if (IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    Target.Indices.emplace_back(baseVertex + *reinterpret_cast<u16 const*>(pIdxData + Idx * IdxStride));
                }
                else if (IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                {
                    Target.Indices.emplace_back(baseVertex + *reinterpret_cast<u32 const*>(pIdxData + Idx * IdxStride));
                }
                else if (IdxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                {
                    Target.Indices.emplace_back(baseVertex + *reinterpret_cast<u8 const*>(pIdxData + Idx * IdxStride));
                }
                else
                {
                    std::unreachable();
                }
            }
        }
    }

    return Jafg::LStaticMesh::EResult::Success;
}

} /* ~Namespace <Anonymous> */

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

    if (this->Path.native().ends_with(LITERAL_TEXT(".glb")))
    {
        if (auto Rc{::LoadViaTinyGltf(*this, ETinyObjHint::Binary)}; Rc != EResult::Success)
        {
            return Rc;
        }
    }
    else if (this->Path.native().ends_with(LITERAL_TEXT(".gltf")))
    {
        if (auto Rc{::LoadViaTinyGltf(*this, ETinyObjHint::Json)}; Rc != EResult::Success)
        {
            return Rc;
        }
    }
    else
    {
        LOG_FATAL(LogRhi, "Unsupported static mesh file format: [{}].", this->Path)
    }

    if (this->Vertices.empty() || this->Indices.empty())
    {
        LOG_ERROR(LogRhi, "[{}]: No vertices or indices were loaded.", this->GetPath())
        return EResult::LoadingError;
    }

    return EResult::Success;
}

void Jafg::LStaticMesh::LoadToDevice()
{
    check(this->IsOnHost())
    LOG_TRACE(LogRhi, "Loading static mesh [{}] to device.", this->Path)

    if (this->IsOnDevice())
    {
        LOG_TRACE(LogRhi, "Static mesh [{}] already loaded to device memory. Freeing previous device memory and reloading.", this->Path)
        this->FreeFromDevice();
    }
    check(this->IndexCount == 0 && this->VertexBuffer.GetAllocation() == nullptr && this->IndexBuffer.GetAllocation() == nullptr)

    this->VertexBuffer = Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Vertex({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(this->Vertices[0]) * this->Vertices.size()},
        .Data = this->Vertices.data(),
        }));

    this->IndexBuffer = Detail::GMutableEngine->GetLocalEgo().GetFrontend().Vk_StageBuffer(LStageBufferCreateInfo::Index({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(this->Indices[0]) * this->Indices.size()},
        .Data = this->Indices.data(),
        }));

    this->IndexCount = static_cast<u32>(this->Indices.size());

    return;
}

void Jafg::LStaticMesh::DrawIndexed(LRenderInfo const& Info) const
{
    check(this->IndexCount > 0)
    check(this->VertexBuffer.GetBuffer() && this->IndexBuffer.GetBuffer())

    Info.CommandBuffer.bindVertexBuffers(0, this->VertexBuffer.GetBuffer(), {0});
    Info.CommandBuffer.bindIndexBuffer(this->IndexBuffer.GetBuffer(), 0, vk::IndexTypeValue<decltype(this->Indices)::value_type>::value);

    Info.CommandBuffer.drawIndexed(static_cast<u32>(this->IndexCount), 1, 0, 0, 0);

    return;
}
