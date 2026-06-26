// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace rhi
{

struct graphics_pipeline final
{
    NODISCARD constexpr decltype(auto) operator*() const& noexcept { return *this->pipeline; }

    vk::raii::Pipeline pipeline{ nullptr };
    vk::raii::PipelineLayout pipeline_layout{ nullptr };

    struct descriptor_set_layout
    {
        u32 space;
        vk::raii::DescriptorSetLayout layout;
    };
    TArray<descriptor_set_layout> unique_descriptor_set_layouts;
    NODISCARD vk::raii::DescriptorSetLayout const& get_unique_layout(u32 Space) const noexcept
    {
        for (auto const& Layout: this->unique_descriptor_set_layouts)
        {
            if (Layout.space == Space)
            {
                return Layout.layout;
            }
        }
        LOG_FATAL(LogRhi, "No such descriptor set layout for space [{}].", Space)
    }
};

} /* ~Namespace rhi */
