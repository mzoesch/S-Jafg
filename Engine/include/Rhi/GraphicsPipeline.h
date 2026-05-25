// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

struct LGraphicsDevicePipeline
{
    void Free() noexcept
    {
        this->Pipeline.clear();
        this->Layout.clear();
        algo::orphan(&this->DescriptorSetLayouts);
        algo::orphan(&this->_UniqueDescriptorSetLayout);
    }

    inline decltype(auto) operator*() const & noexcept { return *this->Pipeline; }

    vk::raii::Pipeline Pipeline{ nullptr };
    vk::raii::PipelineLayout Layout{ nullptr };

    TArray<vk::DescriptorSetLayout> DescriptorSetLayouts;
    TArray<vk::raii::DescriptorSetLayout> _UniqueDescriptorSetLayout;
};

} /* ~Namespace Jafg */
