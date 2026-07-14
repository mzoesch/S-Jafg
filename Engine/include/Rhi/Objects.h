// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"
#include "Rhi/GraphicsPipeline.h"

namespace rhi
{

namespace detail
{

template<typename T>
concept device_element = requires
{
    { T::name() } -> std::same_as<LStringView>;
    { T::name_str() } -> std::same_as<LString>;
};
template<typename T>
struct device_element_template
{
    NODISCARD static constexpr LStringView name() noexcept { return algo::type_name<T>(); }
    NODISCARD static constexpr LString name_str() noexcept { return LString{T::name()}; }
};

//# Traits for a device object.
template<typename T>
concept object = device_element<T> && requires
{
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
//# Convenience template for a device object. This template is meant to be overridden if you need custom behavior.
template<typename T, vk::ShaderStageFlagBits... Flags>
struct object_template: device_element_template<T>
{
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

    static constexpr vk::BufferCreateInfo buffer_create_info(std::size_t count = 1) noexcept
    {
        return vk::BufferCreateInfo{
            .size = sizeof(T) * count,
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

    void upload(mapped_device_buffer const& buf) const noexcept
    {
        check(buf.data())
        std::memcpy(buf.data(), this, sizeof(T));
    }
    void upload_and_update(vk::raii::Device const& d, vk::raii::DescriptorSet const& s, u32 binding, mapped_device_buffer const& buf) const noexcept
    {
        this->upload(buf);
        auto info{T::write_info(*buf)};
        std::array Writes{vk::WriteDescriptorSet{
            .dstSet = s,
            .dstBinding = binding, .dstArrayElement = 0, .descriptorCount = 1,
            .descriptorType = T::descriptor_type(),
            .pBufferInfo = &info,
            }};
        d.updateDescriptorSets(Writes, {});
    }
};

} /* ~Namespace detail */

//# Traits a vertex input has to fulfill.
template<typename T>
concept vertex_input = detail::device_element<T> && requires
{
    { T::binding_descriptions().data() } -> std::same_as<vk::VertexInputBindingDescription const*>;
    { T::binding_descriptions().size() } -> std::same_as<std::size_t>;
    { T::attribute_descriptions().data() } -> std::same_as<vk::VertexInputAttributeDescription const*>;
    { T::attribute_descriptions().size() } -> std::same_as<std::size_t>;

    { T::vertex_input_create_info() } -> std::same_as<vk::PipelineVertexInputStateCreateInfo>;
};
//# Convenience template.
template<typename T>
struct vertex_input_template: detail::device_element_template<T>
{
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

//# Template for a uniform buffer object. This template is meant to be overridden if you need custom behavior.
template<typename T>
concept ubo = detail::object<T>;
template<typename T, vk::ShaderStageFlagBits... Flags>
struct ubo_template: detail::object_template<T, Flags...>
{
    NODISCARD static constexpr vk::DescriptorType descriptor_type() noexcept { return vk::DescriptorType::eUniformBuffer; }
    NODISCARD static constexpr vk::BufferUsageFlagBits buffer_usage() noexcept { return vk::BufferUsageFlagBits::eUniformBuffer; }
};

template<typename T>
concept ssbo = detail::object<T>;
//# Template for a storage buffer object. This template is meant to be overridden if you need custom behavior.
template<typename T, vk::ShaderStageFlagBits... Flags>
struct ssbo_template: detail::object_template<T, Flags...>
{
    NODISCARD static constexpr vk::DescriptorType descriptor_type() noexcept { return vk::DescriptorType::eStorageBuffer; }
    NODISCARD static constexpr vk::BufferUsageFlagBits buffer_usage() noexcept { return vk::BufferUsageFlagBits::eStorageBuffer; }
};

template<typename T>
concept pc = detail::device_element<T> && requires
{
    { T::shader_stage_flags() } -> std::same_as<vk::ShaderStageFlags>;
    requires requires(T&& Obj, vk::CommandBuffer Buffer, graphics_pipeline const& Pipeline)
    {
        { Obj.upload(Buffer, Pipeline) } -> std::same_as<void>;
    };
};
//# Template for a push constant. This template is meant to be overridden if you need custom behavior.
template<typename T, vk::ShaderStageFlagBits... Flags>
struct pc_template: detail::device_element_template<T>
{
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

//#
//# Convenience template to upload elements to a subrange inside a device buffer.
//# Treat it like a vector where at the end you submit with #upload_and_update.
//#
template<typename T>
struct object_range
{
    typedef T value_type;

    explicit object_range(std::size_t reverse, std::optional<std::size_t> max = {}): max{max}
    {
        this->values.reserve(reverse);
    }
    std::optional<std::size_t> max;

    TArray<value_type> values;
    NODISCARD TArray<value_type>& operator*() { return this->values; }
    NODISCARD TArray<value_type>* operator->() { return &this->values; }

    NODISCARD vk::DescriptorBufferInfo upload(rhi::mapped_device_buffer const& buf, std::size_t offset = 0) const noexcept
    {
        check(buf.data())
        if (this->values.empty())
        {
            LOG_FATAL(LogRhi, "Cannot upload empty object_range.")
        }
        if (this->max && this->values.size() > *this->max)
        {
            LOG_FATAL(LogRhi, "[{}]: Failed to upload due to overflow [{}>{}]."
                , algo::type_name<value_type>(), this->values.size(), *this->max)
        }

        vk::DeviceSize byte_offset{static_cast<vk::DeviceSize>(offset) * sizeof(value_type)};
        vk::DeviceSize byte_size{static_cast<vk::DeviceSize>(this->values.size()) * sizeof(value_type)};
        std::memcpy(static_cast<std::byte*>(buf.data()) + byte_offset, this->values.data(), byte_size);
        return vk::DescriptorBufferInfo{
            .buffer = *buf,
            .offset = byte_offset,
            .range = byte_size,
            };
    }

    void upload_and_update(vk::raii::Device const& d, vk::raii::DescriptorSet const& s, u32 binding, rhi::mapped_device_buffer const& buf, std::size_t offset = 0) const noexcept
    {
        vk::DescriptorBufferInfo info{this->upload(buf, offset)};
        std::array Writes{vk::WriteDescriptorSet{
            .dstSet = s,
            .dstBinding = binding, .dstArrayElement = 0, .descriptorCount = 1,
            .descriptorType = value_type::descriptor_type(),
            .pBufferInfo = &info,
            }};
        d.updateDescriptorSets(Writes, {});
    }
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
