// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/GraphicsPipeline.h"

namespace rhi
{

struct graphics_pipeline_factory final
{
    constexpr graphics_pipeline_factory() noexcept = default;
    /* The factory might reference itself. */
    PROHIBIT_REALLOC_OF_ANY_FORM(graphics_pipeline_factory)

    template<typename T>
    struct descriptor_set_layout_pair
    {
        u32 binding;
        T descriptor_set_layout;
    };

    ENGINE_API graphics_pipeline build(vk::raii::Device const& device);

    ///////////////////////////////////////////////////////////////////////////////
    // Graphics pipeline
    ///////////////////////////////////////////////////////////////////////////////
    // Required is at least vert+frag.
    std::inplace_vector<vk::raii::ShaderModule, 2> shader_modules;
    std::inplace_vector<vk::PipelineShaderStageCreateInfo, 2> shaders;
    // Completely valid. Not all vertex shaders take vertices.
    vk::PipelineVertexInputStateCreateInfo pipeline_vertex_input_state{
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
        };
    // BEGIN Required.
    vk::PipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state;
    vk::PipelineViewportStateCreateInfo pipeline_viewport_state{.viewportCount=1,.scissorCount=1};
    vk::PipelineRasterizationStateCreateInfo pipeline_rasterization_state;
    vk::PipelineMultisampleStateCreateInfo pipeline_multisample_state;
    vk::PipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state;
    TArray<vk::PipelineColorBlendAttachmentState> pipeline_color_blend_attachment_states;
    vk::PipelineColorBlendStateCreateInfo pipeline_color_blend_state;
    // ~END Required.
    // Hardcoded into the pipeline for now. In the future we might want to make this dynamic.
    std::array<vk::DynamicState, 3> const dynamic_states{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
        vk::DynamicState::ePolygonModeEXT,
        };
    TArray<descriptor_set_layout_pair<vk::raii::DescriptorSetLayout>> unique_descriptor_set_layouts;
    TArray<descriptor_set_layout_pair<vk::DescriptorSetLayout>> shared_descriptor_set_layouts;
    TArray<vk::PushConstantRange> push_constant_ranges;

    ///////////////////////////////////////////////////////////////////////////////
    // Rendering pipeline
    ///////////////////////////////////////////////////////////////////////////////
    /* TODO: We currently support up to two color outputs. Do we need more? */
    std::inplace_vector<vk::Format, 2uz> color_attachment_formats;
    vk::Format depth_attachment_format{ vk::Format::eUndefined };
    vk::Format stencil_attachment_format{ vk::Format::eUndefined };
};

} /* ~Namespace rhi */
