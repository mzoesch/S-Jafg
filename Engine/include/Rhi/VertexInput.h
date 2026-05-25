// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

/* TODO: We can probably solve this with reflection in C++26? Bombastic sideeye */
template<typename T>
concept CDeviceVertexInput = std::is_standard_layout_v<T> && requires
{
    { T::BindingDescriptions().data() } -> std::same_as<vk::VertexInputBindingDescription const*>;
    { T::BindingDescriptions().size() } -> std::same_as<std::size_t>;
    { T::AttributeDescriptions().data() } -> std::same_as<vk::VertexInputAttributeDescription const*>;
    { T::AttributeDescriptions().size() } -> std::same_as<std::size_t>;
};

typedef vk::PipelineVertexInputStateCreateInfo(*LDeviceVertexInputStateCreateProviderSig)();

template<typename T> requires CDeviceVertexInput<T>
struct TDeviceVertexInputStateCreateProviderFn
{
    static vk::PipelineVertexInputStateCreateInfo operator()() noexcept
    {
        return vk::PipelineVertexInputStateCreateInfo{
            .vertexBindingDescriptionCount = static_cast<u32>(T::BindingDescriptions().size()),
            .pVertexBindingDescriptions = T::BindingDescriptions().data(),
            .vertexAttributeDescriptionCount = static_cast<u32>(T::AttributeDescriptions().size()),
            .pVertexAttributeDescriptions = T::AttributeDescriptions().data(),
            };
    }
};
template<typename T> requires CDeviceVertexInput<T>
inline constexpr TDeviceVertexInputStateCreateProviderFn<T> DeviceVertexInputStateCreateProvider{};

namespace Detail
{

extern void AddVertexProviderImpl(LString Name, LDeviceVertexInputStateCreateProviderSig Sig) noexcept;
template<typename TVertexInput> requires CDeviceVertexInput<TVertexInput>
inline void AddVertexProvider() noexcept
{
    AddVertexProviderImpl(LString{GetTypeName<TVertexInput>()}, &DeviceVertexInputStateCreateProvider<TVertexInput>.operator());
}

} /* ~Namespace Detail */

template<typename T> requires CDeviceVertexInput<T>
struct LRegisterDeviceVertexInput final
{
    inline LRegisterDeviceVertexInput() noexcept
    {
        Detail::AddVertexProvider<T>();
    }
};

} /* ~Namespace Jafg */
