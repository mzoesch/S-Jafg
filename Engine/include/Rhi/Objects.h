// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"
#include "Rhi/GraphicsPipeline.h"

namespace rhi
{

template<typename T>
struct vertex_input_template
{
    NODISCARD static constexpr LStringView name() noexcept { return algo::type_name<T>(); }
    NODISCARD static constexpr LString name_str() noexcept { return LString{T::name()}; }

    NODISCARD static vk::PipelineVertexInputStateCreateInfo vertex_input_create_info() noexcept
    {
        return vk::PipelineVertexInputStateCreateInfo{
            .vertexBindingDescriptionCount = static_cast<u32>(T::binding_descriptions().size()),
            .pVertexBindingDescriptions = T::binding_descriptions().data(),
            .vertexAttributeDescriptionCount = static_cast<u32>(T::attribute_descriptions().size()),
            .pVertexAttributeDescriptions = T::attribute_descriptions().data(),
            };
    }
};

template<typename T>
concept vertex_input = requires
{
    { T::name() } -> std::same_as<LStringView>;
    { T::name_str() } -> std::same_as<LString>;

    { T::binding_descriptions().data() } -> std::same_as<vk::VertexInputBindingDescription const*>;
    { T::binding_descriptions().size() } -> std::same_as<std::size_t>;
    { T::attribute_descriptions().data() } -> std::same_as<vk::VertexInputAttributeDescription const*>;
    { T::attribute_descriptions().size() } -> std::same_as<std::size_t>;

    { T::vertex_input_create_info() } -> std::same_as<vk::PipelineVertexInputStateCreateInfo>;
};

// Template for a uniform buffer object. This template is meant to be overridden if you need custom behavior.
template<typename T, vk::ShaderStageFlagBits... Flags>
struct ubo_template
{
    NODISCARD static constexpr LStringView name() noexcept { return algo::type_name<T>(); }
    NODISCARD static constexpr LString name_str() noexcept { return LString{T::name()}; }
    NODISCARD static constexpr vk::DescriptorType descriptor_type() noexcept { return vk::DescriptorType::eUniformBuffer; }
    NODISCARD static constexpr vk::BufferUsageFlagBits buffer_usage() noexcept { return vk::BufferUsageFlagBits::eUniformBuffer; }

    static constexpr vk::ShaderStageFlags shader_stage_flags() noexcept { return (Flags|...); }
    static std::array<vk::DescriptorBindingFlags, 1> const& descriptor_binding_flags() noexcept
    {
        static std::array Dummy{vk::DescriptorBindingFlags{},};
        return Dummy;
    }
    static vk::DescriptorSetLayoutBindingFlagsCreateInfo const& descriptor_set_layout_binding_flags()
    {
        static vk::DescriptorSetLayoutBindingFlagsCreateInfo Info{
            .bindingCount = static_cast<u32>(T::descriptor_binding_flags().size()),
            .pBindingFlags = T::descriptor_binding_flags().data(),
            };
        return Info;
    }
    static std::array<vk::DescriptorSetLayoutBinding, 1> const& bindings() noexcept
    {
        static std::array Bindings{vk::DescriptorSetLayoutBinding{
            .binding = 0,
            .descriptorType = T::descriptor_type(),
            .descriptorCount = 1,
            .stageFlags = T::shader_stage_flags(),
            .pImmutableSamplers = nullptr
            },};
        return Bindings;
    }
    static vk::DescriptorSetLayoutCreateFlags descriptor_set_layout_flags() noexcept { return {}; }

    static constexpr vk::BufferCreateInfo buffer_create_info() noexcept
    {
        return vk::BufferCreateInfo{
            .size = sizeof(T),
            .usage = T::buffer_usage(),
            .sharingMode = vk::SharingMode::eExclusive
            };
    }

    NODISCARD static vk::raii::DescriptorSetLayout build(vk::raii::Device const& device, auto&&... Args) noexcept
    {
        auto Bindings{T::bindings(std::forward<decltype(Args)>(Args)...)};
        return vk_build(device, vk::DescriptorSetLayoutCreateInfo{
            .pNext = &T::descriptor_set_layout_binding_flags(),
            .flags = T::descriptor_set_layout_flags(),
            .bindingCount = static_cast<u32>(Bindings.size()),
            .pBindings = Bindings.data(),
            });
    }

    static constexpr vk::DescriptorBufferInfo write_info(vk::Buffer Buffer, vk::DeviceSize Offset = 0) noexcept
    {
        return {
            .buffer = Buffer,
            .offset = Offset,
            .range = sizeof(T)
            };
    }
    void upload(mapped_device_buffer const& Buffer) const noexcept
    {
        check(Buffer.data())
        std::memcpy(Buffer.data(), this, sizeof(T));
    }
};

template<typename T>
concept ubo = requires
{
    { T::name() } -> std::same_as<LStringView>;
    { T::name_str() } -> std::same_as<LString>;
    { T::descriptor_type() } -> std::same_as<vk::DescriptorType>;
    { T::buffer_usage() } -> std::same_as<vk::BufferUsageFlagBits>;

    { T::shader_stage_flags() } -> std::same_as<vk::ShaderStageFlags>;
    { T::descriptor_binding_flags() }; requires std::same_as<
        typename std::remove_cvref_t<decltype(T::descriptor_binding_flags())>::value_type,
        vk::DescriptorBindingFlags
        >;
    { T::descriptor_set_layout_binding_flags() } -> std::same_as<vk::DescriptorSetLayoutBindingFlagsCreateInfo const&>;
    requires requires { &T::bindings; }; /* Must return range of vk::DescriptorSetLayoutBinding. */
    { T::descriptor_set_layout_flags() } -> std::same_as<vk::DescriptorSetLayoutCreateFlags>;
    { T::buffer_create_info() } -> std::same_as<vk::BufferCreateInfo>;
    requires requires(T&& Obj, vk::raii::Device const& Device)
    {
        { T::build(Device) } -> std::same_as<vk::raii::DescriptorSetLayout>;
    };
    requires requires(T&& Obj, mapped_device_buffer const& Buffer)
    {
        { Obj.upload(Buffer) } -> std::same_as<void>;
    };
};

//# Template for a push constant. This template is meant to be overridden if you need custom behavior.
template<typename T, vk::ShaderStageFlagBits... Flags>
struct pc_template
{
    NODISCARD static constexpr LStringView name() noexcept { return algo::type_name<T>(); }
    NODISCARD static constexpr LString name_str() noexcept { return LString{T::name()}; }

    static constexpr vk::ShaderStageFlags shader_stage_flags() noexcept { return (Flags|...); }

    void upload(vk::CommandBuffer Buffer, graphics_pipeline const& Pipeline, u32 Offset = 0) const noexcept // TODO: Can we automate Offset?
    {
        Buffer.pushConstants2({
            .layout = *Pipeline.pipeline_layout,
            .stageFlags = T::shader_stage_flags(),
            .offset = Offset,
            .size = sizeof(T),
            .pValues = this
            });
    }
};

template<typename T>
concept pc = requires
{
    { T::name() } -> std::same_as<LStringView>;
    { T::name_str() } -> std::same_as<LString>;

    { T::shader_stage_flags() } -> std::same_as<vk::ShaderStageFlags>;

    requires requires(T&& Obj, vk::CommandBuffer Buffer, graphics_pipeline const& Pipeline)
    {
        { Obj.upload(Buffer, Pipeline) } -> std::same_as<void>;
    };
};

} /* ~Namespace rhi */

namespace Jafg
{

struct LMaterial;
struct LActorRenderInfo;

struct LVertexInputProvider
{
    LString Identifier;
    mutable TFunction2<vk::PipelineVertexInputStateCreateInfo()> Provider;
};

struct LPushConstantProvider final
{
    struct LActorInfo final
    {
        LTransformF Transform;
    };

    LString Identifier;
    mutable TFunction2<void(LActorRenderInfo const& Info, LMaterial const& Material, LActorInfo const& ActorInfo)> PushForActor;
};

namespace Detail
{

ENGINE_API void RegisterVertexInputGlobally(LVertexInputProvider Provider) noexcept;
ENGINE_API void RegisterPushConstantGlobally(LPushConstantProvider Provider) noexcept;

} /* ~Namespace Detail */

//# Helper struct to automatically register a vertex input. Usually only used for static storage initialization.
template<rhi::vertex_input T>
struct LVertexInputRegistrator
{
    LVertexInputRegistrator()
    {
        Detail::RegisterVertexInputGlobally({
            .Identifier = T::name_str(),
            .Provider = &T::vertex_input_create_info,
            });
    }
};

//# Helper struct to automatically register a pc. Usually only used for static storage initialization.
template<rhi::pc T>
struct LPushConstantRegistrator final
{
    LPushConstantRegistrator()
    {
        if constexpr (requires { T::PushForActor; })
        {
            Detail::RegisterPushConstantGlobally({
                .Identifier = T::name_str(),
                .PushForActor = &T::PushForActor,
                });
        }
        else
        {
            Detail::RegisterPushConstantGlobally({
                .Identifier = T::name_str(),
                });
        }
    }
};

} /* ~Namespace Jafg */
