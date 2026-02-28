// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"

namespace Jafg
{

template<typename T>
concept CUniformBufferObject = std::is_standard_layout_v<T> && requires
{
    // { T::Binding() } -> std::same_as<u32>;
    { T::Flags() } -> std::same_as<vk::ShaderStageFlags>;
};

namespace UBO
{

//# A generic uniform buffer object structure.
template<typename T, u32 Binding, vk::ShaderStageFlagBits... InFlags>
struct TUbo
{
    inline void Upload(LMappedDeviceBuffer const& Buffer) const noexcept
    {
        check(Buffer.GetData() != nullptr)
        std::memcpy(Buffer.GetData(), this, sizeof(T));
    }
    inline static constexpr vk::BufferCreateInfo CreateInfo()
    {
        return vk::BufferCreateInfo{
            .size = sizeof(T),
            .usage = vk::BufferUsageFlagBits::eUniformBuffer,
            .sharingMode = vk::SharingMode::eExclusive
            };
    }
    inline static constexpr vk::DescriptorBufferInfo WriteInfo(vk::Buffer Buffer, vk::DeviceSize Offset = 0) noexcept
    {
        return {
            .buffer = Buffer,
            .offset = Offset,
            .range = sizeof(T)
            };
    }
    static std::array<vk::DescriptorSetLayoutBinding, 1> const& Bindings() noexcept
    {
        static std::array Bindings{
            vk::DescriptorSetLayoutBinding{
                .binding = Binding,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1,
                .stageFlags = Flags(),
                .pImmutableSamplers = nullptr
                },
            };

        return Bindings;
    }
    inline static constexpr vk::ShaderStageFlags Flags() noexcept { return (InFlags | ...); }
};

} /* ~Namespace UBO */

} /* ~Namespace Jafg */
