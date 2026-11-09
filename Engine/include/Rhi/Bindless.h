// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/Objects.h"

namespace Jafg::UBO
{

struct Bindless: rhi::ubo_template<Bindless, vk::ShaderStageFlagBits::eFragment>
{
    //# Binding point for the bindless samplers.
    static constexpr u32 SamplerBinding{ 0 };
    //# Binding point for the bindless array.
    static constexpr u32 ArrayBinding{ 1 };

    //# Binding index of the identity texture for multiplication inside #ArrayBinding. Always valid.
    static constexpr u32 IdentityMulIdx{ 0 };

    enum struct Sampler: rhi::resource_index_t /* type required by shaders. */
    {
        LinearRepeat = 0,
        LinearMirroredRepeat = 1,
        LinearClampToEdge = 2,
        LinearClampToBorder = 3,
        // LinearMirrorClampToEdge = ,
        NearestRepeat = 4,
        NearestMirroredRepeat = 5,
        NearestClampToEdge = 6,
        NearestClampToBorder = 7,
        // NearestMirrorClampToEdge = ,
    };
    static constexpr rhi::resource_index_t LinearRepeatSamplerIdx{std::to_underlying(Sampler::LinearRepeat)};
    static constexpr rhi::resource_index_t LinearMirroredRepeatSamplerIdx{std::to_underlying(Sampler::LinearMirroredRepeat)};
    static constexpr rhi::resource_index_t LinearClampToEdgeSamplerIdx{std::to_underlying(Sampler::LinearClampToEdge)};
    static constexpr rhi::resource_index_t LinearClampToBorderSamplerIdx{std::to_underlying(Sampler::LinearClampToBorder)};
    // static constexpr rhi::resource_index_t LinearMirrorClampToEdgeSamplerIdx{std::to_underlying(Sampler::LinearMirrorClampToEdge)};
    static constexpr rhi::resource_index_t NearestRepeatSamplerIdx{std::to_underlying(Sampler::NearestRepeat)};
    static constexpr rhi::resource_index_t NearestMirroredRepeatSamplerIdx{std::to_underlying(Sampler::NearestMirroredRepeat)};
    static constexpr rhi::resource_index_t NearestClampToEdgeSamplerIdx{std::to_underlying(Sampler::NearestClampToEdge)};
    static constexpr rhi::resource_index_t NearestClampToBorderSamplerIdx{std::to_underlying(Sampler::NearestClampToBorder)};
    // static constexpr rhi::resource_index_t NearestMirrorClampToEdgeSamplerIdx{std::to_underlying(Sampler::NearestMirrorClampToEdge)};
    static constexpr u32 SamplerCount{std::to_underlying(Sampler::NearestClampToBorder) + 1};

    static std::array<vk::DescriptorBindingFlags, 2> const& descriptor_binding_flags() noexcept
    {
        static std::array Flags{
            vk::DescriptorBindingFlags{},
            vk::DescriptorBindingFlags{
                  vk::DescriptorBindingFlagBits::ePartiallyBound
                | vk::DescriptorBindingFlagBits::eVariableDescriptorCount
                | vk::DescriptorBindingFlagBits::eUpdateAfterBind
                },
            };
        return Flags;
    }
    static std::array<vk::DescriptorSetLayoutBinding, 2> bindings(u32 Capacity) noexcept
    {
        return std::array{
            vk::DescriptorSetLayoutBinding{
                .binding = SamplerBinding,
                .descriptorType = vk::DescriptorType::eSampler,
                .descriptorCount = SamplerCount,
                .stageFlags = shader_stage_flags(),
                .pImmutableSamplers = nullptr
                },
            vk::DescriptorSetLayoutBinding{
                .binding = ArrayBinding,
                .descriptorType = vk::DescriptorType::eSampledImage,
                .descriptorCount = Capacity,
                .stageFlags = shader_stage_flags(),
                .pImmutableSamplers = nullptr
                },
            };
    }
    static vk::DescriptorSetLayoutCreateFlags descriptor_set_layout_flags() noexcept
    {
        return vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;
    }
};
static_assert(rhi::ubo<Bindless>);
static_assert(std::is_same_v<u32, std::underlying_type_t<Bindless::Sampler>>);

SERDE_JSON_ENUM_PAIR(Bindless::Sampler, {
      {Bindless::Sampler::LinearRepeat, "LinearRepeat",}
    , {Bindless::Sampler::LinearMirroredRepeat, "LinearMirroredRepeat",}
    , {Bindless::Sampler::LinearClampToEdge, "LinearClampToEdge",}
    , {Bindless::Sampler::LinearClampToBorder, "LinearClampToBorder",}
    // , {Bindless::Sampler::LinearMirrorClampToEdgeSamplerIdx, "LinearMirrorClampToEdge",}
    , {Bindless::Sampler::NearestRepeat, "NearestRepeat",}
    , {Bindless::Sampler::NearestMirroredRepeat, "NearestMirroredRepeat",}
    , {Bindless::Sampler::NearestClampToEdge, "NearestClampToEdge",}
    , {Bindless::Sampler::NearestClampToBorder, "NearestClampToBorder",}
    // , {Bindless::Sampler::NearestMirrorClampToEdgeSamplerIdx, "NearestMirrorClampToEdge",}
    })

} /* ~Namespace Jafg::UBO */
