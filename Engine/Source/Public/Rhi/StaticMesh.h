// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/Rhi.h"
#include "Rhi/ResourceReference.h"

namespace Jafg
{

//# The state that is required after the mesh load function was called.
enum struct EStaticMeshStateBits
{
    //#
    //# No state is required. Just allocate the resource. The client will handle the rest.
    //# If another client already requested host or device memory then the resource will be in the same
    //# state as requested previously by the other client.
    //#
    None = 0 << 0,
    //#
    //# The mesh data will be loaded to host memory. If another client already requested device memory
    //# then the resource will be loaded to host and device memory.
    //#
    Host = 1 << 0,
    //#
    //# The mesh data will be loaded to device memory. Host memory will be orphaned if host memory was
    //# not requested by another client.
    //#
    Device = 1 << 1,
};
ENUM_STRUCT_FLAGS(EStaticMeshStateBits, EStaticMeshState)

//# TODO: Make also an abstraction for instanced static meshes.
//# TODO: Abstract the size of indices (u16 vs u32). (Currently u32 only.)
class LStaticMesh final
{
public:

    inline static constexpr auto DefaultShader{ "Content/Shaders/Spir-V/StaticMesh.spv" };

    struct Vertex final
    {
        LVec3F Location;
        LVec3F Color;
        LVec2F TexCoord;

        static std::array<vk::VertexInputBindingDescription, 1> const& BindingDescriptions() noexcept
        {
            static std::array Desc{vk::VertexInputBindingDescription{
                .binding = 0,
                .stride = sizeof(LStaticMesh::Vertex),
                .inputRate = vk::VertexInputRate::eVertex
                }};
            return Desc;
        }

        static std::array<vk::VertexInputAttributeDescription, 3> const& AttributeDescriptions() noexcept
        {
            static std::array Desc{
                vk::VertexInputAttributeDescription{
                    .location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LStaticMesh::Vertex, Location)
                    },
                vk::VertexInputAttributeDescription{
                    .location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LStaticMesh::Vertex, Color)
                    },
                vk::VertexInputAttributeDescription{
                    .location = 2, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(LStaticMesh::Vertex, TexCoord)
                    }
                };
            return Desc;
        }

        FORCEINLINE constexpr bool operator==(LStaticMesh::Vertex const& V) const noexcept
        {
            return this->Location == V.Location && this->Color == V.Color && this->TexCoord == V.TexCoord;
        }
    };
    static_assert(Jafg::CDeviceVertexInput<LStaticMesh::Vertex>);

    struct PipelineLayout final
    {
        static std::array<vk::DescriptorSetLayoutBinding, 2> const& Bindings() noexcept
        {
            static std::array Bindings{
                vk::DescriptorSetLayoutBinding{
                    .binding = 0,
                    .descriptorType = vk::DescriptorType::eUniformBuffer,
                    .descriptorCount = 1,
                    .stageFlags = vk::ShaderStageFlagBits::eVertex,
                    .pImmutableSamplers = nullptr
                    },
                vk::DescriptorSetLayoutBinding{
                    .binding = 1,
                    .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                    .descriptorCount = 1,
                    .stageFlags = vk::ShaderStageFlagBits::eFragment,
                    .pImmutableSamplers = nullptr
                    },
                };

            return Bindings;
        }
    };
    static_assert(Jafg::CDeviceLayout<PipelineLayout>);

    struct VPC final : public TVertexPushConstant<LStaticMesh::VPC>
    {
        LMat4F Model;
    };
    static_assert(Jafg::CPushConstant<LStaticMesh::VPC>);

    enum struct EResult
    {
        Success,
        FileNotFound,
        LoadingError,
    };

    inline LStaticMesh() = default;
    inline LStaticMesh(LPath InPath, EStaticMeshState State = EStaticMeshStateBits::None) : Path{std::move(InPath)}
    {
        if (State & EStaticMeshStateBits::Host || State & EStaticMeshStateBits::Device)
        {
            auto Result{this->LoadToHost()};
            jassert(Result == EResult::Success)
        }

        if (State & EStaticMeshStateBits::Device)
        {
            this->LoadToDevice();
            if ((State & EStaticMeshStateBits::Host) == EStaticMeshStateBits::None)
            {
                this->FreeFromHost();
            }
        }

        return;
    }

    FORCEINLINE constexpr bool IsOnHost() const noexcept { return this->Vertices.size() > 0; }
    ENGINE_API EResult LoadToHost();
    inline void FreeFromHost() noexcept
    {
        algo::orphan(&this->Vertices);
        algo::orphan(&this->Indices);
    }

    FORCEINLINE constexpr bool IsOnDevice() const noexcept { return this->IndexCount > 0; }
    ENGINE_API void LoadToDevice();
    inline void FreeFromDevice() noexcept
    {
        this->IndexCount = 0;
        this->VertexBuffer.Free();
        this->IndexBuffer.Free();
    }

    ENGINE_API void DrawIndex(LRenderInfo const& Info) const;

    //# Modify with care.
    TArray<LStaticMesh::Vertex> Vertices;
    TArray<u32> Indices;

    //# Modify with care.
    u32 IndexCount{};
    LDeviceBuffer VertexBuffer;
    LDeviceBuffer IndexBuffer;

private:

    LPath Path;
};

typedef TResourceReference<LStaticMesh> LStaticMeshRef_v2;

} /* ~Namespace Jafg */

template<>
struct std::hash<Jafg::LStaticMesh::Vertex>
{
    FORCEINLINE size_t operator()(Jafg::LStaticMesh::Vertex const& Vertex) const noexcept
    {
        return ((hash<glm::vec3>()(Vertex.Location) ^ (hash<glm::vec3>()(Vertex.Color) << 1)) >> 1) ^ (hash<glm::vec2>()(Vertex.TexCoord) << 1);
    }
};
