// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

template<typename T>
concept CDeviceLayout = std::is_standard_layout_v<T> && requires
{
    { T::Bindings().data() } -> std::same_as<vk::DescriptorSetLayoutBinding const*>;
    { T::Bindings().size() } -> std::same_as<std::size_t>;
};

} /* ~Namespace Jafg */
