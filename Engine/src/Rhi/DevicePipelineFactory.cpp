// Copyright mzoesch. All rights reserved.

#include "Rhi/GraphicsPipelineFactory.h"

rhi::graphics_pipeline rhi::graphics_pipeline_factory::build(vk::raii::Device const& device)
{
    checkCode /* Should not happen as slangc already checks this. */
    (
        std::unordered_set<u32> UniqueSpaces;
        auto Traverse{[&UniqueSpaces](auto&& List)
        {
            for (auto const& Layout: List)
            {
                if (UniqueSpaces.contains(Layout.binding))
                {
                    LOG_FATAL(LogMaterialSubsystem, "Multiple descriptor set layouts have the same space binding [{}].", Layout.binding)
                }
                UniqueSpaces.insert(Layout.binding);
            }
        }};
        Traverse(this->unique_descriptor_set_layouts);
        Traverse(this->shared_descriptor_set_layouts);
    )
    TArray<vk::DescriptorSetLayout> DescriptorSetLayouts; DescriptorSetLayouts.reserve(this->shared_descriptor_set_layouts.size() + this->unique_descriptor_set_layouts.size());
    for (auto Idx{0uz}; Idx < this->unique_descriptor_set_layouts.size() + this->shared_descriptor_set_layouts.size(); ++Idx)
    {
        if (auto It{algo::find(this->unique_descriptor_set_layouts, Idx, &descriptor_set_layout_pair<vk::raii::DescriptorSetLayout>::binding)};
            It != this->unique_descriptor_set_layouts.end())
        {
            DescriptorSetLayouts.emplace_back(*It->descriptor_set_layout);
            continue;
        }
        if (auto It{algo::find(this->shared_descriptor_set_layouts, Idx, &descriptor_set_layout_pair<vk::DescriptorSetLayout>::binding)};
            It != this->shared_descriptor_set_layouts.end())
        {
            DescriptorSetLayouts.emplace_back(It->descriptor_set_layout);
            continue;
        }
        LOG_FATAL(LogRhi, "Failed to find descriptor set layout binding [{}].", Idx)
    }
    vk::raii::PipelineLayout Layout{std::move(*device.createPipelineLayout({
        .setLayoutCount = static_cast<u32>(DescriptorSetLayouts.size()),
        .pSetLayouts = DescriptorSetLayouts.data(),
        .pushConstantRangeCount = static_cast<u32>(this->push_constant_ranges.size()),
        .pPushConstantRanges = this->push_constant_ranges.data(),
        }))};

    vk::PipelineDynamicStateCreateInfo PipelineDynamicState{
        .dynamicStateCount = static_cast<u32>(this->dynamic_states.size()),
        .pDynamicStates = this->dynamic_states.data(),
        };
    check(!this->color_attachment_formats.empty())
    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> StructureChain{
        {
            .stageCount          = static_cast<u32>(this->shaders.size()),
            .pStages             = this->shaders.data(),
            .pVertexInputState   = &this->pipeline_vertex_input_state,
            .pInputAssemblyState = &this->pipeline_input_assembly_state,
            .pViewportState      = &this->pipeline_viewport_state,
            .pRasterizationState = &this->pipeline_rasterization_state,
            .pMultisampleState   = &this->pipeline_multisample_state,
            .pDepthStencilState  = &this->pipeline_depth_stencil_state,
            .pColorBlendState    = &this->pipeline_color_blend_state,
            .pDynamicState       = &PipelineDynamicState,
            .layout              = Layout,
            .renderPass          = nullptr,
        },
        {
            .colorAttachmentCount    = static_cast<u32>(this->color_attachment_formats.size()),
            .pColorAttachmentFormats = this->color_attachment_formats.data(),
            .depthAttachmentFormat   = this->depth_attachment_format,
            .stencilAttachmentFormat = this->stencil_attachment_format,
        },};

    TArray<graphics_pipeline::descriptor_set_layout> Temp; Temp.reserve(this->unique_descriptor_set_layouts.size());
    for (auto& [Binding, DescriptorSetLayout]: this->unique_descriptor_set_layouts)
    {
        Temp.emplace_back(Binding, std::move(DescriptorSetLayout));
    }
    return {
        .pipeline = std::move(*device.createGraphicsPipeline(nullptr, StructureChain.get<vk::GraphicsPipelineCreateInfo>())),
        .pipeline_layout = std::move(Layout),
        .unique_descriptor_set_layouts = std::move(Temp),
        };
}
