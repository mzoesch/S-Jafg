// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/ResourceReference.h"
#include "Rhi/GraphicsPipeline.h"
#include "Rhi/ReflectedShader.h"

namespace rhi
{

struct material_template final
{
    reflected_shader const& my_shader;
    LString identifier;

    vk::PipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state;
    vk::PipelineRasterizationStateCreateInfo pipeline_rasterization_state;
    // vk::PipelineMultisampleStateCreateInfo pipeline_multisample_state;
    vk::PipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state;
    struct pipeline_color_blend_state_wrapper final
    {
        vk::Bool32 logic_op_enable;
        vk::LogicOp logic_op;
        TArray<vk::PipelineColorBlendAttachmentState> pipeline_color_blend_attachment_states;
    };
    pipeline_color_blend_state_wrapper pipeline_color_blend_state;

    std::inplace_vector<vk::Format, 2uz> color_attachment_formats;
    vk::Format depth_attachment_format;
    vk::Format stencil_attachment_format;

    struct property final
    {
        LString key;
        LString value;
    };
    TArray<property> properties;
};

template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0>
void from_json(BasicJsonType const& j, material_template::pipeline_color_blend_state_wrapper& t)
{
    j.at("logicOpEnable").get_to(t.logic_op_enable);
    j.at("logicOp").get_to(t.logic_op);
    j.at("pipelineColorBlendAttachmentStates").get_to(t.pipeline_color_blend_attachment_states);
}

template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0>
void from_json(BasicJsonType const& j, material_template::property& t)
{
    j.at("key").get_to(t.key);
    if (auto& v{j.at("value")}; v.is_string())
    {
        t.value = v.template get<LString>();
    }
    else if (v.is_number_float())
    {
        t.value = algo::sprintf("{}", v.template get<f64>());
    }
    else if (v.is_number())
    {
        t.value = algo::sprintf("{}", v.template get<i64>());
    }
    else if (v.is_boolean())
    {
        if (auto b{v.template get<bool>()}; b)
        {
            t.value = "true";
        }
        else
        {
            t.value = "false";
        }
    }
    else
    {
        LOG_FATAL(LogSerialization, "Unsupported property value type [{}].", v.type_name())
    }
}

} /* ~Namespace rhi */

namespace Jafg
{

struct LTexture2;
struct LMaterial;
typedef rhi::shared_ref<LMaterial> LMaterialRef;
struct LMaterialInstance;
typedef rhi::shared_ref<LMaterialInstance> LMaterialInstanceRef;

struct LMaterial final
{
    rhi::material_template const& Template;
    rhi::graphics_pipeline Pipeline;
};

struct LMaterialInstance final
{
    LMaterialRef Material;

    TArray<std::pair<u32, vk::raii::DescriptorSet>> InfrequentDescriptorSets;
    rhi::frame_array<TArray<std::pair<u32, vk::raii::DescriptorSet>>> FrequentDescriptorSets;

    //# @return The descriptor set for the given space or nullptr if not found.
    NODISCARD vk::raii::DescriptorSet const* Vk_FindUniqueDescriptorSet(u32 Space, std::optional<u32> Frame = {}) const
    {
        if (auto It{algo::find(this->InfrequentDescriptorSets, Space, algo::pair_first)}; It != this->InfrequentDescriptorSets.end())
        {
            return &It->second;
        }
        if (Frame)
        {
            check(algo::valid_index(this->FrequentDescriptorSets, *Frame))
            auto const& FrequentSets{this->FrequentDescriptorSets[*Frame]};
            if (auto It{algo::find(FrequentSets, Space, algo::pair_first)}; It != FrequentSets.end())
            {
                return &It->second;
            }
        }
        return nullptr;
    }

    //# Checked method of #Vk_FindUniqueDescriptorSet.
    NODISCARD vk::raii::DescriptorSet const& Vk_GetUniqueDescriptorSet(u32 Space, std::optional<u32> Frame = {}) const
    {
        if (auto* Set{this->Vk_FindUniqueDescriptorSet(Space, Frame)}; Set)
        {
            return *Set;
        }
        if (!Frame)
        {
            check(!this->FrequentDescriptorSets.empty())
            auto const& FrequentSets{this->FrequentDescriptorSets[0uz]};
            if (auto It{algo::find(FrequentSets, Space, algo::pair_first)}; It != FrequentSets.end())
            {
                LOG_FATAL(LogMaterialSubsystem
                    , "[{}]: Space [{}] is specified as a frequent descriptor set but frame was not specified."
                    , this->Material->Template.my_shader.Identifier, Space)
            }
        }
        LOG_FATAL(LogMaterialSubsystem
            , "[{}]: No descriptor set for space [{}] found."
            , this->Material->Template.my_shader.Identifier, Space)
    }

    NODISCARD ENGINE_API rhi::vk_binding GetBinding(LStringView Key) const noexcept;
    ENGINE_API void Vk_SetField(LFrontend& Frontend, rhi::vk_binding Where, LStringView Value);
    ENGINE_API void Vk_SetSampler(LFrontend const& Frontend, rhi::vk_binding Where, vk::Sampler const& Sampler);
    ENGINE_API void Vk_SetSampledImage(LFrontend const& Frontend, rhi::vk_binding Where, LTexture2 const& Texture);
};

} /* ~Namespace rhi */
