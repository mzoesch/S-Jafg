// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/ResourceReference.h"
#include "Rhi/GraphicsPipeline.h"
#include "Rhi/ReflectedShader.h"
#include "Rhi/Texture2.h"
#include "Rhi/TextureCube.h"
#include "Rhi/Bindless.h"

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
struct LTextureCube2;
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

    struct DescriptorSetInstance final
    {
        struct Resource final
        {
            typedef std::variant<std::monostate, rhi::mapped_device_buffer, UBO::Bindless::Sampler, LTexture2Ref, LTextureCube2>
                value_type;

            NODISCARD constexpr bool IsNone() const noexcept { return std::holds_alternative<std::monostate>(this->Value); }
            NODISCARD constexpr bool IsBuffer() const noexcept { return std::holds_alternative<rhi::mapped_device_buffer>(this->Value); }
            NODISCARD constexpr bool IsSampler() const noexcept { return std::holds_alternative<UBO::Bindless::Sampler>(this->Value); }
            NODISCARD constexpr bool IsSampledImage() const noexcept { return std::holds_alternative<LTexture2Ref>(this->Value); }
            NODISCARD constexpr bool IsSampledTextureCube() const noexcept { return std::holds_alternative<LTextureCube2>(this->Value); }

            NODISCARD constexpr rhi::mapped_device_buffer& AsBuffer() noexcept { return std::get<rhi::mapped_device_buffer>(this->Value); }
            NODISCARD constexpr rhi::mapped_device_buffer const& AsBuffer() const noexcept { return std::get<rhi::mapped_device_buffer>(this->Value); }
            NODISCARD constexpr UBO::Bindless::Sampler& AsSampler() noexcept { return std::get<UBO::Bindless::Sampler>(this->Value); }
            NODISCARD constexpr UBO::Bindless::Sampler const& AsSampler() const noexcept { return std::get<UBO::Bindless::Sampler>(this->Value); }
            NODISCARD constexpr LTexture2Ref& AsSampledImage() noexcept { return std::get<LTexture2Ref>(this->Value); }
            NODISCARD constexpr LTexture2Ref const& AsSampledImage() const noexcept { return std::get<LTexture2Ref>(this->Value); }

            value_type Value;
            NODISCARD value_type& operator*() noexcept { return this->Value; }
            NODISCARD value_type const& operator*() const noexcept { return this->Value; }
        };

        u32 Space{};
        vk::raii::DescriptorSet DescriptorSet;
        /* Index always corresponds to the index specified inside the shader. */
        TArray<Resource> Resources;

        NODISCARD constexpr vk::raii::DescriptorSet& operator*() noexcept { return this->DescriptorSet; }
        NODISCARD constexpr vk::raii::DescriptorSet const& operator*() const noexcept { return this->DescriptorSet; }
    };

    TArray<DescriptorSetInstance> InfrequentDescriptorSets;
    rhi::frame_array<TArray<DescriptorSetInstance>> FrequentDescriptorSets;

    //# @return The descriptor set for the given space or nullptr if not found.
    NODISCARD auto* Vk_FindUniqueDescriptorSet(this auto&& Self, u32 Space, std::optional<u32> Frame = {})
    {
        if (auto It{algo::find(Self.InfrequentDescriptorSets, Space, &DescriptorSetInstance::Space)}; It != Self.InfrequentDescriptorSets.end())
        {
            return &*It;
        }
        if (Frame)
        {
            check(algo::valid_index(Self.FrequentDescriptorSets, *Frame))
            auto& FrequentSets{Self.FrequentDescriptorSets[*Frame]};
            if (auto It{algo::find(FrequentSets, Space, &DescriptorSetInstance::Space)}; It != FrequentSets.end())
            {
                return &*It;
            }
        }
        return static_cast<decltype(&*algo::find(Self.InfrequentDescriptorSets, Space, &DescriptorSetInstance::Space))>(nullptr);
    }

    //# Checked method of #Vk_FindUniqueDescriptorSet.
    NODISCARD auto& Vk_GetUniqueDescriptorSet(this auto&& Self, u32 Space, std::optional<u32> Frame = {})
    {
        if (auto* Set{Self.Vk_FindUniqueDescriptorSet(Space, Frame)}; Set)
        {
            return *Set;
        }
        if (!Frame)
        {
            check(!Self.FrequentDescriptorSets.empty())
            auto const& FrequentSets{Self.FrequentDescriptorSets[0uz]};
            if (auto It{algo::find(FrequentSets, Space, &DescriptorSetInstance::Space)}; It != FrequentSets.end())
            {
                LOG_FATAL(LogMaterialSubsystem
                    , "[{}]: Space [{}] is specified as a frequent descriptor set but frame was not specified."
                    , Self.Material->Template.my_shader.Identifier, Space)
            }
        }
        LOG_FATAL(LogMaterialSubsystem
            , "[{}]: No descriptor set for space [{}] found."
            , Self.Material->Template.my_shader.Identifier, Space)
    }

    NODISCARD ENGINE_API rhi::vk_binding GetBinding(LStringView Key) const noexcept;
    ENGINE_API void Vk_SetField(LFrontend& Frontend, rhi::vk_binding Where, LStringView Value);
    ENGINE_API void Vk_SetSampler(LFrontend const& Frontend, rhi::vk_binding Where, vk::Sampler const& Sampler);
    ENGINE_API void Vk_SetSampledImage(LFrontend const& Frontend, rhi::vk_binding Where, LTexture2 const& Texture);
    ENGINE_API void Vk_SetSampledTextureCube(LFrontend const& Frontend, rhi::vk_binding Where, LTextureCube2 const& Texture);
};

} /* ~Namespace rhi */
