// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/Rhi.h"

namespace Jafg
{

//# TODO: Make also an abstraction for instanced static meshes.
//# TODO: Abstract the size of indices (u16 vs u32). (Currently u32 only.)
class LStaticMesh
{
public:

    static constexpr auto DefaultShader{ "Content/Shaders/Spir-V/StaticMesh.spv" };

    struct LVertex
    {
        glm::vec3 Location;
        glm::vec3 Color;
        glm::vec2 TexCoord;

        static std::array<vk::VertexInputBindingDescription, 1> const& BindingDescriptions() noexcept
        {
            static std::array Desc{vk::VertexInputBindingDescription{
                .binding = 0,
                .stride = sizeof(LVertex),
                .inputRate = vk::VertexInputRate::eVertex
                }};

            return Desc;
        }

        static std::array<vk::VertexInputAttributeDescription, 3> const& AttributeDescriptions() noexcept
        {
            static std::array Desc{
                vk::VertexInputAttributeDescription{
                    .location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LVertex, Location)
                    },
                vk::VertexInputAttributeDescription{
                    .location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(LVertex, Color)
                    },
                vk::VertexInputAttributeDescription{
                    .location = 2, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(LVertex, TexCoord)
                    }
                };

            return Desc;
        }

        FORCEINLINE constexpr bool operator==(LVertex const& V) const
        {
            return this->Location == V.Location && this->Color == V.Color && this->TexCoord == V.TexCoord;
        }
    };
    static_assert(Jafg::CDeviceVertexInput<LVertex>);

    struct LPipelineLayout
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
    static_assert(Jafg::CDeviceLayout<LPipelineLayout>);

    struct LRootLocation final : public TVertexPushConstant<LRootLocation>
    {
        LMat4F Model;
    };
    static_assert(Jafg::CPushConstant<LRootLocation>);

    enum struct EResult
    {
        Success,
        FileNotFound,
        LoadingError,
    };

    enum struct ELoadBehavior
    {
        //# Default behavior. Do nothing.
        Deferred,
        //# Load the model data immediately to host memory.
        Load,
        //# Additionally to #Load also upload the model data to the device immediately.
        LoadToDevice,
    };

    enum struct EUploadHostMemoryBehavior
    {
        //# Free host memory.
        Free,
        //# Keep host memory allocated.
        Keep,
    };

    inline LStaticMesh() = default;
    inline LStaticMesh(LPath const& InPath,
        ELoadBehavior Behavior = ELoadBehavior::Deferred,
        EUploadHostMemoryBehavior HostMemoryBehavior = EUploadHostMemoryBehavior::Free
        ) : Path{InPath}
    {
        if (Behavior != ELoadBehavior::Deferred)
        {
            auto Result{this->ReloadModel(Behavior, HostMemoryBehavior)};
            jassert( Result == EResult::Success )
        }

        return;
    }

    //# Load or reload the model from disk and optionally upload it to the device.
    ENGINE_API EResult ReloadModel(
          ELoadBehavior Behavior = ELoadBehavior::LoadToDevice
        , EUploadHostMemoryBehavior HostMemoryBehavior = EUploadHostMemoryBehavior::Free);
    ENGINE_API void LoadToDevice(EUploadHostMemoryBehavior Behavior = EUploadHostMemoryBehavior::Free);

    ENGINE_API void DrawIndex(LRenderInfo const& Info) const;

    inline void FreeFromHost() noexcept
    {
        algo::orphan(&this->Vertices);
        algo::orphan(&this->Indices);

        return;
    }

    inline void FreeFromDevice() noexcept
    {
        this->IndexCount = 0;
        this->VertexBuffer.Free();
        this->IndexBuffer.Free();

        return;
    }

    LPath Path;

    TArray<LVertex> Vertices;
    TArray<u32> Indices;

    u32 IndexCount{};
    LDeviceBuffer VertexBuffer;
    LDeviceBuffer IndexBuffer;
};

} /* ~Namespace Jafg */

template<>
struct std::hash<Jafg::LStaticMesh::LVertex>
{
    FORCEINLINE size_t operator()(Jafg::LStaticMesh::LVertex const& Vertex) const noexcept
    {
        return ((hash<glm::vec3>()(Vertex.Location) ^ (hash<glm::vec3>()(Vertex.Color) << 1)) >> 1) ^ (hash<glm::vec2>()(Vertex.TexCoord) << 1);
    }
};

#include "Rhi/StaticMeshRef.h"
