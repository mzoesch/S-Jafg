// Copyright mzoesch. All rights reserved.

#include "Rhi/GraphicsPipelineFactory.h"

Jafg::LGraphicsDevicePipeline Jafg::LDevicePipelineFactory::Build()
{
    vk::PipelineVertexInputStateCreateInfo DummyPipelineVertexInputStateCreateInfo{
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
        };

    vk::PipelineMultisampleStateCreateInfo MultisamplingInfo{
        // TODO: Max user defined limit. Preferred material limit?
        .rasterizationSamples = this->MultisamplingSampleCount ? *this->MultisamplingSampleCount : this->Frontend.Vk_GetMaxMsaaSampleCount(),
        .sampleShadingEnable = this->MultisamplingShadingEnable
        };

    vk::PipelineDynamicStateCreateInfo DynamicStateInfo{
        .dynamicStateCount = static_cast<u32>(this->DynamicStateInfo.size()),
        .pDynamicStates = this->DynamicStateInfo.data(),
        };

    TArray<vk::DescriptorSetLayout> DescriptorSetLayouts;
    for (auto Idx{0uz}; Idx < this->SharedDescriptorSetLayouts.size() + this->UniqueDescriptorSetLayouts.size(); ++Idx)
    {
        if (auto It{algo::find(this->SharedDescriptorSetLayouts, Idx, &TDescriptorSetLayout<vk::DescriptorSetLayout>::Binding)};
            It != this->SharedDescriptorSetLayouts.end())
        {
            DescriptorSetLayouts.emplace_back(It->DescriptorSetLayout);
            continue;
        }

        if (auto It{algo::find(this->UniqueDescriptorSetLayouts, Idx, &TDescriptorSetLayout<vk::raii::DescriptorSetLayout>::Binding)};
            It != this->UniqueDescriptorSetLayouts.end())
        {
            DescriptorSetLayouts.emplace_back(*It->DescriptorSetLayout);
            continue;
        }

        LOG_FATAL(LogRhi, "Failed to find descriptor set layout binding [{}].", Idx)
    }

    vk::raii::PipelineLayout Layout{
        Frontend.Vk_GetDevice(),
        vk::PipelineLayoutCreateInfo{
            .setLayoutCount = static_cast<u32>(DescriptorSetLayouts.size()),
            .pSetLayouts = DescriptorSetLayouts.data(),
            .pushConstantRangeCount = static_cast<u32>(this->PushConstantRange.size()),
            .pPushConstantRanges = this->PushConstantRange.data(),
            }
       };

    check(this->ColorAttachmentFormat != vk::Format::eUndefined)
    check(this->DepthAttachmentFormat != vk::Format::eUndefined)

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> Chain{
        {
            .stageCount = static_cast<u32>(this->Shaders.size()),
            .pStages = this->Shaders.data(),
            .pVertexInputState = this->VertexInputInfo.has_value() ? &*this->VertexInputInfo : &DummyPipelineVertexInputStateCreateInfo,
            .pInputAssemblyState = &this->InputAssemblyInfo,
            .pViewportState = &this->ViewportStateInfo,
            .pRasterizationState = &this->RasterizationInfo,
            .pMultisampleState   = &MultisamplingInfo,
            .pDepthStencilState  = &this->DepthStencilInfo,
            .pColorBlendState    = this->PipelineColorBlendStateCreateInfo,
            .pDynamicState       = &DynamicStateInfo,
            .layout = Layout,
            .renderPass = nullptr,
        },
        {
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &this->ColorAttachmentFormat,
            .depthAttachmentFormat = this->DepthAttachmentFormat,
        }
    };

    TArray<vk::raii::DescriptorSetLayout> Temp; Temp.reserve(this->UniqueDescriptorSetLayouts.size());
    for (auto& DescriptorSetLayout : this->UniqueDescriptorSetLayouts)
    {
        Temp.emplace_back(std::move(DescriptorSetLayout.DescriptorSetLayout));
    }
    algo::orphan(&this->UniqueDescriptorSetLayouts);

    return LGraphicsDevicePipeline{
        .Pipeline = vk::raii::Pipeline{
            Frontend.Vk_GetDevice(),
            nullptr,
            Chain.get<vk::GraphicsPipelineCreateInfo>()
            },
        .Layout = std::move(Layout),
        .DescriptorSetLayouts = std::move(DescriptorSetLayouts),
        ._UniqueDescriptorSetLayout = std::move(Temp),
        };
}
